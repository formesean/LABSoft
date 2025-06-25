#include "LAB_Software_Navigation.h"

#include "LAB.h"

LAB_Software_Navigation::
LAB_Software_Navigation (LAB& LAB)
  : LAB_Module (LAB)
{

};

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
};

void LAB_Software_Navigation::
start_navigation()
{
  if (!m_is_running)
  {
    run();
    m_should_stop = false;
    m_polling_thread = std::thread(&LAB_Software_Navigation::polling_loop, this);
    m_is_running = true;
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
  while (!m_should_stop)
  {
    poll_spi();
  }
}

void LAB_Software_Navigation::
poll_spi()
{
  uint8_t rx_buffer[m_parent_data.BUFFER_SIZE] = {0x00};
  uint8_t tx_buffer[m_parent_data.BUFFER_SIZE] = {0x00, 0x00};

  lab().rpi().gpio.write(m_parent_data.CS_PIN, false);

  lab().rpi().aux.spi(0).xfer(
    reinterpret_cast<char *>(rx_buffer),
    reinterpret_cast<char *>(tx_buffer),
    m_parent_data.BUFFER_SIZE
  );

  lab().rpi().gpio.write(m_parent_data.CS_PIN, true);

  uint16_t packet = (
    static_cast<uint16_t>(rx_buffer[0] << 8) |
    static_cast<uint16_t>(rx_buffer[1])
  );

  if (packet != 0x0000 && packet != 0xFFFF)
  {
    parse_and_handle_packet(&packet);
  }
};

void LAB_Software_Navigation::
parse_and_handle_packet(uint16_t* packet)
{
  std::cout << "Received: 0x"
            << std::hex << std::setw(4)
            << std::setfill('0')
            << *packet <<
  std::endl;
};

// EOF
