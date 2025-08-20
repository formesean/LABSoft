#ifndef LAB_SOFTWARE_NAVIGATION_H
#define LAB_SOFTWARE_NAVIGATION_H

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"

#include <cstdint>
#include <array>

class LAB_Software_Navigation : public LAB_Module
{
  private:
    LAB_Parent_Data_Software_Navigation m_parent_data;

    std::array<uint8_t, LABC::PIN::SNM::TRANSFER_SIZE> m_rx_buffer = {0};
    std::array<uint8_t, LABC::PIN::SNM::TRANSFER_SIZE> m_tx_buffer = {0};

  public:
    LAB_Software_Navigation                (LAB &lab);
    ~LAB_Software_Navigation               ();

    std::array<uint8_t, 3> update_spi_data ();

  private:
    void init_spi          ();
    void spi_transfer      ();

    bool validate_spi_data (uint16_t spi_data) noexcept;
};

#endif

// EOF
