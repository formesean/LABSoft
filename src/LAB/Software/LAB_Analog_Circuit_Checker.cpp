#include "LAB_Analog_Circuit_Checker.h"
#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <cstdlib>

#include "../../Utility/pugixml.hpp"

static std::vector<double>
parse_csv_doubles (const char* text)
{
  std::vector<double> values;
  if (!text) return values;
  const char* p = text;
  while (*p)
  {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') ++p;
    if (!*p) break;
    char* endptr = nullptr;
    const double v = std::strtod(p, &endptr);
    if (endptr == p)
    {
      ++p;
      continue;
    }
    values.push_back(v);
    p = endptr;

    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') ++p;
  }
  return values;
}

LAB_Analog_Circuit_Checker::LAB_Analog_Circuit_Checker(LAB &_lab)
    : LAB_Module(_lab)
{

}

void LAB_Analog_Circuit_Checker::
load_data_from_file_acc()
{
  clear_data_acc();
  load_metadata_acc();
  load_channel_data_acc();
  load_function_generator_data_acc();
}

void LAB_Analog_Circuit_Checker::
load_metadata_acc()
{
  pugi::xml_node osc = m_xml_doc.child("root").child("oscilloscope");

  if (!osc) throw std::runtime_error("Missing oscilloscope section in .labacc file");

  if (pugi::xml_node chans = osc.child("channels"))
  {
    unsigned cnt = 0;
    for (auto ch : chans.children("channel"))
    {
      (void)ch;
      ++cnt;
    }
    m_channels = (cnt > 0) ? cnt : 1;
  }
  else
  {
    m_channels = 1;
  }

  m_samples = osc.child("samples").text().as_uint();

  if (pugi::xml_node chans = osc.child("channels"))
  {
    for (pugi::xml_node ch : chans.children("channel"))
    {
      if (pugi::xml_node samples_node = ch.child("samples"))
      {
        const char* csv = samples_node.text().as_string();
        const auto csv_vals = parse_csv_doubles(csv);
        if (!csv_vals.empty())
        {
          m_samples = static_cast<unsigned>(csv_vals.size());
          break;
        }
      }
    }
  }

  m_sampling_rate = osc.child("sampling_rate").text().as_double();
  m_time_per_division = osc.child("time_per_division").text().as_double();
  m_horizontal_offset = osc.child("horizontal_offset").text().as_double();

  m_trigger_mode = osc.child("trigger_mode").text().as_uint();
  m_trigger_source = osc.child("trigger_source").text().as_uint();
  m_trigger_type = osc.child("trigger_type").text().as_uint();
  m_trigger_condition = osc.child("trigger_condition").text().as_uint();
  m_trigger_level = osc.child("trigger_level").text().as_double();

  pugi::xml_node cmp = m_xml_doc.child("root").child("comparison");
  if (cmp)
  {
    m_cmp_time_domain = cmp.child("time_domain").text().as_bool();
    m_cmp_frequency_domain = cmp.child("frequency_domain").text().as_bool();

    if (auto n = cmp.child("time_similarity_threshold"))
      m_cmp_time_similarity_threshold = n.text().as_double();
    else
      m_cmp_time_similarity_threshold = cmp.child("similarity_threshold").text().as_double();

    if (auto n = cmp.child("frequency_similarity_threshold"))
      m_cmp_frequency_similarity_threshold = n.text().as_double();
    else
      m_cmp_frequency_similarity_threshold = cmp.child("similarity_threshold").text().as_double();
  }
  else
  {
    m_cmp_time_domain = osc.child("time_domain").text().as_bool(false);
    m_cmp_frequency_domain = osc.child("frequency_domain").text().as_bool(false);
    const double combined = osc.child("similarity_threshold").text().as_double(0.0);
    m_cmp_time_similarity_threshold = combined;
    m_cmp_frequency_similarity_threshold = combined;
  }
}

void LAB_Analog_Circuit_Checker::
load_channel_data_acc()
{
  pugi::xml_node osc = m_xml_doc.child("root").child("oscilloscope");

  if (!osc) throw std::runtime_error("Missing oscilloscope section in .labacc file");

  m_channel_data.clear();

  if (pugi::xml_node chans = osc.child("channels"))
  {
    size_t idx = 0;
    for (pugi::xml_node ch : chans.children("channel"))
    {
      ChannelData channel;
      unsigned index_attr = ch.attribute("index").as_uint(idx);
      channel.name = std::string("Channel ") + std::to_string(index_attr + 1);

      channel.samples = osc.child("samples").text().as_uint();
      channel.coupling = ch.child("coupling").text().as_bool(osc.child("coupling").text().as_bool());
      channel.scaling = ch.child("scaling").text().as_uint(osc.child("scaling").text().as_uint());
      channel.voltage_per_division = ch.child("voltage_per_division").text().as_double(osc.child("voltage_per_division").text().as_double());
      channel.vertical_offset = ch.child("vertical_offset").text().as_double(osc.child("vertical_offset").text().as_double());
      channel.is_enabled = ch.child("is_enabled").text().as_bool(osc.child("is_enabled").text().as_bool());
      channel.scaling_corrector = ch.child("scaling_corrector").text().as_double(1.0);
      if (channel.scaling_corrector == 0.0)
        channel.scaling_corrector = 1.0;

      if (pugi::xml_node measurements = ch.child("measurements"))
      {
        channel.measurements.min = measurements.child("min").text().as_double();
        channel.measurements.max = measurements.child("max").text().as_double();
        channel.measurements.avg = measurements.child("avg").text().as_double();
        channel.measurements.trms = measurements.child("trms").text().as_double();
      }

      if (pugi::xml_node samples_node = ch.child("samples"))
      {
        const char* csv = samples_node.text().as_string();
        channel.sample_data = parse_csv_doubles(csv);
        channel.samples = static_cast<unsigned>(channel.sample_data.size());

        // Debug print: first 10 samples for this channel
        if (!channel.sample_data.empty())
        {
          std::cout << "[ACC] Channel " << (index_attr + 1) << " first 10 samples: ";
          const size_t num_to_print = std::min<size_t>(10, channel.sample_data.size());
          for (size_t k = 0; k < num_to_print; ++k)
          {
            if (k) std::cout << ", ";
            std::cout << channel.sample_data[k];
          }
          std::cout << "\n";
        }
      }

      m_channel_data.push_back(std::move(channel));
      ++idx;
    }
  }
  else
  {
    ChannelData channel;
    channel.name = "Channel 1";

    channel.samples = osc.child("samples").text().as_uint();
    channel.coupling = osc.child("coupling").text().as_bool();
    channel.scaling = osc.child("scaling").text().as_uint();
    channel.voltage_per_division = osc.child("voltage_per_division").text().as_double();
    channel.vertical_offset = osc.child("vertical_offset").text().as_double();
    channel.is_enabled = osc.child("is_enabled").text().as_bool();
    channel.scaling_corrector = osc.child("scaling_corrector").text().as_double();

    if (channel.scaling_corrector == 0.0)
      channel.scaling_corrector = 1.0;

    if (pugi::xml_node measurements = osc.child("measurements"))
    {
      channel.measurements.min = measurements.child("min").text().as_double();
      channel.measurements.max = measurements.child("max").text().as_double();
      channel.measurements.avg = measurements.child("avg").text().as_double();
      channel.measurements.trms = measurements.child("trms").text().as_double();
    }

    if (pugi::xml_node samples_node = osc.child("samples"))
    {
      for (pugi::xml_node sample : samples_node.children("sample"))
        channel.sample_data.push_back(sample.text().as_double());

      // Debug print: first 10 samples for channel 1
      if (!channel.sample_data.empty())
      {
        std::cout << "[ACC] Channel 1 first 10 samples: ";
        const size_t num_to_print = std::min<size_t>(10, channel.sample_data.size());
        for (size_t k = 0; k < num_to_print; ++k)
        {
          if (k) std::cout << ", ";
          std::cout << channel.sample_data[k];
        }
        std::cout << "\n";
      }
    }

    m_channel_data.push_back(channel);
  }
}

void LAB_Analog_Circuit_Checker::
load_function_generator_data_acc()
{
  pugi::xml_node fg = m_xml_doc.child("root").child("function_generator");

  if (!fg) return;

  m_func_gen_data.wave_type = fg.child("wave_type").text().as_uint();
  m_func_gen_data.frequency = fg.child("frequency").text().as_double();
  m_func_gen_data.period = fg.child("period").text().as_double();
}

void LAB_Analog_Circuit_Checker::
clear_data_acc()
{
  m_channel_data.clear();
  m_func_gen_data = {0, 0.0, 0.0};
}

void LAB_Analog_Circuit_Checker::
    load_file(const std::string &path)
{
  try
  {
    pugi::xml_parse_result result = m_xml_doc.load_file(path.c_str());

    if (!result) throw std::runtime_error("Failed to load .labacc file: " + std::string(result.description()));

    m_file_path = path;
    m_is_file_loaded = true;

    load_data_from_file_acc();
  }
  catch (const std::exception &e)
  {
    clear_data_acc();
    m_is_file_loaded = false;
    std::printf("[ACC][ERROR] %s\n", e.what());
    throw;
  }
}

void LAB_Analog_Circuit_Checker::
unload_file()
{
  clear_data_acc();
  m_is_file_loaded = false;
  m_file_path.clear();
  m_xml_doc.reset();
}

void LAB_Analog_Circuit_Checker::
    load_data_acc()
{
  // Intentionally left blank in ACC module. Presenter consumes parsed data for display.
}

// EOF
