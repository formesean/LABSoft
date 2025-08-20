#ifndef LAB_AD9833_H
#define LAB_AD9833_H

#include "../../lib/AD9833/AD9833.h"
#include "../../lib/AikaPi/AikaPi.h"

class LAB_AD9833 : public AD9833
{
  private:
    AikaPi::SPI_BB m_spi_bb;

  public:
    LAB_AD9833 (int CS, int MISO, int MOSI, int SCLK, double baud);
   ~LAB_AD9833 ();

    void spi_xfer (char* rxd, char* txd, unsigned length);
};

#endif 