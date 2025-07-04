#include "LAB_Calibration.h"

#include <cctype>
#include <locale>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "../LAB.h"
#include "../../Utility/LAB_Constants.h"
#include "../../Utility/LAB_Utility_Functions.h"

LAB_Calibration:: 
LAB_Calibration (LAB& _LAB)
  : LAB_Module (_LAB)
{
  load_factory_calibration_file (LABC::LABSOFT::FACTORY_CALIBRATION_FILE_PATH);
  load_calibration_file         (LABC::LABSOFT::DEFAULT_CALIBRATION_FILE_PATH);
  apply_calibration_data        ();
}

void LAB_Calibration:: 
load_factory_calibration_file (const std::string& file_path)
{
  std::ifstream file (file_path);

  if (file.is_open ())
  {
    m_factory_calibration_data = parse_calibration_file (file_path);

    file.close ();
  }
  else 
  {
    throw (std::runtime_error ("Can't find/load factory calibration file in given path: " + file_path));
  }
}

void LAB_Calibration:: 
apply_calibration_data_to_instruments (const LAB_Calibration_Data& data)
{
  // osc
  {
    // channel 0
    { 
      // adc_vref
      lab ().m_Oscilloscope.calibration ().adc_reference_voltage (0,
        data.at ("osc_adc_vref_chan_0")); 

      // vertical_offset_corrector
      lab ().m_Oscilloscope.calibration ().vertical_offset_corrector (
        0,
        LABE::OSC::SCALING::DOUBLE,
        data.at ("osc_vertical_offset_corrector_double_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().vertical_offset_corrector (
        0,
        LABE::OSC::SCALING::HALF,
        data.at ("osc_vertical_offset_corrector_half_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().vertical_offset_corrector (
        0,
        LABE::OSC::SCALING::FOURTH,
        data.at ("osc_vertical_offset_corrector_fourth_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().vertical_offset_corrector (
        0,
        LABE::OSC::SCALING::EIGHTH,
        data.at ("osc_vertical_offset_corrector_eighth_chan_0")
      );

      // scaling_corrector_to_unity
      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_unity (
        0,
        LABE::OSC::SCALING::DOUBLE,
        data.at ("osc_scaling_corrector_to_unity_double_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_unity (
        0,
        LABE::OSC::SCALING::HALF,
        data.at ("osc_scaling_corrector_to_unity_half_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_unity (
        0,
        LABE::OSC::SCALING::FOURTH,
        data.at ("osc_scaling_corrector_to_unity_fourth_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_unity (
        0,
        LABE::OSC::SCALING::EIGHTH,
        data.at ("osc_scaling_corrector_to_unity_eighth_chan_0")
      );

      // scaling_corrector_to_actual
      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
        0,
        LABE::OSC::SCALING::DOUBLE,
        data.at ("osc_scaling_corrector_to_actual_double_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
        0,
        LABE::OSC::SCALING::HALF,
        data.at ("osc_scaling_corrector_to_actual_half_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
        0,
        LABE::OSC::SCALING::FOURTH,
        data.at ("osc_scaling_corrector_to_actual_fourth_chan_0")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
        0,
        LABE::OSC::SCALING::EIGHTH,
        data.at ("osc_scaling_corrector_to_actual_eighth_chan_0")
      );
    }

    { // chan 1
      lab ().m_Oscilloscope.calibration ().adc_reference_voltage (1,
        data.at ("osc_adc_vref_chan_1")); 

      // vertical_offset_corrector
      lab ().m_Oscilloscope.calibration ().vertical_offset_corrector (
        1,
        LABE::OSC::SCALING::DOUBLE,
        data.at ("osc_vertical_offset_corrector_double_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().vertical_offset_corrector (
        1,
        LABE::OSC::SCALING::HALF,
        data.at ("osc_vertical_offset_corrector_half_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().vertical_offset_corrector (
        1,
        LABE::OSC::SCALING::FOURTH,
        data.at ("osc_vertical_offset_corrector_fourth_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().vertical_offset_corrector (
        1,
        LABE::OSC::SCALING::EIGHTH,
        data.at ("osc_vertical_offset_corrector_eighth_chan_1")
      );

      // scaling_corrector_to_unity
      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_unity (
        1,
        LABE::OSC::SCALING::DOUBLE,
        data.at ("osc_scaling_corrector_to_unity_double_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_unity (
        1,
        LABE::OSC::SCALING::HALF,
        data.at ("osc_scaling_corrector_to_unity_half_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_unity (
        1,
        LABE::OSC::SCALING::FOURTH,
        data.at ("osc_scaling_corrector_to_unity_fourth_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_unity (
        1,
        LABE::OSC::SCALING::EIGHTH,
        data.at ("osc_scaling_corrector_to_unity_eighth_chan_1")
      );

      // scaling_corrector_to_actual
      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
        1,
        LABE::OSC::SCALING::DOUBLE,
        data.at ("osc_scaling_corrector_to_actual_double_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
        1,
        LABE::OSC::SCALING::HALF,
        data.at ("osc_scaling_corrector_to_actual_half_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
        1,
        LABE::OSC::SCALING::FOURTH,
        data.at ("osc_scaling_corrector_to_actual_fourth_chan_1")
      );

      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
        1,
        LABE::OSC::SCALING::EIGHTH,
        data.at ("osc_scaling_corrector_to_actual_eighth_chan_1")
      );
    }
  }

  // ohmmeter
  lab ().m_Ohmmeter.calibration ().vref (0,
    data.at ("ohm_vref_chan_0"));

  lab ().m_Ohmmeter.calibration ().r1 (0, 
    data.at ("ohm_r1_chan_0"));
}

LAB_Calibration_Data LAB_Calibration:: 
parse_calibration_file (const std::string& file_path)
{
  std::ifstream file (file_path);

  if (file.is_open ())
  {
    std::string           line;
    LAB_Calibration_Data  calibration_data;

    while (std::getline (file, line))
    {
      double      value;
      std::string key;

      if (line[0] == '#')       // skip lines that begin with a pound sign, are comments
      {
        continue;
      }
      else 
      {
        LABF::str_remove_whitespaces (line);

        size_t equal_sign_pos = line.find ('=');

        if (equal_sign_pos != std::string::npos)
        {
          key   = line.substr (0, equal_sign_pos);
          value = std::stod (line.substr (equal_sign_pos + 1));

          calibration_data[key] = value;
        }
      }
    }

    file.close ();

    return (calibration_data);
  }
  else 
  {
    throw (std::runtime_error ("Can't find/load calibration file in given path: " + file_path));
  }
}

void LAB_Calibration:: 
load_calibration_file (const std::string& file_path)
{
  m_calibration_data_incoming = parse_calibration_file (file_path);
}

void LAB_Calibration:: 
load_calibration_data (const LAB_Calibration_Data& data)
{
  m_calibration_data_incoming = data;
}

void LAB_Calibration:: 
load_factory_calibration_data ()
{ 
  m_calibration_data_incoming = m_factory_calibration_data;
}

void LAB_Calibration::
apply_calibration_data ()
{
  apply_calibration_data_to_instruments (m_calibration_data_incoming);
}

void LAB_Calibration:: 
discard_calibration_data_changes ()
{
  m_calibration_data_incoming = current_calibration_data ();
}

void LAB_Calibration:: 
save_calibration_data_to_file (const std::string& file_path)
{
  std::ofstream file (file_path);

  if (file.is_open ())
  {
    for (const std::pair<std::string, double>& pair : current_calibration_data ())
    {
      file << pair.first << " = " << pair.second << "\n";
    }

    file.close ();
  }
  else 
  {
    throw (std::runtime_error ("Error in creating calibration file."));
  }
}

void LAB_Calibration:: 
save_calibration_data_to_default ()
{
  save_calibration_data_to_file (LABC::LABSOFT::DEFAULT_CALIBRATION_FILE_PATH);
}

const LAB_Calibration_Data LAB_Calibration:: 
current_calibration_data () const
{
  LAB_Calibration_Data data;

  // osc
  {
    // chan 1
    {
      data.emplace ("osc_adc_vref_chan_0", 
        lab ().m_Oscilloscope.calibration ().adc_reference_voltage (0));
      
      // vertical_offset_corrector
      data.emplace ("osc_vertical_offset_corrector_double_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        vertical_offset_corrector (0, LABE::OSC::SCALING::DOUBLE));
      
      data.emplace ("osc_vertical_offset_corrector_half_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        vertical_offset_corrector (0, LABE::OSC::SCALING::HALF));
      
      data.emplace ("osc_vertical_offset_corrector_fourth_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        vertical_offset_corrector (0, LABE::OSC::SCALING::FOURTH));
    
      data.emplace ("osc_vertical_offset_corrector_eighth_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        vertical_offset_corrector (0, LABE::OSC::SCALING::EIGHTH));
      
      // scaling_corrector_to_unity
      data.emplace ("osc_scaling_corrector_to_unity_double_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_unity (0, LABE::OSC::SCALING::DOUBLE));
      
      data.emplace ("osc_scaling_corrector_to_unity_half_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_unity (0, LABE::OSC::SCALING::HALF));
      
      data.emplace ("osc_scaling_corrector_to_unity_fourth_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_unity (0, LABE::OSC::SCALING::FOURTH));
    
      data.emplace ("osc_scaling_corrector_to_unity_eighth_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_unity (0, LABE::OSC::SCALING::EIGHTH));
      
      // scaling_corrector_to_actual
      data.emplace ("osc_scaling_corrector_to_actual_double_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_actual (0, LABE::OSC::SCALING::DOUBLE));
      
      data.emplace ("osc_scaling_corrector_to_actual_half_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_actual (0, LABE::OSC::SCALING::HALF));
      
      data.emplace ("osc_scaling_corrector_to_actual_fourth_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_actual (0, LABE::OSC::SCALING::FOURTH));
    
      data.emplace ("osc_scaling_corrector_to_actual_eighth_chan_0", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_actual (0, LABE::OSC::SCALING::EIGHTH));
    }
  
    // chan 2
    {
      data.emplace ("osc_adc_vref_chan_1", 
        lab ().m_Oscilloscope.calibration ().adc_reference_voltage (1));
      
      // vertical_offset_corrector
      data.emplace ("osc_vertical_offset_corrector_double_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        vertical_offset_corrector (1, LABE::OSC::SCALING::DOUBLE));
      
      data.emplace ("osc_vertical_offset_corrector_half_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        vertical_offset_corrector (1, LABE::OSC::SCALING::HALF));
      
      data.emplace ("osc_vertical_offset_corrector_fourth_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        vertical_offset_corrector (1, LABE::OSC::SCALING::FOURTH));
    
      data.emplace ("osc_vertical_offset_corrector_eighth_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        vertical_offset_corrector (1, LABE::OSC::SCALING::EIGHTH));
      
      // scaling_corrector_to_unity
      data.emplace ("osc_scaling_corrector_to_unity_double_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_unity (1, LABE::OSC::SCALING::DOUBLE));
      
      data.emplace ("osc_scaling_corrector_to_unity_half_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_unity (1, LABE::OSC::SCALING::HALF));
      
      data.emplace ("osc_scaling_corrector_to_unity_fourth_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_unity (1, LABE::OSC::SCALING::FOURTH));
    
      data.emplace ("osc_scaling_corrector_to_unity_eighth_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_unity (1, LABE::OSC::SCALING::EIGHTH));
      
      // scaling_corrector_to_actual
      data.emplace ("osc_scaling_corrector_to_actual_double_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_actual (1, LABE::OSC::SCALING::DOUBLE));
      
      data.emplace ("osc_scaling_corrector_to_actual_half_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_actual (1, LABE::OSC::SCALING::HALF));
      
      data.emplace ("osc_scaling_corrector_to_actual_fourth_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_actual (1, LABE::OSC::SCALING::FOURTH));
    
      data.emplace ("osc_scaling_corrector_to_actual_eighth_chan_1", 
        lab ().m_Oscilloscope.calibration ().
        scaling_corrector_to_actual (1, LABE::OSC::SCALING::EIGHTH));
    }
  }

  // ohmmeter
  {
    data.emplace ("ohm_r1_chan_0", 
      lab ().m_Ohmmeter.calibration ().r1 (0));
    
    data.emplace ("ohm_vref_chan_0", 
      lab ().m_Ohmmeter.calibration ().vref (0));  
  }

  return (data);
}

const LAB_Calibration_Data& LAB_Calibration:: 
calibration_data_incoming () const 
{
  return (m_calibration_data_incoming);
}

// eof