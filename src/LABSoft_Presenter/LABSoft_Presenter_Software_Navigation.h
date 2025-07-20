#ifndef LABSOFT_PRESENTER_SOFTWARE_NAVIGATION_H
#define LABSOFT_PRESENTER_SOFTWARE_NAVIGATION_H

#include "LABSoft_Presenter_Unit.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LAB_Enumerations.h"

#include <FL/Fl_Group.H>
#include <cstdio>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <functional>

class LABSoft_Presenter_Software_Navigation : public LABSoft_Presenter_Unit
{
  private:
    std::chrono::steady_clock::time_point last_nav_time;
    const std::chrono::milliseconds nav_debounce_delay{500};

    LABE::SNM::FOCUS_LEVEL current_focus_level = LABE::SNM::FOCUS_LEVEL::TAB;

    int group_index                = 0;
    int widget_index               = 0;
    int current_tab_index          = 0;
    static constexpr int tab_count = 8;

    std::vector<Fl_Group*>  current_groups_in_tab;
    std::vector<Fl_Widget*> current_widgets_in_group;

    Fl_Group*  tab_groups[tab_count];
    Fl_Group*  previous_focused_group  = nullptr;
    Fl_Widget* previous_focused_widget = nullptr;

    std::unordered_map<std::string_view, std::function<void()>> run_key_actions;

  public:
    LABSoft_Presenter_Software_Navigation (LABSoft_Presenter& _LABSoft_Presenter);

    void update_data_cycle       ();
    void switch_tab_by_direction (int direction);
    void sync_current_tab_index  ();
    void highlight_group         (Fl_Group* group);
    void highlight_widget        (Fl_Widget* widget);

  private:
    void clear_group_focus          ();
    void clear_widget_focus         ();
    void initialize_run_key_actions ();

    std::vector<Fl_Group*>  get_groups_in_tab    (Fl_Group* tab)   const;
    std::vector<Fl_Widget*> get_widgets_in_group (Fl_Group* group) const;
    LABE::SNM::TAB_ID       get_current_tab_id   ()                const;

    std::unordered_map<LABE::SNM::TAB_ID, std::vector<Fl_Group*>> get_focusable_groups_map() const;
};

#endif

// EOF
