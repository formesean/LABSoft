#include "LAB_Software_Navigation.h"

#include "LAB.h"

#define BAUD_RATE 20000000
#define CS_PIN 16
#define MISO_PIN 19
#define MOSI_PIN 20
#define SCLK_PIN 21

#define BUFFER_SIZE 2

LAB_Software_Navigation::
LAB_Software_Navigation (LAB& LAB)
  : LAB_Module (_LAB)
{

}

void LAB_Software_Navigation::
run()
{
  lab().rpi().gpio.set(SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  lab().rpi().gpio.set(MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  lab().rpi().gpio.set(MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  lab().rpi().gpio.set(CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
  lab().rpi().gpio.write(CS_PIN, true);

  lab().rpi().aux.master_enable_spi(0);
  lab().rpi().aux.spi(0).enable();
  lab().rpi().aux.spi(0).frequency(BAUD_RATE);
}

void LAB_Software_Navigation::
poll_spi()
{
  uint8_t rx_buffer[BUFFER_SIZE] = {0x00};
  uint8_t tx_buffer[BUFFER_SIZE] = {0x00, 0x00};

  lab().rpi().gpio.write(CS_PIN, false);

  lab().rpi().aux.spi(0).xfer(
    reinterpret_cast<char *>(rx_buffer),
    reinterpret_cast<char *>(tx_buffer),
    BUFFER_SIZE
  );

  lab().rpi().gpio.write(CS_PIN, true);

  uint16_t packet = (
    static_cast<uint16_t>(rx_buffer[0] << 8) |
    static_cast<uint16_t>(rx_buffer[1])
  );

  if (packet != 0x0000 && packet != 0xFFFF)
  {
    parse_and_handle_packet(packet);
  }
}

void LAB_Software_Navigation::
parse_and_handle_packet(uint16_t* packet)
{
  std::cout << "Received: 0x"
            << std::hex << std::setw(4)
            << std::setfill('0')
            << packet <<
  std::endl;
}

// EOF
