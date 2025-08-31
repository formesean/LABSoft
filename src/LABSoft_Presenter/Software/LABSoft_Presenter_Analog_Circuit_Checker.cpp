//student side
#include "LABSoft_Presenter_Analog_Circuit_Checker.h"

#include <cstdio>
#include <string>
#include <algorithm>
#include <cmath>

#include <FL/fl_ask.H>

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"
#include "../../Utility/LAB_Utility_Functions.h"

LABSoft_Presenter_Analog_Circuit_Checker::LABSoft_Presenter_Analog_Circuit_Checker(LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  load_gui();
}

void LABSoft_Presenter_Analog_Circuit_Checker::
load_gui()
{
  LABSoft_GUI_Analog_Circuit_Checker_Display& analog_disp_gui = *(gui().analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);

  analog_disp_gui.load_presenter(m_presenter);
}

static inline double clamp01 (double v) { return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v); }

static void print_bool(const char* label, bool v) { std::printf("%s: %s\n", label, v?"true":"false"); }

void LABSoft_Presenter_Analog_Circuit_Checker::
log_metadata_to_terminal () const
{
  std::printf("\n=== ACC METADATA ===\n");
  std::printf("Time/Div: %.9f s\n", m_metadata.time_per_division);
  std::printf("Samples: %u\n", m_metadata.samples);
  std::printf("Sampling Rate: %.9f Hz\n", m_metadata.sampling_rate);
  std::printf("Horizontal Offset: %.9f s\n", m_metadata.horizontal_offset);

  std::printf("\n--- COMPARISON ---\n");
  print_bool("Time Domain", m_metadata.comparison.time_domain);
  print_bool("Frequency Domain", m_metadata.comparison.frequency_domain);
  std::printf("Similarity Threshold: %.4f\n", m_metadata.comparison.similarity_threshold);

  std::printf("\n--- FUNCTION GENERATOR ---\n");
  print_bool("Enabled", m_metadata.function_generator.is_enabled);
  std::printf("Wave Type: %u\n", m_metadata.function_generator.wave_type);
  std::printf("Frequency: %.9f Hz\n", m_metadata.function_generator.frequency);
  std::printf("Period: %.9f s\n", m_metadata.function_generator.period);
  std::printf("Amplitude: %.6f V\n", m_metadata.function_generator.amplitude);
  std::printf("Vertical Offset: %.6f V\n", m_metadata.function_generator.vertical_offset);
  std::printf("Phase: %.6f deg\n", m_metadata.function_generator.phase);

  std::printf("\n--- CHANNELS (%zu) ---\n", m_metadata.channels.size());
  for (size_t i = 0; i < m_metadata.channels.size(); ++i)
  {
    const auto& c = m_metadata.channels[i];
    std::printf("\nChannel %zu\n", i);
    std::printf("  Name: %s\n", c.name.c_str());
    std::printf("  Samples: %u\n", c.samples);
    print_bool("  Coupling(AC)", c.coupling);
    std::printf("  Scaling: %u\n", c.scaling);
    std::printf("  Voltage/Div: %.6f V\n", c.voltage_per_div);
    std::printf("  Vertical Offset: %.6f V\n", c.vertical_offset);
    print_bool("  Enabled", c.is_enabled);
    std::printf("  Scaling Corrector: %.6f\n", c.scaling_corrector);
    std::printf("  Measurements: min=%.6f, max=%.6f, avg=%.6f, trms=%.6f\n",
                c.measurements.min, c.measurements.max, c.measurements.avg, c.measurements.trms);
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
display_signals ()
{
  LABSoft_GUI_Analog_Circuit_Checker_Display& analog_display =
    *(gui().analog_circuit_checker_labsoft_gui_analog_circuit_checker_display);

  if (m_presenter.lab().m_Analog_Circuit_Checker.is_file_loaded()) {
    const auto& channel_data   = m_presenter.lab().m_Analog_Circuit_Checker.get_channel_data();
    const auto& func_gen_data  = m_presenter.lab().m_Analog_Circuit_Checker.get_function_generator_data();
    const auto& analog_checker = m_presenter.lab().m_Analog_Circuit_Checker;

    try {
      // Persist non-signal metadata
      m_metadata.time_per_division = analog_checker.get_time_per_division();
      m_metadata.samples           = analog_checker.get_samples();
      m_metadata.sampling_rate     = analog_checker.get_sampling_rate();
      m_metadata.horizontal_offset = analog_checker.get_horizontal_offset();
      // Persist comparison settings
      m_metadata.comparison.time_domain          = analog_checker.get_cmp_time_domain();
      m_metadata.comparison.frequency_domain     = analog_checker.get_cmp_frequency_domain();
      m_metadata.comparison.similarity_threshold = analog_checker.get_cmp_similarity_threshold();

      m_metadata.channels.clear();
      m_metadata.channels.reserve(channel_data.size());
      for (size_t i = 0; i < channel_data.size(); ++i) {
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

      // Log to terminal
      log_metadata_to_terminal();

      // Configure UI from metadata
      analog_display.time_per_division(m_metadata.time_per_division);
      analog_display.samples(m_metadata.samples);
      analog_display.sampling_rate(m_metadata.sampling_rate);

      // Convert sample data to pixel points for display (CH2 only)
      LABSoft_GUI_Analog_Circuit_Checker_Display::PixelPoints pixel_points;
      for (size_t idx = 0; idx < channel_data.size() && idx < LABC::OSC::NUMBER_OF_CHANNELS; ++idx) {
        if (idx != 1) continue;
        const auto& channel = channel_data[idx];

        pixel_points[0].clear();
        pixel_points[1].clear();

        if (channel.is_enabled && !channel.sample_data.empty()) {
          const unsigned display_width  = analog_display.display_width();
          const unsigned display_height = analog_display.display_height();

          const size_t N = channel.sample_data.size();
          pixel_points[1].reserve(N);

          const double voltage_per_division = std::max(1e-9, channel.voltage_per_division);
          const double voltage_range        = voltage_per_division * 8.0; // 8 vertical divisions

          for (size_t i = 0; i < N; ++i) {
            int x = static_cast<int>((static_cast<double>(i) * display_width) / std::max<size_t>(1, N));

            double normalized_voltage = (channel.sample_data[i] + channel.vertical_offset + (voltage_range / 2.0)) / voltage_range;
            normalized_voltage = clamp01(normalized_voltage);
            int y = static_cast<int>(display_height * (1.0 - normalized_voltage));

            x = std::max(0, std::min(x, static_cast<int>(display_width - 1)));
            y = std::max(0, std::min(y, static_cast<int>(display_height - 1)));

            pixel_points[1].push_back({x, y});
          }
        }
      }

      analog_display.load_pixel_points(pixel_points);
      analog_display.channel_enable_disable(0, false);
      analog_display.channel_enable_disable(1, true);
      analog_display.update_display();

      // Optional comparisons removed for now to resolve unresolved references

    } catch (const std::exception& e) {
      fl_message("Error updating analog circuit checker display: %s", e.what());
    }
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_display ()
{
  LABSoft_GUI& gui = m_presenter.gui ();
  gui.analog_circuit_checker_labsoft_gui_analog_circuit_checker_display->update_display();
}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_load_file_acc (Fl_Button*  w,
                     void*    data)
{
  Fl_Native_File_Chooser chooser;

  chooser.title   ("Open Analog Circuit Checker Template File");
  chooser.type    (Fl_Native_File_Chooser::BROWSE_FILE);
  chooser.filter  ("LAB Analog Circuit Checker\t*.labacc");

  Fl_Output& selected_file = *(m_presenter.gui ().analog_circuit_checker_fl_output_selected_file);

  try
  {
    switch (chooser.show ())
    {
      case (0):
      {
        std::string path (chooser.filename ());

        if (LABF::has_filename_this_extension(path,
          LABC::LABSOFT::ANALOG_CIRCUIT_CHECKER_FILENAME_EXTENSION))
        {
          update_gui_display ();

          selected_file.value (LABF::get_filename_from_path(path).c_str());

          // Load the .labacc file
          m_presenter.lab ().m_Analog_Circuit_Checker.load_file (path);

          fl_message("File loaded successfully. Click 'Run Checker' to display data in oscilloscope.");
        }
        else
        {
          throw (std::runtime_error ("Invalid file selected."));
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
  catch (const std::exception& e)
  {
    fl_message (e.what ());

    selected_file.value ("");
  }

}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_run_checker_acc (Fl_Button*  w,
                     void*      data)
{
  std::printf("\n=== ANALOG CIRCUIT CHECKER - RUN CHECKER TRIGGERED ===\n");

  try
  {
    if (!m_presenter.lab().m_Analog_Circuit_Checker.is_file_loaded())
    {
      fl_message("Please load a .labacc file first before running the checker.");
      return;
    }

    display_signals();
    std::printf("\n=== ANALOG CIRCUIT CHECKER - COMPLETED ===\n\n");
  }
  catch (const std::exception& e)
  {
    fl_message("Error running analog circuit checker: %s", e.what());
  }
}
