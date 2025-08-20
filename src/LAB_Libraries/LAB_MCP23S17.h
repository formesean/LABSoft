#ifndef LAB_MCP23S17_H
#define LAB_MCP23S17_H

#include "../../lib/MCP23S17/MCP23S17.h"
#include "../../lib/AikaPi/AikaPi.h"

class LAB_MCP23S17 : public MCP23S17
{
  private:
    AikaPi::SPI_BB m_spi_bb;

  public:
    LAB_MCP23S17 (int CS, int MISO, int MOSI, int SCLK, double baud, uint8_t hw_addr_bits);

    void spi_xfer (uint8_t* rxd, uint8_t* txd, unsigned length);
};

#endif