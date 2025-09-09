// student side
#include "LABSoft_Presenter_Analog_Circuit_Checker.h"

#include <cstdio>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <array>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu_Button.H>

#include <FL/fl_ask.H>

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"
#include "../../LABSoft_GUI/LABSoft_GUI_Analog_Circuit_Checker_Display.h"
#include "../../Utility/LAB_Utility_Functions.h"

enum class UnitKind { VOLT, SECOND, HERTZ, SAMPLES };

static inline double clamp01 (double v) { return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v); }

static double parse_number_prefix (const char* s)
{
  char* endptr = nullptr;
  double v = std::strtod(s, &endptr);
  if (endptr == s) return std::numeric_limits<double>::quiet_NaN();
  return v;
}

static double parse_labeled_value (const char* label, UnitKind kind)
{
  if (!label) return std::numeric_limits<double>::quiet_NaN();
  std::string low(label);
  for (char& ch : low) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

  double base = parse_number_prefix(low.c_str());
  if (!std::isfinite(base)) return base;

  double mul = 1.0;
  switch (kind)
  {
    case UnitKind::VOLT:
    {
      if (low.find("kv") != std::string::npos) mul = 1e3;
      else if (low.find("mv") != std::string::npos) mul = 1e-3;
      else if (low.find("uv") != std::string::npos) mul = 1e-6;
      else mul = 1.0; // V
      break;
    }
    case UnitKind::SECOND:
    {
      if (low.find("ms") != std::string::npos) mul = 1e-3;
      else if (low.find("us") != std::string::npos) mul = 1e-6;
      else mul = 1.0; // s
      break;
    }
    case UnitKind::HERTZ:
    {
      if (low.find("mhz") != std::string::npos) mul = 1e6;
      else if (low.find("khz") != std::string::npos) mul = 1e3;
      else mul = 1.0; // Hz
      break;
    }
    case UnitKind::SAMPLES:
    default:
      if (low.find('k') != std::string::npos) mul = 1e3; else mul = 1.0;
      break;
  }
  return base * mul;
}

static int find_best_menu_index_for_value (const Fl_Menu_Item* menu, UnitKind kind, double target)
{
  if (!menu) return -1;
  int best_index = -1;
  double best_err = std::numeric_limits<double>::infinity();
  for (int i = 0; menu[i].label(); ++i)
  {
    const char* lbl = menu[i].label();
    const double v = parse_labeled_value(lbl, kind);
    if (!std::isfinite(v)) continue;
    double err = std::fabs(v - target);
    if (err < best_err)
    {
      best_err = err;
      best_index = i;
    }
  }
  return best_index;
}

static void set_input_choice_to_value (LABSoft_GUI_Fl_Input_Choice_With_Scroll* w, UnitKind kind, double value)
{
  if (!w) return;
  Fl_Menu_Button* mb = w->menubutton();
  if (!mb) { return; }
  const Fl_Menu_Item* menu = mb->menu();
  if (!menu) { return; }
  const int idx = find_best_menu_index_for_value(menu, kind, value);
  if (idx >= 0)
  {
    mb->value(idx);
    if (w->input())
    {
      if (kind == UnitKind::SAMPLES)
      {
        long long integer_value = static_cast<long long>(std::llround(value));
        if (integer_value >= 1000 && (integer_value % 1000) == 0)
        {
          long long thousands = integer_value / 1000;
          char buf[32];
          std::snprintf(buf, sizeof(buf), "%lld k", thousands);
          w->input()->value(buf);
        }
        else
        {
          w->input()->value(menu[idx].label());
        }
      }
      else
      {
        w->input()->value(menu[idx].label());
      }
    }
  }
}

static int menu_length (const Fl_Menu_Item* menu)
{
  if (!menu) return 0;
  int n = 0; while (menu[n].label()) ++n; return n;
}

static void set_choice_index (LABSoft_GUI_Fl_Choice_With_Scroll* w, int desired_index)
{
  if (!w) return;
  const Fl_Menu_Item* menu = w->menu();
  const int len = menu_length(menu);
  if (len <= 0) return;
  int idx = desired_index;
  if (idx < 0) idx = 0; if (idx >= len) idx = len - 1;
  w->value(idx);
}

LABSoft_Presenter_Analog_Circuit_Checker::LABSoft_Presenter_Analog_Circuit_Checker(LABSoft_Presenter &_LABSoft_Presenter)
    : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  load_gui();
}

void LABSoft_Presenter_Analog_Circuit_Checker::
load_gui()
{
  LABSoft_GUI_Analog_Circuit_Checker_Display &analog_disp_gui = *(gui().analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);

  analog_disp_gui.load_presenter(m_presenter);
}

void LABSoft_Presenter_Analog_Circuit_Checker::
import_metadata()
{
  const auto& analog_checker = m_presenter.lab().m_Analog_Circuit_Checker;
  const auto& channel_data   = analog_checker.get_channel_data();
  const auto& func_gen_data  = analog_checker.get_function_generator_data();

  // Persist non-signal metadata
  m_metadata.time_per_division = analog_checker.get_time_per_division();
  m_metadata.samples           = analog_checker.get_samples();
  m_metadata.sampling_rate     = analog_checker.get_sampling_rate();
  m_metadata.horizontal_offset = analog_checker.get_horizontal_offset();

  // Trigger
  m_metadata.trigger_mode       = analog_checker.get_trigger_mode();
  m_metadata.trigger_source     = analog_checker.get_trigger_source();
  m_metadata.trigger_type       = analog_checker.get_trigger_type();
  m_metadata.trigger_condition  = analog_checker.get_trigger_condition();
  m_metadata.trigger_level      = analog_checker.get_trigger_level();

  // Persist comparison settings
  m_metadata.comparison.time_domain                   = analog_checker.get_cmp_time_domain();
  m_metadata.comparison.frequency_domain              = analog_checker.get_cmp_frequency_domain();
  m_metadata.comparison.time_similarity_threshold     = analog_checker.get_cmp_time_similarity_threshold();
  m_metadata.comparison.frequency_similarity_threshold= analog_checker.get_cmp_frequency_similarity_threshold();

  // Channels
  m_metadata.channels.clear();
  m_metadata.channels.reserve(channel_data.size());
  for (size_t i = 0; i < channel_data.size(); ++i)
  {
    const auto& c = channel_data[i];
    ACC_Metadata::ChannelMeta out;
    out.name              = c.name;
    out.samples           = c.samples;
    out.coupling          = c.coupling;
    out.scaling           = c.scaling;
    out.voltage_per_div   = c.voltage_per_division;
    out.vertical_offset   = c.vertical_offset;
    out.is_enabled        = c.is_enabled;
    out.scaling_corrector = c.scaling_corrector;
    out.measurements.min  = c.measurements.min;
    out.measurements.max  = c.measurements.max;
    out.measurements.avg  = c.measurements.avg;
    out.measurements.trms = c.measurements.trms;
    m_metadata.channels.push_back(out);
  }

  // Function generator basic fields available from loader
  m_metadata.function_generator.wave_type       = func_gen_data.wave_type;
  m_metadata.function_generator.frequency       = func_gen_data.frequency;
  m_metadata.function_generator.period          = func_gen_data.period;
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_analog_circuit_checker()
{
  LABSoft_GUI& gui = m_presenter.gui();
  LABSoft_GUI_Analog_Circuit_Checker_Display& analog_display =
    *(gui.analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);

  if (m_presenter.lab().m_Analog_Circuit_Checker.is_file_loaded())
  {
    const auto& channel_data   = m_presenter.lab().m_Analog_Circuit_Checker.get_channel_data();
    const auto& func_gen_data  = m_presenter.lab().m_Analog_Circuit_Checker.get_function_generator_data();
    const auto& analog_checker = m_presenter.lab().m_Analog_Circuit_Checker;

    try
    {
      // Configure UI from metadata
      analog_display.time_per_division(m_metadata.time_per_division);
      analog_display.samples(m_metadata.samples);
      analog_display.sampling_rate(m_metadata.sampling_rate);

      // Convert imported CH2 to overlay points (drawn independently in red)
      LABSoft_GUI_Analog_Circuit_Checker_Display::PixelPoints pixel_points;
      std::vector<std::array<int, 2>> overlay_points;
      for (size_t idx = 0; idx < channel_data.size() && idx < LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS; ++idx)
      {
        if (idx != 1) continue;
        const auto& channel = channel_data[idx];

        pixel_points[0].clear();
        pixel_points[1].clear();
        overlay_points.clear();

        if (channel.is_enabled && !channel.sample_data.empty())
        {
          const unsigned display_width  = analog_display.display_width();
          const unsigned display_height = analog_display.display_height();

          const size_t N = channel.sample_data.size();
          overlay_points.reserve(N);

          const double voltage_per_division = std::max(1e-9, channel.voltage_per_division);
          const double voltage_range        = voltage_per_division * static_cast<double>(LABC::OSC_DISPLAY::NUMBER_OF_ROWS);

          for (size_t i = 0; i < N; ++i)
          {
            const size_t denom = (N > 1) ? (N - 1) : 1;
            int x = static_cast<int>((static_cast<double>(i) * static_cast<double>(display_width - 1)) / static_cast<double>(denom));

            const double corrected_voltage = (channel.sample_data[i] * channel.scaling_corrector) + channel.vertical_offset;
            double normalized_voltage = (corrected_voltage + (voltage_range / 2.0)) / voltage_range;
            normalized_voltage = clamp01(normalized_voltage);
            int y = static_cast<int>(display_height * (1.0 - normalized_voltage));

            x = std::max(0, std::min(x, static_cast<int>(display_width - 1)));
            y = std::max(0, std::min(y, static_cast<int>(display_height - 1)));

            overlay_points.push_back({x, y});
          }

          // Sync GUI parameters for consistency (use ch2 for reference scaling when needed)
          analog_display.voltage_per_division(static_cast<unsigned>(idx), channel.voltage_per_division);
          analog_display.vertical_offset(static_cast<unsigned>(idx), channel.vertical_offset);
        }
      }

      // Sync horizontal offset to GUI
      analog_display.horizontal_offset(m_metadata.horizontal_offset);

      analog_display.load_pixel_points(pixel_points);
      analog_display.load_overlay_points(overlay_points, FL_RED, true);

      // Disable base channels; overlay shows imported CH2 independently
      analog_display.channel_enable_disable(0, false);
      analog_display.channel_enable_disable(1, false);
      analog_display.update_display();
    }
    catch (const std::exception& e)
    {
      fl_message("Error updating analog circuit checker display: %s", e.what());
    }
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_acc_comparison()
{
  LABSoft_GUI& gui = m_presenter.gui();

  if (gui.analog_circuit_checker_fl_checkbutton_time_domain)
    gui.analog_circuit_checker_fl_checkbutton_time_domain->value(m_metadata.comparison.time_domain ? 1 : 0);

  if (gui.analog_circuit_checker_fl_checkbutton_frequency_domain)
    gui.analog_circuit_checker_fl_checkbutton_frequency_domain->value(m_metadata.comparison.frequency_domain ? 1 : 0);
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_oscilloscope()
{
  LABSoft_GUI& gui = m_presenter.gui();
  LAB_Oscilloscope &osc = lab().m_Oscilloscope;

  // Apply imported metadata to the underlying oscilloscope state
  // Channels
  if (m_metadata.channels.size() >= 1)
  {
    const auto &ch1 = m_metadata.channels[0];
    osc.channel_enable_disable(0, ch1.is_enabled);
    osc.coupling(0, ch1.coupling ? LABE::OSC::COUPLING::AC : LABE::OSC::COUPLING::DC);
    osc.scaling(0, static_cast<LABE::OSC::SCALING>(ch1.scaling));
    osc.voltage_per_division(0, ch1.voltage_per_div);
    osc.vertical_offset(0, ch1.vertical_offset);
  }
  if (m_metadata.channels.size() >= 2)
  {
    const auto &ch2 = m_metadata.channels[1];
    osc.channel_enable_disable(1, ch2.is_enabled);
    osc.coupling(1, ch2.coupling ? LABE::OSC::COUPLING::AC : LABE::OSC::COUPLING::DC);
    osc.scaling(1, static_cast<LABE::OSC::SCALING>(ch2.scaling));
    osc.voltage_per_division(1, ch2.voltage_per_div);
    osc.vertical_offset(1, ch2.vertical_offset);
  }

  // Horizontal/global
  osc.horizontal_offset(m_metadata.horizontal_offset);
  osc.time_per_division(m_metadata.time_per_division);
  osc.samples(m_metadata.samples);
  osc.sampling_rate(m_metadata.sampling_rate);

  // Triggers
  auto clamp_mode = [](unsigned v) -> LABE::OSC::TRIG::MODE {
    if (v > 2) v = 2; return static_cast<LABE::OSC::TRIG::MODE>(v);
  };
  auto clamp_type = [](unsigned v) -> LABE::OSC::TRIG::TYPE {
    if (v > 1) v = 1; return static_cast<LABE::OSC::TRIG::TYPE>(v);
  };
  auto clamp_cnd = [](unsigned v) -> LABE::OSC::TRIG::CND {
    if (v > 2) v = 2; return static_cast<LABE::OSC::TRIG::CND>(v);
  };

  osc.trigger_mode(clamp_mode(m_metadata.trigger_mode));
  osc.trigger_source((m_metadata.trigger_source == 0) ? 0u : 1u);
  osc.trigger_type(clamp_type(m_metadata.trigger_type));
  osc.trigger_condition(clamp_cnd(m_metadata.trigger_condition));
  osc.trigger_level(m_metadata.trigger_level);

  if (m_metadata.channels.size() >= 1)
  {
    const auto& ch1 = m_metadata.channels[0];

    if (gui.oscilloscope_fl_light_button_channel_0_enable)
      gui.oscilloscope_fl_light_button_channel_0_enable->value(ch1.is_enabled ? 1 : 0);

    if (gui.oscilloscope_fl_light_button_channel_0_ac_coupling)
      gui.oscilloscope_fl_light_button_channel_0_ac_coupling->value(ch1.coupling ? 1 : 0);

    if (gui.oscilloscope_labsoft_gui_fl_choice_with_scroll_channel_0_scaling)
      gui.oscilloscope_labsoft_gui_fl_choice_with_scroll_channel_0_scaling->value(static_cast<int>(ch1.scaling));

    if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_voltage_per_division)
    {
      set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_voltage_per_division,
                                UnitKind::VOLT,
                                ch1.voltage_per_div);
    }

    if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_vertical_offset)
    {
      set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_vertical_offset,
                                UnitKind::VOLT,
                                ch1.vertical_offset);
    }
  }

  if (m_metadata.channels.size() >= 2)
  {
    const auto& ch2 = m_metadata.channels[1];

    if (gui.oscilloscope_fl_light_button_channel_1_enable)
      gui.oscilloscope_fl_light_button_channel_1_enable->value(ch2.is_enabled ? 1 : 0);

    if (gui.oscilloscope_fl_light_button_channel_1_ac_coupling)
      gui.oscilloscope_fl_light_button_channel_1_ac_coupling->value(ch2.coupling ? 1 : 0);

    if (gui.oscilloscope_labsoft_gui_fl_choice_with_scroll_channel_1_scaling)
      gui.oscilloscope_labsoft_gui_fl_choice_with_scroll_channel_1_scaling->value(static_cast<int>(ch2.scaling));

    if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_voltage_per_division)
    {
      set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_voltage_per_division,
                                UnitKind::VOLT,
                                ch2.voltage_per_div);
    }

    if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_vertical_offset)
    {
      set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_vertical_offset,
                                UnitKind::VOLT,
                                ch2.vertical_offset);
    }
  }

  // Horizontal/Global
  if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_horizontal_offset)
  {
    set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_horizontal_offset,
                              UnitKind::SECOND,
                              m_metadata.horizontal_offset);
  }

  if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_time_per_division)
  {
    set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_time_per_division,
                              UnitKind::SECOND,
                              m_metadata.time_per_division);
  }

  if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_samples)
  {
    set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_samples,
                              UnitKind::SAMPLES,
                              static_cast<double>(m_metadata.samples));
  }

  if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_sampling_rate)
  {
    set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_sampling_rate,
                              UnitKind::HERTZ,
                              m_metadata.sampling_rate);
  }

  // Triggers
  if (gui.oscilloscope_fl_choice_trigger_mode)
  {
    int mode_val = static_cast<int>(m_metadata.trigger_mode);
    mode_val = (mode_val <= 0) ? 0 : 1;
    int menu_index = (mode_val == 0) ? 0 : 1;
    set_choice_index(gui.oscilloscope_fl_choice_trigger_mode, menu_index);
  }

  if (gui.oscilloscope_fl_choice_trigger_source)
  {
    int src = static_cast<int>(m_metadata.trigger_source);
    src = (src <= 0) ? 0 : 1;
    gui.oscilloscope_fl_choice_trigger_source->activate();
    set_choice_index(gui.oscilloscope_fl_choice_trigger_source, src);
  }

  if (gui.oscilloscope_fl_choice_trigger_type)
  {
    gui.oscilloscope_fl_choice_trigger_type->activate();
    set_choice_index(gui.oscilloscope_fl_choice_trigger_type, static_cast<int>(m_metadata.trigger_type));
  }

  if (gui.oscilloscope_fl_choice_trigger_condition)
  {
    gui.oscilloscope_fl_choice_trigger_condition->activate();
    set_choice_index(gui.oscilloscope_fl_choice_trigger_condition, static_cast<int>(m_metadata.trigger_condition));
  }

  if (gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_trigger_level)
  {
    gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_trigger_level->activate();
    set_input_choice_to_value(gui.oscilloscope_labsoft_gui_fl_input_choice_with_scroll_trigger_level, UnitKind::VOLT, m_metadata.trigger_level);
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_function_generator()
{
  LABSoft_GUI& gui = m_presenter.gui();

  if (gui.function_generator_fl_input_choice_frequency)
  {
    set_input_choice_to_value(gui.function_generator_fl_input_choice_frequency,
                              UnitKind::HERTZ,
                              m_metadata.function_generator.frequency);
  }

  if (gui.function_generator_fl_input_choice_period)
  {
    set_input_choice_to_value(gui.function_generator_fl_input_choice_period,
                              UnitKind::SECOND,
                              m_metadata.function_generator.period);
  }

  if (gui.function_generator_fl_choice_wave_type)
    set_choice_index(gui.function_generator_fl_choice_wave_type,
                     static_cast<int>(m_metadata.function_generator.wave_type));
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_display()
{
  LABSoft_GUI &gui = m_presenter.gui();
  gui.analog_circuit_checker_labsoft_gui_analog_circuit_checker_display->update_display();
}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_load_file_acc (Fl_Button* w, void* data)
{
  Fl_Native_File_Chooser chooser;

  chooser.title("Open Analog Circuit Checker Template File");
  chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
  chooser.filter("LAB Analog Circuit Checker\t*.labacc");

  Fl_Output &selected_file = *(m_presenter.gui().analog_circuit_checker_fl_output_selected_file);

  try
  {
    switch (chooser.show())
    {
      case (0):
      {
        std::string path(chooser.filename());

        if (LABF::has_filename_this_extension(path,
                                              LABC::LABSOFT::ANALOG_CIRCUIT_CHECKER_FILENAME_EXTENSION))
        {
          update_gui_display();

          selected_file.value(LABF::get_filename_from_path(path).c_str());

          // Load the .labacc file
          m_presenter.lab().m_Analog_Circuit_Checker.load_file(path);
          import_metadata();

          // turn off oscilloscope and function generator run button
          gui().oscilloscope_fl_light_button_run_stop->value(0);
          gui().function_generator_fl_light_button_run_stop->value(0);
          presenter().m_Oscilloscope.cb_run_stop(gui().oscilloscope_fl_light_button_run_stop, nullptr);
          presenter().m_Function_Generator.cb_run_stop(gui().function_generator_fl_light_button_run_stop, 0);

          // update gui
          update_gui_oscilloscope           ();
          update_gui_function_generator     ();
          update_gui_acc_comparison         ();
          update_gui_analog_circuit_checker ();

          fl_message("File loaded successfully. Click 'Run Checker' to display data in oscilloscope.");
        }
        else
        {
          throw(std::runtime_error("Invalid file selected."));
        }

        break;
      }

      case (1):
      {
        break;
      }

      case (-1):
      {
        break;
      }
    }
  }
  catch (const std::exception &e)
  {
    fl_message(e.what());

    selected_file.value("");
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_run_checker_acc (Fl_Button* w, void* data)
{
  auto &checker = lab().m_Analog_Circuit_Checker;

  std::printf("\n=== ANALOG CIRCUIT CHECKER - RUN CHECKER TRIGGERED ===\n");

  try
  {
    LABSoft_GUI &gui_ref = m_presenter.gui();
    auto *acc_disp_ptr = gui_ref.analog_circuit_checker_labsoft_gui_analog_circuit_checker_display;
    if (!acc_disp_ptr) return;

    LAB_Oscilloscope &osc = lab().m_Oscilloscope;
    LAB_Oscilloscope_Display &osc_disp = lab().m_Oscilloscope_Display;

    const unsigned analog_w = acc_disp_ptr->display_width();
    const unsigned analog_h = acc_disp_ptr->display_height();

    LABSoft_GUI_Oscilloscope_Display &osc_gui_disp = *(gui_ref.oscilloscope_labsoft_gui_oscilloscope_display);
    const unsigned osc_tab_w = osc_gui_disp.display_width();
    const unsigned osc_tab_h = osc_gui_disp.display_height();

    const bool same_size = (analog_w == osc_tab_w) && (analog_h == osc_tab_h);
    if (!same_size)
    {
      osc_disp.display_parameters(
        analog_w,
        analog_h,
        LABC::OSC_DISPLAY::NUMBER_OF_ROWS,
        LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS
      );
    }

    osc_disp.update_pixel_points();
    const auto &raw_buf = osc_disp.pixel_points();

    // Populate ACC display with CH2 only
    LABSoft_GUI_Analog_Circuit_Checker_Display::PixelPoints acc_pixels{};
    if (raw_buf.size() > 1)
      acc_pixels[1] = raw_buf[1];

    acc_disp_ptr->voltage_per_division(1, osc.voltage_per_division(1));
    acc_disp_ptr->vertical_offset(1, osc.vertical_offset(1));
    acc_disp_ptr->time_per_division(osc.time_per_division());
    acc_disp_ptr->horizontal_offset(osc.horizontal_offset());
    acc_disp_ptr->samples(osc.samples());
    acc_disp_ptr->sampling_rate(osc.sampling_rate());

    acc_disp_ptr->load_pixel_points(acc_pixels);
    acc_disp_ptr->channel_enable_disable(0, false);
    acc_disp_ptr->channel_enable_disable(1, true);
    acc_disp_ptr->update_display();

    if (!same_size)
    {
      osc_disp.display_parameters(
        osc_tab_w,
        osc_tab_h,
        LABC::OSC_DISPLAY::NUMBER_OF_ROWS,
        LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS
      );
      osc_disp.update_pixel_points();
    }
    /*update_gui_analog_circuit_checker();
    double comparison = checker->lab().m_Analog__Circuit_Checker.compute.similarity();
    std::cout << "\nSimilarity: " << sim << "%\n";*/
    std::printf("\n=== ANALOG CIRCUIT CHECKER - COMPLETED ===\n\n");
  }
  catch (const std::exception &e)
  {
    fl_message("Error running analog circuit checker: %s", e.what());
  }
}

// EOF
