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
  LAB_Oscilloscope_Display&           osc_disp      = lab ().m_Oscilloscope_Display;
  LABSoft_GUI_LABChecker_Analog_Checker_Display& analog_checker_disp_gui = *(gui ().analog_labsoft_gui_analog_checker_display);

   analog_checker_disp_gui.load_presenter         (m_presenter);
   analog_checker_disp_gui.load_pixel_points              (osc_disp.pixel_points ());
   analog_checker_disp_gui.update_display       ();

}

void LABSoft_Presenter_LABChecker_Analog::
update_display ()
{
  LAB_Oscilloscope_Display&           osc_disp      = lab ().m_Oscilloscope_Display;
  LABSoft_GUI_LABChecker_Analog_Checker_Display&   analog_checker_disp_gui  = *(gui ().analog_labsoft_gui_analog_checker_display);

  osc_disp       .update_pixel_points  ();
  analog_checker_disp_gui.update_display       ();
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
  auto& osc_data = lab().m_Oscilloscope.parent_data();

  LOG("====================");
  std::cout << "SUCCESS: Signal captured successfully" << std::endl;

  LOG("Oscilloscope Data:");
  for (int ch = 0; ch < 2; ++ch)
  {
    const auto& ch_data = osc_data.channel_data[ch];

    std::cout << "Channel " << (ch+1) << " samples: "          << ch_data.samples.size()      << '\n'
              << "Channel " << (ch+1) << " Coupling: "         << (int)ch_data.coupling       << '\n'
              << "Channel " << (ch+1) << " Scaling: "          << (int)ch_data.scaling        << '\n'
              << "Channel " << (ch+1) << " Voltage/div: "      << ch_data.voltage_per_division<< '\n'
              << "Channel " << (ch+1) << " Vertical offset: "  << ch_data.vertical_offset     << '\n';

    std::cout << "  is_enabled: "         << ch_data.is_enabled           << '\n'
              << "  scaling_corrector: "  << ch_data.scaling_corrector    << '\n'
              << "  measurements: "
              << "min=" << ch_data.measurements.min
              << ", max=" << ch_data.measurements.max
              << ", avg=" << ch_data.measurements.avg
              << ", trms="<< ch_data.measurements.trms
              << '\n';

  for (int ch = 0; ch < 2; ++ch) {
    std::cout << "[DEBUG] Pixel points (channel " << ch << ", first 10): ";
    for (size_t i = 0; i < std::min<size_t>(10, raw_buf[ch].size()); i++) {
        std::cout << "(" << raw_buf[ch][i][0] << "," << raw_buf[ch][i][1] << ") ";
    }
    std::cout << std::endl;
    }
    
  }

  std::cout << "Oscilloscope mode: " << (int)osc.mode()              << '\n'
            << "Horizontal offset: " << osc.horizontal_offset()      << '\n'
            << "Time per division: " << osc.time_per_division()      << '\n'
            << "Samples: "           << osc.samples()                << '\n'
            << "Sampling rate: "     << osc.sampling_rate()          << '\n'
            << "Trigger mode: "      << (int)osc.trigger_mode()      << '\n'
            << "Trigger source: "    << osc.trigger_source()         << '\n'
            << "Trigger type: "      << (int)osc.trigger_type()      << '\n'
            << "Trigger condition: " << (int)osc.trigger_condition() << '\n'
            << "Trigger level: "     << osc.trigger_level()          << '\n';

  LOG("Function Generator Data:");
  LOG("Channel 1:");
  std::cout << "Wave type: "                          << (int)fg.wave_type(0)          << '\n'
            << "Frequency: "                          << fg.frequency(0)      << " Hz" << '\n'
            << "Period: "                             << fg.period(0)         << " s"  << '\n';

  LOG("====================\n");
}

void LABSoft_Presenter_LABChecker_Analog::
update_gui_with_captured_data()
{
  LAB_Oscilloscope& osc = lab().m_Oscilloscope;
  LAB_Oscilloscope_Display& osc_disp = lab().m_Oscilloscope_Display;
  osc.update_data_samples();

  auto& raw_buf = osc_disp.pixel_points();

  std::cout << "[DEBUG] Pixel points (channel 0, first 10): ";
  for (size_t i = 0; i < std::min<size_t>(10, raw_buf[0].size()); i++) {
    std::cout << "(" << raw_buf[0][i][0] << "," << raw_buf[0][i][1] << ") ";
}
std::cout << std::endl;

  LABSoft_GUI_LABChecker_Analog_Checker_Display& analog_checker_disp_gui =
    *(gui ().analog_labsoft_gui_analog_checker_display);

  analog_checker_disp_gui.voltage_per_division(0, osc.voltage_per_division(0));
  analog_checker_disp_gui.voltage_per_division(1, osc.voltage_per_division(1));
  analog_checker_disp_gui.time_per_division(osc.time_per_division());
  analog_checker_disp_gui.samples(osc.samples());
  analog_checker_disp_gui.sampling_rate(osc.sampling_rate());

  osc_disp.update_pixel_points(); // refresh waveform

  auto& raw_buf = osc_disp.pixel_points();
  
  analog_checker_disp_gui.load_pixel_points(osc_disp.pixel_points());

  analog_checker_disp_gui.update_display();

  std::cout << "GUI updated with captured oscilloscope data" << std::endl;
}

/*
void LABSoft_Presenter_LABChecker_Analog::create_file()
{

  if (!can_capture_signal())
  {
    std::cout << "ERROR: Cannot create file - capture signal requirements not met" << std::endl;
    return;
  }

  std::string file_name("analog_checker.labacc");

  Fl_Native_File_Chooser chooser;

  chooser.title("Save Analog Circuit Checker File...");
  chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  chooser.directory("/");
  chooser.preset_file(file_name.c_str());
  chooser.options(Fl_Native_File_Chooser::NEW_FOLDER |
                  Fl_Native_File_Chooser::SAVEAS_CONFIRM);

  switch (chooser.show())
  {
    case 1: // cancel
    {
      std::cout << "File save cancelled by user" << std::endl;
      break;
    }

    default:
    {
      try
      {
        // Get the captured data
        LAB_Oscilloscope& osc = lab().m_Oscilloscope;
        LAB_Function_Generator& fg = lab().m_Function_Generator;

        // Call the LAB component to create the file
        // This assumes you have an analog checker component in your LAB class
        // that can handle oscilloscope and function generator data

        m_presenter.lab().m_LABChecker_Analog.create_file(
          osc, fg, chooser.filename());

        std::cout << "Analog circuit checker file saved successfully: "
                  << chooser.filename() << std::endl;
      }
      catch (const std::exception& err)
      {
        std::cout << "ERROR: Failed to save file - " << err.what() << std::endl;
      }

      break;
    }
  }
}*/



/*LABSoft_Presenter_LABChecker_Analog_Display::
LABSoft_Presenter_LABChecker_Analog_Display (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{
  load_gui ();
}

void LABSoft_Presenter_LABChecker_Analog_Display::
load_gui ()
{
  LAB_Oscilloscope&                                   osc              = lab ().m_Oscilloscope;
  LAB_Oscilloscope_Display&                           osc_disp         = lab ().m_Oscilloscope_Display;
  LABSoft_GUI_LABChecker_Analog_Checker_Display&      analog_disp_gui  = *(gui ().analog_labsoft_gui_analog_checker_display);

  analog_disp_gui.channel_enable_disable (0, osc.is_channel_enabled    (0));
  analog_disp_gui.channel_enable_disable (0, osc.is_channel_enabled    (1));
  analog_disp_gui.voltage_per_division   (0, osc.voltage_per_division  (0));
  analog_disp_gui.voltage_per_division   (1, osc.voltage_per_division  (1));
  analog_disp_gui.vertical_offset        (1, osc.vertical_offset       (1));
  analog_disp_gui.vertical_offset        (1, osc.vertical_offset       (1));
  analog_disp_gui.horizontal_offset      (osc.horizontal_offset        ( ));
  analog_disp_gui.time_per_division      (osc.time_per_division        ( ));
  analog_disp_gui.load_presenter         (m_presenter);
  analog_disp_gui.load_pixel_points      (osc_disp.pixel_points ());

  osc_disp.display_parameters (
    analog_disp_gui.display_width (),
    analog_disp_gui.display_height (),
    LABC::OSC_DISPLAY::NUMBER_OF_ROWS,
    LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS
  );
}

void LABSoft_Presenter_LABChecker_Analog_Display::
cb_horizontal_offset (double value)
{

}

void LABSoft_Presenter_LABChecker_Analog_Display::
cb_mouse_wheel (int direction) const
{

}

void LABSoft_Presenter_LABChecker_Analog_Display::
update_display ()
{
  LAB_Oscilloscope_Display&           osc_disp      = lab ().m_Oscilloscope_Display;
  LABSoft_GUI_LABChecker_Analog_Checker_Display&   analog_disp_gui  = *(gui ().analog_labsoft_gui_analog_checker_display);

  osc_disp       .update_pixel_points  ();
  analog_disp_gui.update_display       ();
}*/
