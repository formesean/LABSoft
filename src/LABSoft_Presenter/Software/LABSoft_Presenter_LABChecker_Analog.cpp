// teacher side
#include "LABSoft_Presenter_LABChecker_Analog.h"

#include <iostream>
#include <fstream>

#include "../../LAB/LAB.h"
#include "../../LAB/LAB_Oscilloscope.h"
#include "../../LAB/LAB_Function_Generator.h"
#include "../../Utility/LABSoft_GUI_Label_Values.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"

#define LOG(msg) fprintf(stdout, "%s\n", msg)

LABSoft_Presenter_LABChecker_Analog::LABSoft_Presenter_LABChecker_Analog(LABSoft_Presenter &_LABSoft_Presenter)
    : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  load_gui();
  init_gui_callbacks();
}

void LABSoft_Presenter_LABChecker_Analog::
load_gui()
{
  LAB_Oscilloscope_Display &osc_disp = lab().m_Oscilloscope_Display;
  LABSoft_GUI_LABChecker_Analog_Checker_Display &analog_checker_disp_gui = *(gui().analog_labsoft_gui_analog_checker_display);

  analog_checker_disp_gui.load_presenter(m_presenter);

  osc_disp.update_pixel_points();
  auto &raw_buf = osc_disp.pixel_points();

  if (raw_buf.size() < 2 || raw_buf[1].empty()) return;

  std::array<std::vector<std::array<int, 2>>, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> channel2_only_pixel_points;
  channel2_only_pixel_points[0] = raw_buf[1]; // Channel 2 data to display channel 0
  channel2_only_pixel_points[1] = raw_buf[1]; // Channel 2 data to display channel 1

  analog_checker_disp_gui.load_pixel_points(channel2_only_pixel_points);
  analog_checker_disp_gui.update_display();
}

void LABSoft_Presenter_LABChecker_Analog::
update_display()
{
  LAB_Oscilloscope_Display &osc_disp = lab().m_Oscilloscope_Display;
  LABSoft_GUI_LABChecker_Analog_Checker_Display &analog_checker_disp_gui = *(gui().analog_labsoft_gui_analog_checker_display);

  // Update with channel 2 data only
  osc_disp.update_pixel_points();
  auto &raw_buf = osc_disp.pixel_points();
  std::array<std::vector<std::array<int, 2>>, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> channel2_only_pixel_points;
  channel2_only_pixel_points[0] = raw_buf[1]; // Channel 2 data to display channel 0
  channel2_only_pixel_points[1] = raw_buf[1]; // Channel 2 data to display channel 1

  analog_checker_disp_gui.load_pixel_points(channel2_only_pixel_points);
  analog_checker_disp_gui.enable_channels(true);
  analog_checker_disp_gui.update_display();
}

void LABSoft_Presenter_LABChecker_Analog::
init_gui_callbacks()
{
  if (gui().analog_fl_button_capture_signal)
  {
    gui().analog_fl_button_capture_signal->callback((Fl_Callback *)cb_capture_signal, this);
  }
}

void LABSoft_Presenter_LABChecker_Analog::
cb_capture_signal(Fl_Button *w,
                  void *data)
{
  LABSoft_Presenter_LABChecker_Analog *acc = static_cast<LABSoft_Presenter_LABChecker_Analog *>(data);

  if (acc->can_capture_signal())
  {
    acc->capture_oscilloscope_and_function_generator_data();
    acc->update_gui_with_captured_data();
  }
}

bool LABSoft_Presenter_LABChecker_Analog::
can_capture_signal() const
{
  LAB_Oscilloscope &osc = lab().m_Oscilloscope;
  LAB_Function_Generator &fg = lab().m_Function_Generator;

  if (!osc.is_frontend_running()) return false;

  if (!fg.is_running()) return false;

  return true;
}

void LABSoft_Presenter_LABChecker_Analog::
capture_oscilloscope_and_function_generator_data()
{
  LAB_Oscilloscope &osc = lab().m_Oscilloscope;
  LAB_Function_Generator &fg = lab().m_Function_Generator;
  LAB_Oscilloscope_Display &osc_disp = lab().m_Oscilloscope_Display;
  auto &osc_data = lab().m_Oscilloscope.parent_data();

  osc_disp.update_pixel_points();

  LOG("====================");
  std::cout << "SUCCESS: Signal captured successfully" << std::endl;
  std::cout << "NOTE: Only Channel 2 data will be displayed on the LABChecker Analog tab" << std::endl;

  LOG("Oscilloscope Data:");
  auto &raw_buf = osc_disp.pixel_points();

  for (int ch = 0; ch < 2; ++ch)
  {
    const auto &ch_data = osc_data.channel_data[ch];

    std::cout << "Channel " << (ch + 1) << " samples: " << ch_data.samples.size() << '\n'
              << "Channel " << (ch + 1) << " Coupling: " << (int)ch_data.coupling << '\n'
              << "Channel " << (ch + 1) << " Scaling: " << (int)ch_data.scaling << '\n'
              << "Channel " << (ch + 1) << " Voltage/div: " << ch_data.voltage_per_division << '\n'
              << "Channel " << (ch + 1) << " Vertical offset: " << ch_data.vertical_offset << '\n';

    std::cout << "  is_enabled: " << ch_data.is_enabled << '\n'
              << "  scaling_corrector: " << ch_data.scaling_corrector << '\n'
              << "  measurements: "
              << "min=" << ch_data.measurements.min
              << ", max=" << ch_data.measurements.max
              << ", avg=" << ch_data.measurements.avg
              << ", trms=" << ch_data.measurements.trms
              << '\n';

    std::cout << "[DEBUG] Pixel points (channel " << ch << ", first 10): ";
    for (size_t i = 0; i < std::min<size_t>(10, raw_buf[ch].size()); i++)
    {
      std::cout << "(" << raw_buf[ch][i][0] << "," << raw_buf[ch][i][1] << ") ";
    }
    std::cout << std::endl;
  }

  // Highlight channel 2 data specifically
  std::cout << "\n[INFO] Channel 2 data (index 1) will be displayed on LABChecker Analog tab:" << std::endl;
  const auto &ch2_data = osc_data.channel_data[1];
  std::cout << "  Channel 2 samples: " << ch2_data.samples.size() << std::endl;
  std::cout << "  Channel 2 voltage/div: " << ch2_data.voltage_per_division << std::endl;
  std::cout << "  Channel 2 measurements - min: " << ch2_data.measurements.min
            << ", max: " << ch2_data.measurements.max
            << ", avg: " << ch2_data.measurements.avg
            << ", trms: " << ch2_data.measurements.trms << std::endl;

  std::cout << "Oscilloscope mode: " << (int)osc.mode() << '\n'
            << "Horizontal offset: " << osc.horizontal_offset() << '\n'
            << "Time per division: " << osc.time_per_division() << '\n'
            << "Samples: " << osc.samples() << '\n'
            << "Sampling rate: " << osc.sampling_rate() << '\n'
            << "Trigger mode: " << (int)osc.trigger_mode() << '\n'
            << "Trigger source: " << osc.trigger_source() << '\n'
            << "Trigger type: " << (int)osc.trigger_type() << '\n'
            << "Trigger condition: " << (int)osc.trigger_condition() << '\n'
            << "Trigger level: " << osc.trigger_level() << '\n';

  LOG("Function Generator Data:");
  LOG("Channel 1:");
  std::cout << "Wave type: " << (int)fg.wave_type(0) << '\n'
            << "Frequency: " << fg.frequency(0) << " Hz" << '\n'
            << "Period: " << fg.period(0) << " s" << '\n';

  LOG("====================\n");
}

void LABSoft_Presenter_LABChecker_Analog::
update_gui_with_captured_data()
{
  LAB_Oscilloscope &osc = lab().m_Oscilloscope;
  LAB_Oscilloscope_Display &osc_disp = lab().m_Oscilloscope_Display;
  osc.update_data_samples();

  LABSoft_GUI_LABChecker_Analog_Checker_Display &analog_checker_disp_gui =
      *(gui().analog_labsoft_gui_analog_checker_display);

  if (!gui().analog_labsoft_gui_analog_checker_display) return;

  analog_checker_disp_gui.voltage_per_division(0, osc.voltage_per_division(1));
  analog_checker_disp_gui.voltage_per_division(1, osc.voltage_per_division(1));
  analog_checker_disp_gui.vertical_offset(0, osc.vertical_offset(1));
  analog_checker_disp_gui.vertical_offset(1, osc.vertical_offset(1));
  analog_checker_disp_gui.time_per_division(osc.time_per_division());
  analog_checker_disp_gui.horizontal_offset(osc.horizontal_offset());
  analog_checker_disp_gui.samples(osc.samples());
  analog_checker_disp_gui.sampling_rate(osc.sampling_rate());

  unsigned analog_w = analog_checker_disp_gui.display_width();
  unsigned analog_h = analog_checker_disp_gui.display_height();

  LABSoft_GUI_Oscilloscope_Display &osc_gui_disp = *(gui().oscilloscope_labsoft_gui_oscilloscope_display);
  unsigned osc_tab_w = osc_gui_disp.display_width();
  unsigned osc_tab_h = osc_gui_disp.display_height();

  osc_disp.display_parameters(
    analog_w,
    analog_h,
    LABC::OSC_DISPLAY::NUMBER_OF_ROWS,
    LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS
  );

  osc_disp.update_pixel_points();
  auto &raw_buf = osc_disp.pixel_points();

  if (raw_buf.size() < 2 || raw_buf[1].empty())
  {
    osc_disp.display_parameters(
      osc_tab_w,
      osc_tab_h,
      LABC::OSC_DISPLAY::NUMBER_OF_ROWS,
      LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS
    );
    osc_disp.update_pixel_points();
    return;
  }

  std::array<std::vector<std::array<int, 2>>, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> channel2_only_pixel_points;
  channel2_only_pixel_points[0] = raw_buf[1];
  channel2_only_pixel_points[1] = raw_buf[1];

  for (auto& ch_pts : channel2_only_pixel_points)
  {
    for (auto& p : ch_pts)
    {
      p[0] = static_cast<int>(analog_w) - 1 - p[0];
    }
  }

  analog_checker_disp_gui.load_pixel_points(channel2_only_pixel_points);
  analog_checker_disp_gui.enable_channels(true);
  analog_checker_disp_gui.update_display();

  osc_disp.display_parameters(
    osc_tab_w,
    osc_tab_h,
    LABC::OSC_DISPLAY::NUMBER_OF_ROWS,
    LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS
  );
  osc_disp.update_pixel_points();
}

void LABSoft_Presenter_LABChecker_Analog::
cb_analog_create_file(Fl_Button *w, void *data)
{
  auto *acc = this;
  if (!acc) return;

  // Step 1: Check GUI
  auto gui_ptr = acc->gui().analog_labsoft_gui_analog_checker_display;
  if (!gui_ptr) return;

  // Step 2: Check oscilloscope parent data
  auto &parent_data = acc->lab().m_Oscilloscope.parent_data();
  if (parent_data.channel_data.empty()) return;

  // Grab only the first channel for now
  const auto &src = parent_data.channel_data.front();

  LAB_Channel_Data_Oscilloscope osc_data;
  osc_data.is_enabled = src.is_enabled;
  osc_data.scaling_corrector = src.scaling_corrector;
  osc_data.voltage_per_division = src.voltage_per_division;
  osc_data.vertical_offset = src.vertical_offset;
  osc_data.scaling = src.scaling;
  osc_data.coupling = src.coupling;

  std::copy(src.samples.begin(), src.samples.end(), osc_data.samples.begin());

  osc_data.calibration = src.calibration;
  osc_data.measurements = src.measurements;

  LAB_Parent_Data_Oscilloscope osc_parent;
  osc_parent.mode = parent_data.mode;
  osc_parent.samples = parent_data.samples;
  osc_parent.sampling_rate = parent_data.sampling_rate;
  osc_parent.time_per_division = parent_data.time_per_division;
  osc_parent.horizontal_offset = parent_data.horizontal_offset;
  osc_parent.trigger_mode = parent_data.trigger_mode;

  LAB_Channel_Data_Function_Generator fg_data;
  auto &fg = acc->lab().m_Function_Generator;
  fg_data.is_enabled = fg.is_running();
  fg_data.wave_type = fg.wave_type(0);
  fg_data.frequency = fg.frequency(0);
  fg_data.period = fg.period(0);
  fg_data.amplitude = fg.amplitude(0);
  fg_data.vertical_offset = fg.vertical_offset(0);
  fg_data.phase = fg.phase(0);

  // Build XML
  auto ss = acc->lab().m_LABChecker_Analog.create_circuit_checker_xml_string_with_data(osc_data, osc_parent, fg_data);

  // Save dialog
  Fl_Native_File_Chooser chooser;
  chooser.title("Save Oscilloscope Data...");
  chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  chooser.preset_file("osc_data.labacc");
  if (chooser.show() != 0) return;
  std::string file_path = chooser.filename();

  std::ofstream ofs(file_path);
  if (!ofs) return;

  ofs << ss.str();
  ofs.close();

  std::cout << "[SUCCESS] File saved to: " << file_path << std::endl;
}

// EOF
