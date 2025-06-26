#include "LAB_Software_Navigation.h"
#include "LAB.h"

LAB_Software_Navigation::
LAB_Software_Navigation(LAB& LAB)
  : LAB_Module(LAB), m_transfer_size(2)
{
  std::memset(m_rx_buffer, 0x00, m_transfer_size);
  std::memset(m_tx_buffer, 0x00, m_transfer_size);
}

LAB_Software_Navigation::
~LAB_Software_Navigation()
{
  stop_navigation();
}

void LAB_Software_Navigation::
run()
{
  lab().rpi().gpio.set(m_parent_data.SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  lab().rpi().gpio.set(m_parent_data.MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  lab().rpi().gpio.set(m_parent_data.MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  lab().rpi().gpio.set(m_parent_data.CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
  lab().rpi().gpio.write(m_parent_data.CS_PIN, true);

  lab().rpi().aux.master_enable_spi(0);
  lab().rpi().aux.spi(0).enable();
  lab().rpi().aux.spi(0).frequency(m_parent_data.BAUD_RATE);
}

void LAB_Software_Navigation::
start_navigation()
{
  if (!m_is_running)
  {
    run();
    m_should_stop = false;
    m_is_running = true;
    m_polling_thread = std::thread(&LAB_Software_Navigation::polling_loop, this);
  }
}

void LAB_Software_Navigation::
stop_navigation()
{
  if (m_is_running)
  {
    m_should_stop = true;

    if (m_polling_thread.joinable())
    {
      m_polling_thread.join();
    }

    m_is_running = false;
  }
}

void LAB_Software_Navigation::
polling_loop()
{
  auto next_poll = std::chrono::high_resolution_clock::now();

  while (!m_should_stop)
  {
    auto start_time = std::chrono::high_resolution_clock::now();

    poll_spi();

    next_poll += m_poll_interval;
    std::this_thread::sleep_until(next_poll);
  }
}

void LAB_Software_Navigation::
poll_spi()
{
  if (m_buffer_dirty.exchange(false))
  {
    std::memset(m_rx_buffer, 0x00, m_transfer_size);
  }

  lab().rpi().gpio.write(m_parent_data.CS_PIN, false);

  lab().rpi().aux.spi(0).xfer(
    reinterpret_cast<char*>(m_rx_buffer),
    reinterpret_cast<char*>(m_tx_buffer),
    m_transfer_size
  );

  lab().rpi().gpio.write(m_parent_data.CS_PIN, true);

  const uint16_t packet = (static_cast<uint16_t>(m_rx_buffer[0]) << 8) |
                          static_cast<uint16_t>(m_rx_buffer[1]);

  if (is_valid_packet(packet))
  {
    parse_and_handle_packet(packet);
    m_buffer_dirty = true;
  }
}

bool LAB_Software_Navigation::
is_valid_packet(uint16_t packet) const
{
  return packet != INVALID_PACKET_1 && packet != INVALID_PACKET_2;
}

bool LAB_Software_Navigation::
validate_checksum(uint16_t packet)
{
  uint8_t type = get_packet_type(packet);
  uint8_t action = get_packet_action(packet);
  uint8_t value = get_packet_value(packet);
  uint8_t checksum = get_packet_checksum(packet);

  uint8_t calculated_checksum = type ^ action ^ value;
  return (calculated_checksum & 0x0F) == checksum;
}

void LAB_Software_Navigation::
parse_and_handle_packet(uint16_t packet)
{
  if (!validate_checksum(packet))
  {
    printf("Invalid checksum for packet: 0x%04X\n", packet);
    return;
  }

  uint8_t type = get_packet_type(packet);
  uint8_t action = get_packet_action(packet);
  uint8_t value = get_packet_value(packet);

  switch (type)
  {
    case 0x01: // Macro Key Event
      printf("Macro Key %u %s\n", action, value ? "PRESSED" : "RELEASED");
      break;

    case 0x02: // Encoder Rotate Event
      printf("Encoder %s %u steps\n", (action == 0x01) ? "CW" : "CCW", value);
      break;

    case 0x03: // Encoder Switch Event
      printf("Encoder Button %s\n", value ? "PRESSED" : "RELEASED");
      break;

    default:
      printf("Unknown packet type: 0x%04X\n", packet);
      break;
  }

  fflush(stdout);
}

// EOF
