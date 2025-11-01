#ifndef LAB_DEFINITIONS_H
#define LAB_DEFINITIONS_H

#include <array>
#include <vector>

#include "LAB_Constants.h"
#include "LAB_Defaults.h"
#include "LAB_Enumerations.h"

// Calibration Data
struct LAB_Calibration_Data_Oscilloscope
{
  double adc_reference_voltage        = LABD::OSC::ADC_REFERENCE_VOLTAGE;
  double conversion_reference_voltage = LABD::OSC::CONVERSION_REFERENCE_VOLTAGE;
  double conversion_constant          = LABD::OSC::CONVERSION_CONSTANT;

  std::unordered_map <LABE::OSC::SCALING, double> vertical_offset_corrector   = LABD::OSC::VERTICAL_OFFSET_CORRECTOR;
  std::unordered_map <LABE::OSC::SCALING, double> scaling_corrector_to_unity  = LABD::OSC::SCALING_CORRECTOR_TO_UNITY;
  std::unordered_map <LABE::OSC::SCALING, double> scaling_corrector_to_actual = LABD::OSC::SCALING_CORRECTOR_TO_ACTUAL;
};

struct LAB_Calibration_Data_Voltmeter
{

};

struct LAB_Calibration_Data_Ohmmeter
{
  double r1   = LABC::OHMMETER::R1_RESISTOR_VALUE;
  double vref = LABC::OHMMETER::VIN_VOLTAGE_VALUE;
};

// Measurements Data
struct LAB_Measurements_Data_Oscilloscope
{
  double min  = 0.0;
  double max  = 0.0;
  double avg  = 0.0;
  double trms = 0.0;
};

// Channel Data
struct LAB_Channel_Data_Oscilloscope
{
  bool                is_enabled            = LABD::OSC::IS_ENABLED;

  double              scaling_corrector     = 1.0;

  double              voltage_per_division  = LABD::OSC::VOLTAGE_PER_DIVISION;
  double              vertical_offset       = LABD::OSC::VERTICAL_OFFSET;
  LABE::OSC::SCALING  scaling               = LABD::OSC::SCALING;
  LABE::OSC::COUPLING coupling              = LABD::OSC::COUPLING;

  std::array<double, LABC::OSC::NUMBER_OF_SAMPLES> samples = {0.0};

  LAB_Calibration_Data_Oscilloscope   calibration;
  LAB_Measurements_Data_Oscilloscope  measurements;
};

struct LAB_Channel_Data_Voltmeter
{

};

struct LAB_Channel_Data_Ohmmeter
{
  LAB_Calibration_Data_Ohmmeter calibration;
};

struct LAB_Channel_Data_Function_Generator
{
  // Channel
  bool is_enabled = false;

  // Parameters
  LABE::FUNC_GEN::WAVE_TYPE wave_type       = LABC::FUNC_GEN::WAVE_TYPE;
  double                    frequency       = LABC::FUNC_GEN::FREQUENCY;
  double                    period          = LABC::FUNC_GEN::PERIOD;
  double                    amplitude       = LABC::FUNC_GEN::AMPLITUDE;
  double                    vertical_offset = LABC::FUNC_GEN::VERTICAL_OFFSET;
  double                    phase           = LABC::FUNC_GEN::PHASE;
  double                    Rf              = LABC::FUNC_GEN::RF_RESISTANCE;
};

struct LAB_Channel_Data_Logic_Analyzer
{
  bool      is_enabled                      = true;
  unsigned  raw_sample_buffer_working_size  = 0;

  // triggre
  LABE::LOGAN::TRIG::CND trigger_condition = LABD::LOGAN::TRIGGER_CONDITION;

  // Data/Samples
  std::vector <std::array<int, 2>> pixel_points;
  std::array  <bool, LABC::LOGAN::MAX_NUMBER_OF_SAMPLES> samples;
};

// Parent Data
struct LAB_Parent_Data_Oscilloscope
{
  public:
    // state
    bool              is_backend_running      = false;
    bool              is_frontend_running     = false;
    bool              single                  = false;
    bool              do_measurements         = false;
    LABE::OSC::STATUS status                  = LABE::OSC::STATUS::READY;
    bool              is_calibration_enabled  = true;

    // mode
    LABE::OSC::MODE mode                      = LABD::OSC::MODE;
    LABE::OSC::MODE last_mode_before_repeated = LABE::OSC::MODE::SCREEN;
    bool            auto_mode_frame_ready     = false;

    // horizontal
    double    horizontal_offset               = LABD::OSC::HORIZONTAL_OFFSET;
    double    horizontal_offset_raw_buffer    = horizontal_offset;
    double    time_per_division               = LABD::OSC::TIME_PER_DIVISION;
    double    time_per_division_last_repeated = time_per_division;
    double    time_per_division_raw_buffer    = time_per_division;
    unsigned  samples                         = LABD::OSC::SAMPLES;
    unsigned  samples_raw_buffer              = samples;
    double    samples_displayed               = 0;
    double    sampling_rate                   = LABD::OSC::SAMPLING_RATE;
    double    sampling_rate_raw_buffer        = sampling_rate;

    // Data/Samples/Pixels
    std::vector<uint32_t> recording_raw_sample_buffer;

    std::array<
      uint32_t,
      LABC::OSC::NUMBER_OF_SAMPLES
    > raw_data_buffer;

    std::array<
      LAB_Channel_Data_Oscilloscope,
      LABC::OSC::NUMBER_OF_CHANNELS
    > channel_data;

    // trigger
    bool                  trigger_enabled         = false;
    bool                  trigger_frame_ready     = false;
    bool                  trigger_found           = false;
    bool                  find_trigger_timeout    = false;
    LABE::OSC::TRIG::MODE trigger_mode            = LABD::OSC::TRIGGER_MODE;
    unsigned              trigger_source          = LABD::OSC::TRIGGER_SOURCE;
    LABE::OSC::TRIG::TYPE trig_type               = LABD::OSC::TRIGGER_TYPE;
    LABE::OSC::TRIG::CND  trig_condition          = LABD::OSC::TRIGGER_CONDITION;
    double                trigger_level           = LABD::OSC::TRIGGER_LEVEL;
    double                trigger_level_raw_bits  = (LABC::OSC::ADC_RESOLUTION_INT - 1) / 2;
    unsigned              find_trig_sample_skip   = 4;
    unsigned              trigger_buffer_index    = 0;
    unsigned              trig_index              = 0;

    // Debug
    unsigned              buffer_overflow_count   = 0;

    struct TriggerBuffers
    {
      std::array<
        std::array<uint32_t, LABC::OSC::NUMBER_OF_SAMPLES>,
        LABC::OSC::NUMBER_OF_CHANNELS
      > pre_trigger;

      std::array<
        std::array<uint32_t, LABC::OSC::NUMBER_OF_SAMPLES>,
        LABC::OSC::NUMBER_OF_CHANNELS
      > post_trigger;

      std::array<uint32_t, LABC::OSC::NUMBER_OF_SAMPLES> assembled_block;
    } trig_buffs;

  public:
    bool has_enabled_channels () const
    {
      for (const auto& e : channel_data)
      {
        if (e.is_enabled)
        {
          return (true);
        }
      }

      return (false);
    }
};

struct LAB_Parent_Data_Voltmeter
{
  bool is_backend_running   = false;
  bool is_frontend_running  = false;

  std::array<
    LAB_Channel_Data_Voltmeter,
    LABC::VOLTMETER::NUMBER_OF_CHANNELS
  > channel_data;
};

struct LAB_Parent_Data_Ohmmeter
{
  bool is_backend_running     = false;
  bool is_frontend_running    = false;
  bool is_calibration_enabled = true;

  std::array<
    LAB_Channel_Data_Ohmmeter,
    LABC::OHMMETER::NUMBER_OF_CHANNELS
  > channel_data;
};

class LAB_Parent_Data_Function_Generator
{
  public:
    std::array <LAB_Channel_Data_Function_Generator, LABC::FUNC_GEN::NUMBER_OF_CHANNELS> channel_data;

  public:
    bool has_enabled_channels ()
    {
      for (const auto& cdata : channel_data)
      {
        if (cdata.is_enabled)
        {
          return (true);
        }
      }

      return (false);
    }
};

struct LAB_Parent_Data_Logic_Analyzer
{
  // State
  bool                is_backend_running  = false;
  bool                is_frontend_running = false;
  bool                single              = false;
  LABE::LOGAN::STATUS status              = LABE::LOGAN::STATUS::READY;

  // Mode
  LABE::LOGAN::MODE mode                      = LABD::LOGAN::MODE;
  LABE::LOGAN::MODE last_mode_before_repeated = mode;

  // Horizontal
  double    horizontal_offset             = LABD::LOGAN::HORIZONTAL_OFFSET;
  double    time_per_division             = LABD::LOGAN::TIME_PER_DIVISION;
  double    time_per_division_raw_buffer  = time_per_division;
  unsigned  samples                       = LABD::LOGAN::SAMPLES;
  unsigned  samples_raw_buffer            = samples;
  double    sampling_rate                 = LABD::LOGAN::SAMPLING_RATE;
  double    sampling_rate_raw_buffer      = sampling_rate;
  double    sampling_period               = LABD::LOGAN::SAMPLING_PERIOD;

  std::array <
    uint32_t,
    LABC::LOGAN::MAX_NUMBER_OF_SAMPLES
  > raw_data_buffer;

  std::array <LAB_Channel_Data_Logic_Analyzer,
    LABC::LOGAN::NUMBER_OF_CHANNELS> channel_data;

  // Trigger
  bool                    trigger_enabled             = false;
  bool                    trigger_frame_ready         = false;
  bool                    trigger_found               = false;
  bool                    find_trigger_timeout        = false;
  LABE::LOGAN::TRIG::MODE trigger_mode                = LABD::LOGAN::TRIGGER_MODE;
  double                  check_trigger_sleep_period  = LABD::LOGAN::CHECK_TRIGGER_SLEEP_PERIOD;
  uint32_t                trigger_flags               = 0;
  unsigned                trigger_buffer_index        = 0;
  unsigned                trigger_index               = 0;
  std::vector<unsigned>   trigger_cache_edge;
  std::vector<unsigned>   trigger_cache_level;

  struct TriggerBuffers
    {
      std::array<
        std::array<uint32_t, LABC::OSC::NUMBER_OF_SAMPLES>,
        LABC::OSC::NUMBER_OF_CHANNELS
      > pre_trigger;

      std::array<
        std::array<uint32_t, LABC::OSC::NUMBER_OF_SAMPLES>,
        LABC::OSC::NUMBER_OF_CHANNELS
      > post_trigger;

      std::array<
        uint32_t,
        LABC::OSC::NUMBER_OF_SAMPLES
      > assembled_frame;
    } trigger_buffers;
};

struct LAB_Parent_Data_Software_Navigation
{
  int     BAUD_RATE     = LABC::PIN::SNM::BAUD_RATE;
  uint8_t CS_PIN        = LABC::PIN::SNM::CS_PIN;
  uint8_t MISO_PIN      = LABC::PIN::SNM::MISO_PIN;
  uint8_t MOSI_PIN      = LABC::PIN::SNM::MOSI_PIN;
  uint8_t SCLK_PIN      = LABC::PIN::SNM::SCLK_PIN;

  uint8_t TRANSFER_SIZE = LABC::PIN::SNM::TRANSFER_SIZE;
  bool    SNM_ATTACHED  = true;
};

// DMA Data
struct LAB_DMA_Data_Oscilloscope
{
  AP::DMA::CTL_BLK cbs[15];

  uint32_t spi_samp_size      = 0,
           spi_cs             = 0,
           spi_cs_fifo_reset  = 0,
           pwm_duty_cycle     = 0,
           txd                = 0;

  volatile uint32_t status[LABC::OSC::NUMBER_OF_CHANNELS];

  volatile uint32_t rxd[2][LABC::OSC::NUMBER_OF_SAMPLES] = {{0}};
};

struct LAB_DMA_Data_Logic_Analyzer
{
  AP::DMA::CTL_BLK cbs[15];

  uint32_t  buffer_ok_flag = 0x1,
            pwm_duty_cycle;

  volatile  uint32_t status[LABC::LOGAN::NUMBER_OF_CHANNELS];
  volatile  uint32_t rxd[2][LABC::LOGAN::MAX_NUMBER_OF_SAMPLES];
};

//
struct Oscilloscope_Calibration_GUI_Info
{
  unsigned            channel;
  LABE::OSC::SCALING  scaling;
};

#endif
