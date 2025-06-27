#include "LAB_Software_Navigation.h"
#include "LAB.h"

LAB_Software_Navigation::
LAB_Software_Navigation(LAB &lab)
  : LAB_Module(lab)
{
  m_parent_data.BAUD_RATE = 20000000;
  m_parent_data.CS_PIN = 16;
  m_parent_data.MISO_PIN = 19;
  m_parent_data.MOSI_PIN = 20;
  m_parent_data.SCLK_PIN = 21;

  m_rx_buffer.fill(0x00);
  m_tx_buffer.fill(0x00);

  // Set default packet handler
  m_packet_handler = [this](uint8_t type, uint8_t action, uint8_t value)
  {
    switch (type)
    {
    case 0x01: // Macro Key
      printf("Key %u %s\n", action, value ? "PRESS" : "RELEASE");
      break;
    case 0x02: // Encoder Rotate
      printf("Encoder %s %u\n", (action == 0x01) ? "CW" : "CCW", value);
      break;
    case 0x03: // Encoder Switch
      printf("Encoder %s\n", value ? "PRESS" : "RELEASE");
      break;
    }
  };
}

LAB_Software_Navigation::
~LAB_Software_Navigation()
{
  stop_navigation();
}

void LAB_Software_Navigation::
initialize_spi()
{
  auto &gpio = lab().rpi().gpio;
  auto &aux = lab().rpi().aux;

  // Configure pins with optimal settings
  gpio.set(m_parent_data.SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  gpio.set(m_parent_data.MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  gpio.set(m_parent_data.MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  gpio.set(m_parent_data.CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
  gpio.write(m_parent_data.CS_PIN, true);

  // Initialize SPI with optimal settings
  aux.master_enable_spi(0);
  auto &spi = aux.spi(0);
  spi.enable();
  spi.frequency(m_parent_data.BAUD_RATE);

  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void LAB_Software_Navigation::
cleanup_spi()
{
  try
  {
    lab().rpi().aux.spi(0).disable();
  }
  catch (const std::exception &e)
  {
    std::cerr << "[WARN] Failed to disable SPI: " << e.what() << std::endl;
  }
}

void LAB_Software_Navigation::
start_navigation()
{
  if (m_is_running.exchange(true, std::memory_order_acq_rel))
  {
    return; // Already running
  }

  try
  {
    initialize_spi();
    m_should_stop.store(false, std::memory_order_relaxed);
    m_polling_thread = std::make_unique<std::thread>(
        &LAB_Software_Navigation::polling_loop, this);
  }
  catch (const std::exception &e)
  {
    std::cerr << "[ERROR] start_navigation() failed: " << e.what() << std::endl;
    m_is_running.store(false, std::memory_order_relaxed);
    cleanup_spi();
    throw;
  }
}

void LAB_Software_Navigation::
stop_navigation()
{
  if (!m_is_running.exchange(false, std::memory_order_acq_rel))
  {
    return; // Already stopped
  }

  m_should_stop.store(true, std::memory_order_relaxed);

  if (m_polling_thread && m_polling_thread->joinable())
  {
    m_polling_thread->join();
  }

  cleanup_spi();
}

void LAB_Software_Navigation::
polling_loop()
{
  auto next_poll = std::chrono::high_resolution_clock::now();

  while (!m_should_stop.load(std::memory_order_relaxed))
  {
    if (spi_transfer_fast())
    {
      const uint16_t packet = (static_cast<uint16_t>(m_rx_buffer[0]) << 8) |
                              static_cast<uint16_t>(m_rx_buffer[1]);

      if (packet != INVALID_PACKET)
      {
        process_packet(packet);
      }
    }

    next_poll += POLL_INTERVAL;
    std::this_thread::sleep_until(next_poll);
  }
}

inline bool LAB_Software_Navigation::
spi_transfer_fast() noexcept
{
  try
  {
    auto &gpio = lab().rpi().gpio;
    auto &spi = lab().rpi().aux.spi(0);

    gpio.write(m_parent_data.CS_PIN, false);
    std::this_thread::sleep_for(CS_SETUP_DELAY);

    spi.xfer(
        reinterpret_cast<char *>(m_rx_buffer.data()),
        reinterpret_cast<char *>(m_tx_buffer.data()),
        TRANSFER_SIZE);

    gpio.write(m_parent_data.CS_PIN, true);

    printf("Raw SPI data: 0x%02X 0x%02X\n", m_rx_buffer[0], m_rx_buffer[1]);

    return true;
  }
  catch (const std::exception &e)
  {
    std::cerr << "[WARN] SPI transfer failed: " << e.what() << std::endl;
    return false;
  }
}

inline bool LAB_Software_Navigation::
process_packet(uint16_t packet) noexcept
{
  if (!validate_packet_fast(packet))
  {
    m_invalid_packets.fetch_add(1, std::memory_order_relaxed);
    return false;
  }

  const uint8_t type = (packet >> 12) & 0x0F;
  const uint8_t action = (packet >> 8) & 0x0F;
  const uint8_t value = (packet >> 4) & 0x0F;

  handle_valid_packet(type, action, value);
  m_packets_processed.fetch_add(1, std::memory_order_relaxed);

  return true;
}

inline bool LAB_Software_Navigation::
validate_packet_fast(uint16_t packet) noexcept
{
  const uint8_t type = (packet >> 12) & 0x0F;
  const uint8_t action = (packet >> 8) & 0x0F;
  const uint8_t value = (packet >> 4) & 0x0F;
  const uint8_t checksum = packet & 0x0F;

  return checksum == ((type ^ action ^ value) & 0x0F);
}

inline void LAB_Software_Navigation::
handle_valid_packet(uint8_t type, uint8_t action, uint8_t value) noexcept
{
  if (m_packet_handler)
  {
    try
    {
      m_packet_handler(type, action, value);
    }
    catch (const std::exception &e)
    {
      std::cerr << "[WARN] Packet handler threw an exception: " << e.what() << std::endl;
    }
  }
}

// EOF
