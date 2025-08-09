#include "LABSoft_Presenter_Analog_Circuit_Checker.h"

#include <iostream>

#include "../../LAB/LAB.h"
#include "../../LAB/LAB_Oscilloscope.h"
#include "../../LAB/LAB_Function_Generator.h"
#include "../../Utility/LABSoft_GUI_Label_Values.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"

LABSoft_Presenter_Analog_Circuit_Checker::LABSoft_Presenter_Analog_Circuit_Checker(LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  load_gui();
  init_gui_callbacks();
}

void LABSoft_Presenter_Analog_Circuit_Checker::load_gui()
{
  LABSoft_GUI_Analog_Circuit_Checker_Display& analog_checker_disp_gui = *(gui ().analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);

   analog_checker_disp_gui.load_presenter         (m_presenter);
  
}

void LABSoft_Presenter_Analog_Circuit_Checker::
init_gui_callbacks()
{
  if (gui().analog_fl_button_capture_signal)
  {
    gui().analog_fl_button_capture_signal->callback((Fl_Callback*)cb_capture_signal, this);
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_capture_signal(Fl_Button* w,
                   void*     data)
{
  LABSoft_Presenter_Analog_Circuit_Checker* acc = static_cast<LABSoft_Presenter_Analog_Circuit_Checker*>(data);

  if (acc->can_capture_signal())
  {
    std::cout << "Capture Success" << std:: endl;
    acc->capture_oscilloscope_and_function_generator_data();
  }
  else
  {
    std::cout << "Cannot capture signal - requirements not met" << std::endl;
  }
}

bool LABSoft_Presenter_Analog_Circuit_Checker::
can_capture_signal() const
{
  LAB_Oscilloscope& osc = lab().m_Oscilloscope;
  LAB_Function_Generator& fg = lab().m_Function_Generator;

  if (osc.mode() != LABE::OSC::MODE::RECORD)
  {
    std::cout << "ERROR: Oscilloscope not in RECORD mode" << std::endl;
    return false;
  }  

  if (!osc.is_frontend_running())
  {
    std::cout << "ERROR: Oscilloscope is not running" << std::endl;
    return false;
  }

  if (!fg.is_running())
  {
    std::cout <<"ERROR: Function Generator is not running" << std::endl;
    return false;
  }

  return true;
}

void LABSoft_Presenter_Analog_Circuit_Checker::
capture_oscilloscope_and_function_generator_data()
{
  LAB_Oscilloscope& osc = lab().m_Oscilloscope;
  LAB_Function_Generator& fg = lab().m_Function_Generator;

  const auto& ch0_samples = osc.chan_samples(0);
  const auto& ch1_samples = osc.chan_samples(1);

  std::cout << "SUCCESS: Signal captured successfully" << std::endl;
  std::cout << "Channel 0 samples: " << ch0_samples.size() << std::endl;
  std::cout << "Channel 1 samples: " << ch1_samples.size() << std::endl;
  std::cout << "Oscilloscope mode: " << (int)osc.mode() << std::endl;
  std::cout << "Horizontal offset: " << osc.horizontal_offset() << std::endl;
  std::cout << "Time per division: " << osc.time_per_division() << std::endl;
  std::cout << "Samples: " << osc.samples() << std::endl;
  std::cout << "Sampling rate: " << osc.sampling_rate() << std::endl;
  std::cout << "Trigger mode: " << (int)osc.trigger_mode() << std::endl;
  // std::cout << "Wave type: " << (int)fg.wave_type() << std::endl;
  // std::cout << "Frequency: " << fg.frequency() << " Hz" << std::endl;
  // std::cout << "Period: " << fg.period() << " s" << std::endl;
}