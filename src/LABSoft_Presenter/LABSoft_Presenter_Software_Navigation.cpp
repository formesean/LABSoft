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
  const char* current_tab = gui().main_fl_tabs->value()->label();

  if (data[0] == 0 && data[1] == 0 && data[2] == 0)
    return;

  // Macro Keys
  if (data[0] == 1)
  {
    // Back Key
    if (data[1] == 1 && data[2] == 0)
    {
      printf("Back Key Pressed\n");
      fflush(stdout);

      if (strcmp(current_tab, "Function Generator") == 0)
      {
        Fl_Widget* function_generator_fields[] = {
          gui().function_generator_fl_choice_wave_type,
          gui().function_generator_fl_input_choice_frequency,
          gui().function_generator_fl_input_choice_period
        };

        static int current_focus_index = 0;
        current_focus_index = (current_focus_index - 1 + 3) % 3;
        function_generator_fields[current_focus_index]->take_focus();
      }
    }

    // Next Key
    if (data[1] == 2 && data[2] == 0)
    {
      printf("Next Key Pressed\n");
      fflush(stdout);

      if (strcmp(current_tab, "Function Generator") == 0)
      {
        Fl_Widget* function_generator_fields[] = {
          gui().function_generator_fl_choice_wave_type,
          gui().function_generator_fl_input_choice_frequency,
          gui().function_generator_fl_input_choice_period
        };

        static int current_focus_index = 0;
        current_focus_index = (current_focus_index + 1) % 3;
        function_generator_fields[current_focus_index]->take_focus();
      }
    }

    // Run Key
    if (data[1] == 3 && data[2] == 0)
    {
      printf("Run Key Pressed\n");
      fflush(stdout);

      if (strcmp(current_tab, "Oscilloscope") == 0)
      {
        int state = gui().oscilloscope_fl_light_button_run_stop->value();
        gui().oscilloscope_fl_light_button_run_stop->value(!state);
        presenter().m_Oscilloscope.cb_run_stop(
          gui().oscilloscope_fl_light_button_run_stop,
          nullptr
        );
      }

      if (strcmp(current_tab, "Voltmeter") == 0)
      {
        int state = gui().voltmeter_fl_light_button_run_stop->value();
        gui().voltmeter_fl_light_button_run_stop->value(!state);
        presenter().m_Voltmeter.cb_run_stop(
          gui().voltmeter_fl_light_button_run_stop,
          nullptr
        );
      }

      if (strcmp(current_tab, "Ohmmeter") == 0)
      {
        int state = gui().ohmmeter_fl_light_button_run_stop->value();
        gui().ohmmeter_fl_light_button_run_stop->value(!state);
        presenter().m_Ohmmeter.cb_run_stop(
          gui().ohmmeter_fl_light_button_run_stop,
          nullptr
        );
      }

      if (strcmp(current_tab, "Function Generator") == 0)
      {
        int state = gui().function_generator_fl_light_button_run_stop->value();
        gui().function_generator_fl_light_button_run_stop->value(!state);
        presenter().m_Function_Generator.cb_run_stop(
          gui().function_generator_fl_light_button_run_stop,
          0
        );
      }

      if (strcmp(current_tab, "Logic Analyzer") == 0)
      {
        int state = gui().logic_analyzer_fl_light_button_run_stop->value();
        gui().logic_analyzer_fl_light_button_run_stop->value(!state);
        presenter().m_Logic_Analyzer.cb_run_stop(
          gui().logic_analyzer_fl_light_button_run_stop,
          0
        );
      }

      if (strcmp(current_tab, "Digital Circuit Checker") == 0)
      {
        int state = gui().digital_circuit_checker_fl_button_run_checker->value();
        gui().digital_circuit_checker_fl_button_run_checker->value(!state);
        presenter().m_Digital_Circuit_Checker.cb_run_checker(
          gui().digital_circuit_checker_fl_button_run_checker,
          0
        );
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

      if (data[1] == 1)
      {
        switch_tab_by_direction(+1);
        printf("Encoder Rotated CW\n");
      }
      else if (data[1] == 2)
      {
        switch_tab_by_direction(-1);
        printf("Encoder Rotated CCW\n");
      }

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

  if (new_index < 0)
    new_index = 7;
  else if (new_index >= tab_count)
    new_index = 0;

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

// EOF
