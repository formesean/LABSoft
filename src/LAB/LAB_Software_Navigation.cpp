#include "LAB_Software_Navigation.h"
#include "LAB.h"
#include <cstdio>

LAB_Software_Navigation::
LAB_Software_Navigation(LAB &lab)
  : LAB_Module(lab)
{
  init_spi();
}

LAB_Software_Navigation::
~LAB_Software_Navigation()
{
  rpi().aux.spi(0).disable();
}

void
LAB_Software_Navigation::
init_spi()
{
  rpi().gpio.set(m_parent_data.SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi().gpio.set(m_parent_data.MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi().gpio.set(m_parent_data.MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi().gpio.set(m_parent_data.CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
  rpi().gpio.write(m_parent_data.CS_PIN, true);

  rpi().aux.master_enable_spi(0);
  rpi().aux.spi(0).clear_fifos();
  rpi().aux.spi(0).frequency(m_parent_data.BAUD_RATE);
  rpi().aux.spi(0).enable();
}

std::array<uint8_t, 3>
LAB_Software_Navigation::
update_spi_data()
{
  spi_transfer();

  const uint16_t spi_data = (static_cast<uint16_t>(m_rx_buffer[0]) << 8) | static_cast<uint16_t>(m_rx_buffer[1]);

  if (spi_data == 0x0000 || spi_data == 0xFFFF)
    return {0, 0, 0};

  std::printf("0x%04X (0x%02X 0x%02X)\n", static_cast<unsigned>(spi_data), static_cast<unsigned>(m_rx_buffer[0]), static_cast<unsigned>(m_rx_buffer[1]));

  if (!validate_spi_data(spi_data))
    return {0, 0, 0};

  const uint8_t type   = (spi_data >> 12) & 0x0F;
  const uint8_t action = (spi_data >> 8)  & 0x0F;
  const uint8_t value  = (spi_data >> 4)  & 0x0F;

  return { type, action, value};
}

void
LAB_Software_Navigation::
set_tx_message(uint8_t type, uint8_t action, uint8_t value)
{
  type   &= 0x0F;
  action &= 0x0F;
  value  &= 0x0F;

  const uint8_t checksum = (type ^ action ^ value) & 0x0F;

  m_tx_buffer[0] = static_cast<uint8_t>((type << 4) | action);
  m_tx_buffer[1] = static_cast<uint8_t>((value << 4) | checksum);
}

void
LAB_Software_Navigation::
set_tx_logan_config(unsigned samples, double sampling_rate)
{
  uint8_t samples_nibble = 0;
  if (samples == 2000) samples_nibble = 0xA;
  else if (samples == 1000) samples_nibble = 0x9;
  else if (samples == 500) samples_nibble = 0x8;
  else if (samples == 200) samples_nibble = 0x7;
  else if (samples == 100) samples_nibble = 0x6;
  else if (samples == 50) samples_nibble = 0x5;
  else if (samples == 20) samples_nibble = 0x4;
  else if (samples == 10) samples_nibble = 0x3;
  else if (samples == 5)  samples_nibble = 0x2;
  else if (samples == 2)  samples_nibble = 0x1;

  uint8_t rate_nibble = 0;
  if (sampling_rate >= 100) rate_nibble = 0x7;
  else if (sampling_rate >= 50)  rate_nibble = 0x6;
  else if (sampling_rate >= 20)  rate_nibble = 0x5;
  else if (sampling_rate >= 10)  rate_nibble = 0x4;
  else if (sampling_rate >= 5)   rate_nibble = 0x3;
  else if (sampling_rate >= 2)   rate_nibble = 0x2;
  else if (sampling_rate >= 1)   rate_nibble = 0x1;

  set_tx_message(0x6, samples_nibble, rate_nibble);
}

void
LAB_Software_Navigation::
spi_transfer()
{
  rpi().aux.spi(0).clear_fifos();

  rpi().gpio.write(m_parent_data.CS_PIN, false);

  rpi().aux.spi(0).xfer(
    reinterpret_cast<char *>(m_rx_buffer.data()),
    reinterpret_cast<char *>(m_tx_buffer.data()),
    m_parent_data.TRANSFER_SIZE
  );

  rpi().gpio.write(m_parent_data.CS_PIN, true);

  m_tx_buffer[0] = 0x00;
  m_tx_buffer[1] = 0x00;
}

bool
LAB_Software_Navigation::
validate_spi_data(uint16_t spi_data) noexcept
{
  const uint8_t type     = (spi_data >> 12) & 0x0F;
  const uint8_t action   = (spi_data >> 8)  & 0x0F;
  const uint8_t value    = (spi_data >> 4)  & 0x0F;
  const uint8_t checksum =  spi_data        & 0x0F;

  return checksum == ((type ^ action ^ value) & 0x0F);
}

// EOF
