// teacher side
#include "LABSoft_Presenter_LABChecker_Analog.h"

#include <iostream>
#include <fstream>

#include "../../LAB/LAB.h"
#include "../../LAB/LAB_Oscilloscope.h"
#include "../../LAB/LAB_Function_Generator.h"
#include "../../Utility/LABSoft_GUI_Label.h"
#include "../../Utility/LABSoft_GUI_Label_Values.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"

#define LOG(msg) fprintf(stdout, "%s\n", msg)

LABSoft_Presenter_LABChecker_Analog::LABSoft_Presenter_LABChecker_Analog(LABSoft_Presenter &_LABSoft_Presenter)
    : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  load_gui();
}

void LABSoft_Presenter_LABChecker_Analog::
load_gui()
{
  LAB_Oscilloscope_Display &osc_disp = lab().m_Oscilloscope_Display;
  LABSoft_GUI_LABChecker_Analog_Checker_Display &analog_checker_disp_gui = *(gui().analog_labsoft_gui_analog_checker_display);

  analog_checker_disp_gui.load_presenter(m_presenter);
}

void LABSoft_Presenter_LABChecker_Analog::
update_display()
{
  LAB_Oscilloscope_Display &osc_disp = lab().m_Oscilloscope_Display;
  LABSoft_GUI_LABChecker_Analog_Checker_Display &analog_checker_disp_gui = *(gui().analog_labsoft_gui_analog_checker_display);

  osc_disp.update_pixel_points();
  const auto &raw_buf = osc_disp.pixel_points();
  if (raw_buf.size() < 2 || raw_buf[1].empty())
  {
    // Explicitly clear so we don't keep showing stale traces.
    std::array<std::vector<std::array<int, 2>>, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> empty_pixel_points{};
    analog_checker_disp_gui.load_pixel_points(empty_pixel_points);
    analog_checker_disp_gui.channel_enable_disable(0, false);
    analog_checker_disp_gui.channel_enable_disable(1, true);
    analog_checker_disp_gui.update_display();
    return;
  }

  std::array<std::vector<std::array<int, 2>>, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> channel2_only_pixel_points;
  channel2_only_pixel_points[1] = raw_buf[1];

  analog_checker_disp_gui.load_pixel_points(channel2_only_pixel_points);
  analog_checker_disp_gui.channel_enable_disable(0, false);
  analog_checker_disp_gui.channel_enable_disable(1, true);
  analog_checker_disp_gui.update_display();
}

void LABSoft_Presenter_LABChecker_Analog::
cb_capture_signal(Fl_Button *w,
                  void *data)
{
  (void)w;
  (void)data;

  if (can_capture_signal())
  {
    capture_oscilloscope_and_function_generator_data();
    update_gui_with_captured_data();
  }
  else
  {
    fl_message_title ("LABChecker - Analog");
    fl_message("Must turn on both the Oscilloscope and Function Generator before capturing data.");
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

  analog_checker_disp_gui.voltage_per_division(1, osc.voltage_per_division(1));
  analog_checker_disp_gui.vertical_offset(1, osc.vertical_offset(1));
  analog_checker_disp_gui.time_per_division(osc.time_per_division());
  analog_checker_disp_gui.horizontal_offset(osc.horizontal_offset());
  analog_checker_disp_gui.samples(osc.samples());
  analog_checker_disp_gui.sampling_rate(osc.sampling_rate());

  const unsigned analog_w = analog_checker_disp_gui.display_width();
  const unsigned analog_h = analog_checker_disp_gui.display_height();

  LABSoft_GUI_Oscilloscope_Display &osc_gui_disp = *(gui().oscilloscope_labsoft_gui_oscilloscope_display);
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
  if (raw_buf.size() < 2 || raw_buf[1].empty())
  {
    if (!same_size)
    {
      osc_disp.display_parameters(osc_tab_w, osc_tab_h, LABC::OSC_DISPLAY::NUMBER_OF_ROWS, LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS);
      osc_disp.update_pixel_points();
    }
    return;
  }

  std::array<std::vector<std::array<int, 2>>, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> ch2_only{};
  ch2_only[1] = raw_buf[1];

  analog_checker_disp_gui.load_pixel_points(ch2_only);
  analog_checker_disp_gui.channel_enable_disable(0, false);
  analog_checker_disp_gui.channel_enable_disable(1, true);
  analog_checker_disp_gui.update_display();

  if (!same_size)
  {
    osc_disp.display_parameters(osc_tab_w, osc_tab_h, LABC::OSC_DISPLAY::NUMBER_OF_ROWS, LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS);
    osc_disp.update_pixel_points();
  }
}

void LABSoft_Presenter_LABChecker_Analog::
sync_comparison_inputs_enabled()
{
  // Capture baseline style per-widget so activation/deactivation returns to the
  // exact FLUID/theme defaults (and never falls back to 0/black).
  struct BaselineOne
  {
    bool      captured = false;
    Fl_Boxtype box{};
    Fl_Color  color{};
    Fl_Color  textcolor{};
    Fl_Font   textfont{};
  };
  static BaselineOne time_base;
  static BaselineOne freq_base;

  if (gui().analog_fl_input_time_domain_similarity_threshold && !time_base.captured)
  {
    auto* w = gui().analog_fl_input_time_domain_similarity_threshold;
    time_base.box       = w->box();
    time_base.color     = w->color();
    time_base.textcolor = w->textcolor();
    time_base.textfont  = w->textfont();
    time_base.captured  = true;
  }
  if (gui().analog_fl_input_frequency_domain_similarity_threshold && !freq_base.captured)
  {
    auto* w = gui().analog_fl_input_frequency_domain_similarity_threshold;
    freq_base.box       = w->box();
    freq_base.color     = w->color();
    freq_base.textcolor = w->textcolor();
    freq_base.textfont  = w->textfont();
    freq_base.captured  = true;
  }

  const int time_enabled = gui().analog_fl_checkbutton_time_domain ? gui().analog_fl_checkbutton_time_domain->value() : 0;
  const int freq_enabled = gui().analog_fl_checkbutton_frequency_domain ? gui().analog_fl_checkbutton_frequency_domain->value() : 0;

  if (gui().analog_fl_input_time_domain_similarity_threshold)
  {
    if (time_enabled)
    {
      gui().analog_fl_input_time_domain_similarity_threshold->activate();
      gui().analog_fl_input_time_domain_similarity_threshold->box(FL_BORDER_BOX);
      gui().analog_fl_input_time_domain_similarity_threshold->color(FL_BACKGROUND2_COLOR);
      gui().analog_fl_input_time_domain_similarity_threshold->textcolor(FL_BLACK);
      // On enable, populate a sensible default if empty.
      const char* v = gui().analog_fl_input_time_domain_similarity_threshold->value();
      if (!v || *v == '\0')
      {
        gui().analog_fl_input_time_domain_similarity_threshold->value("100%");
      }
    }
    else
    {
      gui().analog_fl_input_time_domain_similarity_threshold->deactivate();
      // Fully restore baseline so the disabled look matches FLUID/theme.
      if (time_base.captured)
      {
        gui().analog_fl_input_time_domain_similarity_threshold->box(time_base.box);
        gui().analog_fl_input_time_domain_similarity_threshold->color(time_base.color);
        gui().analog_fl_input_time_domain_similarity_threshold->textcolor(time_base.textcolor);
        gui().analog_fl_input_time_domain_similarity_threshold->textfont(time_base.textfont);
      }
    }
    gui().analog_fl_input_time_domain_similarity_threshold->redraw();
  }
  if (gui().analog_fl_input_frequency_domain_similarity_threshold)
  {
    if (freq_enabled)
    {
      gui().analog_fl_input_frequency_domain_similarity_threshold->activate();
      gui().analog_fl_input_frequency_domain_similarity_threshold->box(FL_BORDER_BOX);
      gui().analog_fl_input_frequency_domain_similarity_threshold->color(FL_BACKGROUND2_COLOR);
      gui().analog_fl_input_frequency_domain_similarity_threshold->textcolor(FL_BLACK);
      // On enable, populate a sensible default if empty.
      const char* v = gui().analog_fl_input_frequency_domain_similarity_threshold->value();
      if (!v || *v == '\0')
      {
        gui().analog_fl_input_frequency_domain_similarity_threshold->value("100%");
      }
    }
    else
    {
      gui().analog_fl_input_frequency_domain_similarity_threshold->deactivate();
      // Fully restore baseline so the disabled look matches FLUID/theme.
      if (freq_base.captured)
      {
        gui().analog_fl_input_frequency_domain_similarity_threshold->box(freq_base.box);
        gui().analog_fl_input_frequency_domain_similarity_threshold->color(freq_base.color);
        gui().analog_fl_input_frequency_domain_similarity_threshold->textcolor(freq_base.textcolor);
        gui().analog_fl_input_frequency_domain_similarity_threshold->textfont(freq_base.textfont);
      }
    }
    gui().analog_fl_input_frequency_domain_similarity_threshold->redraw();
  }
}

void LABSoft_Presenter_LABChecker_Analog::
cb_toggle_time_domain(Fl_Check_Button *w, void *data)
{
  (void)w;
  (void)data;

  sync_comparison_inputs_enabled();
  presenter().m_Software_Navigation.refresh_widget_list();
}

void LABSoft_Presenter_LABChecker_Analog::
cb_toggle_frequency_domain(Fl_Check_Button *w, void *data)
{
  (void)w;
  (void)data;

  sync_comparison_inputs_enabled();
  presenter().m_Software_Navigation.refresh_widget_list();
}

void LABSoft_Presenter_LABChecker_Analog::
cb_analog_create_file(Fl_Button *w, void *data)
{
  auto *acc = this;
  if (!acc) return;

  // Step 1: Check GUI
  auto gui_ptr = acc->gui().analog_labsoft_gui_analog_checker_display;
  if (!gui_ptr) return;

  // Step 2: Gather oscilloscope state (from parent_data)
  auto &parent_data = acc->lab().m_Oscilloscope.parent_data();
  if (parent_data.channel_data.empty()) return;

  // Comparison settings from UI
  bool cmp_time_domain = acc->gui().analog_fl_checkbutton_time_domain->value();
  bool cmp_frequency_domain = acc->gui().analog_fl_checkbutton_frequency_domain->value();
  double cmp_time_similarity_threshold = 0.0;
  double cmp_frequency_similarity_threshold = 0.0;
  if (auto *fld = acc->gui().analog_fl_input_time_domain_similarity_threshold) {
    try { cmp_time_similarity_threshold = std::stod(fld->value()); } catch (...) { cmp_time_similarity_threshold = 0.0; }
  }
  if (auto *fld = acc->gui().analog_fl_input_frequency_domain_similarity_threshold) {
    try { cmp_frequency_similarity_threshold = std::stod(fld->value()); } catch (...) { cmp_frequency_similarity_threshold = 0.0; }
  }

  // FG snapshot
  LAB_Channel_Data_Function_Generator fg_data;
  auto &fg = acc->lab().m_Function_Generator;
  fg_data.is_enabled = fg.is_running();
  fg_data.wave_type = fg.wave_type(0);
  fg_data.frequency = fg.frequency(0);
  fg_data.period = fg.period(0);
  fg_data.amplitude = fg.amplitude(0);
  fg_data.vertical_offset = fg.vertical_offset(0);
  fg_data.phase = fg.phase(0);

  // Prefer exact values typed in the Function Generator tab over any display/menu rounding
  if (acc->gui().function_generator_fl_input_choice_frequency)
  {
    LABSoft_GUI_Label lbl(
      acc->gui().function_generator_fl_input_choice_frequency->value(),
      fg_data.frequency,
      LABSoft_GUI_Label::UNIT::HERTZ
    );

    if (lbl.is_valid())
    {
      double f = lbl.actual_value();
      if (f > 0.0)
      {
        fg_data.frequency = f;
        fg_data.period = 1.0 / f;
      }
    }
  }
  else if (acc->gui().function_generator_fl_input_choice_period)
  {
    LABSoft_GUI_Label lbl(
      acc->gui().function_generator_fl_input_choice_period->value(),
      fg_data.period,
      LABSoft_GUI_Label::UNIT::SECOND
    );

    if (lbl.is_valid())
    {
      double p = lbl.actual_value();
      if (p > 0.0)
      {
        fg_data.period = p;
        fg_data.frequency = 1.0 / p;
      }
    }
  }

  // Save dialog
  Fl_Native_File_Chooser chooser;
  chooser.title("Save Oscilloscope Data...");
  chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  chooser.preset_file("osc_data.labacc");
  if (chooser.show() != 0) return;
  std::string file_path = chooser.filename();

  // Write file with comparison section
  acc->lab().m_LABChecker_Analog.create_circuit_checker_file(
    file_path,
    parent_data,
    fg_data,
    cmp_time_domain,
    cmp_frequency_domain,
    cmp_time_similarity_threshold,
    cmp_frequency_similarity_threshold
  );

  std::cout << "[SUCCESS] File saved to: " << file_path << std::endl;
}

// EOF
