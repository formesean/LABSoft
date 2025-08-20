#include "LAB_MCP23S17.h"

LAB_MCP23S17::
LAB_MCP23S17 (int     CS, 
              int     MISO, 
              int     MOSI, 
              int     SCLK, 
              double  baud, 
              uint8_t hw_addr_bits)
  : MCP23S17 (hw_addr_bits),
    m_spi_bb (AikaPi::SPI_BB (CS, MISO, MOSI, SCLK, baud))
{

}

void LAB_MCP23S17:: 
spi_xfer (uint8_t*  rxd, 
          uint8_t*  txd, 
          unsigned  length)
{
  m_spi_bb.xfer ((char*)rxd, (char*)txd, length);
}