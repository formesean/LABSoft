#include "LAB_Analog_Circuit_Checker.h"

#include <stdexcept>
#include <cstdio>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <complex>

#include "../../Utility/pugixml.hpp"

extern "C"
{
  #include "../../../lib/KISSFFT/kiss_fftr.h"
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

LAB_Analog_Circuit_Checker::CorrelationResult LAB_Analog_Circuit_Checker::
cross_correlation(const std::vector<double> &x,
                  const std::vector<double> &y)
{
  CorrelationResult result{0.0, 0.0};

  if (x.empty() || y.empty()) return result;

  // Use actual signal length or 2000, whichever is smaller
  const size_t min_len = std::min(std::min(x.size(), y.size()), static_cast<size_t>(2000));
  
  if (min_len == 0) return result;
  
  // Calculate means to remove DC components
  double x_mean = 0.0;
  double y_mean = 0.0;
  for (size_t i = 0; i < min_len; i++)
  {
    x_mean += x[i];
    y_mean += y[i];
  }
  x_mean /= min_len;
  y_mean /= min_len;
  
  // Create DC-removed versions of the signals
  std::vector<double> x_ac(min_len);
  std::vector<double> y_ac(min_len);
  for (size_t i = 0; i < min_len; i++)
  {
    x_ac[i] = x[i] - x_mean;
    y_ac[i] = y[i] - y_mean;
  }
  
  // Calculate signal norms for proper normalization (using DC-removed signals)
  double x_norm = 0.0;
  double y_norm = 0.0;
  for (size_t i = 0; i < min_len; i++)
  {
    x_norm += x_ac[i] * x_ac[i];
    y_norm += y_ac[i] * y_ac[i];
  }
  x_norm = std::sqrt(x_norm);
  y_norm = std::sqrt(y_norm);
  
  if (x_norm == 0.0 || y_norm == 0.0) return result;
  
  // full cross correlation with bidirectional shifting
  const size_t max_shift = min_len - 1;
  double method1_max_correlation = -1.0; // Best correlation from Method 1
  double method2_max_correlation = -1.0; // Best correlation from Method 2
  
  // METHOD 1: Shift student signal (y) relative to static teacher signal (x)
  for (int shift_offset = -static_cast<int>(max_shift); shift_offset <= static_cast<int>(max_shift); shift_offset++)
  {
    double correlation = 0.0;
    size_t valid_samples = 0;
    
    // cross correlation calculation - shifting student signal
    for (size_t i = 0; i < min_len; i++)
    {
      const int y_idx = static_cast<int>(i) + shift_offset;
      
      if (y_idx >= 0 && y_idx < static_cast<int>(min_len))
      {
        correlation += x_ac[i] * y_ac[y_idx];
        valid_samples++;
      }
    }
    
    // Normalize the correlation for this shift
    if (valid_samples > 0)
    {
      double normalized_correlation = correlation / (x_norm * y_norm);
      
      // Track the maximum normalized correlation for Method 1
      if (normalized_correlation > method1_max_correlation)
      {
        method1_max_correlation = normalized_correlation;
      }
    }
  }
  
  // METHOD 2: Shift teacher signal (x) relative to static student signal (y)
  for (int shift_offset = -static_cast<int>(max_shift); shift_offset <= static_cast<int>(max_shift); shift_offset++)
  {
    double correlation = 0.0;
    size_t valid_samples = 0;
    
    // cross correlation calculation - shifting teacher signal
    for (size_t i = 0; i < min_len; i++)
    {
      const int x_idx = static_cast<int>(i) + shift_offset;
      
      if (x_idx >= 0 && x_idx < static_cast<int>(min_len))
      {
        correlation += x_ac[x_idx] * y_ac[i];
        valid_samples++;
      }
    }
    
    // Normalize the correlation for this shift
    if (valid_samples > 0)
    {
      double normalized_correlation = correlation / (x_norm * y_norm);
      
      // Track the maximum normalized correlation for Method 2
      if (normalized_correlation > method2_max_correlation)
      {
        method2_max_correlation = normalized_correlation;
      }
    }
  }
  
  // Select the BEST correlation from both methods
  double final_correlation = std::max(method1_max_correlation, method2_max_correlation);
  
  result.coefficient = final_correlation;
  result.percentage = final_correlation * 100;
  
  return result;
}

std::vector<std::complex<double>> LAB_Analog_Circuit_Checker::
compute_fft(const std::vector<double> &data)
{
  using scalar_t = kiss_fft_scalar;
  using cpx_t    = kiss_fft_cpx;

  // Input validation
  if (data.empty()) throw std::runtime_error("Data is empty");

  const size_t actual_N = data.size();

  if (actual_N == 0) throw std::runtime_error("No data available for FFT computation");

  // Configure KISS FFT for real-to-complex transform
  kiss_fftr_cfg cfg = kiss_fftr_alloc(static_cast<int>(actual_N), 0, nullptr, nullptr);

  if (!cfg) throw std::runtime_error("Failed to allocate KISS FFT configuration");

  // Process data (time domain -> frequency domain)
  std::vector<scalar_t> xA(actual_N);
  for (size_t n = 0; n < actual_N; ++n)
  {
    xA[n] = static_cast<scalar_t>(data[n]);
  }
  std::vector<cpx_t> XA(actual_N/2 + 1);
  kiss_fftr(cfg, xA.data(), XA.data());

  // Convert KISS FFT complex results to std::complex<double>
  std::vector<std::complex<double>> data_freq(actual_N/2 + 1);

  for (size_t k = 0; k < XA.size(); ++k)
  {
    data_freq[k] = std::complex<double>(XA[k].r, XA[k].i);
  }

  // Clean up KISS FFT configuration
  free(cfg);

  // Return frequency domain results
  return data_freq;
}

double LAB_Analog_Circuit_Checker::
compute_magnitude_error_similarity(const std::vector<double>& freq_instructor, 
                                   const std::vector<double>& freq_student)
{
  if (freq_instructor.empty() || freq_student.empty()) 
    return 0.0;
    
  // Use the smaller size to avoid out-of-bounds access
  const size_t min_size = std::min(freq_instructor.size(), freq_student.size());
  
  if (min_size == 0) 
    return 0.0;
    
  // Calculate Mean Squared Error (MSE)
  double mse = 0.0;
  double sum_instructor_squared = 0.0;
  
  for (size_t i = 0; i < min_size; ++i)
  {
    double error = freq_instructor[i] - freq_student[i];
    mse += error * error;
    sum_instructor_squared += freq_instructor[i] * freq_instructor[i];
  }
  
  mse /= min_size;
  sum_instructor_squared /= min_size;
  
  // Calculate similarity as percentage (100% - normalized error percentage)
  // Avoid division by zero
  if (sum_instructor_squared < 1e-12) 
    return 0.0;
    
  double normalized_error = std::sqrt(mse / sum_instructor_squared);
  double similarity_percentage = std::max(0.0, (1.0 - normalized_error) * 100.0);
  
  return similarity_percentage;
  /*
  Normalized Error	Similarity %	Meaning
  0.0	              100%	        Perfect match - signals identical
  0.1	              90%	          Very similar - small differences
  0.2	              80%	          Good match - moderate differences
  0.5	              50%	          Moderately similar - significant differences
  0.8	              20%	          Poor match - large differences
  1.0 or higher	    0%	          No similarity - completely different
  */
}

LAB_Analog_Circuit_Checker::CorrelationResult LAB_Analog_Circuit_Checker::
signal_analysis(const std::vector<double> &instructor,
                const std::vector<double> &student)
{
  CorrelationResult result = cross_correlation(instructor, student);
  return result;
}
// EOF
