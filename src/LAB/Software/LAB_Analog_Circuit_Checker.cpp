#include "LAB_Analog_Circuit_Checker.h"
#include <stdexcept>
#include <cstdio>
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <iomanip>
#include <cassert>
#include <algorithm>

#include "../../Utility/pugixml.hpp"
// #include "../../../lib/KISSFFT/kiss_fftr.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// constexpr size_t N        = 2000;

LAB_Analog_Circuit_Checker::
    LAB_Analog_Circuit_Checker(LAB &_lab)
    : LAB_Module(_lab)
{
}

/*static void print_samples(const std::vector<double> &instructor,
                          const std::vector<double> &student);

// Member version to satisfy linkage when called from within the class
void LAB_Analog_Circuit_Checker::
    print_samples(const std::vector<double> &instructor,
                  const std::vector<double> &student)
{
  constexpr size_t NUM_TO_PRINT = 10;
  const size_t N_instructor = std::min(NUM_TO_PRINT, instructor.size());
  const size_t N_student = std::min(NUM_TO_PRINT, student.size());

  std::cout << "Instructor Data (first 10): [ ";
  for (size_t i = 0; i < N_instructor; i++)
  {
    std::cout << std::fixed << std::setprecision(4) << instructor[i];
    if (i < N_instructor - 1)
      std::cout << ", ";
  }
  std::cout << " ]\n";

  std::cout << "Student Data (first 10):    [ ";
  for (size_t i = 0; i < N_student; i++)
  {
    std::cout << std::fixed << std::setprecision(4) << student[i];
    if (i < N_student - 1)
      std::cout << ", ";
  }
  std::cout << " ]\n";
}*/

/*LAB_Analog_Circuit_Checker::
    LAB_Analog_Circuit_Checker(LAB &_lab)
    : LAB_Module(_lab)
{
  ChannelData channel;

  constexpr size_t NUM_SAMPLES = 2000;
  dummy_student_data.resize(NUM_SAMPLES);

  double Fs = 40e3; // Sampling frequency
  double amplitude = 1500;
  double f = 100; // 100 Hz square wave
  double offset = 2048;
  size_t delay = 2;

  for (size_t n = 0; n < NUM_SAMPLES; ++n)
  {
    // Apply delay (if needed)
    size_t delayed_index = (n >= delay) ? (n - delay) : 0;
    double t_delayed = static_cast<double>(delayed_index) / Fs;

    // Generate square wave:  +amplitude for half cycle, -amplitude for other half
    double sq_val = (fmod(f * t_delayed, 1.0) < 0.5) ? amplitude : -amplitude;

    // Add offset (to simulate ADC raw values around midscale)
    dummy_student_data[n] = sq_val + offset;
  }

  // Normalize values to [-1, 1]
  for (auto &val : dummy_student_data)
  {
    val = (val - offset) / amplitude;
  }*/

// using scalar_t = kiss_fft_scalar;
// using cpx_t    = kiss_fft_cpx;

// kiss_fftr_cfg  cfg = kiss_fftr_alloc(static_cast<int>(N), 0, nullptr, nullptr);
// assert(cfg && "kiss_fftr_alloc failed");

// //dummy student data
// std::vector<scalar_t>xB(N);
// for (size_t n = 0; n < N; ++n) xB[n] = static_cast<scalar_t>(dummy_student_data[n]);
// std::vector<cpx_t> XB(N/2 + 1);
// kiss_fftr(cfg, xB.data(), XB.data());

// //instructor data
// std::vector<scalar_t> xA(N);
// for (size_t n = 0; n<N; ++n) xA[n] = static_cast<scalar_t>(channel.sample_data[n]);
// std::vector<cpx_t> XA(N/2 + 1);
// kiss_fftr(cfg, xA.data(), XA.data());

// std::vector<double> magB(N/2 + 1);
// size_t peak_bin = 0;
// double peak_val = -1.0;

// for (size_t k = 0; k <XB.size(); ++k)
// {
//   const double re = XB[k].r;
//   const double im = XB[k].i;
//   magB[k] = std::sqrt(re*re + im*im);

//   if (k > 0 && magB[k] > peak_val)
//   {   // ignore DC
//     peak_val = magB[k];
//     peak_bin = k;
//   }
// }

// const double bin_hz  = Fs / N;
// const double peak_hz = peak_bin * bin_hz;

// std::cout << "\nFFT summary (student signal, noisy):\n";
//   std::cout << "  Peak bin: " << peak_bin
//             << "  (~" << std::fixed << std::setprecision(1) << peak_hz << " Hz)\n";

//   const double re = XB[peak_bin].r, im = XB[peak_bin].i;
//   const double phase_rad = std::atan2(im, re);
//   std::cout << "  Phase at peak: " << std::setprecision(6) << phase_rad << " rad\n";

//   const double expected_phi = std::fmod(2.0 * M_PI * f * (static_cast<double>(delay)/Fs), 2.0*M_PI);
//   std::cout << "  Expected phase from " << delay << "-sample delay: " << expected_phi << " rad\n";

//   std::cout << "\nFirst 8 bins (magnitude):\n";
//   for (size_t k = 0; k < 8 && k < magB.size(); ++k)
//   {
//     std::cout << "  k=" << k
//               << "  f=" << std::setw(6) << std::fixed << std::setprecision(1) << k*bin_hz
//               << " Hz  |X|=" << std::setprecision(6) << magB[k] << "\n";
//   }

//   double dot = 0.0, nA = 0.0, nB = 0.0;
//   for (size_t k = 1; k < XA.size() && k < XB.size(); ++k)
//   { // skip DC
//     const double mA = std::hypot(XA[k].r, XA[k].i);
//     const double mB = std::hypot(XB[k].r, XB[k].i);
//     dot += mA * mB;
//     nA  += mA * mA;
//     nB  += mB * mB;
//   }
//   const double spec_cos = dot / (std::sqrt(nA * nB) + 1e-20);
//   std::cout << "\nFFT similarity (magnitude spectrum, noisy): "
//             << std::fixed << std::setprecision(2) << (spec_cos * 100.0) << " %\n";

//   free(cfg);
//}

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
  // New format: data is under root/oscilloscope
  pugi::xml_node osc = m_xml_doc.child("root").child("oscilloscope");

  if (!osc)
  {
    throw std::runtime_error("Missing oscilloscope section in .labacc file");
  }

  // Extract oscilloscope configuration from XML
  // Channels: prefer explicit count from <channels>, else default to 1
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
  m_sampling_rate = osc.child("sampling_rate").text().as_double();
  m_time_per_division = osc.child("time_per_division").text().as_double();
  m_horizontal_offset = osc.child("horizontal_offset").text().as_double();

  // Extract trigger configuration
  m_trigger_mode = osc.child("trigger_mode").text().as_uint();
  m_trigger_source = osc.child("trigger_source").text().as_uint();
  m_trigger_type = osc.child("trigger_type").text().as_uint();
  m_trigger_condition = osc.child("trigger_condition").text().as_uint();
  m_trigger_level = osc.child("trigger_level").text().as_double();

  // Comparison settings (optional): look under root/comparison or osc children
  pugi::xml_node cmp = m_xml_doc.child("root").child("comparison");
  if (cmp)
  {
    m_cmp_time_domain = cmp.child("time_domain").text().as_bool();
    m_cmp_frequency_domain = cmp.child("frequency_domain").text().as_bool();
    m_cmp_similarity_threshold = cmp.child("similarity_threshold").text().as_double();
  }
  else
  {
    // try osc-level for backward-compat
    m_cmp_time_domain = osc.child("time_domain").text().as_bool(false);
    m_cmp_frequency_domain = osc.child("frequency_domain").text().as_bool(false);
    m_cmp_similarity_threshold = osc.child("similarity_threshold").text().as_double(0.0);
  }
}

void LAB_Analog_Circuit_Checker::
    load_channel_data_acc()
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

      // Only Channel 2 (index 1) will have samples in new files; parse if present
      if (pugi::xml_node samples_node = ch.child("samples"))
      {
        for (pugi::xml_node sample : samples_node.children("sample"))
          channel.sample_data.push_back(sample.text().as_double());
      }

      if (index_attr == 1)

      {
        time_domain_analysis(channel.sample_data);
        // instructor_data = channel.sample_data;

        // double comparison = compute_similarity();
        // std::cout << "\nSimilarity: " << comparison << "%\n";
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
  // New format: function generator data under root/function_generator
  pugi::xml_node fg = m_xml_doc.child("root").child("function_generator");

  if (!fg)
  {
    // Not strictly required; leave defaults
    return;
  }

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
    // Load and parse the .labacc XML file
    pugi::xml_parse_result result = m_xml_doc.load_file(path.c_str());

    if (!result)
    {
      throw std::runtime_error("Failed to load .labacc file: " + std::string(result.description()));
    }

    m_file_path = path;
    m_is_file_loaded = true;

    // Load all data from the file
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

/*void LAB_Analog_Circuit_Checker::
    cross_correlation(const std::vector<double> &instructor,
                          const std::vector<double> &student)
{
  ChannelData channel;

  const size_t N = std::min(instructor.size(), student.size());
  double numerator = 0.0;
  double denom_x = 0.0;
  double denom_y = 0.0;

  for (size_t n = 0; n < N; n++)
  {
    numerator += instructor[n] * student[n];
    denom_x += instructor[n] * instructor[n];
    denom_y += student[n] * student[n];
  }

  if (denom_x == 0.0 || denom_y == 0.0){

    std::cout << "\nInvalid Comparison%\n";
  }
    // avoid div by zero

  double corr = numerator / std::sqrt(denom_x * denom_y); // normalized correlation
  double percentage = corr * 100.0;



  const size_t N = std::min(instructor_data.size(), dummy_student_data.size());
  if (N == 0)
    return 0.0;

  // Compute means
  double mean_x = 0.0, mean_y = 0.0;
  for (size_t n = 0; n < N; n++)
  {
    mean_x += instructor_data[n];
    mean_y += dummy_student_data[n];
  }
  mean_x /= N;
  mean_y /= N;

  // Compute numerator and denominators
  double numerator = 0.0;
  double denom_x = 0.0;
  double denom_y = 0.0;

  for (size_t n = 0; n < N; n++)
  {
    double x = instructor_data[n] - mean_x;
    double y = dummy_student_data[n] - mean_y;

    numerator += x * y;
    denom_x += x * x;
    denom_y += y * y;
  }

  if (denom_x == 0.0 || denom_y == 0.0)
    return 0.0; // avoid div by zero

  return numerator / std::sqrt(denom_x * denom_y); // Pearson correlation

}*/

/*double LAB_Analog_Circuit_Checker::
    compute_similarity()
{
  double corr = compute_cross_correlation();

  return corr * 100.0; // percentage
                       // if corr =  1.0 ->  100% similar
                       // if corr =  0.0 ->  0%   similar
                       // if corr = -1.0 -> -100% (perfectly opposite)
}*/

LAB_Analog_Circuit_Checker::CorrelationResult
LAB_Analog_Circuit_Checker::cross_correlation(const std::vector<double> &x,
                                              const std::vector<double> &y)
{
  CorrelationResult result{0.0, 0.0};

  if (x.empty() || y.empty())
  {
    std::cout << "\nInvalid Comparison: empty signal\n";
    return result;
  }

  const size_t N = std::min(x.size(), y.size());
  double numerator = 0.0;
  double denom_x = 0.0;
  double denom_y = 0.0;

  for (size_t n = 0; n < N; n++)
  {
    numerator += x[n] * y[n];
    denom_x += x[n] * x[n];
    denom_y += y[n] * y[n];
  }

  if (denom_x == 0.0 || denom_y == 0.0)
  {
    std::cout << "\nInvalid Comparison: divide by zero\n";
    return result;
  }

  constexpr size_t NUM_TO_PRINT = 10;
  const size_t N_instructor = std::min(NUM_TO_PRINT, x.size());
  const size_t N_student = std::min(NUM_TO_PRINT, y.size());

  std::cout << "Instructor Data (first 10): [ ";
  for (size_t i = 0; i < N_instructor; i++)
  {
    std::cout << std::fixed << std::setprecision(4) << x[i];
    if (i < N_instructor - 1)
      std::cout << ", ";
  }
  std::cout << " ]\n";

  std::cout << "Student Data (first 10):    [ ";
  for (size_t i = 0; i < N_student; i++)
  {
    std::cout << std::fixed << std::setprecision(4) << y[i];
    if (i < N_student - 1)
      std::cout << ", ";
  }
  std::cout << " ]\n";

  result.coefficient = numerator / std::sqrt(denom_x * denom_y);
  result.percentage = result.coefficient * 100.0;

  return result;
}

void LAB_Analog_Circuit_Checker::time_domain_analysis(const std::vector<double> &instructor)
{
  /*  if (m_channel_data.size() < 2)
    {
      std::cout << "\nNot enough channels loaded for comparison\n";
      return;
    }*/

  // Channel 2 (index 1) as Instructor
  //  ChannelData channel;

  // const std::vector<double> &instructor = channel.sample_data;

  // Student data (dummy signal)
  constexpr size_t NUM_SAMPLES = 2000;
  std::vector<double> student(NUM_SAMPLES);

  double Fs = 40e3; // Sampling frequency
  double amplitude = 1500;
  double f = 100; // 100 Hz square wave
  double offset = 2048;
  size_t delay = 2;

  for (size_t n = 0; n < NUM_SAMPLES; ++n)
  {
    // Apply delay
    size_t delayed_index = (n >= delay) ? (n - delay) : 0;
    double t_delayed = static_cast<double>(delayed_index) / Fs;

    // Generate square wave
    double sq_val = (fmod(f * t_delayed, 1.0) < 0.5) ? amplitude : -amplitude;

    // Add offset
    student[n] = sq_val + offset;
  }

  // Normalize to [-1, 1]
  for (auto &val : student)
  {
    val = (val - offset) / amplitude;
  }
  // Perform cross-correlation
  CorrelationResult result = cross_correlation(instructor, instructor);

  std::cout << "\nTime-Domain Analysis Result:\n";
  std::cout << "  Correlation Coefficient: " << std::fixed << std::setprecision(4)
            << result.coefficient << "\n";
  std::cout << "  Similarity Percentage:   " << std::fixed << std::setprecision(2)
            << result.percentage << " %\n";
}
