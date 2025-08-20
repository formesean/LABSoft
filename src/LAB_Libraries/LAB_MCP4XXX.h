#ifndef LAB_MCP4XXX_H
#define LAB_MCP4XXX_H

#include "../../lib/MCP4XXX/MCP4XXX.h"
#include "../../lib/AikaPi/AikaPi.h"

// https://ww1.microchip.com/downloads/en/DeviceDoc/22059b.pdf
class LAB_MCP4XXX : public MCP4XXX
{
  private:
    AikaPi* m_rpi = nullptr;

  public:
    LAB_MCP4XXX (MCP4XXX::PART_NUMBER part, MCP4XXX::RESISTANCE_VERSION resistance, unsigned cs_pin);
   ~LAB_MCP4XXX ();

   void spi_xfer  (char* rxd, char* txd, unsigned length);
};

#endif

// EOF