//student side
#include "LABSoft_Presenter_Analog_Circuit_Checker.h"

#include <cstdio>
#include <string>
#include <algorithm>

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
  LABSoft_GUI_LABChecker_Analog_Checker_Display& analog_disp_gui = *(gui ().analog_labsoft_gui_analog_checker_display);

   analog_disp_gui.load_presenter         (m_presenter);

  
}

void LABSoft_Presenter_Analog_Circuit_Checker::
display_signals ()
{
  // Update the analog circuit checker display to show the loaded signals
  LABSoft_GUI_LABChecker_Analog_Checker_Display& analog_display = 
    *(gui().analog_labsoft_gui_analog_checker_display);
  
  if (m_presenter.lab().m_Analog_Circuit_Checker.is_file_loaded()) {
    const auto& channel_data = m_presenter.lab().m_Analog_Circuit_Checker.get_channel_data();
    const auto& func_gen_data = m_presenter.lab().m_Analog_Circuit_Checker.get_function_generator_data();
    
    try {
      // Configure display parameters from .labacc metadata
      const auto& analog_checker = m_presenter.lab().m_Analog_Circuit_Checker;
      
      // Set oscilloscope-like parameters from loaded data
      analog_display.time_per_division(analog_checker.get_time_per_division());
      analog_display.samples(analog_checker.get_samples());
      analog_display.sampling_rate(analog_checker.get_sampling_rate());
      
      // Configure channel parameters
      for (size_t i = 0; i < channel_data.size() && i < LABC::OSC::NUMBER_OF_CHANNELS; ++i) {
        const auto& channel = channel_data[i];
        analog_display.voltage_per_division(i, channel.voltage_per_division);
      }
      
      // Convert sample data to pixel points for display
      LABSoft_GUI_LABChecker_Analog_Checker_Display::PixelPoints pixel_points;
      
      // Convert waveform data to pixel coordinates
      for (size_t ch = 0; ch < channel_data.size() && ch < LABC::OSC::NUMBER_OF_CHANNELS; ++ch) {
        const auto& channel = channel_data[ch];
        
        if (channel.is_enabled && !channel.sample_data.empty()) {
          pixel_points[ch].clear();
          
          // Calculate display dimensions
          unsigned display_width = analog_display.display_width();
          unsigned display_height = analog_display.display_height();
          
          // Convert sample data to pixel coordinates
          for (size_t i = 0; i < channel.sample_data.size(); ++i) {
            // Calculate X position (time axis)
            int x = static_cast<int>((i * display_width) / channel.sample_data.size());
            
            // Calculate Y position (voltage axis) - invert Y because screen coordinates
            double voltage_range = channel.voltage_per_division * 8; // 8 divisions typical
            double normalized_voltage = (channel.sample_data[i] + voltage_range/2) / voltage_range;
            int y = static_cast<int>(display_height * (1.0 - normalized_voltage));
            
            // Clamp to display bounds
            x = std::max(0, std::min(x, static_cast<int>(display_width - 1)));
            y = std::max(0, std::min(y, static_cast<int>(display_height - 1)));
            
            pixel_points[ch].push_back({x, y});
          }
        }
      }
      
      // Load the pixel points into the display
      analog_display.load_pixel_points(pixel_points);
      
      // Update the display
      analog_display.update_display();
      
    } catch (const std::exception& e) {
      fl_message("Error updating analog circuit checker display: %s", e.what());
    }
  }
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_display ()
{
  LABSoft_GUI& gui = m_presenter.gui ();

  // Update the analog circuit checker display
  gui.analog_labsoft_gui_analog_checker_display->update_display();
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

          // Load the .labacc file (but don't load data into oscilloscope yet)
          std::printf("\n=== LOADING .labacc FILE ===\n");
          std::printf("File path: %s\n", path.c_str());
          
          m_presenter.lab ().m_Analog_Circuit_Checker.load_file (path);
          
          std::printf("✓ File loaded successfully into LAB_Analog_Circuit_Checker\n");
          std::printf("File loaded status: %s\n", 
                     m_presenter.lab().m_Analog_Circuit_Checker.is_file_loaded() ? "TRUE" : "FALSE");
          std::printf("=== FILE LOADING COMPLETED ===\n\n");
          
          fl_message("File loaded successfully. Click 'Run Checker' to display data in oscilloscope.");
        }
        else
        {
          throw (std::runtime_error ("Invalid file selected."));
        }

        break;
      }

      // user cancelled
      case (1):
      {
        break;
      }

      // open failed
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
    // Check if a .labacc file has been loaded
    std::printf("Checking if .labacc file is loaded...\n");
    
    if (!m_presenter.lab().m_Analog_Circuit_Checker.is_file_loaded())
    {
      std::printf("ERROR: No .labacc file is currently loaded!\n");
      std::printf("Showing message dialog to user...\n");
      fl_message("Please load a .labacc file first before running the checker.");
      std::printf("Message dialog closed.\n");
      return;
    }
    
    std::printf("✓ .labacc file is loaded successfully!\n");
    
    // Get reference to the analog circuit checker
    const auto& analog_checker = m_presenter.lab().m_Analog_Circuit_Checker;
    
    // Print metadata information
    std::printf("\n--- METADATA ---\n");
    std::printf("Channels: %u\n", analog_checker.get_samples() > 0 ? 1 : 0); 
    std::printf("Samples: %u\n", analog_checker.get_samples());
    std::printf("Sampling Rate: %.2f Hz\n", analog_checker.get_sampling_rate());
    std::printf("Time per Division: %.6f s\n", analog_checker.get_time_per_division());
    std::printf("Horizontal Offset: %.6f s\n", analog_checker.get_horizontal_offset());
    
    // Get and display channel data
    const auto& channel_data = analog_checker.get_channel_data();
    std::printf("\n--- CHANNEL DATA ---\n");
    std::printf("Number of channels loaded: %zu\n", channel_data.size());
    
    for (size_t i = 0; i < channel_data.size(); ++i)
    {
      const auto& channel = channel_data[i];
      std::printf("\nChannel %zu:\n", i);
      std::printf("  Name: %s\n", channel.name.c_str());
      std::printf("  Samples: %u\n", channel.samples);
      std::printf("  Coupling: %s\n", channel.coupling ? "AC" : "DC");
      std::printf("  Scaling: %u\n", channel.scaling);
      std::printf("  Voltage per Division: %.3f V\n", channel.voltage_per_division);
      std::printf("  Vertical Offset: %.3f V\n", channel.vertical_offset);
      std::printf("  Enabled: %s\n", channel.is_enabled ? "Yes" : "No");
      std::printf("  Scaling Corrector: %.6f\n", channel.scaling_corrector);
      std::printf("  Sample Data Size: %zu\n", channel.sample_data.size());
      
      // Print measurements
      std::printf("  Measurements:\n");
      std::printf("    Min: %.6f V\n", channel.measurements.min);
      std::printf("    Max: %.6f V\n", channel.measurements.max);
      std::printf("    Avg: %.6f V\n", channel.measurements.avg);
      std::printf("    TRMS: %.6f V\n", channel.measurements.trms);
      
      // Print first 10 sample values 
      if (!channel.sample_data.empty())
      {
        std::printf("  First 10 sample values: ");
        size_t samples_to_show = std::min(static_cast<size_t>(10), channel.sample_data.size());
        for (size_t j = 0; j < samples_to_show; ++j)
        {
          std::printf("%.3f", channel.sample_data[j]);
          if (j < samples_to_show - 1) std::printf(", ");
        }
        if (channel.sample_data.size() > 10) 
        {
          std::printf(", ... (%zu total)", channel.sample_data.size());
        }
        std::printf("\n");
      }
    }
    
    // Get and display function generator data
    const auto& func_gen_data = analog_checker.get_function_generator_data();
    std::printf("\n--- FUNCTION GENERATOR DATA ---\n");
    std::printf("Wave Type: %u\n", func_gen_data.wave_type);
    std::printf("Frequency: %.2f Hz\n", func_gen_data.frequency);
    std::printf("Period: %.6f s\n", func_gen_data.period);
    
    // Get reference to the analog circuit checker display
    LABSoft_GUI_LABChecker_Analog_Checker_Display& analog_display = 
      *(gui().analog_labsoft_gui_analog_checker_display);
    
    std::printf("\n--- DISPLAYING SIGNALS ---\n");
    std::printf("Calling display_signals() to show data in oscilloscope...\n");
    
    // Display the reference signals in the analog circuit checker display
    display_signals();
    
    std::printf("✓ Signals displayed successfully!\n");
    std::printf("=== ANALOG CIRCUIT CHECKER - COMPLETED ===\n\n");
    
    fl_message("Analog circuit checker reference data displayed successfully.\nCheck terminal for detailed data output.");
  }
  catch (const std::exception& e)
  {
    std::printf("ERROR CAUGHT: %s\n", e.what());
    std::printf("=== ANALOG CIRCUIT CHECKER - ERROR ===\n\n");
    fl_message("Error running analog circuit checker: %s", e.what());
  }
} 