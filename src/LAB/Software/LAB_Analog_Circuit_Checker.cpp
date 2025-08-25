#include "LAB_Analog_Circuit_Checker.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "../LAB.h"
#include "../../Utility/LAB_Utility_Functions.h"
#include "../../Utility/pugixml.hpp"

LAB_Analog_Circuit_Checker::
LAB_Analog_Circuit_Checker(LAB& _lab)
    : LAB_Module(_lab)
{


}

void LAB_Analog_Circuit_Checker::
load_data_from_file_acc ()
{
  clear_data_acc();
  load_metadata_acc();
  load_channel_data_acc();
  load_function_generator_data_acc();
}

void LAB_Analog_Circuit_Checker::
load_metadata_acc ()
{
  pugi::xml_node metadata = m_xml_doc.child ("root").child ("metadata");
  
  if (!metadata) {
    throw std::runtime_error("Missing metadata section in .labacc file");
  }
  
  // Extract oscilloscope configuration from XML
  m_channels = metadata.child("channels").text().as_uint();
  m_samples = metadata.child("samples").text().as_uint();
  m_sampling_rate = metadata.child("sampling_rate").text().as_double();
  m_time_per_division = metadata.child("time_per_division").text().as_double();
  m_horizontal_offset = metadata.child("horizontal_offset").text().as_double();
  
  // Extract trigger configuration
  m_trigger_mode = metadata.child("trigger_mode").text().as_uint();
  m_trigger_source = metadata.child("trigger_source").text().as_uint();
  m_trigger_type = metadata.child("trigger_type").text().as_uint();
  m_trigger_condition = metadata.child("trigger_condition").text().as_uint();
  m_trigger_level = metadata.child("trigger_level").text().as_double();
}

void LAB_Analog_Circuit_Checker::
load_channel_data_acc ()
{
  pugi::xml_node data = m_xml_doc.child("root").child("data");
  
  if (!data) {
    throw std::runtime_error("Missing data section in .labacc file");
  }
  
  m_channel_data.clear();
  
  // Iterate through all data pairs and extract channel data
  for (pugi::xml_node data_pair : data.children("data_pair")) {
    std::string input = data_pair.child("input").text().as_string();
    
    // Only process channel data (skip function generator)
    if (input.find("Channel") != std::string::npos) {
      ChannelData channel;
      channel.name = input;
      
      pugi::xml_node output = data_pair.child("output");
      
      // Extract channel configuration
      channel.samples = output.child("samples").text().as_uint();
      channel.coupling = output.child("coupling").text().as_bool();
      channel.scaling = output.child("scaling").text().as_uint();
      channel.voltage_per_division = output.child("voltage_per_division").text().as_double();
      channel.vertical_offset = output.child("vertical_offset").text().as_double();
      channel.is_enabled = output.child("is_enabled").text().as_bool();
      channel.scaling_corrector = output.child("scaling_corrector").text().as_double();
      
      // Extract measurements
      pugi::xml_node measurements = output.child("measurements");
      if (measurements) {
        channel.measurements.min = measurements.child("min").text().as_double();
        channel.measurements.max = measurements.child("max").text().as_double();
        channel.measurements.avg = measurements.child("avg").text().as_double();
        channel.measurements.trms = measurements.child("trms").text().as_double();
      }
      
      // Extract sample data from first_10_samples
      pugi::xml_node samples_node = output.child("first_10_samples");
      if (samples_node) {
        for (pugi::xml_node sample : samples_node.children("sample")) {
          channel.sample_data.push_back(sample.text().as_double());
        }
      }
      
      m_channel_data.push_back(channel);
    }
  }
}

void LAB_Analog_Circuit_Checker::
load_function_generator_data_acc ()
{
  pugi::xml_node data = m_xml_doc.child("root").child("data");
  
  // Find function generator data
  for (pugi::xml_node data_pair : data.children("data_pair")) {
    std::string input = data_pair.child("input").text().as_string();
    
    if (input.find("Function Generator") != std::string::npos) {
      pugi::xml_node output = data_pair.child("output");
      
      m_func_gen_data.wave_type = output.child("wave_type").text().as_uint();
      m_func_gen_data.frequency = output.child("frequency").text().as_double();
      m_func_gen_data.period = output.child("period").text().as_double();
      
      break;
    }
  }
}

void LAB_Analog_Circuit_Checker::
clear_data_acc ()
{
  m_channel_data.clear();
  m_func_gen_data = {0, 0.0, 0.0};
}

void LAB_Analog_Circuit_Checker::
load_file (const std::string& path)
{
  try {
    // Load and parse the .labacc XML file
    pugi::xml_parse_result result = m_xml_doc.load_file(path.c_str());
    
    if (!result) {
      throw std::runtime_error("Failed to load .labacc file: " + std::string(result.description()));
    }
    
    m_file_path = path;
    m_is_file_loaded = true;
    
    // Load all data from the file
    load_data_from_file_acc();
    
  } catch (const std::exception& e) {
    clear_data_acc();
    m_is_file_loaded = false;
    throw;
  }
}

void LAB_Analog_Circuit_Checker::
unload_file ()
{
  clear_data_acc();
  m_is_file_loaded = false;
  m_file_path.clear();
  m_xml_doc.reset();
}

void LAB_Analog_Circuit_Checker::
load_data_acc ()
{
  if (!m_is_file_loaded) {
    throw std::runtime_error("No .labacc file loaded");
  }
  
  // Get reference to oscilloscope from LAB
  LAB_Oscilloscope& oscilloscope = lab().m_Oscilloscope;
  
  // Configure oscilloscope horizontal settings
  oscilloscope.horizontal_offset(m_horizontal_offset);
  oscilloscope.time_per_division(m_time_per_division);
  oscilloscope.samples(m_samples);
  oscilloscope.sampling_rate(m_sampling_rate);
  
  // Configure trigger settings
  oscilloscope.trigger_mode(static_cast<LABE::OSC::TRIG::MODE>(m_trigger_mode));
  oscilloscope.trigger_source(m_trigger_source);
  oscilloscope.trigger_type(static_cast<LABE::OSC::TRIG::TYPE>(m_trigger_type));
  oscilloscope.trigger_condition(static_cast<LABE::OSC::TRIG::CND>(m_trigger_condition));
  oscilloscope.trigger_level(m_trigger_level);
  
  // Configure channel settings based on loaded data
  for (size_t i = 0; i < m_channel_data.size() && i < LABC::OSC::NUMBER_OF_CHANNELS; ++i) {
    const ChannelData& channel = m_channel_data[i];
    
    // Configure channel parameters
    oscilloscope.channel_enable_disable(i, channel.is_enabled);
    oscilloscope.coupling(i, static_cast<LABE::OSC::COUPLING>(channel.coupling));
    oscilloscope.scaling(i, static_cast<LABE::OSC::SCALING>(channel.scaling));
    oscilloscope.voltage_per_division(i, channel.voltage_per_division);
    oscilloscope.vertical_offset(i, channel.vertical_offset);
    
    // Note: Loading actual sample data would require accessing the oscilloscope's
    // internal data structures or adding methods to load external sample data.
    // For demonstration purposes, we're configuring the oscilloscope parameters
    // that would be used to display the loaded signal data.
  }
  
  // Configure function generator if available
  if (LABC::FUNC_GEN::NUMBER_OF_CHANNELS > 0) {
    LAB_Function_Generator& func_gen = lab().m_Function_Generator;
    
    // Set function generator parameters from loaded data
    func_gen.wave_type(0, static_cast<LABE::FUNC_GEN::WAVE_TYPE>(m_func_gen_data.wave_type));
    func_gen.frequency(0, m_func_gen_data.frequency);
    func_gen.period(0, m_func_gen_data.period);
  }
}
