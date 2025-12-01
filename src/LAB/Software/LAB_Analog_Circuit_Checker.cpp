#include "LAB_Analog_Circuit_Checker.h"

#include <stdexcept>
#include <cstdio>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <complex>
#include <string>

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
cross_correlation(const std::vector<double> &teacher_signal,
                  const std::vector<double> &student_signal)
{
  CorrelationResult result{0.0, 0.0, 0.0};

  if (teacher_signal.empty() || student_signal.empty()) return result;

  const size_t signal_length = std::min(std::min(teacher_signal.size(), student_signal.size()), 
                                        static_cast<size_t>(2000));

  if (signal_length == 0) return result;

  // Calculate means to remove DC components
  double teacher_mean = 0.0;
  double student_mean = 0.0;
  for (size_t i = 0; i < signal_length; i++)
  {
    teacher_mean += teacher_signal[i];
    student_mean += student_signal[i];
  }
  teacher_mean /= signal_length;
  student_mean /= signal_length;

  // Create DC-removed (AC-coupled) versions of the signals
  std::vector<double> teacher_ac(signal_length);
  std::vector<double> student_ac(signal_length);
  for (size_t i = 0; i < signal_length; i++)
  {
    teacher_ac[i] = teacher_signal[i] - teacher_mean;
    student_ac[i] = student_signal[i] - student_mean;
  }

  // Calculate teacher auto-correlation: Rxx(0) = Σ(x[i]²)
  double teacher_autocorr = 0.0;
  for (size_t i = 0; i < signal_length; i++)
  {
    teacher_autocorr += teacher_ac[i] * teacher_ac[i];
  }

  if (teacher_autocorr < 1e-12) return result;

  // Calculate unnormalized cross-correlation: Rxy(0) = Σ(teacher[i] × student[i])
  double cross_corr_at_lag0 = 0.0;
  for (size_t i = 0; i < signal_length; i++)
  {
    cross_corr_at_lag0 += teacher_ac[i] * student_ac[i];
  }

  int optimal_lag = 0;  // Static comparison (no shifting)

  // Energy-ratio similarity metric: Normalize by teacher auto-correlation only
  double similarity_coefficient = 0.0;
  if (teacher_autocorr > 1e-12)
  {
    double energy_ratio = cross_corr_at_lag0 / teacher_autocorr;
    
    // Apply bidirectional penalty for amplitude mismatch. Ensures that both under-amplitude and over-amplitude are penalized symmetrically
    if (energy_ratio > 1.0)
    {
      // Student has excess amplitude → penalize (invert ratio)
      similarity_coefficient = 1.0 / energy_ratio;
    }
    else
    {
      // Student has equal or deficient amplitude → use ratio directly
      similarity_coefficient = std::abs(energy_ratio);
    }
    
    // Clamp to valid range [0.0, 1.0]
    similarity_coefficient = std::max(0.0, similarity_coefficient);
  }

  // MSE-based similarity calculation
  std::vector<double> teacher_magnitude(signal_length);
  std::vector<double> student_magnitude(signal_length);
  for (size_t i = 0; i < signal_length; i++)
  {
    teacher_magnitude[i] = std::abs(teacher_ac[i]);
    student_magnitude[i] = std::abs(student_ac[i]);
  }

  double mse_similarity_percentage = compute_magnitude_error_similarity(
    teacher_magnitude, 
    student_magnitude, 
    optimal_lag
  );

  result.lag = static_cast<double>(optimal_lag);
  result.coefficient = similarity_coefficient;
  
  // Calculate weighted average of both metrics 
  double similarity_coefficient_percentage = similarity_coefficient * 100.0;
  double weighted_average_percentage = (similarity_coefficient_percentage + mse_similarity_percentage) / 2.0;
  result.percentage = similarity_coefficient_percentage;

  // Debug output
  // std::printf("[Time Domain - Unnormalized Cross-Correlation at Lag=0]\n");
  // std::printf("  Formula: Rxy(0) = Σ(teacher[n] × student[n])\n");
  // std::printf("  Teacher autocorr Rxx(0): %.2f\n", teacher_autocorr);
  // std::printf("  Cross-corr Rxy(0): %.2f\n", cross_corr_at_lag0);
  // std::printf("  Energy ratio: %.4f\n", cross_corr_at_lag0 / teacher_autocorr);
  // std::printf("  Similarity coefficient: %.4f (%.2f%%)\n", 
  //             similarity_coefficient, similarity_coefficient_percentage);
  // std::printf("  MSE-based similarity: %.2f%%\n", mse_similarity_percentage);
  // std::printf("  Weighted average (final result): %.2f%%\n", weighted_average_percentage);
  // std::printf("  Optimal lag: %d samples (static - no shifting)\n", optimal_lag);
  // std::printf("-------------------------------------\n");

  return result;
}

LAB_Analog_Circuit_Checker::CorrelationResult LAB_Analog_Circuit_Checker::
cross_correlation_complex(const std::vector<std::complex<double>> &teacher_spectrum,
                          const std::vector<std::complex<double>> &student_spectrum)
{
  CorrelationResult result{0.0, 0.0, 0.0};

  if (teacher_spectrum.empty() || student_spectrum.empty()) return result;

  // Use actual spectrum length or 2000, whichever is smaller
  const size_t spectrum_length = std::min(std::min(teacher_spectrum.size(), student_spectrum.size()), 
                                          static_cast<size_t>(2000));

  if (spectrum_length == 0) return result;

  // Calculate means to remove DC components (complex mean for frequency domain)
  std::complex<double> teacher_mean(0.0, 0.0);
  std::complex<double> student_mean(0.0, 0.0);
  for (size_t i = 0; i < spectrum_length; i++)
  {
    teacher_mean += teacher_spectrum[i];
    student_mean += student_spectrum[i];
  }
  teacher_mean /= static_cast<double>(spectrum_length);
  student_mean /= static_cast<double>(spectrum_length);

  // Create DC-removed versions of the spectra
  std::vector<std::complex<double>> teacher_ac(spectrum_length);
  std::vector<std::complex<double>> student_ac(spectrum_length);
  for (size_t i = 0; i < spectrum_length; i++)
  {
    teacher_ac[i] = teacher_spectrum[i] - teacher_mean;
    student_ac[i] = student_spectrum[i] - student_mean;
  }

  // Calculate teacher auto-correlation: Rxx(0) = Σ|teacher[i]|²
  double teacher_autocorr = 0.0;
  for (size_t i = 0; i < spectrum_length; i++)
  {
    teacher_autocorr += std::norm(teacher_ac[i]);  // |z|² = real² + imag²
  }

  if (teacher_autocorr < 1e-12) return result;

  // Calculate unnormalized complex cross-correlation: Rxy(0) = Σ(teacher[i] × conj(student[i]))
  std::complex<double> cross_corr_complex(0.0, 0.0);
  for (size_t i = 0; i < spectrum_length; i++)
  {
    cross_corr_complex += teacher_ac[i] * std::conj(student_ac[i]);
  }

  // Extract magnitude of complex correlation (phase-aware similarity measure)
  double cross_corr_magnitude = std::abs(cross_corr_complex);
  int optimal_lag = 0;  // Static comparison

  // Normalize by teacher auto-correlation only
  double similarity_coefficient = 0.0;
  if (teacher_autocorr > 1e-12)
  {
    double energy_ratio = cross_corr_magnitude / teacher_autocorr;
    
    // Apply bidirectional penalty for amplitude mismatch
    if (energy_ratio > 1.0)
    {
      // Student spectrum has excess energy → penalize (invert ratio)
      similarity_coefficient = 1.0 / energy_ratio;
    }
    else
    {
      // Student spectrum has equal or deficient energy → use ratio directly
      similarity_coefficient = std::abs(energy_ratio);
    }
    
    // Clamp to valid range [0.0, 1.0]
    similarity_coefficient = std::max(0.0, similarity_coefficient);
  }

  
  result.lag = static_cast<double>(optimal_lag);
  result.coefficient = similarity_coefficient;
  
  //  MSE-based similarity calculation
  std::vector<double> teacher_magnitude(spectrum_length);
  std::vector<double> student_magnitude(spectrum_length);
  for (size_t i = 0; i < spectrum_length; i++)
  {
    teacher_magnitude[i] = std::abs(teacher_ac[i]);
    student_magnitude[i] = std::abs(student_ac[i]);
  }

  double mse_similarity_percentage = compute_magnitude_error_similarity(
    teacher_magnitude, 
    student_magnitude, 
    optimal_lag
  );
  
  // Calculate weighted average of both metrics 
  double similarity_coefficient_percentage = similarity_coefficient * 100.0;
  double weighted_average_percentage = (similarity_coefficient_percentage + mse_similarity_percentage) / 2.0;
  result.percentage = weighted_average_percentage;

  // // Debug 
  // std::printf("[Frequency Domain - Unnormalized Complex Cross-Correlation at Lag=0]\n");
  // std::printf("  Formula: Rxy(0) = Σ(teacher[n] × conj(student[n]))\n");
  // std::printf("  Teacher autocorr Rxx(0): %.2f\n", teacher_autocorr);
  // std::printf("  Cross-corr magnitude |Rxy(0)|: %.2f\n", cross_corr_magnitude);
  // std::printf("  Energy ratio: %.4f\n", cross_corr_magnitude / teacher_autocorr);
  // std::printf("  Similarity coefficient: %.4f (%.2f%%)\n", 
  //             similarity_coefficient, similarity_coefficient_percentage);
  // std::printf("  MSE-based similarity: %.2f%%\n", mse_similarity_percentage);
  // std::printf("  Weighted average (final result): %.2f%%\n", weighted_average_percentage);
  // std::printf("  Optimal lag: %d frequency bins (static - no shifting)\n", optimal_lag);
  // std::printf("  Note: Phase information IS included in cross-correlation\n");
  // std::printf("-------------------------------------\n");

  return result;
}

LAB_Analog_Circuit_Checker::CorrelationResult LAB_Analog_Circuit_Checker::
signal_analysis_complex(const std::vector<std::complex<double>> &instructor,
                        const std::vector<std::complex<double>> &student)
{
  return cross_correlation_complex(instructor, student);
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
                                   const std::vector<double>& freq_student,
                                   int lag)
{
  if (freq_instructor.empty() || freq_student.empty())
    return 0.0;

  // Use the smaller size to avoid out-of-bounds access
  const size_t min_size = std::min(freq_instructor.size(), freq_student.size());

  if (min_size == 0)
    return 0.0;

  // Calculate Mean Squared Error (MSE) with optional lag
  double mse = 0.0;
  double sum_instructor_squared = 0.0;
  size_t valid_count = 0;

  for (size_t i = 0; i < min_size; ++i)
  {
    const int student_idx = static_cast<int>(i) + lag;
    
    // Check bounds for lagged comparison
    if (student_idx >= 0 && student_idx < static_cast<int>(min_size))
    {
      double error = freq_instructor[i] - freq_student[student_idx];
      mse += error * error;
      sum_instructor_squared += freq_instructor[i] * freq_instructor[i];
      valid_count++;
    }
  }

  if (valid_count == 0)
    return 0.0;

  mse /= valid_count;
  sum_instructor_squared /= valid_count;

  // Calculate similarity as percentage (100% - normalized error percentage)
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

bool LAB_Analog_Circuit_Checker::
export_result_file(const std::string& file_path,
                   double time_similarity_percentage,
                   double time_lag_samples,
                   double freq_similarity_percentage,
                   double freq_lag_bins,
                   const std::vector<double>& student_signal)
{
  if (!m_is_file_loaded)
    return false;

  const std::string out_path = file_path + ".result";

  std::string xml;
  xml.reserve(1024 + (student_signal.size() * 12));
  xml += "<result>\n";
  xml += "  <source_file>" + file_path + "</source_file>\n";
  xml += "  <time_domain>\n";
  {
    char buf[128];
    std::snprintf(buf, sizeof(buf), "%.2f", time_similarity_percentage);
    xml += "    <similarity_percentage>"; xml += buf; xml += "</similarity_percentage>\n";
    std::snprintf(buf, sizeof(buf), "%.0f", time_lag_samples);
    xml += "    <lag_samples>"; xml += buf; xml += "</lag_samples>\n";
  }
  xml += "  </time_domain>\n";
  xml += "  <frequency_domain>\n";
  {
    char buf[128];
    std::snprintf(buf, sizeof(buf), "%.2f", freq_similarity_percentage);
    xml += "    <similarity_percentage>"; xml += buf; xml += "</similarity_percentage>\n";
    std::snprintf(buf, sizeof(buf), "%.0f", freq_lag_bins);
    xml += "    <lag_bins>"; xml += buf; xml += "</lag_bins>\n";
  }
  xml += "  </frequency_domain>\n";
  xml += "  <student_signal>\n";
  xml += "    <samples>";
  for (size_t i = 0; i < student_signal.size(); ++i)
  {
    char sbuf[64];
    std::snprintf(sbuf, sizeof(sbuf), "%.6f", student_signal[i]);
    xml += sbuf;
    if (i + 1 < student_signal.size()) xml += ",";
  }
  xml += "</samples>\n";
  xml += "  </student_signal>\n";
  xml += "</result>\n";

  if (FILE* fp = std::fopen(out_path.c_str(), "wb"))
  {
    std::fwrite(xml.data(), 1, xml.size(), fp);
    std::fclose(fp);
    return true;
  }
  return false;
}

// EOF
