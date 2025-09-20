#include "LABSoft_Presenter_Software_Navigation.h"
#include <string>
#include <cstring>
#include <cmath>

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"

#define LOG(msg) fprintf(stdout, "%s\n", msg)

LABSoft_Presenter_Software_Navigation::
LABSoft_Presenter_Software_Navigation(LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  Fl::visible_focus(0);

  tab_groups[0] = gui().main_fl_group_oscilloscope_tab;
  tab_groups[1] = gui().main_fl_group_voltmeter_tab;
  tab_groups[2] = gui().main_fl_group_ohmmeter_tab;
  tab_groups[3] = gui().main_fl_group_function_generator_tab;
  tab_groups[4] = gui().main_fl_group_power_supply_tab;
  tab_groups[5] = gui().main_fl_group_logic_analyzer_tab;
  tab_groups[6] = gui().main_fl_group_digital_circuit_checker_tab;
  tab_groups[7] = gui().main_fl_group_labchecker_digital;
  tab_groups[8] = gui().main_fl_group_analog_circuit_checker;
  tab_groups[9] = gui().main_fl_group_labchecker_analog_tab;

  sync_current_tab_index();
  initialize_run_key_actions();
}

void
LABSoft_Presenter_Software_Navigation::
update_data_cycle()
{
  auto process_one = [&](const std::array<uint8_t, 3>& data)
  {
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) return;

    // Macro Keys
    if (data[0] == 1)
    {
      // Customizable Key 1
      if (data[1] == 1 && data[2] == 0)
      {
        LOG("Customizable Key 1 Pressed");
        handle_customizable_macro_key(1);
      }

      // Customizable Key 2
      if (data[1] == 2 && data[2] == 0)
      {
        LOG("Customizable Key 2 Pressed");
        handle_customizable_macro_key(2);
      }

      // Back Key
      if (data[1] == 3 && data[2] == 0)
      {
        LOG("Back Key Pressed");

        if (current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET)
        {
          widget_index = -1;
          clear_widget_focus();

          auto tab_id = get_current_tab_id();
          auto focusable_map = get_focusable_groups_map();
          auto it = focusable_map.find(tab_id);
          current_groups_in_tab = (it != focusable_map.end()) ? it->second : std::vector<Fl_Group*>{};

          group_index = 0;
          if (previous_focused_group && !current_groups_in_tab.empty())
          {
            for (int gi = 0; gi < static_cast<int>(current_groups_in_tab.size()); ++gi)
            {
              if (current_groups_in_tab[gi] == previous_focused_group)
              {
                group_index = gi;
                break;
              }
            }
          }

          if (!current_groups_in_tab.empty() && group_index > 0)
          {
            current_focus_level = LABE::SNM::FOCUS_LEVEL::GROUP;
            group_index--;
            auto* group = current_groups_in_tab[group_index];
            group->take_focus();
            highlight_group(group);
          }
          else
          {
            current_focus_level = LABE::SNM::FOCUS_LEVEL::TAB;
            group_index = 0;
            current_widgets_in_group.clear();
            clear_group_focus();
            highlight_tab();
          }
        }
        else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::GROUP)
        {
          if (current_groups_in_tab.size() <= 1)
          {
            auto tab_id = get_current_tab_id();
            auto focusable_map = get_focusable_groups_map();
            auto it = focusable_map.find(tab_id);
            current_groups_in_tab = (it != focusable_map.end()) ? it->second : std::vector<Fl_Group*>{};

            group_index = 0;
            if (previous_focused_group && !current_groups_in_tab.empty())
            {
              for (int gi = 0; gi < static_cast<int>(current_groups_in_tab.size()); ++gi)
              {
                if (current_groups_in_tab[gi] == previous_focused_group)
                {
                  group_index = gi;
                  break;
                }
              }
            }
          }

          if (!current_groups_in_tab.empty())
          {
            if (group_index < 0 || group_index >= static_cast<int>(current_groups_in_tab.size()))
              group_index = static_cast<int>(current_groups_in_tab.size()) - 1;

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
              highlight_tab();
            }
          }
          else
          {
            current_focus_level = LABE::SNM::FOCUS_LEVEL::TAB;
            group_index = 0;
            widget_index = -1;
            clear_group_focus();
            clear_widget_focus();
            highlight_tab();
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

          if (tab_id == LABE::SNM::TAB_ID::OHMMETER ||
              tab_id == LABE::SNM::TAB_ID::VOLTMETER ||
              tab_id == LABE::SNM::TAB_ID::POWER_SUPPLY)
          {
            return;
          }

          auto focusable_map = get_focusable_groups_map();
          auto it = focusable_map.find(tab_id);

          current_groups_in_tab = (it != focusable_map.end()) ? it->second : std::vector<Fl_Group*>{};
          group_index = 0;

          clear_tab_focus();

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
        else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET)
        {
          widget_index = -1;
          clear_widget_focus();

          auto tab_id = get_current_tab_id();
          auto focusable_map = get_focusable_groups_map();
          auto it = focusable_map.find(tab_id);
          current_groups_in_tab = (it != focusable_map.end()) ? it->second : std::vector<Fl_Group*>{};

          group_index = 0;
          if (previous_focused_group && !current_groups_in_tab.empty())
          {
            for (int gi = 0; gi < static_cast<int>(current_groups_in_tab.size()); ++gi)
            {
              if (current_groups_in_tab[gi] == previous_focused_group)
              {
                group_index = gi;
                break;
              }
            }
          }

          if (!current_groups_in_tab.empty() && group_index < static_cast<int>(current_groups_in_tab.size()) - 1)
          {
            current_focus_level = LABE::SNM::FOCUS_LEVEL::GROUP;
            group_index++;
            auto* group = current_groups_in_tab[group_index];
            group->take_focus();
            highlight_group(group);
          }
          else
          {
            current_focus_level = LABE::SNM::FOCUS_LEVEL::TAB;
            group_index = 0;
            current_widgets_in_group.clear();
            clear_group_focus();
            highlight_tab();
          }
        }
        else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::GROUP)
        {
          if (current_groups_in_tab.size() <= 1)
          {
            auto tab_id = get_current_tab_id();
            auto focusable_map = get_focusable_groups_map();
            auto it = focusable_map.find(tab_id);
            current_groups_in_tab = (it != focusable_map.end()) ? it->second : std::vector<Fl_Group*>{};

            group_index = 0;
            if (previous_focused_group && !current_groups_in_tab.empty())
            {
              for (int gi = 0; gi < static_cast<int>(current_groups_in_tab.size()); ++gi)
              {
                if (current_groups_in_tab[gi] == previous_focused_group)
                {
                  group_index = gi;
                  break;
                }
              }
            }
          }

          if (current_groups_in_tab.empty()) return;

          if (group_index < 0 || group_index >= static_cast<int>(current_groups_in_tab.size()))
            group_index = 0;

          if (group_index < static_cast<int>(current_groups_in_tab.size()) - 1)
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
            current_focus_level = LABE::SNM::FOCUS_LEVEL::TAB;
            group_index = 0;
            widget_index = -1;
            current_groups_in_tab.clear();
            current_widgets_in_group.clear();
            clear_group_focus();
            clear_widget_focus();
            highlight_tab();
          }
        }
      }

      // Run Key
      if (data[1] == 5 && data[2] == 0)
      {
        LOG("Run Key Pressed");
        auto tab_id = get_current_tab_id();

        if (tab_id == LABE::SNM::TAB_ID::LABCHECKER_DIGITAL)
        {
          auto* table = gui().labchecker_digital_labsoft_gui_labchecker_digital_input_table;
          if (table && previous_focused_widget == table && current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET)
          {
            char curr = table->get_selected_cell_value();
            char next = (curr == '1') ? '0' : '1';
            table->set_selected_cell_value(next);
            table->set_show_selection(true);
            table->redraw();
            return;
          }
        }

        std::string_view label = gui().main_fl_tabs->value()->label();
        if (auto it = run_key_actions.find(label); it != run_key_actions.end())
          it->second();
      }
    }

    // Encoder Rotation
    if (data[0] == 2)
    {
      int dir = (data[1] == 1) ? +1 : -1;
      Fl_Widget* widget = previous_focused_widget;

      if (is_encoder_switch_pressed && widget)
      {
        if (auto* input = dynamic_cast<Fl_Input*>(widget))
        {
          if (input == gui().analog_fl_input_time_domain_similarity_threshold ||
              input == gui().analog_fl_input_frequency_domain_similarity_threshold)
          {

            const char* current_str = input->value();
            int current = 0;
            if (current_str && strlen(current_str) > 0)
            {
              try {
                std::string str(current_str);
                if (str.back() == '%') {
                  str.pop_back();
                }
                current = std::stoi(str);
              } catch (...) {
                current = 0;
              }
            }

            if (current == 0 && (current_str == nullptr || strlen(current_str) == 0))
            {
              current = 100;
              input->value("100%");
              input->redraw();
            }

            int next = current + dir;
            if (next < 0) next = 0;
            if (next > 100) next = 100;

            if (next != current)
            {
              std::string value_string = std::to_string(next) + "%";
              input->value(value_string.c_str());
              input->do_callback();
              input->redraw();
            }

            LOG(dir > 0 ? "Similarity threshold increased" : "Similarity threshold decreased");
            return;
          }
        }
      }

      auto now = std::chrono::steady_clock::now();
      if (now - last_nav_time >= nav_debounce_delay)
      {
        last_nav_time = now;

        if (is_encoder_switch_pressed && widget)
        {
          if (auto* custom_choice = dynamic_cast<LABSoft_GUI_Fl_Input_Choice_With_Scroll*>(widget))
          {
            Fl_Menu_Button* menu = custom_choice->menubutton();
            int n = menu->size();

            if (n > 0)
            {
              const char* current = custom_choice->input()->value();
              int curr_index = -1;

              // 1) Try exact string match first
              if (current && *current)
              {
                curr_index = menu->find_index(current);
              }

              // 2) If not found, try numeric match using LABSoft_GUI_Label parsing
              if (curr_index < 0 && current)
              {
                try {
                  LABSoft_GUI_Label current_label(std::string(current), 0, LABSoft_GUI_Label::UNIT::ANY);
                  if (current_label.is_valid())
                  {
                    double current_value = current_label.actual_value();
                    for (int i = 0; i < n; ++i)
                    {
                      const Fl_Menu_Item* it = menu->menu() + i;
                      if (!it || !it->text) continue;
                      LABSoft_GUI_Label item_label(std::string(it->text), 0, LABSoft_GUI_Label::UNIT::ANY);
                      if (!item_label.is_valid()) continue;
                      if (std::fabs(item_label.actual_value() - current_value) < 0.5)
                      {
                        curr_index = i;
                        break;
                      }
                    }
                  }
                } catch (...) {
                }
              }

              if (curr_index < 0) curr_index = -1;

              int next_index = (curr_index + dir + n) % n;

              int attempts = 0;
              while (attempts < n)
              {
                const Fl_Menu_Item* item = menu->menu() + next_index;

                if (item && item->text && !(item->flags & FL_MENU_INACTIVE))
                {
                  custom_choice->input()->value(item->text);
                  custom_choice->value(item->text);
                  custom_choice->do_callback();
                  refresh_widget_list();
                  custom_choice->redraw();
                  break;
                }

                next_index = (next_index + dir + n) % n;
                attempts++;
              }
            }

            LOG(dir > 0 ? "Choice Scrolled CW (Scroll)" : "Choice Scrolled CCW (Scroll)");
            return;
          }
          else if (auto* choice = dynamic_cast<Fl_Choice*>(widget))
          {
            int n = choice->size();
            if (n > 0)
            {
              int curr = choice->value();
              if (curr < 0) curr = -1;

              int next = (curr + dir + n) % n;

              int attempts = 0;
              while (attempts < n)
              {
                const Fl_Menu_Item* item = choice->menu() + next;
                if (!(item->flags & FL_MENU_INACTIVE))
                {
                  choice->value(next);
                  choice->do_callback();
                  refresh_widget_list();
                  choice->redraw();
                  break;
                }

                next = (next + dir + n) % n;
                ++attempts;
              }
            }
            LOG(dir > 0 ? "Choice Rotated CW" : "Choice Rotated CCW");
            return;
          }
          else if (auto* input = dynamic_cast<Fl_Input*>(widget))
          {
            if (input == gui().digital_fl_input_output_count)
            {
              auto* table = gui().labchecker_digital_labsoft_gui_labchecker_digital_input_table;
              if (table)
              {
                int current = static_cast<int>(table->output_count());
                int maximum = static_cast<int>(table->max_output_count());
                if (maximum < 1) maximum = 1;

                int next = current + dir;
                if (next < 1) next = 1;
                if (next > maximum) next = maximum;

                if (next != current)
                {
                  std::string value_string = std::to_string(next);
                  input->value(value_string.c_str());
                  input->do_callback();
                  input->redraw();
                }
              }

              LOG(dir > 0 ? "Output count increased" : "Output count decreased");
              return;
            }
          }
        }

        if (current_focus_level == LABE::SNM::FOCUS_LEVEL::TAB)
        {
          switch_tab_by_direction(dir);
        }
        else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::GROUP)
        {
          if (!current_groups_in_tab.empty())
          {
            if (group_index < 0 || group_index >= static_cast<int>(current_groups_in_tab.size()))
              group_index = 0;

            auto* group = current_groups_in_tab[group_index];
            current_widgets_in_group = get_widgets_in_group(group);

            if (!current_widgets_in_group.empty())
            {
              widget_index = (widget_index == -1)
                ? 0
                : (widget_index + dir + current_widgets_in_group.size()) % current_widgets_in_group.size();

              auto* widget = current_widgets_in_group[widget_index];
              Fl::focus(nullptr);
              highlight_widget(widget);
              current_focus_level = LABE::SNM::FOCUS_LEVEL::WIDGET;
            }
          }
        }
        else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET)
        {
          if (!current_groups_in_tab.empty())
          {
            if (group_index < 0 || group_index >= static_cast<int>(current_groups_in_tab.size()))
              group_index = 0;

            auto* group = current_groups_in_tab[group_index];
            current_widgets_in_group = get_widgets_in_group(group);

            if (!current_widgets_in_group.empty())
            {
              if (widget_index < 0 || widget_index >= static_cast<int>(current_widgets_in_group.size()))
              {
                int found = -1;
                for (int i = 0; i < static_cast<int>(current_widgets_in_group.size()); ++i)
                {
                  if (current_widgets_in_group[i] == previous_focused_widget)
                  {
                    found = i;
                    break;
                  }
                }
                widget_index = (found != -1) ? found : 0;
              }

              widget_index = (widget_index + dir + current_widgets_in_group.size()) % current_widgets_in_group.size();
              auto* w = current_widgets_in_group[widget_index];
              Fl::focus(nullptr);
              highlight_widget(w);
            }
          }
        }

        LOG(dir > 0 ? "Encoder Rotated CW" : "Encoder Rotated CCW");
      }
    }

    // Encoder Switch
    if (data[0] == 3)
    {
      if (data[1] == 1 && data[2] == 1) // Pressed
      {
        LOG("Encoder Switch Pressed");
        if (is_encoder_switch_pressed) return;
        is_encoder_switch_pressed = true;

        Fl_Widget* widget = previous_focused_widget;

        if (!widget || !widget->visible() || !widget->active() || !widget->takesevents()) return;

        const char* widget_type = typeid(*widget).name();
        LOG(("Focused Widget Type: " + std::string(widget_type)).c_str());
        LOG(("Widget Label: " + std::string(widget->label() ? widget->label() : "<no label>")).c_str());

        if (auto* lightBtn = dynamic_cast<Fl_Light_Button*>(widget))
        {
          int next = !lightBtn->value();
          lightBtn->value(next);
          lightBtn->do_callback();
          lightBtn->redraw();
        }
        else if (auto* btn = dynamic_cast<Fl_Button*>(widget))
        {
          btn->set_changed();
          btn->do_callback();
          btn->redraw();
          Fl::flush();
        }
        else if (auto* input = dynamic_cast<Fl_Input*>(widget))
        {
          input->take_focus();
          input->position(input->size());
          input->redraw();
        }
        else
        {
          Fl::focus(nullptr);
          widget->do_callback();
          refresh_widget_list();
          widget->redraw();
        }
      }
      else if (data[1] == 1 && data[2] == 0) // Released
      {
        LOG("Encoder Switch Released");
        is_encoder_switch_pressed = false;
      }
    }
  };

  // Drain the queue this frame
  for (;;)
  {
    auto data = lab().m_Software_Navigation.update_spi_data();
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) break;
    process_one(data);
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

  if (current_tab_index == 8)
  {
    gui().analog_fl_input_time_domain_similarity_threshold->value("100%");
    gui().analog_fl_input_time_domain_similarity_threshold->redraw();
    gui().analog_fl_input_frequency_domain_similarity_threshold->value("100%");
    gui().analog_fl_input_frequency_domain_similarity_threshold->redraw();
  }
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
highlight_tab()
{
  gui().main_fl_tabs->labelcolor(Fl_Color(221));
  gui().main_fl_tabs->redraw();
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
    // Revert special highlighting for inputs
    if (auto* prev_input = dynamic_cast<Fl_Input*>(previous_focused_widget))
    {
      prev_input->color(FL_BLACK);
      prev_input->redraw();
    }

    previous_focused_widget->labelcolor(Fl_Color(0));
    previous_focused_widget->redraw();
  }

  if (!widget)
    return;

  if (widget == gui().digital_circuit_checker_fl_output_selected_file ||
      widget == gui().digital_circuit_checker_fl_output_results ||
      widget == gui().analog_circuit_checker_fl_output_selected_file ||
      widget == gui().analog_circuit_checker_fl_checkbutton_time_domain ||
      widget == gui().analog_circuit_checker_fl_input_time_domain_similarity_threshold ||
      widget == gui().analog_circuit_checker_fl_checkbutton_frequency_domain ||
      widget == gui().analog_circuit_checker_fl_input_frequency_domain_similarity_threshold ||
      widget == gui().logic_analyzer_fl_button_record_config ||
      widget == gui().logic_analyzer_fl_button_record)
  {
    previous_focused_widget = widget;
    return;
  }

  if (auto* input = dynamic_cast<Fl_Input*>(widget))
  {
    input->color(Fl_Color(221));
    input->redraw();
  }
  else
  {
    widget->labelcolor(Fl_Color(221));
    widget->redraw();
  }
  previous_focused_widget = widget;
}

void
LABSoft_Presenter_Software_Navigation::
clear_tab_focus()
{
  gui().main_fl_tabs->labelcolor(FL_BLACK);
  gui().main_fl_tabs->redraw();
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
    if (auto* t = dynamic_cast<LABSoft_GUI_LABChecker_Digital_Input_Table*>(previous_focused_widget))
    {
      t->set_show_selection(false);
      t->redraw();
    }
    // Revert special highlighting for inputs
    if (auto* prev_input = dynamic_cast<Fl_Input*>(previous_focused_widget))
    {
      prev_input->color(FL_BLACK);
      prev_input->redraw();
    }
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
      }},
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

void
LABSoft_Presenter_Software_Navigation::
handle_customizable_macro_key(int key_id)
{
  using LABE::SNM::ACTION_TYPE;
  using LABE::SNM::TAB_ID;
  using LABE::SNM::tab_label_to_id;

  const auto config = lab().m_Shortcuts.get_config(key_id);
  auto tab_id = get_current_tab_id();

  // --- Oscilloscope tab override ---
  if (tab_id == TAB_ID::OSCILLOSCOPE)
  {
    switch (key_id)
    {
      case 1: // voltage per division (CH0/CH1)
      {
        static int cycle_index = 0;
        Fl_Widget* targets[] = {
          gui().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_voltage_per_division,
          gui().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_voltage_per_division,
        };

        int group_indices[] = { 2, 3 };

        Fl_Widget* target = targets[cycle_index];
        int g_index = group_indices[cycle_index];

        if (target && g_index >= 0 && g_index < tab_groups[current_tab_index]->children())
        {
          clear_widget_focus();
          clear_group_focus();
          clear_tab_focus();

          auto* parent_group = tab_groups[current_tab_index]->child(g_index);
          if (auto* group = dynamic_cast<Fl_Group*>(parent_group))
          {
            highlight_group(group);
            previous_focused_group = group;
            current_groups_in_tab = { group };
            group_index = 0;

            current_widgets_in_group = get_widgets_in_group(group);
            widget_index = -1;
            for (int i = 0; i < group->children(); ++i)
            {
              if (group->child(i) == target)
              {
                widget_index = i;
                break;
              }
            }

            if (widget_index != -1)
            {
              current_focus_level = LABE::SNM::FOCUS_LEVEL::WIDGET;
              previous_focused_widget = target;
              Fl::focus(nullptr);
              highlight_widget(target);
            }
          }
        }

        cycle_index = (cycle_index + 1) % 2;
        return;
      }

      case 2: // time per division
      {
        Fl_Widget* target = gui().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_time_per_division;
        int g_index = 5;

        if (target && g_index >= 0 && g_index < tab_groups[current_tab_index]->children())
        {
          clear_widget_focus();
          clear_group_focus();
          clear_tab_focus();

          auto* parent_group = tab_groups[current_tab_index]->child(g_index);
          if (auto* group = dynamic_cast<Fl_Group*>(parent_group))
          {
            highlight_group(group);
            previous_focused_group = group;
            current_groups_in_tab = { group };
            group_index = 0;

            widget_index = -1;
            current_widgets_in_group = get_widgets_in_group(group);
            for (int i = 0; i < group->children(); ++i)
            {
              if (group->child(i) == target)
              {
                widget_index = i;
                break;
              }
            }

            if (widget_index != -1)
            {
              current_focus_level = LABE::SNM::FOCUS_LEVEL::WIDGET;
              previous_focused_widget = target;
              Fl::focus(nullptr);
              highlight_widget(target);
            }
          }
        }
        return;
      }
    }
  }

  // --- LABChecker Digital tab override ---
  if (tab_id == TAB_ID::LABCHECKER_DIGITAL)
  {
    switch (key_id)
    {
      case 1: // previous cell
      case 2: // next cell
      {
        auto* table = gui().labchecker_digital_labsoft_gui_labchecker_digital_input_table;
        if (!table) return;

        bool already_on_table = (previous_focused_widget == table && current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET);
        if (!already_on_table)
        {
          clear_widget_focus();
          clear_group_focus();
          clear_tab_focus();
        }

        if (auto* group = gui().labchecker_digital_fl_group_2)
        {
          if (!already_on_table)
          {
            group->color(FL_BACKGROUND_COLOR);
            group->redraw();
            previous_focused_group = group;
            current_groups_in_tab = { group };
            group_index = 0;

            current_widgets_in_group = get_widgets_in_group(group);
            widget_index = -1;
            for (int i = 0; i < group->children(); ++i)
            {
              if (group->child(i) == table)
              {
                widget_index = i;
                break;
              }
            }

            for (int i = 0; i < group->children(); ++i)
            {
              Fl_Widget* w = group->child(i);
              if (w && w != table)
              {
                w->labelcolor(Fl_Color(0));
                w->redraw();
              }
            }

            Fl_Group* groups_to_clear[] = {
              gui().labchecker_digital_fl_group_1,
              gui().labchecker_digital_fl_group_2
            };

            for (Fl_Group* g : groups_to_clear)
            {
              if (!g || g == group) continue;

              g->color(FL_BACKGROUND_COLOR);
              g->redraw();

              for (int i = 0; i < g->children(); ++i)
              {
                Fl_Widget* w = g->child(i);
                if (w)
                {
                  w->labelcolor(Fl_Color(0));
                  w->redraw();
                }
              }
            }

            bool fresh_focus = true;
            current_focus_level = LABE::SNM::FOCUS_LEVEL::WIDGET;
            previous_focused_widget = table;
            table->take_focus();
            highlight_widget(table);
            table->set_selection(0, 0, 0, 0);
            table->set_show_selection(false);
            table->redraw();
          }
        }

        int r1 = 0, c1 = 0, r2 = 0, c2 = 0;
        table->get_selection(r1, c1, r2, c2);
        bool invalid_selection = (r1 < 0 || c1 < 0);
        if (invalid_selection)
        {
          r1 = c1 = r2 = c2 = 0;
          table->set_selection(0, 0, 0, 0);
        }

        int rows = table->rows();
        int cols = table->cols();
        if (rows <= 0 || cols <= 0) return;

        int r = r1, c = c1;
        if (!table->show_selection())
        {
          table->set_selection(0, 0, 0, 0);
          table->set_show_selection(true);
          table->redraw();
          return;
        }
        if (key_id == 1)
        {
          if (c > 0) { c -= 1; }
          else {
            if (r > 0) { r -= 1; c = cols - 1; }
            else { r = rows - 1; c = cols - 1; }
          }
        }
        else // key_id == 2
        {
          if (c < cols - 1) { c += 1; }
          else {
            if (r < rows - 1) { r += 1; c = 0; }
            else { r = 0; c = 0; }
          }
        }

        table->set_selection(r, c, r, c);
        table->set_show_selection(true);
        table->redraw();
        return;
      }
    }
  }

  // --- Configuration File ---
  if (config.action == ACTION_TYPE::GOTO)
  {
    if (std::holds_alternative<TAB_ID>(config.target))
    {
      TAB_ID target_tab = std::get<TAB_ID>(config.target);
      int tab_index = static_cast<int>(target_tab);

      if (tab_index >= 0 && tab_index < tab_count)
      {
        current_tab_index = tab_index;
        gui().main_fl_tabs->value(tab_groups[current_tab_index]);
        gui().main_fl_tabs->redraw();
      }
    }
  }
  else if (config.action == ACTION_TYPE::RUN)
  {
    std::string_view target_label;

    if (std::holds_alternative<TAB_ID>(config.target))
    {
      for (const auto& [label, id] : tab_label_to_id)
      {
        if (id == std::get<TAB_ID>(config.target))
        {
          target_label = label;
          break;
        }
      }
    }
    else
    {
      target_label = std::get<std::string>(config.target);
    }

    if (!target_label.empty())
    {
      if (auto it = run_key_actions.find(target_label); it != run_key_actions.end())
        it->second();
    }
  }
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

    if (w == gui().digital_circuit_checker_fl_output_selected_file ||
        w == gui().digital_circuit_checker_fl_output_results ||
        w == gui().analog_circuit_checker_fl_output_selected_file ||
        w == gui().analog_circuit_checker_fl_checkbutton_time_domain ||
        w == gui().analog_circuit_checker_fl_input_time_domain_similarity_threshold ||
        w == gui().analog_circuit_checker_fl_checkbutton_frequency_domain ||
        w == gui().analog_circuit_checker_fl_input_frequency_domain_similarity_threshold ||
        w == gui().logic_analyzer_fl_button_record_config ||
        w == gui().logic_analyzer_fl_button_record) continue;

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
  using LABE::SNM::tab_label_to_id;

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
        gui().labchecker_digital_fl_group_2 }},
    { TAB::ANALOG_CIRCUIT_CHECKER, {
        gui().analog_circuit_checker_fl_group_1,
        gui().analog_circuit_checker_fl_group_2 }},
    { TAB::LABCHECKER_ANALOG, {
        gui().labchecker_analog_fl_group }}
  };
}

void
LABSoft_Presenter_Software_Navigation::
refresh_widget_list()
{
  if (current_focus_level == LABE::SNM::FOCUS_LEVEL::GROUP &&
      !current_groups_in_tab.empty() &&
      group_index >= 0 &&
      group_index < static_cast<int>(current_groups_in_tab.size()))
  {
    auto* group = current_groups_in_tab[group_index];
    current_widgets_in_group = get_widgets_in_group(group);

    if (widget_index >= static_cast<int>(current_widgets_in_group.size()))
      widget_index = 0;
  }
  else if (current_focus_level == LABE::SNM::FOCUS_LEVEL::WIDGET &&
           !current_groups_in_tab.empty() &&
           group_index >= 0 &&
           group_index < static_cast<int>(current_groups_in_tab.size()))
  {
    auto* group = current_groups_in_tab[group_index];
    current_widgets_in_group = get_widgets_in_group(group);

    if (widget_index >= static_cast<int>(current_widgets_in_group.size()))
      widget_index = 0;
  }
}

// EOF
