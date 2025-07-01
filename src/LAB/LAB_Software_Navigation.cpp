#include "LAB_Software_Navigation.h"
#include "LAB.h"

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

  const uint16_t spi_data = (static_cast<uint16_t>(m_m_rx_buffer[0]) << 8) |
                             static_cast<uint16_t>(m_m_rx_buffer[1]);

  if (spi_data == 0x0000 || spi_data == 0xFFFF)
    return {0, 0, 0};

  if (!validate_spi_data(spi_data))
    return {0, 0, 0};

  const uint8_t type   = (spi_data >> 12) & 0x0F;
  const uint8_t action = (spi_data >> 8)  & 0x0F;
  const uint8_t value  = (spi_data >> 4)  & 0x0F;

  return { type, action, value};
}

void
LAB_Software_Navigation::
spi_transfer()
{
  rpi().gpio.write(m_parent_data.CS_PIN, false);

  rpi().aux.spi(0).xfer(
    reinterpret_cast<char *>(m_rx_buffer.data()),
    reinterpret_cast<char *>(m_tx_buffer.data()),
    m_parent_data.TRANSFER_SIZE
  );

  rpi().gpio.write(m_parent_data.CS_PIN, true);
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
