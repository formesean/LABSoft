#ifndef LAB_SOFTWARE_NAVIGATION_H
#define LAB_SOFTWARE_NAVIGATION_H

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"

#include <iostream>
#include <iomanip>

class LAB_Software_Navigation : public LAB_Module
{
  private:
    LAB_Parent_Data_Software_Navigation m_parent_data;

  private:
    void parse_and_handle_packet (uint16_t* packet);

  public:
    LAB_Software_Navigation (LAB& LAB);

    void run      ();
    void poll_spi ();
};

#endif

// EOF
