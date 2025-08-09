#include "LABSoft_Presenter_Analog_Circuit_Checker.h"

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"

LABSoft_Presenter_Analog_Circuit_Checker::LABSoft_Presenter_Analog_Circuit_Checker(LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  load_gui();
}

void LABSoft_Presenter_Analog_Circuit_Checker::load_gui()
{
  LABSoft_GUI_Analog_Circuit_Checker_Display& analog_checker_disp_gui = *(gui ().analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);

   analog_checker_disp_gui.load_presenter         (m_presenter);
  
}