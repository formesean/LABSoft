#include "LAB_MCP4XXX.h"

LAB_MCP4XXX::
LAB_MCP4XXX (MCP4XXX::PART_NUMBER         part, 
             MCP4XXX::RESISTANCE_VERSION  resistance, 
             unsigned                     cs_pin) 
  : MCP4XXX (part, resistance),
    m_rpi   (&AikaPi::get_instance ())
{
  m_cs = cs_pin;
}

LAB_MCP4XXX::
~LAB_MCP4XXX ()
{

}   

void LAB_MCP4XXX::
spi_xfer (char* rxd, char* txd, unsigned length)
{
  m_rpi->aux.spi (0).xfer (rxd, txd, length, m_cs);
}

// EOF