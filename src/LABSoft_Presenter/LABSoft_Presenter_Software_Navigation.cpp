#include "LABSoft_Presenter_Software_Navigation.h"

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"

LABSoft_Presenter_Software_Navigation::
LABSoft_Presenter_Software_Navigation (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
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
}

void
LABSoft_Presenter_Software_Navigation::
update_data_cycle()
{
  auto data = lab().m_Software_Navigation.update_spi_data();

  // main_fl_group_oscilloscope_tab             = Oscilloscope (label)
  // main_fl_group_voltmeter_tab                = Voltmeter
  // main_fl_group_ohmmeter_tab                 = Ohmmeter
  // main_fl_group_funtion_generator_tab        = Function Generator
  // main_fl_group_power_supply_tab             = Power Supply
  // main_fl_group_logic_analyzer_tab           = Logic Analyzer
  // main_fl_group_digital_circuit_checker_tab  = Digital Circuit Checker
  // main_fl_group_labchecker_digital_tab       = LABChecker - Digital

  const char* current_tab = gui().main_fl_tabs->value()->label();
  // printf("current tab: %s\n", current_tab);

  if (data[0] == 0 && data[1] == 0 && data[2] == 0)
    return;

  printf("Type: %u, Action: %u, Value: %u\n", data[0], data[1], data[2]);

  if (data[0] == 1)
  {
    if (data[1] == 3 && data[2] == 0)
    {
      if (strcmp(current_tab, "Oscilloscope") == 0)
      {
        int state = gui().oscilloscope_fl_light_button_run_stop->value();
        // printf("osc run btn state: %d\n", !state);

        if (!state)
        {
          presenter ().m_Oscilloscope.stop_gui ();
          presenter ().m_Voltmeter   .stop_gui ();
          presenter ().m_Ohmmeter    .stop_gui ();
          lab       ().m_Oscilloscope.run      ();
          presenter ().m_Oscilloscope.run_gui  ();
        }
        else
        {
          lab       ().m_Oscilloscope.stop     ();
          presenter ().m_Oscilloscope.stop_gui ();
        }
      }

      if (strcmp(current_tab, "Function Generator") == 0)
      {
        int state = gui().function_generator_fl_light_button_run_stop->value();

        if (!state)
        {
          lab().m_Function_Generator.run(0);
          gui().function_generator_fl_light_button_run_stop->set();
        }
        else
        {
          lab().m_Function_Generator.stop(0);
          gui().function_generator_fl_light_button_run_stop->clear();
        }
      }
    }
  }

  if (data[0] == 2)
  {
    auto now = std::chrono::steady_clock::now();

    if (now - last_nav_time >= nav_debounce_delay)
    {
      last_nav_time = now;

      if (data[1] == 1)
        switch_tab_by_direction(+1);
      else if (data[1] == 2)
        switch_tab_by_direction(-1);
    }
  }
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
  gui().main_fl_tabs->value(tab_groups[current_tab_index]);
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
