#include "LAB_Analog_Circuit_Checker.h"
#include <stdexcept>
#include <cstdio>

#include "../../Utility/pugixml.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void print_samples(const std::vector<double>& instructor,
                          const std::vector<double>& student);

LAB_Analog_Circuit_Checker::
LAB_Analog_Circuit_Checker(LAB& _lab)
    : LAB_Module(_lab)
{
  // Initialize small dummy signals for similarity demonstration
  constexpr size_t NUM_SAMPLES = 2000;
  dummy_instructor_data.resize(NUM_SAMPLES);
  dummy_student_data.resize(NUM_SAMPLES);

  double Fs     = 40e3;
  double f      = 1000;
  double offset = 0.2;
  size_t delay  = 2;

  for (size_t n = 0; n < NUM_SAMPLES; n++)
  {
    double t = n / Fs;                                        // convert sample index to time

    size_t delayed_index = (n >= delay) ? (n - delay) : 0;    // shifts the student’s signal by delay samples
    double t_delayed = delayed_index / Fs;                    // convert delayed index to delayed time

    dummy_instructor_data[n] = std::sin(2 * M_PI * f * t);
    dummy_student_data[n]    = std::sin(2 * M_PI * f * t_delayed) + offset;
  }

  // Print samples
  print_samples(dummy_instructor_data, dummy_student_data);

  // Print similarity
  double sim = compute_similarity();
  std::cout << "\nSimilarity: " << sim << "%\n";
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
  // New format: data is under root/oscilloscope
  pugi::xml_node osc = m_xml_doc.child("root").child("oscilloscope");

  if (!osc) {
    throw std::runtime_error("Missing oscilloscope section in .labacc file");
  }

  // Extract oscilloscope configuration from XML
  // Channels: prefer explicit count from <channels>, else default to 1
  if (pugi::xml_node chans = osc.child("channels"))
  {
    unsigned cnt = 0; for (auto ch : chans.children("channel")) { (void)ch; ++cnt; }
    m_channels = (cnt > 0) ? cnt : 1;
  }
  else
  {
    m_channels = 1;
  }
  m_samples             = osc.child("samples").text().as_uint();
  m_sampling_rate       = osc.child("sampling_rate").text().as_double();
  m_time_per_division   = osc.child("time_per_division").text().as_double();
  m_horizontal_offset   = osc.child("horizontal_offset").text().as_double();

  // Extract trigger configuration
  m_trigger_mode        = osc.child("trigger_mode").text().as_uint();
  m_trigger_source      = osc.child("trigger_source").text().as_uint();
  m_trigger_type        = osc.child("trigger_type").text().as_uint();
  m_trigger_condition   = osc.child("trigger_condition").text().as_uint();
  m_trigger_level       = osc.child("trigger_level").text().as_double();

  std::printf("[ACC][Metadata]\n");
  std::printf("  Channels           : %u\n", m_channels);
  std::printf("  Samples            : %u\n", m_samples);
  std::printf("  Sampling Rate (Hz) : %.6f\n", m_sampling_rate);
  std::printf("  Time/Div (s)       : %.6f\n", m_time_per_division);
  std::printf("  Horiz Offset (s)   : %.6f\n", m_horizontal_offset);
  std::printf("  Trigger Mode       : %u\n", m_trigger_mode);
  std::printf("  Trigger Source     : %u\n", m_trigger_source);
  std::printf("  Trigger Type       : %u\n", m_trigger_type);
  std::printf("  Trigger Condition  : %u\n", m_trigger_condition);
  std::printf("  Trigger Level (V)  : %.6f\n", m_trigger_level);
}

void LAB_Analog_Circuit_Checker::
load_channel_data_acc ()
{
  // New format: channel-like data lives under root/oscilloscope
  pugi::xml_node osc = m_xml_doc.child("root").child("oscilloscope");

  if (!osc)
  {
    throw std::runtime_error("Missing oscilloscope section in .labacc file");
  }

  m_channel_data.clear();

  // Preferred new format with <channels><channel index="..."> blocks
  if (pugi::xml_node chans = osc.child("channels"))
  {
    size_t idx = 0;
    for (pugi::xml_node ch : chans.children("channel"))
    {
      ChannelData channel;
      unsigned index_attr = ch.attribute("index").as_uint(idx);
      channel.name = std::string("Channel ") + std::to_string(index_attr + 1);

      // Pull per-channel values, fallback to osc-level when missing
      channel.samples               = osc.child("samples").text().as_uint();
      channel.coupling              = ch.child("coupling").text().as_bool(osc.child("coupling").text().as_bool());
      channel.scaling               = ch.child("scaling").text().as_uint(osc.child("scaling").text().as_uint());
      channel.voltage_per_division  = ch.child("voltage_per_division").text().as_double(osc.child("voltage_per_division").text().as_double());
      channel.vertical_offset       = ch.child("vertical_offset").text().as_double(osc.child("vertical_offset").text().as_double());
      channel.is_enabled            = ch.child("is_enabled").text().as_bool(osc.child("is_enabled").text().as_bool());
      channel.scaling_corrector     = ch.child("scaling_corrector").text().as_double(1.0);
      if (channel.scaling_corrector == 0.0) channel.scaling_corrector = 1.0;

      if (pugi::xml_node measurements = ch.child("measurements"))
      {
        channel.measurements.min    = measurements.child("min").text().as_double();
        channel.measurements.max    = measurements.child("max").text().as_double();
        channel.measurements.avg    = measurements.child("avg").text().as_double();
        channel.measurements.trms   = measurements.child("trms").text().as_double();
      }

      // Only Channel 2 (index 1) will have samples in new files; parse if present
      if (pugi::xml_node samples_node = ch.child("samples"))
      {
        for (pugi::xml_node sample : samples_node.children("sample"))
          channel.sample_data.push_back(sample.text().as_double());
      }

      m_channel_data.push_back(std::move(channel));
      ++idx;
    }
  }
  else
  {
    // Backward-compatible single-channel at osc-level
    ChannelData channel;
    channel.name = "Channel 1";

    channel.samples               = osc.child("samples").text().as_uint();
    channel.coupling              = osc.child("coupling").text().as_bool();
    channel.scaling               = osc.child("scaling").text().as_uint();
    channel.voltage_per_division  = osc.child("voltage_per_division").text().as_double();
    channel.vertical_offset       = osc.child("vertical_offset").text().as_double();
    channel.is_enabled            = osc.child("is_enabled").text().as_bool();
    channel.scaling_corrector     = osc.child("scaling_corrector").text().as_double();
    if (channel.scaling_corrector == 0.0) channel.scaling_corrector = 1.0;

    if (pugi::xml_node measurements   = osc.child("measurements"))
    {
      channel.measurements.min    = measurements.child("min").text().as_double();
      channel.measurements.max    = measurements.child("max").text().as_double();
      channel.measurements.avg    = measurements.child("avg").text().as_double();
      channel.measurements.trms   = measurements.child("trms").text().as_double();
    }

    if (pugi::xml_node samples_node = osc.child("samples"))
    {
      for (pugi::xml_node sample : samples_node.children("sample"))
        channel.sample_data.push_back(sample.text().as_double());
    }

    m_channel_data.push_back(channel);
  }

  // Log channel config and sample preview
  std::printf("[ACC] Channels parsed: %zu\n", m_channel_data.size());
  for (size_t chIndex = 0; chIndex < m_channel_data.size(); ++chIndex)
  {
    const auto &channel = m_channel_data[chIndex];
    std::printf("[ACC][Channel]\n");
    std::printf("  Name               : %s\n", channel.name.c_str());
    std::printf("  Enabled            : %s\n", channel.is_enabled ? "Yes" : "No");
    std::printf("  Coupling           : %s\n", channel.coupling ? "AC" : "DC");
    std::printf("  Scaling            : %u\n", channel.scaling);
    std::printf("  Volt/Div (V)       : %.6f\n", channel.voltage_per_division);
    std::printf("  Vert Offset (V)    : %.6f\n", channel.vertical_offset);
    std::printf("  Scaling Corrector  : %.6f\n", channel.scaling_corrector);
    if (chIndex == 0 && channel.sample_data.empty())
    {
      std::printf("  Measurements       : [omitted for Channel 1]\n");
    }
    else
    {
      std::printf("  Measurements       : min=%.6f, max=%.6f, avg=%.6f, trms=%.6f\n",
                  channel.measurements.min,
                  channel.measurements.max,
                  channel.measurements.avg,
                  channel.measurements.trms);
    }
    std::printf("  Samples Loaded     : %zu\n", channel.sample_data.size());

    if (channel.sample_data.empty())
    {
      std::printf("  Sample Preview     : [ ]\n");
      if (chIndex == 0)
      {
        std::printf("  Note               : Channel 1 samples are intentionally not saved.\n");
      }
    }
    else
    {
      const size_t preview_count = std::min(static_cast<size_t>(10), channel.sample_data.size());
      std::printf("  Sample Preview     : [ ");
      for (size_t i = 0; i < preview_count; ++i)
      {
        std::printf("%.6f%s", channel.sample_data[i], (i + 1 < preview_count) ? ", " : "");
      }
      if (channel.sample_data.size() > preview_count)
      {
        std::printf(", ...");
      }
      std::printf(" ]\n");
    }
  }
}

void LAB_Analog_Circuit_Checker::
load_function_generator_data_acc ()
{
  // New format: function generator data under root/function_generator
  pugi::xml_node fg = m_xml_doc.child("root").child("function_generator");

  if (!fg)
  {
    // Not strictly required; leave defaults
    return;
  }

  m_func_gen_data.wave_type = fg.child("wave_type").text().as_uint();
  m_func_gen_data.frequency = fg.child("frequency").text().as_double();
  m_func_gen_data.period    = fg.child("period").text().as_double();

  // Log function generator settings (also log optional fields if present)
  std::printf("[ACC][Function Generator]\n");
  std::printf("  Wave Type          : %u\n", m_func_gen_data.wave_type);
  std::printf("  Frequency (Hz)     : %.6f\n", m_func_gen_data.frequency);
  std::printf("  Period (s)         : %.6f\n", m_func_gen_data.period);
  if (auto node = fg.child("amplitude"))        std::printf("  Amplitude (V)      : %.6f\n", node.text().as_double());
  if (auto node = fg.child("vertical_offset"))  std::printf("  Vert Offset (V)    : %.6f\n", node.text().as_double());
  if (auto node = fg.child("phase"))           std::printf("  Phase (deg)        : %.6f\n", node.text().as_double());
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
  try
  {
    // Load and parse the .labacc XML file
    pugi::xml_parse_result result = m_xml_doc.load_file(path.c_str());

    if (!result)
    {
      throw std::runtime_error("Failed to load .labacc file: " + std::string(result.description()));
    }

    m_file_path = path;
    m_is_file_loaded = true;

    std::printf("\n[ACC] Loading .labacc file: %s\n", path.c_str());

    // Load all data from the file
    load_data_from_file_acc();

    std::printf("[ACC] Done loading .labacc file.\n\n");

  } catch (const std::exception& e)
  {
    clear_data_acc();
    m_is_file_loaded = false;
    std::printf("[ACC][ERROR] %s\n", e.what());
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
  // Intentionally left blank in ACC module. Presenter consumes parsed data for display.
}

double LAB_Analog_Circuit_Checker::
compute_cross_correlation ()
{
  const size_t N         = std::min(dummy_instructor_data.size(), dummy_student_data.size());
  double       numerator = 0.0;
  double       denom_x   = 0.0;
  double       denom_y   = 0.0;

  for (size_t n = 0; n < N; n++)
  {
    numerator += dummy_instructor_data[n] * dummy_student_data[n];
    denom_x   += dummy_instructor_data[n] * dummy_instructor_data[n];
    denom_y   += dummy_student_data[n]    * dummy_student_data[n];
  }

  if (denom_x == 0.0 || denom_y == 0.0) return 0.0; // avoid div by zero

  return numerator / std::sqrt(denom_x * denom_y); // normalized correlation
}


double LAB_Analog_Circuit_Checker::
compute_similarity()
{
    double corr = compute_cross_correlation();
    return corr * 100.0; // percentage
    // if corr =  1.0 ->  100% similar
    // if corr =  0.0 ->  0%   similar
    // if corr = -1.0 -> -100% (perfectly opposite)
}

static void print_samples(const std::vector<double>& instructor,
                          const std::vector<double>& student)
{
  constexpr size_t NUM_TO_PRINT = 10;
  const size_t N_instructor = std::min(NUM_TO_PRINT, instructor.size());
  const size_t N_student    = std::min(NUM_TO_PRINT, student.size());

  std::cout << "Instructor Data (first 10): [ ";
  for (size_t i = 0; i < N_instructor; i++)
  {
    std::cout << std::fixed << std::setprecision(4) << instructor[i];
    if (i < N_instructor - 1) std::cout << ", ";
  }
  std::cout << " ]\n";

  std::cout << "Student Data (first 10):    [ ";
  for (size_t i = 0; i < N_student; i++)
  {
    std::cout << std::fixed << std::setprecision(4) << student[i];
    if (i < N_student - 1) std::cout << ", ";
  }
  std::cout << " ]\n";
}
