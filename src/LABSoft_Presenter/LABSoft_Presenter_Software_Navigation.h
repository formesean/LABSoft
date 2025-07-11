#ifndef LABSOFT_PRESENTER_SOFTWARE_NAVIGATION_H
#define LABSOFT_PRESENTER_SOFTWARE_NAVIGATION_H

#include "LABSoft_Presenter_Unit.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LAB_Enumerations.h"

#include <FL/Fl_Group.H>
#include <cstdio>
#include <chrono>

class LABSoft_Presenter_Software_Navigation : public LABSoft_Presenter_Unit
{
  private:
    std::chrono::steady_clock::time_point last_nav_time;
    const std::chrono::milliseconds nav_debounce_delay{500};

    LABE::SNM::FOCUS_LEVEL current_focus_level = LABE::SNM::FOCUS_LEVEL::TAB;

    int group_index  = 0;
    int widget_index = 0;
    std::vector<Fl_Group*>  current_groups_in_tab;
    std::vector<Fl_Widget*> current_widgets_in_group;

    int current_tab_index = 0;
    static constexpr int tab_count = 8;
    Fl_Group* tab_groups[tab_count];
    Fl_Group* previous_focused_group = nullptr;

  public:
    LABSoft_Presenter_Software_Navigation (LABSoft_Presenter& _LABSoft_Presenter);

    void update_data_cycle       ();
    void switch_tab_by_direction (int direction);
    void sync_current_tab_index  ();
    void highlight_group         (Fl_Group* group);

  private:
    std::vector<Fl_Group*>  get_groups_in_tab    (Fl_Group* tab);
    std::vector<Fl_Widget*> get_widgets_in_group (Fl_Group* group);

    LABE::SNM::TAB_ID       get_current_tab_id    () const;
    std::unordered_map<LABE::SNM::TAB_ID, std::vector<Fl_Group*>> get_focusable_groups_per_tab() const;
};

#endif

// EOF
