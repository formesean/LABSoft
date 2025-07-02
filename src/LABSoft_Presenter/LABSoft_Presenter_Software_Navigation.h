#ifndef LABSOFT_PRESENTER_SOFTWARE_NAVIGATION_H
#define LABSOFT_PRESENTER_SOFTWARE_NAVIGATION_H

#include "LABSoft_Presenter_Unit.h"

#include <FL/Fl_Group.H>
#include <cstdio>
#include <chrono>

class LABSoft_Presenter_Software_Navigation : public LABSoft_Presenter_Unit
{
  private:
    std::chrono::steady_clock::time_point last_nav_time;
    const std::chrono::milliseconds nav_debounce_delay{500};

    int current_tab_index = 0;

    static constexpr int tab_count = 8;

    Fl_Group* tab_groups[tab_count];

  public:
    LABSoft_Presenter_Software_Navigation (LABSoft_Presenter& _LABSoft_Presenter);

    void update_data_cycle       ();
    void switch_tab_by_direction (int direction);
    void sync_current_tab_index  ();
};

#endif

// EOF
