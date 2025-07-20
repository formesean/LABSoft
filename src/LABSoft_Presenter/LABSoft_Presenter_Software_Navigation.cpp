#include "LABSoft_Presenter_Software_Navigation.h"

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"

#define LOG(msg) fprintf(stdout, "%s\n", msg)

using LABE::SNM::tab_label_to_id;
using LABE::SNM::TAB_ID;

LABSoft_Presenter_Software_Navigation::
LABSoft_Presenter_Software_Navigation(LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  tab_groups[0] = gui().main_fl_group_oscilloscope_tab;
  tab_groups[1] = gui().main_fl_group_voltmeter_tab;
  tab_groups[2] = gui().main_fl_group_ohmmeter_tab;
  tab_groups[3] = gui().main_fl_group_function_generator_tab;
  tab_groups[4] = gui().main_fl_group_power_supply_tab;
  tab_groups[5] = gui().main_fl_group_logic_analyzer_tab;
  tab_groups[6] = gui().main_fl_group_digital_circuit_checker_tab;
  tab_groups[7] = gui().main_fl_group_labchecker_digital;

  sync_current_tab_index();
  initialize_run_key_actions();
}

void
LABSoft_Presenter_Software_Navigation::
update_data_cycle()
{
  auto data = lab().m_Software_Navigation.update_spi_data();
  if (data[0] == 0 && data[1] == 0 && data[2] == 0) return;

  // Macro Keys
  if (data[0] == 1)
  {
    // Back Key
    if (data[1] == 3 && data[2] == 0)
    {
      LOG("Back Key Pressed");

      if (current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET)
      {
        current_focus_level = LABE::SNM::FOCUS_LEVEL::GROUP;
        widget_index = -1;
        clear_widget_focus();
      }
      else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::GROUP)
      {
        if (group_index > 0)
        {
          group_index--;
          auto* group = current_groups_in_tab[group_index];
          group->take_focus();
          highlight_group(group);
          current_widgets_in_group = get_widgets_in_group(group);
          widget_index = -1;
          clear_widget_focus();
        }
        else
        {
          current_focus_level = LABE::SNM::FOCUS_LEVEL::TAB;
          group_index = 0;
          widget_index = -1;
          current_groups_in_tab.clear();
          current_widgets_in_group.clear();
          clear_group_focus();
          clear_widget_focus();
        }
      }
    }

    // Next Key
    if (data[1] == 4 && data[2] == 0)
    {
      LOG("Next Key Pressed");

      if (current_focus_level == LABE::SNM::FOCUS_LEVEL::TAB)
      {
        auto tab_id = get_current_tab_id();
        auto focusable_map = get_focusable_groups_map();

        auto it = focusable_map.find(tab_id);
        current_groups_in_tab = (it != focusable_map.end()) ? it->second : std::vector<Fl_Group*>{};
        group_index = 0;

        if (!current_groups_in_tab.empty())
        {
          auto* group = current_groups_in_tab[group_index];
          group->take_focus();
          highlight_group(group);
          current_widgets_in_group = get_widgets_in_group(group);
          widget_index = -1;
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
          current_widgets_in_group = get_widgets_in_group(group);
          widget_index = -1;
          clear_widget_focus();
        }
        else
        {
          auto* group = current_groups_in_tab[group_index];
          current_widgets_in_group = get_widgets_in_group(group);
          widget_index = 0;

          if (!current_widgets_in_group.empty())
          {
            current_focus_level = LABE::SNM::FOCUS_LEVEL::WIDGET;
            auto* widget = current_widgets_in_group[widget_index];
            widget->take_focus();
            highlight_widget(widget);
          }
        }
      }
    }

    // Run Key
    if (data[1] == 5 && data[2] == 0)
    {
      LOG("Run Key Pressed");
      std::string_view label = gui().main_fl_tabs->value()->label();
      if (auto it = run_key_actions.find(label); it != run_key_actions.end())
        it->second();
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
        switch_tab_by_direction(dir);

      else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::GROUP)
      {
        if (!current_groups_in_tab.empty())
        {
          auto* group = current_groups_in_tab[group_index];
          current_widgets_in_group = get_widgets_in_group(group);

          if (!current_widgets_in_group.empty())
          {
            widget_index = (widget_index == -1) ? 0 : (widget_index + dir + current_widgets_in_group.size()) % current_widgets_in_group.size();
            auto* widget = current_widgets_in_group[widget_index];
            widget->take_focus();
            highlight_widget(widget);
          }
        }
      }

      LOG(dir > 0 ? "Encoder Rotated CW" : "Encoder Rotated CCW");
    }
  }

  // Encoder Switch
  if (data[0] == 3)
  {
    if (data[1] == 1 && data[2] == 0)
    {
      LOG("Encoder Switch Pressed");

      Fl_Widget* widget = previous_focused_widget;

      if (!widget || !widget->visible() || !widget->active() || !widget->takesevents())
      {
        LOG("No valid widget to interact with.");
        return;
      }

      const char* widget_type = typeid(*widget).name();
      LOG(("Focused Widget Type: " + std::string(widget_type)).c_str());
      LOG(("Widget Label: " + std::string(widget->label() ? widget->label() : "<no label>")).c_str());

      if (auto* lightBtn = dynamic_cast<Fl_Light_Button*>(widget))
      {
        int next = !lightBtn->value();
        lightBtn->value(next);
        lightBtn->do_callback();
        lightBtn->redraw();
        LOG(("Fl_Light_Button toggled to: " + std::to_string(next)).c_str());
      }
      else if (auto* btn = dynamic_cast<Fl_Button*>(widget))
      {
        btn->set_changed();
        btn->do_callback();
        btn->value(1);
        btn->redraw();
        Fl::flush();

        Fl::add_timeout(0.05, [](void* v) {
          auto* b = static_cast<Fl_Button*>(v);
          b->value(0);
          b->set_changed();
          b->do_callback();
          b->redraw();
        }, btn);

        LOG("Fl_Button press/release simulated via timeout");
      }
      else if (auto* input = dynamic_cast<Fl_Input*>(widget))
      {
        input->take_focus();
        input->position(input->size());
        input->redraw();
        LOG("Fl_Input focused and cursor moved");
      }
      else if (auto* custom_choice = dynamic_cast<LABSoft_GUI_Fl_Input_Choice_With_Scroll*>(widget))
      {
        Fl_Menu_Button* menu = custom_choice->menubutton();
        int n = menu->size();

        if (n > 0)
        {
          const char* current = custom_choice->input()->value();
          int curr_index = menu->find_index(current);

          if (curr_index < 0 || curr_index >= n)
            curr_index = -1;

          int next_index = (curr_index + 1) % n;
          const Fl_Menu_Item* item = menu->menu() + next_index;

          if (item && item->text)
          {
            custom_choice->input()->value(item->text);
            custom_choice->value(item->text);
            custom_choice->do_callback();
            custom_choice->redraw();
            LOG(("Custom Fl_Input_Choice set to index: " + std::to_string(next_index)).c_str());
          }
          else
          {
            LOG("Menu item is null or has no text.");
          }
        }
        else
        {
            LOG("Custom Fl_Input_Choice has no menu entries.");
        }
      }
      else if (auto* choice = dynamic_cast<Fl_Choice*>(widget))
      {
        int n = choice->size();
        if (n > 0)
        {
          int curr = choice->value();
          if (curr < 0 || curr >= n) curr = -1;

          int next = (curr + 1) % n;
          choice->value(next);
          choice->do_callback();
          choice->redraw();
          LOG(("Fl_Choice set to index: " + std::to_string(next)).c_str());
        }
        else
        {
          LOG("Fl_Choice has no entries.");
        }
      }
      else
      {
        widget->take_focus();
        widget->do_callback();
        widget->redraw();
        LOG("Fallback: Generic widget callback called.");
      }
    }
  }
}

void
LABSoft_Presenter_Software_Navigation::
switch_tab_by_direction(int direction)
{
  int new_index = current_tab_index + direction;
  if (new_index < 0) new_index = tab_count - 1;
  else if (new_index >= tab_count) new_index = 0;

  current_tab_index = new_index;
  gui().main_fl_tabs->value(tab_groups[current_tab_index]);
  gui().main_fl_tabs->redraw();
}

void
LABSoft_Presenter_Software_Navigation::
sync_current_tab_index()
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

void
LABSoft_Presenter_Software_Navigation::
highlight_widget(Fl_Widget* widget)
{
  if (previous_focused_widget && previous_focused_widget != widget)
  {
    // previous_focused_widget->color(Fl_Color(54));
    previous_focused_widget->labelcolor(Fl_Color(0));
    previous_focused_widget->redraw();
  }

  if (widget)
  {
    // widget->color(Fl_Color(221));
    widget->labelcolor(Fl_Color(221));
    widget->redraw();
    previous_focused_widget = widget;
  }
}

void
LABSoft_Presenter_Software_Navigation::
clear_group_focus()
{
  if (previous_focused_group)
  {
    previous_focused_group->color(FL_BACKGROUND_COLOR);
    previous_focused_group->redraw();
    previous_focused_group = nullptr;
  }
}

void
LABSoft_Presenter_Software_Navigation::
clear_widget_focus()
{
  if (previous_focused_widget)
  {
    // previous_focused_widget->color(Fl_Color(54));
    previous_focused_widget->labelcolor(Fl_Color(0));
    previous_focused_widget->redraw();
    previous_focused_widget = nullptr;
  }
}

void
LABSoft_Presenter_Software_Navigation::
initialize_run_key_actions()
{
  run_key_actions = {
    { "Oscilloscope", [this]() {
        auto* btn = gui().oscilloscope_fl_light_button_run_stop;
        btn->value(!btn->value());
        presenter().m_Oscilloscope.cb_run_stop(btn, nullptr);
      }},
    { "Voltmeter", [this]() {
        auto* btn = gui().voltmeter_fl_light_button_run_stop;
        btn->value(!btn->value());
        presenter().m_Voltmeter.cb_run_stop(btn, nullptr);
      }},
    { "Ohmmeter", [this]() {
        auto* btn = gui().ohmmeter_fl_light_button_run_stop;
        btn->value(!btn->value());
        presenter().m_Ohmmeter.cb_run_stop(btn, nullptr);
      }},
    { "Function Generator", [this]() {
        auto* btn = gui().function_generator_fl_light_button_run_stop;
        btn->value(!btn->value());
        presenter().m_Function_Generator.cb_run_stop(btn, 0);
      }},
    { "Logic Analyzer", [this]() {
        auto* btn = gui().logic_analyzer_fl_light_button_run_stop;
        btn->value(!btn->value());
        presenter().m_Logic_Analyzer.cb_run_stop(btn, nullptr);
      }},
    { "Digital Circuit Checker", [this]() {
        auto* btn = gui().digital_circuit_checker_fl_button_run_checker;
        btn->value(!btn->value());
        presenter().m_Digital_Circuit_Checker.cb_run_checker(btn, nullptr);
      }}
  };
}

std::vector<Fl_Group*>
LABSoft_Presenter_Software_Navigation::
get_groups_in_tab(Fl_Group* tab) const
{
  std::vector<Fl_Group*> groups;
  groups.reserve(tab->children());

  for (int i = 0; i < tab->children(); ++i)
  {
    auto* group = dynamic_cast<Fl_Group*>(tab->child(i));
    if (group && group->visible() && group->active() && group->takesevents() && group->children() > 0)
      groups.push_back(group);
  }

  return groups;
}

std::vector<Fl_Widget*>
LABSoft_Presenter_Software_Navigation::
get_widgets_in_group(Fl_Group* group) const
{
  std::vector<Fl_Widget*> widgets;

  for (int i = 0; i < group->children(); ++i)
  {
    Fl_Widget* w = group->child(i);

    if (!w->takesevents() || !w->visible() || !w->active()) continue;

    if (dynamic_cast<LABSoft_GUI_Fl_Input_Choice_With_Scroll*>(w))
    {
      widgets.push_back(w);
    }
    else if (auto* g = dynamic_cast<Fl_Group*>(w))
    {
      auto sub = get_widgets_in_group(g);
      widgets.insert(widgets.end(), sub.begin(), sub.end());
    }
    else
    {
      widgets.push_back(w);
    }
  }

  return widgets;
}

LABE::SNM::TAB_ID
LABSoft_Presenter_Software_Navigation::
get_current_tab_id() const
{
  std::string_view label = gui().main_fl_tabs->value()->label();
  if (auto it = tab_label_to_id.find(label); it != tab_label_to_id.end())
    return it->second;
  return LABE::SNM::TAB_ID::OSCILLOSCOPE;
}

std::unordered_map<LABE::SNM::TAB_ID, std::vector<Fl_Group*>>
LABSoft_Presenter_Software_Navigation::
get_focusable_groups_map() const
{
  using TAB = LABE::SNM::TAB_ID;

  return {
    { TAB::OSCILLOSCOPE, {
        gui().oscilloscope_fl_group_vertical_channel_0,
        gui().oscilloscope_fl_group_vertical_channel_1,
        gui().oscilloscope_fl_group_display,
        gui().oscilloscope_fl_group_horizontal,
        gui().oscilloscope_fl_group_trigger }},
    { TAB::FUNCTION_GENERATOR, {
        gui().function_generator_fl_group_1,
        gui().function_generator_fl_group_2,
        gui().function_generator_fl_group_3 }},
    { TAB::LOGIC_ANALYZER, {
        gui().logic_analyzer_fl_group_display,
        gui().logic_analyzer_fl_group_trigger,
        gui().logic_analyzer_fl_group_add_remove_channels,
        gui().logic_analyzer_fl_group_horizontal }},
    { TAB::DIGITAL_CIRCUIT_CHECKER, {
        gui().digital_circuit_checker_fl_group_1,
        gui().digital_circuit_checker_fl_group_2 }},
    { TAB::LABCHECKER_DIGITAL, {
        gui().labchecker_digital_fl_group_1,
        gui().labchecker_digital_fl_group_2 }}
  };
}

// EOF
