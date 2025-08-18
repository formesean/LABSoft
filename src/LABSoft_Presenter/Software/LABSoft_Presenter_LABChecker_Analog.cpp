//teacher side
#include "LABSoft_Presenter_LABChecker_Analog.h"

#include <iostream>

#include "../../LAB/LAB.h"
#include "../../LAB/LAB_Oscilloscope.h"
#include "../../LAB/LAB_Function_Generator.h"
#include "../../Utility/LABSoft_GUI_Label_Values.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"

#define LOG(msg) fprintf(stdout, "%s\n", msg)

LABSoft_Presenter_LABChecker_Analog::LABSoft_Presenter_LABChecker_Analog(LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  load_gui();
  init_gui_callbacks();
}

void LABSoft_Presenter_LABChecker_Analog::load_gui()
{
  LABSoft_GUI_Analog_Circuit_Checker_Display& analog_checker_disp_gui = *(gui ().analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);

   analog_checker_disp_gui.load_presenter         (m_presenter);
  
}

void LABSoft_Presenter_LABChecker_Analog::
init_gui_callbacks()
{
  if (gui().analog_fl_button_capture_signal)
  {
    gui().analog_fl_button_capture_signal->callback((Fl_Callback*)cb_capture_signal, this);
  }
}

void LABSoft_Presenter_LABChecker_Analog::
cb_capture_signal(Fl_Button* w,
                   void*     data)
{
  LABSoft_Presenter_LABChecker_Analog* acc = static_cast<LABSoft_Presenter_LABChecker_Analog*>(data);

  if (acc->can_capture_signal())
  {
    std::cout << "Capture Success" << std:: endl;
    acc->capture_oscilloscope_and_function_generator_data();
    
    // Update the GUI with the captured data
    acc->update_gui_with_captured_data();
  }
  else
  {
    std::cout << "Cannot capture signal - requirements not met" << std::endl;
  }
}

bool LABSoft_Presenter_LABChecker_Analog::
can_capture_signal() const
{
  LAB_Oscilloscope& osc = lab().m_Oscilloscope;
  LAB_Function_Generator& fg = lab().m_Function_Generator;

  // if (osc.mode() != LABE::OSC::MODE::RECORD)
  // {
  //   std::cout << "ERROR: Oscilloscope not in RECORD mode" << std::endl;
  //   return false;
  // }  

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

void LABSoft_Presenter_LABChecker_Analog::
capture_oscilloscope_and_function_generator_data()
{
  LAB_Oscilloscope& osc = lab().m_Oscilloscope;
  LAB_Function_Generator& fg = lab().m_Function_Generator;

  const auto& ch0_samples = osc.chan_samples(0);
  const auto& ch1_samples = osc.chan_samples(1);

  LOG("====================");
  std::cout << "SUCCESS: Signal captured successfully" << std::endl;
  LOG("Oscilloscope Data:");
  std::cout << "Channel 1 samples: "                  << ch0_samples.size() << std::endl;
  std::cout << "Channel 2 samples: "                  << ch1_samples.size() << std::endl;
  std::cout << "Channel 1 Coupling:"                  << (int)osc.coupling(0) << std::endl;
  std::cout << "Channel 2: Coupling"                  << (int)osc.coupling(1) << std::endl;
  std::cout << "Channel 1 Scaling: "                  << (int)osc.scaling(0) << std::endl;
  std::cout << "Channel 2 Scaling: "                  << (int)osc.scaling(1) << std::endl;
  std::cout << "Channel 1 Voltage per division: "     << osc.voltage_per_division(0) << std::endl;
  std::cout << "Channel 2 Voltage per division: "     << osc.voltage_per_division(1) << std::endl;
  std::cout << "Channel 1 Vertical offset: "          << osc.vertical_offset(0) << std::endl;
  std::cout << "Channel 2 Vertical offset: "          << osc.vertical_offset(1) << std::endl;
  std::cout << "Oscilloscope mode: "                  << (int)osc.mode() << std::endl;
  std::cout << "Horizontal offset: "                  << osc.horizontal_offset() << std::endl;
  std::cout << "Time per division: "                  << osc.time_per_division() << std::endl;
  std::cout << "Samples: "                            << osc.samples() << std::endl;
  std::cout << "Sampling rate: "                      << osc.sampling_rate() << std::endl;
  std::cout << "Trigger mode: "                       << (int)osc.trigger_mode() << std::endl;
  std::cout << "Trigger source: "                     << osc.trigger_source() << std::endl;
  std::cout << "Trigger type: "                       << (int)osc.trigger_type() << std::endl;
  std::cout << "Trigger condition: "                  << (int)osc.trigger_condition() << std::endl;
  std::cout << "Trigger level: "                      << osc.trigger_level() << std::endl;

  LOG("Function Generator Data:");
  LOG("Channel 1:");
  std::cout << "Wave type: "                          << (int)fg.wave_type(0) << std::endl;
  std::cout << "Frequency: "                          << fg.frequency(0) << " Hz" << std::endl;
  std::cout << "Period: "                             << fg.period(0) << " s" << std::endl;

  LOG("Channel 2:");
  std::cout << "Wave type: " << (int)fg.wave_type(1) << std::endl;
  std::cout << "Frequency: " << fg.frequency(1) << " Hz" << std::endl;
  std::cout << "Period: " << fg.period(1) << " s" << std::endl;
  
}

void LABSoft_Presenter_LABChecker_Analog::
update_gui_with_captured_data()
{
  LAB_Oscilloscope& osc = lab().m_Oscilloscope;
  osc.update_data_samples();
  
  LABSoft_GUI_Analog_Circuit_Checker_Display& analog_checker_disp_gui = 
    *(gui().analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);
  
  analog_checker_disp_gui.voltage_per_division(0, osc.voltage_per_division(0));
  analog_checker_disp_gui.voltage_per_division(1, osc.voltage_per_division(1));
  analog_checker_disp_gui.time_per_division(osc.time_per_division());
  analog_checker_disp_gui.samples(osc.samples());
  analog_checker_disp_gui.sampling_rate(osc.sampling_rate());
  
  analog_checker_disp_gui.update_display();
  
  std::cout << "GUI updated with captured oscilloscope data" << std::endl;
}