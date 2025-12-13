#include "LABSoft_Presenter_Analog_Circuit_Checker.h"

#include <cstdio>
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <array>
#include <complex>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <thread>
#include <chrono>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu_Button.H>

#include <FL/fl_ask.H>

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"
#include "../../LABSoft_GUI/LABSoft_GUI_Analog_Circuit_Checker_Display.h"
#include "../../Utility/LAB_Utility_Functions.h"
#include "../../Utility/LABSoft_GUI_Label.h"

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

void LABSoft_Presenter_Analog_Circuit_Checker::
reset_display_state_for_new_file()
{
  // Clear in-memory signal and analysis buffers
  time_instructor.clear();
  time_student.clear();
  freq_instructor.clear();
  freq_student.clear();
  freq_instructor_complex.clear();
  freq_student_complex.clear();
  time_instructor_pixels.clear();
  time_student_pixels.clear();

  time_domain_result = AnalysisResult{};
  frequency_domain_result = AnalysisResult{};

  m_view_frequency = false;

  LABSoft_GUI &gui_ref = m_presenter.gui();

  // Reset Analog Circuit Checker display contents
  if (auto *acc_disp_ptr = gui_ref.analog_circuit_checker_labsoft_gui_analog_circuit_checker_display)
  {
    LABSoft_GUI_Analog_Circuit_Checker_Display::PixelPoints empty_points{};
    std::vector<std::array<int, 2>> empty_overlay;

    acc_disp_ptr->set_frequency_view(false, 0.0);
    acc_disp_ptr->load_pixel_points(empty_points);
    acc_disp_ptr->load_overlay_points(empty_overlay, FL_RED, false);
    acc_disp_ptr->channel_enable_disable(0, false);
    acc_disp_ptr->channel_enable_disable(1, false);
    acc_disp_ptr->update_display();
  }

  // Reset view toggle button and domain label to defaults
  if (gui_ref.analog_circuit_checker_fl_button_toggle_view)
  {
    gui_ref.analog_circuit_checker_fl_button_toggle_view->copy_label("View Frequency Domain");
    gui_ref.analog_circuit_checker_fl_button_toggle_view->redraw();
  }
  if (gui_ref.analog_circuit_checker_domain_label)
  {
    gui_ref.analog_circuit_checker_domain_label->label("Time Domain");
    gui_ref.analog_circuit_checker_domain_label->redraw();
  }

  // Clear similarity output fields and reset their colors
  if (gui_ref.analog_circuit_checker_fl_input_time_domain_similarity_threshold)
  {
    gui_ref.analog_circuit_checker_fl_input_time_domain_similarity_threshold->value("");
    gui_ref.analog_circuit_checker_fl_input_time_domain_similarity_threshold->textcolor(FL_FOREGROUND_COLOR);
    gui_ref.analog_circuit_checker_fl_input_time_domain_similarity_threshold->redraw();
  }
  if (gui_ref.analog_circuit_checker_fl_input_frequency_domain_similarity_threshold)
  {
    gui_ref.analog_circuit_checker_fl_input_frequency_domain_similarity_threshold->value("");
    gui_ref.analog_circuit_checker_fl_input_frequency_domain_similarity_threshold->textcolor(FL_FOREGROUND_COLOR);
    gui_ref.analog_circuit_checker_fl_input_frequency_domain_similarity_threshold->redraw();
  }
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

  // std::printf("[ACC] Imported trigger settings from .labacc file:\n");
  // std::printf("  trigger_mode: %u\n", m_metadata.trigger_mode);
  // std::printf("  trigger_source: %u\n", m_metadata.trigger_source);
  // std::printf("  trigger_type: %u\n", m_metadata.trigger_type);
  // std::printf("  trigger_condition: %u\n", m_metadata.trigger_condition);
  // std::printf("  trigger_level: %.6f\n", m_metadata.trigger_level);

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
prepare_instructor_data()
{
  time_instructor.clear();
  freq_instructor.clear();
  freq_instructor_complex.clear();
  time_instructor_pixels.clear();

  const auto &ch_data = lab().m_Analog_Circuit_Checker.get_channel_data();

  if (ch_data.size() > 1 && ch_data[1].is_enabled && !ch_data[1].sample_data.empty())
  {
    time_instructor = ch_data[1].sample_data;

    const auto spectrum = lab().m_Analog_Circuit_Checker.compute_fft(time_instructor);
    freq_instructor_complex = spectrum;
    freq_instructor.reserve(spectrum.size());

    for (const auto &c : spectrum)
      freq_instructor.push_back(std::abs(c));

    LABSoft_GUI *gui_ptr = &m_presenter.gui();
    auto *acc_disp_ptr = gui_ptr ? gui_ptr->analog_circuit_checker_labsoft_gui_analog_circuit_checker_display : nullptr;
    if (acc_disp_ptr)
    {
      const unsigned display_width  = acc_disp_ptr->display_width();
      const unsigned display_height = acc_disp_ptr->display_height();

      const auto &channel = ch_data[1];
      const size_t N = time_instructor.size();
      time_instructor_pixels.reserve(N);

      const double voltage_per_division = std::max(1e-9, channel.voltage_per_division);
      const double voltage_range        = voltage_per_division * static_cast<double>(LABC::OSC_DISPLAY::NUMBER_OF_ROWS);

      for (size_t i = 0; i < N; ++i)
      {
        const size_t denom = (N > 1) ? (N - 1) : 1;
        int x = static_cast<int>((static_cast<double>(i) * static_cast<double>(display_width - 1)) / static_cast<double>(denom));

        const double corrected_voltage = (time_instructor[i] * channel.scaling_corrector) + channel.vertical_offset;
        double normalized_voltage = (corrected_voltage + (voltage_range / 2.0)) / voltage_range;
        normalized_voltage = clamp01(normalized_voltage);
        int y = static_cast<int>(display_height * (1.0 - normalized_voltage));

        x = std::max(0, std::min(x, static_cast<int>(display_width - 1)));
        y = std::max(0, std::min(y, static_cast<int>(display_height - 1)));

        time_instructor_pixels.push_back({x, y});
      }
    }
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
prepare_student_data()
{
  time_student.clear();
  freq_student.clear();
  freq_student_complex.clear();
  time_student_pixels.clear();

  LAB_Oscilloscope &osc = lab().m_Oscilloscope;
  if (!osc.is_channel_enabled(1)) return;

  const auto &arr = osc.chan_samples(1);
  const unsigned count = osc.samples();
  const unsigned n = std::min<unsigned>(static_cast<unsigned>(arr.size()), count);

  time_student.reserve(n);
  for (unsigned i = 0; i < n; ++i)
    time_student.push_back(arr[i]);

  const auto spectrum = lab().m_Analog_Circuit_Checker.compute_fft(time_student);
  freq_student_complex = spectrum;
  freq_student.reserve(spectrum.size());

  for (const auto &c : spectrum)
    freq_student.push_back(std::abs(c));

  LABSoft_GUI *gui_ptr = &m_presenter.gui();
  auto *acc_disp_ptr = gui_ptr ? gui_ptr->analog_circuit_checker_labsoft_gui_analog_circuit_checker_display : nullptr;
  if (acc_disp_ptr)
  {
    const unsigned display_width  = acc_disp_ptr->display_width();
    const unsigned display_height = acc_disp_ptr->display_height();

    const size_t N = time_student.size();
    time_student_pixels.reserve(N);

    const double voltage_per_division = std::max(1e-9, osc.voltage_per_division(1));
    const double voltage_range        = voltage_per_division * static_cast<double>(LABC::OSC_DISPLAY::NUMBER_OF_ROWS);
    const double vertical_offset      = osc.vertical_offset(1);

    for (size_t i = 0; i < N; ++i)
    {
      const size_t denom = (N > 1) ? (N - 1) : 1;
      int x = static_cast<int>((static_cast<double>(i) * static_cast<double>(display_width - 1)) / static_cast<double>(denom));

      const double corrected_voltage = time_student[i] + vertical_offset;
      double normalized_voltage = (corrected_voltage + (voltage_range / 2.0)) / voltage_range;
      normalized_voltage = clamp01(normalized_voltage);
      int y = static_cast<int>(display_height * (1.0 - normalized_voltage));

      x = std::max(0, std::min(x, static_cast<int>(display_width - 1)));
      y = std::max(0, std::min(y, static_cast<int>(display_height - 1)));

      time_student_pixels.push_back({x, y});
    }
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_analog_circuit_checker()
{
  LABSoft_GUI& gui = m_presenter.gui();
  LABSoft_GUI_Analog_Circuit_Checker_Display& analog_display =
    *(gui.analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);

  if (m_presenter.lab().m_Analog_Circuit_Checker.is_file_loaded())
  {
    // Configure base UI from metadata
    analog_display.time_per_division  (m_metadata.time_per_division);
    analog_display.samples            (m_metadata.samples);
    analog_display.sampling_rate      (m_metadata.sampling_rate);
    analog_display.horizontal_offset  (m_metadata.horizontal_offset);

    // Apply channel 2 parameters if present (student channel)
    if (m_metadata.channels.size() > 1)
    {
      const auto &ch2m = m_metadata.channels[1];
      analog_display.voltage_per_division (1, ch2m.voltage_per_div);
      analog_display.vertical_offset      (1, ch2m.vertical_offset);
    }

    // Time vs Frequency view
    if (m_view_frequency)
    {
      analog_display.set_frequency_view(true, m_metadata.sampling_rate);

      LABSoft_GUI_Analog_Circuit_Checker_Display::PixelPoints freq_pixels{};
      std::vector<std::array<int, 2>> overlay_points;

      const bool have_instructor = !freq_instructor.empty();
      const bool have_student    = !freq_student.empty();

      if (have_instructor || have_student)
      {
        const unsigned display_width  = analog_display.display_width();
        const unsigned display_height = analog_display.display_height();
        const size_t denom_instructor = (freq_instructor.size() > 1) ? (freq_instructor.size() - 1) : 1;
        const size_t denom_student    = (freq_student.size()    > 1) ? (freq_student.size()    - 1) : 1;

        double max_mag = 0.0;
        if (have_instructor) for (double v : freq_instructor) if (v > max_mag) max_mag = v;
        if (have_student)    for (double v : freq_student)    if (v > max_mag) max_mag = v;
        if (max_mag <= 0.0) max_mag = 1.0;

        // Student -> channel 1 pixels
        if (have_student)
        {
          const size_t N = freq_student.size();
          freq_pixels[1].reserve(N);
          for (size_t i = 0; i < N; ++i)
          {
            int x = static_cast<int>((static_cast<double>(i) * static_cast<double>(display_width  - 1)) / static_cast<double>(denom_student));
            double normalized = clamp01(freq_student[i] / max_mag);
            int y = static_cast<int>(display_height * (1.0 - normalized));
            x = std::max(0, std::min(x, static_cast<int>(display_width  - 1)));
            y = std::max(0, std::min(y, static_cast<int>(display_height - 1)));
            freq_pixels[1].push_back({x, y});
          }
        }

        // Instructor -> red overlay
        if (have_instructor)
        {
          const size_t N = freq_instructor.size();
          overlay_points.reserve(N);
          for (size_t i = 0; i < N; ++i)
          {
            int x = static_cast<int>((static_cast<double>(i) * static_cast<double>(display_width  - 1)) / static_cast<double>(denom_instructor));
            double normalized = clamp01(freq_instructor[i] / max_mag);
            int y = static_cast<int>(display_height * (1.0 - normalized));
            x = std::max(0, std::min(x, static_cast<int>(display_width  - 1)));
            y = std::max(0, std::min(y, static_cast<int>(display_height - 1)));
            overlay_points.push_back({x, y});
          }
        }
      }

      analog_display.load_pixel_points(freq_pixels);
      analog_display.load_overlay_points(overlay_points, FL_RED, true);
      analog_display.channel_enable_disable(0, false);
      analog_display.channel_enable_disable(1, have_student);
      analog_display.update_display();
    }
    else
    {
      analog_display.set_frequency_view(false, m_metadata.sampling_rate);

      // Build time-domain view using prepared pixel buffers
      LABSoft_GUI_Analog_Circuit_Checker_Display::PixelPoints pixel_points{};
      if (!time_student_pixels.empty())
        pixel_points[1] = time_student_pixels;

      const bool have_student = !time_student_pixels.empty();
      std::vector<std::array<int, 2>> overlay_points = time_instructor_pixels; // instructor in red overlay

      analog_display.load_pixel_points(pixel_points);
      analog_display.load_overlay_points(overlay_points, FL_RED, true);
      analog_display.channel_enable_disable(0, false);
      analog_display.channel_enable_disable(1, have_student);
      analog_display.update_display();
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

  // Triggers - Apply from .labacc file
  auto clamp_mode = [](unsigned v) -> LABE::OSC::TRIG::MODE {
    if (v > 2) v = 2; return static_cast<LABE::OSC::TRIG::MODE>(v);
  };
  auto clamp_type = [](unsigned v) -> LABE::OSC::TRIG::TYPE {
    if (v > 1) v = 1; return static_cast<LABE::OSC::TRIG::TYPE>(v);
  };
  auto clamp_cnd = [](unsigned v) -> LABE::OSC::TRIG::CND {
    if (v > 2) v = 2; return static_cast<LABE::OSC::TRIG::CND>(v);
  };

  // std::printf("[ACC] Applying trigger settings to oscilloscope hardware:\n");
  // std::printf("  Before: osc.trigger_mode() = %d, is_running() = %d\n",
  //             static_cast<int>(osc.trigger_mode()), osc.is_running());

  osc.trigger_mode(clamp_mode(m_metadata.trigger_mode));
  osc.trigger_source((m_metadata.trigger_source == 0) ? 0u : 1u);
  osc.trigger_type(clamp_type(m_metadata.trigger_type));
  osc.trigger_condition(clamp_cnd(m_metadata.trigger_condition));
  osc.trigger_level(m_metadata.trigger_level);

  // std::printf("  After: osc.trigger_mode() = %d, is_running() = %d\n",
  //             static_cast<int>(osc.trigger_mode()), osc.is_running());

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

  // Triggers - Update GUI controls to reflect .labacc settings
  if (gui.oscilloscope_fl_choice_trigger_mode)
  {
    // Map trigger_mode (0=NONE, 1=NORMAL, 2=AUTO) to menu index
    // Since AUTO is disabled, only use NONE (0) and NORMAL (1)
    int mode_val = static_cast<int>(m_metadata.trigger_mode);
    if (mode_val > 2) mode_val = 0; // Clamp invalid values
    int menu_index = (mode_val == 0) ? 0 : 1; // 0->NONE, 1/2->NORMAL
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

  // Sync Oscilloscope tab display with imported settings (apply to display widget and refresh caches)
  if (gui.oscilloscope_labsoft_gui_oscilloscope_display)
  {
    // Channel enable/disable and vertical params
    bool ch0_en = (m_metadata.channels.size() >= 1) ? m_metadata.channels[0].is_enabled : false;
    bool ch1_en = (m_metadata.channels.size() >= 2) ? m_metadata.channels[1].is_enabled : false;

    gui.oscilloscope_labsoft_gui_oscilloscope_display->channel_enable_disable(0, ch0_en);
    gui.oscilloscope_labsoft_gui_oscilloscope_display->channel_enable_disable(1, ch1_en);

    if (m_metadata.channels.size() >= 1)
    {
      const auto &ch0 = m_metadata.channels[0];
      gui.oscilloscope_labsoft_gui_oscilloscope_display->voltage_per_division(0, ch0.voltage_per_div);
      gui.oscilloscope_labsoft_gui_oscilloscope_display->vertical_offset(0, ch0.vertical_offset);
    }
    if (m_metadata.channels.size() >= 2)
    {
      const auto &ch1 = m_metadata.channels[1];
      gui.oscilloscope_labsoft_gui_oscilloscope_display->voltage_per_division(1, ch1.voltage_per_div);
      gui.oscilloscope_labsoft_gui_oscilloscope_display->vertical_offset(1, ch1.vertical_offset);
    }

    // Horizontal/global and trigger
    gui.oscilloscope_labsoft_gui_oscilloscope_display->horizontal_offset(m_metadata.horizontal_offset);
    gui.oscilloscope_labsoft_gui_oscilloscope_display->time_per_division(m_metadata.time_per_division);
    gui.oscilloscope_labsoft_gui_oscilloscope_display->samples(m_metadata.samples);
    gui.oscilloscope_labsoft_gui_oscilloscope_display->sampling_rate(m_metadata.sampling_rate);
    gui.oscilloscope_labsoft_gui_oscilloscope_display->trigger_source(m_metadata.trigger_source);
    gui.oscilloscope_labsoft_gui_oscilloscope_display->trigger_level(m_metadata.trigger_level);
  }

  lab().m_Oscilloscope_Display.update_cached_values();
  presenter().m_Oscilloscope_Display.update_display();
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_function_generator()
{
  LABSoft_GUI& gui = m_presenter.gui();

  {
    LAB_Function_Generator &gen = lab().m_Function_Generator;
    gen.wave_type(0, static_cast<LABE::FUNC_GEN::WAVE_TYPE>(m_metadata.function_generator.wave_type));

    if (m_metadata.function_generator.frequency > 0.0)
      gen.frequency(0, m_metadata.function_generator.frequency);
    if (m_metadata.function_generator.period > 0.0)
      gen.period(0, m_metadata.function_generator.period);

    m_presenter.m_Function_Generator.update_gui_frequency_elements();
  }

  if (gui.function_generator_fl_input_choice_frequency && (m_metadata.function_generator.frequency > 0.0))
  {
    LABSoft_GUI_Label lbl(m_metadata.function_generator.frequency, LABSoft_GUI_Label::UNIT::HERTZ);
    gui.function_generator_fl_input_choice_frequency->value(lbl.to_text().c_str());
  }

  if (gui.function_generator_fl_input_choice_period && (m_metadata.function_generator.period > 0.0))
  {
    LABSoft_GUI_Label lbl(m_metadata.function_generator.period, LABSoft_GUI_Label::UNIT::SECOND);
    gui.function_generator_fl_input_choice_period->value(lbl.to_text().c_str());
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
perform_time_domain_analysis()
{
  auto &checker = lab().m_Analog_Circuit_Checker;

  if (!checker.is_file_loaded()) return;
  if (!checker.get_cmp_time_domain()) return;

  if (!time_instructor.empty() && !time_student.empty())
  {
    auto result = checker.signal_analysis(time_instructor, time_student);
    time_domain_result.lag = result.lag;
    time_domain_result.coefficient = result.coefficient;
    time_domain_result.percentage = result.percentage;

    if (gui().analog_circuit_checker_fl_input_time_domain_similarity_threshold)
    {
      char buf[64];
      std::snprintf(buf, sizeof(buf), "%.2f%%", time_domain_result.percentage);
      gui().analog_circuit_checker_fl_input_time_domain_similarity_threshold->value(buf);

      // Color coding: Green if >= threshold, Red if < threshold
      if (time_domain_result.percentage >= checker.get_cmp_time_similarity_threshold())
      {
        gui().analog_circuit_checker_fl_input_time_domain_similarity_threshold->textcolor(FL_DARK_GREEN);
      }
      else
      {
        gui().analog_circuit_checker_fl_input_time_domain_similarity_threshold->textcolor(FL_RED);
      }
      gui().analog_circuit_checker_fl_input_time_domain_similarity_threshold->redraw();
    }
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
perform_frequency_domain_analysis()
{
  auto &checker = lab().m_Analog_Circuit_Checker;

  if (!checker.is_file_loaded()) return;
  if (!checker.get_cmp_frequency_domain()) return;

  if (!freq_instructor_complex.empty() && !freq_student_complex.empty())
  {
    auto result = checker.signal_analysis_complex(freq_instructor_complex, freq_student_complex);
    frequency_domain_result.lag = result.lag;
    frequency_domain_result.coefficient = result.coefficient;
    frequency_domain_result.percentage = result.percentage;

    if (gui().analog_circuit_checker_fl_input_frequency_domain_similarity_threshold)
    {
      char buf[64];
      std::snprintf(buf, sizeof(buf), "%.2f%%", frequency_domain_result.percentage);
      gui().analog_circuit_checker_fl_input_frequency_domain_similarity_threshold->value(buf);

      // Color coding: Green if >= threshold, Red if < threshold
      if (frequency_domain_result.percentage >= checker.get_cmp_frequency_similarity_threshold())
      {
        gui().analog_circuit_checker_fl_input_frequency_domain_similarity_threshold->textcolor(FL_DARK_GREEN);
      }
      else
      {
        gui().analog_circuit_checker_fl_input_frequency_domain_similarity_threshold->textcolor(FL_RED);
      }
      gui().analog_circuit_checker_fl_input_frequency_domain_similarity_threshold->redraw();
    }

  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_load_file_acc(Fl_Button* w, void* data)
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
          // Ensure any previous ACC display state, settings, and signals are cleared
          reset_display_state_for_new_file();

          selected_file.value(LABF::get_filename_from_path(path).c_str());

          // Load the .labacc file
          m_presenter.lab().m_Analog_Circuit_Checker.load_file(path);
          import_metadata();

          // prepare and configure sample data
          prepare_instructor_data           ();

          // update gui
          update_gui_oscilloscope           ();
          update_gui_function_generator     ();
          update_gui_acc_comparison         ();
          update_gui_analog_circuit_checker ();

          // turn off oscilloscope and function generator run button
          gui().oscilloscope_fl_light_button_run_stop->value(0);
          gui().function_generator_fl_light_button_run_stop->value(0);
          presenter().m_Oscilloscope.cb_run_stop(gui().oscilloscope_fl_light_button_run_stop, nullptr);
          presenter().m_Function_Generator.cb_run_stop(gui().function_generator_fl_light_button_run_stop, 0);

          lab().m_Oscilloscope.sync_display_metadata_from_current_settings();
          lab().m_Oscilloscope_Display.update_cached_values();
          presenter().m_Oscilloscope_Display.update_display();
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
cb_run_checker_acc(Fl_Button* w, void* data)
{
  auto &checker = lab().m_Analog_Circuit_Checker;

  LABSoft_GUI &gui_ref = m_presenter.gui();
  auto *acc_disp_ptr = gui_ref.analog_circuit_checker_labsoft_gui_analog_circuit_checker_display;
  if (!acc_disp_ptr) return;
  LAB_Oscilloscope &osc = lab().m_Oscilloscope;

  osc.channel_enable_disable(1, true);
  prepare_student_data();

  // Print student signals to terminal | DEBUG
  // std::printf("<samples>");
  // for (size_t i = 0; i < time_student.size(); ++i)
  // {
  //   std::printf("%.6f", time_student[i]);
  //   if (i < time_student.size() - 1)
  //     std::printf(",");
  // }
  // std::printf("</samples>\n");

  perform_time_domain_analysis();
  perform_frequency_domain_analysis();

  // Update display based on current view mode (time or frequency)
  update_gui_analog_circuit_checker();
}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_toggle_view(Fl_Button* w, void* data)
{
  if (!w) return;

  const char* BTN_LABEL_FREQ = "View Frequency Domain";
  const char* BTN_LABEL_TIME = "View Time Domain";
  const char* LABEL_TIME = "Time Domain";
  const char* LABEL_FREQ = "Frequency Domain";

  const char* current_cstr = w->label();
  const std::string current_label = current_cstr ? current_cstr : "";

  if (current_label == BTN_LABEL_FREQ)
  {
    w->copy_label(BTN_LABEL_TIME);
    m_view_frequency = true;
    gui().analog_circuit_checker_domain_label->label(LABEL_FREQ);
  }
  else
  {
    w->copy_label(BTN_LABEL_FREQ);
    m_view_frequency = false;
    gui().analog_circuit_checker_domain_label->label(LABEL_TIME);
  }

  update_gui_analog_circuit_checker();
  w->redraw();
}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_export_result(Fl_Button* w, void* data)
{
  auto &checker = lab().m_Analog_Circuit_Checker;

  if (!checker.is_file_loaded())
  {
    fl_message("No file loaded. Please load a .labacc file first.");
    return;
  }

  if (time_student.empty())
  {
    fl_message("No student data available. Please run the checker first.");
    return;
  }

  const std::string &file_path = checker.get_file_path();

  bool success = checker.export_result_file(
    file_path,
    time_domain_result.percentage,
    time_domain_result.lag,
    frequency_domain_result.percentage,
    frequency_domain_result.lag,
    time_student
  );

  if (success)
  {
    std::string result_path = file_path + ".result";
    std::string message = "Results exported successfully to:\n" + result_path;
    fl_message(message.c_str());
    // debug
    // std::printf("\n=== ANALOG CIRCUIT CHECKER - RESULT EXPORTED ===\n");
    // std::printf("File: %s\n", result_path.c_str());
    // std::printf("Time Domain Similarity: %.2f%% (Lag: %.0f samples)\n",
    //             time_domain_result.percentage, time_domain_result.lag);
    // std::printf("Frequency Domain Similarity: %.2f%% (Lag: %.0f bins)\n",
    //             frequency_domain_result.percentage, frequency_domain_result.lag);
    // std::printf("===============================================\n\n");
  }
  else
  {
    fl_message("Failed to export results. Please check file permissions.");
  }
}

// EOF
