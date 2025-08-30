#ifndef LAB_ANALOG_CIRCUIT_CHECKER_H
#define LAB_ANALOG_CIRCUIT_CHECKER_H

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <iomanip>
#include <algorithm>

#include "../LAB_Module.h"
#include "../../Utility/pugixml.hpp"

class LAB_Analog_Circuit_Checker : public LAB_Module
{
  private:

  void                  load_data_from_file_acc   ();
  void                  load_metadata_acc         ();
  void                  load_channel_data_acc     ();
  void                  load_function_generator_data_acc ();
  void                  clear_data_acc            ();

  bool                  m_is_file_loaded = false;
  std::string           m_file_path;
  pugi::xml_document    m_xml_doc;

  // Metadata
  unsigned              m_channels;
  unsigned              m_samples;
  double                m_sampling_rate;
  double                m_time_per_division;
  double                m_horizontal_offset;
  unsigned              m_trigger_mode;
  unsigned              m_trigger_source;
  unsigned              m_trigger_type;
  unsigned              m_trigger_condition;
  double                m_trigger_level;

  // Comparison settings
  bool                  m_cmp_time_domain = false;
  bool                  m_cmp_frequency_domain = false;
  double                m_cmp_similarity_threshold = 0.0;

  // Data per channel
  struct ChannelData
  {
    std::string   name;
    unsigned      samples;
    bool          coupling;
    unsigned      scaling;
    double        voltage_per_division;
    double        vertical_offset;
    bool          is_enabled;
    double        scaling_corrector;
    struct
    {
      double      min, max, avg, trms;
    } measurements;

    std::vector<double> sample_data;
  };
  std::vector<ChannelData> m_channel_data;

  // Function Generator Data
  struct FunctionGenData
  {
    unsigned    wave_type;
    double      frequency;
    double      period;
  }m_func_gen_data;

  public:
    LAB_Analog_Circuit_Checker(LAB& _lab);

    void      load_file                    (const std::string& path);
  void      unload_file     ();
    double   compute_cross_correlation  ();
    double   compute_similarity         ();

  void      load_data_acc   ();  // Main function to load data into oscilloscope

  // Getters
  bool      is_file_loaded  () const { return m_is_file_loaded; }
  const std::vector<ChannelData>& get_channel_data() const { return m_channel_data; }
  const FunctionGenData& get_function_generator_data() const { return m_func_gen_data; }

  // Metadata getters
  double    get_time_per_division() const { return m_time_per_division; }
  unsigned  get_samples() const { return m_samples; }
  double    get_sampling_rate() const { return m_sampling_rate; }
  double    get_horizontal_offset() const { return m_horizontal_offset; }

  // Comparison getters
  bool      get_cmp_time_domain() const { return m_cmp_time_domain; }
  bool      get_cmp_frequency_domain() const { return m_cmp_frequency_domain; }
  double    get_cmp_similarity_threshold() const { return m_cmp_similarity_threshold; }

    std::vector<double> dummy_student_data;
    std::vector<double> dummy_instructor_data;

    void     print_samples (const std::vector<double>& instructor,
                            const std::vector<double>& student);
};

#endif
