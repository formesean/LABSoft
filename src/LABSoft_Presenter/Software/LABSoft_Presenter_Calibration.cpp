#include "LABSoft_Presenter_Calibration.h"

#include <cstdio>
#include <string>

#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../Utility/LAB_Constants.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"
#include "../../Utility/LAB_Utility_Functions.h"

LABSoft_Presenter_Calibration::
LABSoft_Presenter_Calibration (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit  (_LABSoft_Presenter)
{
  init_gui_values               ();
  apply_calibration_data_to_gui (lab ().m_Calibration.current_calibration_data ());
}

void LABSoft_Presenter_Calibration:: 
init_gui_values ()
{
  init_gui_oscilloscope_calibration ();
  init_gui_ohmmeter_calibration     ();
}

void LABSoft_Presenter_Calibration:: 
init_gui_oscilloscope_calibration ()
{
  // light button
  if (lab ().m_Oscilloscope.calibration ().is_enabled ())
  {
    gui ().calibration_fl_light_button_oscilloscope_enable_calibration->set ();
  }
  else 
  {
    gui ().calibration_fl_light_button_oscilloscope_enable_calibration->clear ();
  }

  // table
  LABSoft_GUI_Sheet& table = 
    *(gui ().calibration_labsoft_gui_sheet_oscilloscope_calibration_table);
  
  table.dimensions (LABC::OSC::NUMBER_OF_CALIBRATION_PARAMETERS,
    LABC::OSC::NUMBER_OF_CHANNELS);

  table.col_header  (1);
  table.row_header  (1);

  table.row_header_width  (150);
  table.row_height_all    (50);

  table.range (0, 1)  .value ("Channel 1");
  table.range (0, 2)  .value ("Channel 2");
  table.range (1, 0)  .value ("ADC Vref");
  table.range (2, 0)  .value ("Vertical Offset\nCorrector x2");
  table.range (3, 0)  .value ("Vertical Offset\nCorrector x0.5");
  table.range (4, 0)  .value ("Vertical Offset\nCorrector x0.25");
  table.range (5, 0)  .value ("Vertical Offset\nCorrector x0.125");
  table.range (6, 0)  .value ("Scaling Corrector\nto Unity x2");
  table.range (7, 0)  .value ("Scaling Corrector\nto Unity x0.5");
  table.range (8, 0)  .value ("Scaling Corrector\nto Unity x0.25");
  table.range (9, 0)  .value ("Scaling Corrector\nto Unity x0.125");
  table.range (10, 0) .value ("Scaling Corrector\nto Actual x2");
  table.range (11, 0) .value ("Scaling Corrector\nto Actual x0.5");
  table.range (12, 0) .value ("Scaling Corrector\nto Actual x0.25");
  table.range (13, 0) .value ("Scaling Corrector\nto Actual x0.125");
}

void LABSoft_Presenter_Calibration:: 
init_gui_ohmmeter_calibration ()
{
  // light button
  if (lab ().m_Ohmmeter.calibration ().is_enabled ())
  {
    gui ().calibration_fl_light_button_ohmmeter_enable_calibration->set ();
  }
  else 
  {
    gui ().calibration_fl_light_button_ohmmeter_enable_calibration->clear ();
  }
}

void LABSoft_Presenter_Calibration:: 
apply_calibration_data_to_gui (const LAB_Calibration_Data& data)
{
  LABSoft_GUI_Sheet& table = 
    *(gui ().calibration_labsoft_gui_sheet_oscilloscope_calibration_table);

  // oscilloscope
  {
    // channel 0
    {
      // adc_vref
      table.range (1, 1).value (
        std::to_string (data.at ("osc_adc_vref_chan_0")).c_str ()
      );

      // vertical_offset_corrector
      table.range (2, 1).value (
        std::to_string (data.at ("osc_vertical_offset_corrector_double_chan_0")).c_str ()
      );

      table.range (3, 1).value (
        std::to_string (data.at ("osc_vertical_offset_corrector_half_chan_0")).c_str ()
      );

      table.range (4, 1).value (
        std::to_string (data.at ("osc_vertical_offset_corrector_fourth_chan_0")).c_str ()
      );

      table.range (5, 1).value (
        std::to_string (data.at ("osc_vertical_offset_corrector_eighth_chan_0")).c_str ()
      );

      // scaling_corrector_to_unity
      table.range (6, 1).value (
        std::to_string (data.at ("osc_scaling_corrector_to_unity_double_chan_0")).c_str ()
      );

      table.range (7, 1).value (
        std::to_string (data.at ("osc_scaling_corrector_to_unity_half_chan_0")).c_str ()
      );

      table.range (8, 1).value (
        std::to_string (data.at ("osc_scaling_corrector_to_unity_fourth_chan_0")).c_str ()
      );

      table.range (9, 1).value (
        std::to_string (data.at ("osc_scaling_corrector_to_unity_eighth_chan_0")).c_str ()
      );

      // scaling_corrector_to_actual
      table.range (10, 1).value (
        std::to_string (data.at ("osc_scaling_corrector_to_actual_double_chan_0")).c_str ()
      );

      table.range (11, 1).value (
        std::to_string (data.at ("osc_scaling_corrector_to_actual_half_chan_0")).c_str ()
      );

      table.range (12, 1).value (
        std::to_string (data.at ("osc_scaling_corrector_to_actual_fourth_chan_0")).c_str ()
      );

      table.range (13, 1).value (
        std::to_string (data.at ("osc_scaling_corrector_to_actual_eighth_chan_0")).c_str ()
      );
    }

    // channel 1
    {
      // adc_vref
      table.range (1, 2).value (
        std::to_string (data.at ("osc_adc_vref_chan_1")).c_str ()
      );

      // vertical_offset_corrector
      table.range (2, 2).value (
        std::to_string (data.at ("osc_vertical_offset_corrector_double_chan_1")).c_str ()
      );

      table.range (3, 2).value (
        std::to_string (data.at ("osc_vertical_offset_corrector_half_chan_1")).c_str ()
      );

      table.range (4, 2).value (
        std::to_string (data.at ("osc_vertical_offset_corrector_fourth_chan_1")).c_str ()
      );

      table.range (5, 2).value (
        std::to_string (data.at ("osc_vertical_offset_corrector_eighth_chan_1")).c_str ()
      );

      // scaling_corrector_to_unity
      table.range (6, 2).value (
        std::to_string (data.at ("osc_scaling_corrector_to_unity_double_chan_1")).c_str ()
      );

      table.range (7, 2).value (
        std::to_string (data.at ("osc_scaling_corrector_to_unity_half_chan_1")).c_str ()
      );

      table.range (8, 2).value (
        std::to_string (data.at ("osc_scaling_corrector_to_unity_fourth_chan_1")).c_str ()
      );

      table.range (9, 2).value (
        std::to_string (data.at ("osc_scaling_corrector_to_unity_eighth_chan_1")).c_str ()
      );

      // scaling_corrector_to_actual
      table.range (10, 2).value (
        std::to_string (data.at ("osc_scaling_corrector_to_actual_double_chan_1")).c_str ()
      );

      table.range (11, 2).value (
        std::to_string (data.at ("osc_scaling_corrector_to_actual_half_chan_1")).c_str ()
      );

      table.range (12, 2).value (
        std::to_string (data.at ("osc_scaling_corrector_to_actual_fourth_chan_1")).c_str ()
      );

      table.range (13, 2).value (
        std::to_string (data.at ("osc_scaling_corrector_to_actual_eighth_chan_1")).c_str ()
      );
    }
  }
  
  // ohmmeter
  {
    gui ().calibration_labsoft_gui_fl_input_ohm_vref_chan_0->value (
      std::to_string (data.at ("ohm_vref_chan_0")).c_str ()
    );

    gui ().calibration_labsoft_gui_fl_input_ohm_r1_chan_0->value (
      std::to_string (data.at ("ohm_r1_chan_0")).c_str ()
    );
  }
}

LAB_Calibration_Data LABSoft_Presenter_Calibration:: 
get_calibration_data_from_gui () const
{
  LABSoft_GUI_Sheet& table = 
    *(gui ().calibration_labsoft_gui_sheet_oscilloscope_calibration_table);
  
  LAB_Calibration_Data data;

  // oscilloscope
  {
    // channel 1
    {
      // adc vref
      data.emplace (
        "osc_adc_vref_chan_0", 
        std::stod (table.range (1, 1).value ())
      );
      
      // vertical_offset_corrector
      data.emplace (
        "osc_vertical_offset_corrector_double_chan_0", 
        std::stod (table.range (2, 1).value ())
      );
      
      data.emplace (
        "osc_vertical_offset_corrector_half_chan_0", 
        std::stod (table.range (3, 1).value ())
      );
      
      data.emplace (
        "osc_vertical_offset_corrector_fourth_chan_0", 
        std::stod (table.range (4, 1).value ())
      );
    
      data.emplace (
        "osc_vertical_offset_corrector_eighth_chan_0", 
        std::stod (table.range (5, 1).value ())
      );
      
      // scaling_corrector_to_unity
      data.emplace (
        "osc_scaling_corrector_to_unity_double_chan_0", 
        std::stod (table.range (6, 1).value ())
      );
      
      data.emplace (
        "osc_scaling_corrector_to_unity_half_chan_0", 
        std::stod (table.range (7, 1).value ())
      );
      
      data.emplace (
        "osc_scaling_corrector_to_unity_fourth_chan_0", 
        std::stod (table.range (8, 1).value ())
      );
    
      data.emplace (
        "osc_scaling_corrector_to_unity_eighth_chan_0", 
        std::stod (table.range (9, 1).value ())
      );
      
      // scaling_corrector_to_actual
      data.emplace (
        "osc_scaling_corrector_to_actual_double_chan_0", 
        std::stod (table.range (10, 1).value ())
      );
      
      data.emplace (
        "osc_scaling_corrector_to_actual_half_chan_0", 
        std::stod (table.range (11, 1).value ())
      );
      
      data.emplace (
        "osc_scaling_corrector_to_actual_fourth_chan_0", 
        std::stod (table.range (12, 1).value ())
      );
    
      data.emplace (
        "osc_scaling_corrector_to_actual_eighth_chan_0", 
        std::stod (table.range (13, 1).value ())
      );
    }
  
    // channel 2
    {
      data.emplace (
        "osc_adc_vref_chan_1", 
        std::stod (table.range (1, 2).value ())
      );
      
      // vertical_offset_corrector
      data.emplace (
        "osc_vertical_offset_corrector_double_chan_1", 
        std::stod (table.range (2, 2).value ())
      );
      
      data.emplace (
        "osc_vertical_offset_corrector_half_chan_1", 
        std::stod (table.range (3, 2).value ())
      );
      
      data.emplace (
        "osc_vertical_offset_corrector_fourth_chan_1", 
        std::stod (table.range (4, 2).value ())
      );
    
      data.emplace (
        "osc_vertical_offset_corrector_eighth_chan_1", 
        std::stod (table.range (5, 2).value ())
      );
      
      // scaling_corrector_to_unity
      data.emplace (
        "osc_scaling_corrector_to_unity_double_chan_1", 
        std::stod (table.range (6, 2).value ())
      );
      
      data.emplace (
        "osc_scaling_corrector_to_unity_half_chan_1", 
        std::stod (table.range (7, 2).value ())
      );
      
      data.emplace (
        "osc_scaling_corrector_to_unity_fourth_chan_1", 
        std::stod (table.range (8, 2).value ())
      );
    
      data.emplace (
        "osc_scaling_corrector_to_unity_eighth_chan_1", 
        std::stod (table.range (9, 2).value ())
      );
      
      // scaling_corrector_to_actual
      data.emplace (
        "osc_scaling_corrector_to_actual_double_chan_1", 
        std::stod (table.range (10, 2).value ())
      );
      
      data.emplace (
        "osc_scaling_corrector_to_actual_half_chan_1", 
        std::stod (table.range (11, 2).value ())
      );
      
      data.emplace (
        "osc_scaling_corrector_to_actual_fourth_chan_1", 
        std::stod (table.range (12, 2).value ())
      );
    
      data.emplace (
        "osc_scaling_corrector_to_actual_eighth_chan_1", 
        std::stod (table.range (13, 2).value ())
      );
    }
  }

  // ohmmeter
  {
    data.emplace (
      "ohm_r1_chan_0", 
      std::stod (gui ().calibration_labsoft_gui_fl_input_ohm_r1_chan_0->value ())
    );
      
    data.emplace (
      "ohm_vref_chan_0", 
      std::stod (gui ().calibration_labsoft_gui_fl_input_ohm_vref_chan_0->value ())
    );
  }

  return (data);  
}

void LABSoft_Presenter_Calibration:: 
cb_open ()
{
  Fl_Native_File_Chooser fnfc;

  fnfc.title        ("Open Calibration File");
  fnfc.type         (Fl_Native_File_Chooser::BROWSE_FILE);
  fnfc.filter       (LABC::LABSOFT::CALIBRATION_FILENAME_EXTENSION);
  fl_message_title  ("Open Calibration File");

  switch (fnfc.show ())
  {
    case 0:
    {
      try 
      {
        std::string path (fnfc.filename ());

        if (LABF::has_filename_this_extension (path,
          LABC::LABSOFT::CALIBRATION_FILENAME_EXTENSION))
        {
          lab ().m_Calibration.load_calibration_file (path);

          apply_calibration_data_to_gui (lab ().m_Calibration.calibration_data_incoming ());

          fl_message ("Calibration values loaded.");
        }
        else 
        {
          throw (std::runtime_error ("Selected file is not a valid calibration file."));
        }
      }
      catch (const std::exception& e)
      {
        fl_alert ("%s%s", "Error occurred: ", e.what ());
      }
      
      break;
    }

    case 1: // cancel
    {
      break;
    }

    case -1: // error
    {
      fl_alert ("%s%s", "Error occured: ", fnfc.errmsg ());

      break;
    }
  }
}

void LABSoft_Presenter_Calibration:: 
cb_save_to_file ()
{
  Fl_Native_File_Chooser fnfc;

  fnfc.title        ("Save to Calibration File");
  fnfc.type         (Fl_Native_File_Chooser::BROWSE_SAVE_FILE );
  fnfc.directory    ("/");
  fnfc.preset_file  (std::string ("custom" + 
                      std::string (LABC::LABSOFT::CALIBRATION_FILENAME_EXTENSION)).c_str ());
  fnfc.options      (Fl_Native_File_Chooser::NEW_FOLDER | 
                      Fl_Native_File_Chooser::SAVEAS_CONFIRM);

  fl_message_title  ("Save to Calibration File");

  switch (fnfc.show ())
  {
    case 0:
    {
      try 
      {
        lab ().m_Calibration.save_calibration_data_to_file (fnfc.filename ());

        fl_message ("Calibration values saved to file.");
      }
      catch (const std::exception& e)
      {
        fl_alert ("%s%s", "Error occurred: ", e.what ());
      }
      
      break;
    }

    case 1: // cancel
    {
      break;
    }  

    case -1: // error
    {
      fl_alert ("%s%s", "Error occured: ", fnfc.errmsg ());

      break;
    }
  }
}

void LABSoft_Presenter_Calibration:: 
cb_save_as_default ()
{
  switch (fl_choice ("Do you want to save the calibration values as the default?", "No", "Yes", 0))
  {
    case 1: // Yes
    {
      lab ().m_Calibration.save_calibration_data_to_default ();

      fl_message ("Calibration values saved as the default.");

      break;
    }

    case 0: // No
    {
      break;
    }
  }

}

void LABSoft_Presenter_Calibration::
cb_apply ()
{
  fl_message_title ("Device Calibration");

  switch (fl_choice ("Do you want to apply the calibration changes?", "No", "Yes", 0))
  {
    case 1: // Yes
    {
      lab ().m_Calibration.load_calibration_data  (get_calibration_data_from_gui ());
      lab ().m_Calibration.apply_calibration_data ();

      apply_calibration_data_to_gui (lab ().m_Calibration.current_calibration_data ());
      fl_message                    ("Calibration values applied.");

      break;
    }

    case 0: // No
    {
      break;
    }
  }
}

void LABSoft_Presenter_Calibration:: 
cb_discard_changes ()
{
  fl_message_title ("Discard Changes");

  switch (fl_choice ("Are you sure you want to discard your changes?", "No", "Yes", 0))
  {
    case 0: // no
    {
      break;
    }

    case 1: // yes
    {
      lab ().m_Calibration.discard_calibration_data_changes ();

      apply_calibration_data_to_gui (lab ().m_Calibration.calibration_data_incoming ());

      fl_message ("Changes discarded.");

      break;
    }
  }
}

void LABSoft_Presenter_Calibration:: 
cb_load_factory ()
{
  fl_message_title ("Load Factory");

  switch (fl_choice ("Do you want to load and apply the factory calibration values?", "No", "Yes", 0))
  {
    case 1: // Yes
    {
      lab ().m_Calibration.load_factory_calibration_data ();

      apply_calibration_data_to_gui (lab ().m_Calibration.calibration_data_incoming ());

      fl_message ("Factory calibration values loaded and applied.");

      break;
    }

    case 0: // No
    {
      break;
    }
  }
}

void LABSoft_Presenter_Calibration::
cb_show_window ()
{
  gui ().main_fl_window_calibration->set_modal ();
  gui ().main_fl_window_calibration->show ();

  apply_calibration_data_to_gui (lab ().m_Calibration.calibration_data_incoming ());
}

void LABSoft_Presenter_Calibration::
cb_close ()
{
  gui ().main_fl_window_calibration->clear_modal_states ();
  gui ().main_fl_window_calibration->hide ();
}

void LABSoft_Presenter_Calibration:: 
cb_oscilloscope_enable_calibration (Fl_Light_Button* w, 
                                    void*            data)
{
  if (w->value ())
  {
    lab ().m_Oscilloscope.calibration ().enable ();
  }
  else 
  {
    lab ().m_Oscilloscope.calibration ().disable ();
  }
}

void LABSoft_Presenter_Calibration:: 
cb_ohmmeter_enable_calibration (Fl_Light_Button* w, 
                                void*            data)
{
  if (w->value ())
  {
    lab ().m_Ohmmeter.calibration ().enable ();
  }
  else 
  {
    lab ().m_Ohmmeter.calibration ().disable ();
  }
}

// eof