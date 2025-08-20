#include "LABSoft_Presenter.h"

#include <FL/Fl.H>
#include <FL/Fl_Tabs.H>

#include "../LAB/LAB.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
// #include "../Utility/LAB_Constants.h"

LABSoft_Presenter::
LABSoft_Presenter (LAB& _LAB, LABSoft_GUI& _LABSoft_GUI)
  : m_LAB                     (_LAB),
    m_LABSoft_GUI             (_LABSoft_GUI),
    m_Oscilloscope            (*this),
    m_Voltmeter               (*this),
    m_Ohmmeter                (*this),
    m_Function_Generator      (*this),
    m_Logic_Analyzer          (*this),
    m_Digital_Circuit_Checker (*this),
    m_Analog_Circuit_Checker (*this),
    m_Main_Window             (*this),
    m_LABChecker_Digital      (*this),
    m_Exporter                (*this),
    m_Calibration             (*this),
    m_Oscilloscope_Display    (*this),
    m_Software_Navigation     (*this),
    m_Shortcuts               (*this),
    m_LABChecker_Analog       (*this)
{
  load_presenter_to_gui ();

  Fl::add_timeout (LABC::LABSOFT::DISPLAY_UPDATE_RATE, update_display, this);
}

void LABSoft_Presenter::
load_presenter_to_gui ()
{
  m_LABSoft_GUI.m_LABSoft_Presenter = this;

  m_LABSoft_GUI.logic_analyzer_labsoft_gui_logic_analyzer_display->
    load_presenter (*this);
}

void LABSoft_Presenter::
update_gui_tab_colors ()
{
  // 63 is green
  // 52 is inactive tab gray
  // 54 is active tab gray

  static LABSoft_GUI& gui   = m_LABSoft_GUI;
  static Fl_Tabs&     tabs  = *(gui.main_fl_tabs);

  // 1. oscilloscope
  gui.main_fl_group_oscilloscope_tab->selection_color (
    m_LAB.m_Oscilloscope.is_frontend_running () ? 63 : 52
  );

  // 2. voltmeter
  gui.main_fl_group_voltmeter_tab->selection_color (
    m_LAB.m_Voltmeter.is_frontend_running () ? 63 : 52
  );

  gui.main_fl_group_ohmmeter_tab->selection_color (
    m_LAB.m_Ohmmeter.is_frontend_running () ? 63 : 52
  );

  // 3. function generator
  gui.main_fl_group_function_generator_tab->selection_color (
    m_LAB.m_Function_Generator.is_running () ? 63 : 52
  );

  // 4. logic analyzer
  gui.main_fl_group_logic_analyzer_tab->selection_color (
    m_LAB.m_Logic_Analyzer.is_running () ? 63 : 52
  );

  // 5. digital circuit checker
  gui.main_fl_group_digital_circuit_checker_tab->selection_color (
    m_LAB.m_Digital_Circuit_Checker.is_running () ? 63 : 52
  );

  tabs.selection_color  (tabs.value ()->selection_color () == 63 ? 63 : 54);
  tabs.redraw           ();
}

void LABSoft_Presenter::
update_display (void *data)
{
  LABSoft_Presenter& presenter = *(reinterpret_cast<LABSoft_Presenter*>(data));

  presenter.update_gui_tab_colors ();

  presenter.m_Oscilloscope       .display_update_cycle ();
  presenter.m_Voltmeter          .display_update_cycle ();
  presenter.m_Ohmmeter           .display_update_cycle ();
  presenter.m_Logic_Analyzer     .display_update_cycle ();
  presenter.m_Software_Navigation.update_data_cycle    ();

  Fl::repeat_timeout (LABC::LABSOFT::DISPLAY_UPDATE_RATE, update_display, data);
}

void LABSoft_Presenter::
cb_tabs (Fl_Group* w, void* data)
{
  update_gui_tab_colors ();
}

LAB& LABSoft_Presenter::
lab () const
{
  return (m_LAB);
}

LABSoft_GUI& LABSoft_Presenter::
gui () const
{
  return (m_LABSoft_GUI);
}
