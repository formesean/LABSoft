#include "LABSoft_Presenter_Software_Navigation.h"

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"

LABSoft_Presenter_Software_Navigation::
LABSoft_Presenter_Software_Navigation (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{
  tab_groups[0] = gui().main_fl_group_oscilloscope_tab;            // Oscilloscope
  tab_groups[1] = gui().main_fl_group_voltmeter_tab;               // Voltmeter
  tab_groups[2] = gui().main_fl_group_ohmmeter_tab;                // Ohmmeter
  tab_groups[3] = gui().main_fl_group_function_generator_tab;      // Function Generator
  tab_groups[4] = gui().main_fl_group_power_supply_tab;            // Power Supply
  tab_groups[5] = gui().main_fl_group_logic_analyzer_tab;          // Logic Analyzer
  tab_groups[6] = gui().main_fl_group_digital_circuit_checker_tab; // Digital Circuit Checker
  tab_groups[7] = gui().main_fl_group_labchecker_digital;          // LABChecker - Digital

  sync_current_tab_index();
}

void
LABSoft_Presenter_Software_Navigation::
update_data_cycle()
{
  auto data = lab().m_Software_Navigation.update_spi_data();
  if (data[0] == 0 && data[1] == 0 && data[2] == 0) return;

  const char* current_tab_label = gui().main_fl_tabs->value()->label();
  Fl_Group* current_tab_group = static_cast<Fl_Group*>(gui().main_fl_tabs->value());

  const static std::unordered_map<std::string_view, std::vector<Fl_Group*>> focusable_groups_per_tab = {
    { "Oscilloscope", {
      gui().oscilloscope_fl_group_vertical_channel_0,
      gui().oscilloscope_fl_group_vertical_channel_1,
      gui().oscilloscope_fl_group_display,
      gui().oscilloscope_fl_group_horizontal,
      gui().oscilloscope_fl_group_trigger
    } },
    { "Function Generator", {
      gui().function_generator_fl_group_1,
      gui().function_generator_fl_group_2,
      gui().function_generator_fl_group_3
    } },
    { "Logic Analyzer", {
      gui().logic_analyzer_fl_group_display,
      gui().logic_analyzer_fl_group_trigger,
      gui().logic_analyzer_fl_group_add_remove_channels,
      gui().logic_analyzer_fl_group_horizontal
    } },
    { "Digital Circuit Checker", {
      gui().digital_circuit_checker_fl_group_1,
      gui().digital_circuit_checker_fl_group_2
    } },
    { "LABChecker - Digital", {
      gui().labchecker_digital_fl_group_1,
      gui().labchecker_digital_fl_group_2
    } },
  };

  // Macro Keys
  if (data[0] == 1)
  {
    // Back Key
    if (data[1] == 1 && data[2] == 0)
    {
      printf("Back Key Pressed\n");
      fflush(stdout);

      if (current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET)
      {
        current_focus_level = LABE::SNM::FOCUS_LEVEL::GROUP;
        widget_index = 0;
      }
      else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::GROUP)
      {
        if (group_index > 0)
        {
          group_index--;
          auto* group = current_groups_in_tab[group_index];
          group->take_focus();
          highlight_group(group);
        }
        else
        {
          current_focus_level = LABE::SNM::FOCUS_LEVEL::TAB;
          group_index = 0;
          widget_index = 0;
          current_groups_in_tab.clear();
          current_widgets_in_group.clear();

          if (previous_focused_group)
          {
            previous_focused_group->color(FL_BACKGROUND_COLOR);
            previous_focused_group->redraw();
            previous_focused_group = nullptr;
          }
        }
      }
    }

    // Next Key
    if (data[1] == 2 && data[2] == 0)
    {
      printf("Next Key Pressed\n");
      fflush(stdout);

      if (current_focus_level == LABE::SNM::FOCUS_LEVEL::TAB)
      {
        auto group_it = focusable_groups_per_tab.find(current_tab_label);
        if (group_it != focusable_groups_per_tab.end())
          current_groups_in_tab = group_it->second;
        else
          current_groups_in_tab.clear();

        group_index = 0;

        if (!current_groups_in_tab.empty())
        {
          auto* group = current_groups_in_tab[group_index];
          group->take_focus();
          highlight_group(group);
          current_focus_level = LABE::SNM::FOCUS_LEVEL::GROUP;
        }
      }
      else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::GROUP)
      {
        if (!current_groups_in_tab.empty() && group_index < static_cast<int>(current_groups_in_tab.size()) - 1)
        {
          group_index++;
          auto* group = current_groups_in_tab[group_index];
          group->take_focus();
          highlight_group(group);
        }
      }
    }

    // Run Key
    if (data[1] == 3 && data[2] == 0)
    {
      printf("Run Key Pressed\n");
      fflush(stdout);

      const std::unordered_map<std::string_view, std::function<void()>> run_key_actions = {
        { "Oscilloscope", [this]() {
            auto* btn = gui().oscilloscope_fl_light_button_run_stop;
            btn->value(!btn->value());
            presenter().m_Oscilloscope.cb_run_stop(btn, nullptr);
          }
        },
        { "Voltmeter", [this]() {
            auto* btn = gui().voltmeter_fl_light_button_run_stop;
            btn->value(!btn->value());
            presenter().m_Voltmeter.cb_run_stop(btn, nullptr);
          }
        },
        { "Ohmmeter", [this]() {
            auto* btn = gui().ohmmeter_fl_light_button_run_stop;
            btn->value(!btn->value());
            presenter().m_Ohmmeter.cb_run_stop(btn, nullptr);
          }
        },
        { "Function Generator", [this]() {
            auto* btn = gui().function_generator_fl_light_button_run_stop;
            btn->value(!btn->value());
            presenter().m_Function_Generator.cb_run_stop(btn, 0);
          }
        },
        { "Logic Analyzer", [this]() {
            auto* btn = gui().logic_analyzer_fl_light_button_run_stop;
            btn->value(!btn->value());
            presenter().m_Logic_Analyzer.cb_run_stop(btn, nullptr);
          }
        },
        { "Digital Circuit Checker", [this]() {
            auto* btn = gui().digital_circuit_checker_fl_button_run_checker;
            btn->value(!btn->value());
            presenter().m_Digital_Circuit_Checker.cb_run_checker(btn, nullptr);
          }
        }
      };

      if (auto it = run_key_actions.find(current_tab_label); it != run_key_actions.end()) {
        it->second();
      }
    }
  }

  // Encoder Rotation
  if (data[0] == 2)
  {
    auto now = std::chrono::steady_clock::now();
    if (now - last_nav_time >= nav_debounce_delay)
    {
      last_nav_time = now;
      int dir = (data[1] == 1) ? +1 : -1;

      if (current_focus_level == LABE::SNM::FOCUS_LEVEL::TAB)
      {
        switch_tab_by_direction(dir);
      }
      else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET && !current_widgets_in_group.empty())
      {
        widget_index = (widget_index + dir + current_widgets_in_group.size()) % current_widgets_in_group.size();
        current_widgets_in_group[widget_index]->take_focus();
      }

      printf("Encoder Rotated %s\n", dir > 0 ? "CW" : "CCW");
      fflush(stdout);
    }
  }

  // Encoder Switch
  if (data[0] == 3)
  {}
}

void
LABSoft_Presenter_Software_Navigation::
switch_tab_by_direction (int direction)
{
  int new_index = current_tab_index + direction;

  if (new_index < 0) new_index = tab_count - 1;
  else if (new_index >= tab_count) new_index = 0;

  current_tab_index = new_index;
  Fl_Group* active_tab = tab_groups[current_tab_index];
  gui().main_fl_tabs->value(active_tab);
  gui().main_fl_tabs->redraw();
}

void
LABSoft_Presenter_Software_Navigation::
sync_current_tab_index ()
{
  Fl_Group* current = static_cast<Fl_Group*>(gui().main_fl_tabs->value());

  for (int i = 0; i < tab_count; ++i)
  {
    if (tab_groups[i] == current)
    {
      current_tab_index = i;
      break;
    }
  }
}

void
LABSoft_Presenter_Software_Navigation::
highlight_group(Fl_Group* group)
{
  if (previous_focused_group && previous_focused_group != group)
  {
    previous_focused_group->color(FL_BACKGROUND_COLOR);
    previous_focused_group->redraw();
  }

  if (group)
  {
    group->color(Fl_Color(221));
    group->redraw();
    previous_focused_group = group;
  }
}

std::vector<Fl_Group*>
LABSoft_Presenter_Software_Navigation::
get_groups_in_tab(Fl_Group* tab)
{
  std::vector<Fl_Group*> groups;
  groups.reserve(tab->children());

  for (int i = 0; i < tab->children(); ++i)
  {
    auto* group = dynamic_cast<Fl_Group*>(tab->child(i));

    if (
      group &&
      group->visible() &&
      group->active() &&
      group->takesevents() &&
      group->children() > 0
    ) {
      groups.push_back(group);
    }
  }

  return groups;
}

std::vector<Fl_Widget*>
LABSoft_Presenter_Software_Navigation::
get_widgets_in_group(Fl_Group* group)
{
  std::vector<Fl_Widget*> widgets;

  for (int i = 0; i < group->children(); ++i)
  {
    Fl_Widget* w = group->child(i);

    if (auto* g = dynamic_cast<Fl_Group*>(w))
    {
      auto sub = get_widgets_in_group(g);
      widgets.insert(widgets.end(), sub.begin(), sub.end());
    }
    else if (w->takesevents() && w->visible() && w->active())
    {
      widgets.push_back(w);
    }
  }

  return widgets;
}

// EOF
