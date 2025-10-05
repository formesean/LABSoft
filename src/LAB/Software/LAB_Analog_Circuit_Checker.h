#ifndef LAB_ANALOG_CIRCUIT_CHECKER_H
#define LAB_ANALOG_CIRCUIT_CHECKER_H

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <iomanip>
#include <algorithm>
#include <complex>

#include "../LAB_Module.h"
#include "../../Utility/pugixml.hpp"

class LAB_Analog_Circuit_Checker : public LAB_Module
{
  private:
    struct ChannelData
    {
      std::string name;
      unsigned samples;
      bool coupling;
      unsigned scaling;
      double voltage_per_division;
      double vertical_offset;
      bool is_enabled;
      double scaling_corrector;
      struct
      {
        double min, max, avg, trms;
      } measurements;

      std::vector<double> sample_data;
    };
    std::vector<ChannelData> m_channel_data;

    struct FunctionGenData
    {
      unsigned wave_type;
      double frequency;
      double period;
    } m_func_gen_data;

  public:
    struct CorrelationResult
    {
      double lag;
      double coefficient;
      double percentage;
    };

  private:
    void load_data_from_file_acc          ();
    void load_metadata_acc                ();
    void load_channel_data_acc            ();
    void load_function_generator_data_acc ();
    void clear_data_acc                   ();

    CorrelationResult cross_correlation (const std::vector<double> &x,
                                         const std::vector<double> &y);

    bool                m_is_file_loaded = false;
    std::string         m_file_path;
    pugi::xml_document  m_xml_doc;

    // Metadata
    unsigned  m_channels;
    unsigned  m_samples;
    double    m_sampling_rate;
    double    m_time_per_division;
    double    m_horizontal_offset;
    unsigned  m_trigger_mode;
    unsigned  m_trigger_source;
    unsigned  m_trigger_type;
    unsigned  m_trigger_condition;
    double    m_trigger_level;

    // Comparison settings
    bool    m_cmp_time_domain                     = false;
    bool    m_cmp_frequency_domain                = false;
    double  m_cmp_time_similarity_threshold       = 0.0;
    double  m_cmp_frequency_similarity_threshold  = 0.0;

  public:
    LAB_Analog_Circuit_Checker(LAB &_lab);

    void load_file(const std::string &path);
    void unload_file();
    bool is_file_loaded() const { return m_is_file_loaded; }
    void load_data_acc(); // Main function to load data into oscilloscope

    CorrelationResult                 signal_analysis (const std::vector<double> &instructor,
                                                       const std::vector<double> &student);
    std::vector<std::complex<double>> compute_fft     (const std::vector<double> &data);

    double compute_magnitude_error_similarity(const std::vector<double>& freq_instructor,
                                             const std::vector<double>& freq_student);

    bool export_result_file(const std::string& file_path,
                           double time_similarity_percentage,
                           double time_lag_samples,
                           double freq_similarity_percentage,
                           double freq_lag_bins,
                           const std::vector<double>& student_signal);

    // Getters
    const std::vector<ChannelData> &get_channel_data            () const { return m_channel_data; }
    const FunctionGenData          &get_function_generator_data () const { return m_func_gen_data; }
    const std::string              &get_file_path               () const { return m_file_path; }

    // Metadata getters
    unsigned  get_samples                             () const { return m_samples; }
    double    get_time_per_division                   () const { return m_time_per_division; }
    double    get_sampling_rate                       () const { return m_sampling_rate; }
    double    get_horizontal_offset                   () const { return m_horizontal_offset; }

    // Trigger getters
    unsigned  get_trigger_mode                        () const { return m_trigger_mode; }
    unsigned  get_trigger_source                      () const { return m_trigger_source; }
    unsigned  get_trigger_type                        () const { return m_trigger_type; }
    unsigned  get_trigger_condition                   () const { return m_trigger_condition; }
    double    get_trigger_level                       () const { return m_trigger_level; }

    // Comparison getters
    bool      get_cmp_time_domain                     () const { return m_cmp_time_domain; }
    bool      get_cmp_frequency_domain                () const { return m_cmp_frequency_domain; }
    double    get_cmp_time_similarity_threshold       () const { return m_cmp_time_similarity_threshold; }
    double    get_cmp_frequency_similarity_threshold  () const { return m_cmp_frequency_similarity_threshold; }
};

#endif
