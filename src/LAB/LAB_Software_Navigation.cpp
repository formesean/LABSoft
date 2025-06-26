#include "LAB_Software_Navigation.h"
#include "LAB.h"

LAB_Software_Navigation::LAB_Software_Navigation(LAB& lab)
  : LAB_Module(lab)
{
  m_parent_data.BAUD_RATE = 20000000;
  m_parent_data.CS_PIN = 16;
  m_parent_data.MISO_PIN = 19;
  m_parent_data.MOSI_PIN = 20;
  m_parent_data.SCLK_PIN = 21;

  m_rx_buffer.fill(0x00);
  m_tx_buffer.fill(0x00);
}

LAB_Software_Navigation::~LAB_Software_Navigation()
{
  stop_navigation();
}

void LAB_Software_Navigation::initialize_spi()
{
  try {
    auto& gpio = lab().rpi().gpio;
    auto& aux = lab().rpi().aux;

    // Configure GPIO pins
    gpio.set(m_parent_data.SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    gpio.set(m_parent_data.MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    gpio.set(m_parent_data.MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
    gpio.set(m_parent_data.CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
    gpio.write(m_parent_data.CS_PIN, true);

    // Configure SPI
    aux.master_enable_spi(0);
    auto& spi = aux.spi(0);
    spi.enable();
    spi.frequency(m_parent_data.BAUD_RATE);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  catch (const std::exception& e) {
    printf("SPI initialization error: %s\n", e.what());
    throw;
  }
}

void LAB_Software_Navigation::cleanup_spi()
{
  try {
    lab().rpi().aux.spi(0).disable();
  }
  catch (const std::exception& e) {
    printf("SPI cleanup error: %s\n", e.what());
  }
}

void LAB_Software_Navigation::run()
{
  initialize_spi();
}

void LAB_Software_Navigation::start_navigation()
{
  if (m_is_running.exchange(true)) {
    return; // Already running
  }

  try {
    run();
    m_should_stop = false;
    m_polling_thread = std::thread(&LAB_Software_Navigation::polling_loop, this);
  }
  catch (const std::exception& e) {
    m_is_running = false;
    printf("Failed to start navigation: %s\n", e.what());
    throw;
  }
}

void LAB_Software_Navigation::stop_navigation()
{
  if (!m_is_running.exchange(false)) {
    return; // Already stopped
  }

  m_should_stop = true;

  if (m_polling_thread.joinable()) {
    m_polling_thread.join();
  }

  cleanup_spi();
}

void LAB_Software_Navigation::polling_loop()
{
  auto next_poll = std::chrono::high_resolution_clock::now();

  while (!m_should_stop) {
    try {
      poll_spi();

      next_poll += m_poll_interval;
      std::this_thread::sleep_until(next_poll);
    }
    catch (const std::exception& e) {
      printf("Polling error: %s\n", e.what());
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

void LAB_Software_Navigation::poll_spi()
{
  if (m_buffer_dirty.exchange(false)) {
    m_rx_buffer.fill(0x00);
  }

  try {
    auto& gpio = lab().rpi().gpio;
    auto& spi = lab().rpi().aux.spi(0);

    gpio.write(m_parent_data.CS_PIN, false);
    std::this_thread::sleep_for(CS_SETUP_DELAY);

    spi.xfer(
      reinterpret_cast<char*>(m_rx_buffer.data()),
      reinterpret_cast<char*>(m_tx_buffer.data()),
      TRANSFER_SIZE
    );

    gpio.write(m_parent_data.CS_PIN, true);

    const uint16_t packet = (static_cast<uint16_t>(m_rx_buffer[0]) << 8) |
                           static_cast<uint16_t>(m_rx_buffer[1]);

    if (is_valid_packet(packet)) {
      auto packet_data = parse_packet(packet);
      if (packet_data.is_valid) {
        handle_packet(packet_data);
        m_buffer_dirty = true;
      } else {
        log_packet_info(packet_data);
      }
    }
  }
  catch (const std::exception& e) {
    printf("SPI transfer error: %s\n", e.what());
    throw;
  }
}

bool LAB_Software_Navigation::is_valid_packet(uint16_t packet) const
{
  return packet != INVALID_PACKET_1 && packet != INVALID_PACKET_2;
}

LAB_Software_Navigation::PacketData LAB_Software_Navigation::parse_packet(uint16_t packet) const
{
  PacketData data;
  data.type = get_packet_type(packet);
  data.action = get_packet_action(packet);
  data.value = get_packet_value(packet);
  data.checksum = get_packet_checksum(packet);
  data.expected_checksum = calculate_checksum(data.type, data.action, data.value);
  data.is_valid = (data.checksum == data.expected_checksum);

  return data;
}

void LAB_Software_Navigation::handle_packet(const PacketData& packet_data)
{
  switch (packet_data.type) {
    case 0x01: // Macro Key Event
      printf("Macro Key %u %s\n",
             packet_data.action,
             packet_data.value ? "PRESSED" : "RELEASED");
      break;

    case 0x02: // Encoder Rotate Event
      printf("Encoder %s %u steps\n",
             (packet_data.action == 0x01) ? "CW" : "CCW",
             packet_data.value);
      break;

    case 0x03: // Encoder Switch Event
      printf("Encoder Button %s\n",
             packet_data.value ? "PRESSED" : "RELEASED");
      break;

    default:
      printf("Unknown packet type: 0x%02X (Action=0x%02X, Value=0x%02X)\n",
             packet_data.type, packet_data.action, packet_data.value);
      break;
  }

  fflush(stdout);
}

void LAB_Software_Navigation::log_packet_info(const PacketData& packet_data) const
{
  printf("Invalid checksum: Type=0x%02X Action=0x%02X Value=0x%02X "
         "Checksum=0x%02X Expected=0x%02X\n",
         packet_data.type, packet_data.action, packet_data.value,
         packet_data.checksum, packet_data.expected_checksum);
}

bool LAB_Software_Navigation::validate_checksum(uint16_t packet) noexcept
{
  uint8_t type = get_packet_type(packet);
  uint8_t action = get_packet_action(packet);
  uint8_t value = get_packet_value(packet);
  uint8_t checksum = get_packet_checksum(packet);

  return checksum == calculate_checksum(type, action, value);
}

// EOF
