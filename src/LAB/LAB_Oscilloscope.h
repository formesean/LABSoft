#ifndef LAB_OSCILLOSCOPE_H
#define LAB_OSCILLOSCOPE_H

#include <thread>

#include "../Utility/LAB_Definitions.h"
#include "../Utility/LAB_Enumerations.h"
#include "LAB_Module.h"

class LAB_Oscilloscope : public LAB_Module
{
public:
  class Calibration
  {
  private:
    LAB_Parent_Data_Oscilloscope& m_pdata;

  public:
    Calibration(LAB_Parent_Data_Oscilloscope& _LAB_Parent_Data_Oscilloscope);

    void enable();
    void disable();
    bool is_enabled() const;

    bool do_auto_calibration_vertical_offset_corrector();
    bool do_auto_calibration_scaling_correctors();

    void adc_reference_voltage(unsigned channel, double value);
    void vertical_offset_corrector(unsigned channel,
                                   LABE::OSC::SCALING scaling,
                                   double value);
    void scaling_corrector_to_unity(unsigned channel,
                                    LABE::OSC::SCALING scaling,
                                    double value);
    void scaling_corrector_to_actual(unsigned channel,
                                     LABE::OSC::SCALING scaling,
                                     double value);

    double adc_reference_voltage(unsigned channel) const;
    double vertical_offset_corrector(unsigned channel,
                                     LABE::OSC::SCALING scaling) const;
    double scaling_corrector_to_unity(unsigned channel,
                                      LABE::OSC::SCALING scaling) const;
    double scaling_corrector_to_actual(unsigned channel,
                                       LABE::OSC::SCALING scaling) const;
  };

  class Measurements
  {
  private:
    LAB_Parent_Data_Oscilloscope& m_pdata;

  public:
    Measurements(LAB_Parent_Data_Oscilloscope& _LAB_Parent_Data_Oscilloscope);

    double avg(unsigned chan);
    double max(unsigned chan);
    double min(unsigned chan);
    double trms(unsigned chan);
  };

private:
  std::thread m_thread_trigger;
  std::thread m_find_trigger_timer;
  AikaPi::Uncached m_uncached_memory;
  LAB_Parent_Data_Oscilloscope m_parent_data;
  Measurements m_measurements;
  Calibration m_calibration;

private:
  // Setup
  void init_spi();
  void init_pwm();
  void init_gpio_pins();
  void init_dma();
  void init_state();
  void config_dma_cb();

  // Master Controls
  void master_run_stop(bool value);

  // State
  void status(LABE::OSC::STATUS _STATUS);
  void reload_settings();

  // Mode
  void set_mode(LABE::OSC::MODE mode);
  void dma_buffer_count(LABE::OSC::BUFFER_COUNT buffer_count);

  // Horizontal
  double calc_time_per_division(unsigned samples, double sampling_rate) const;
  double calc_sampling_rate(unsigned samples, double time_per_division) const;
  void set_time_per_division(double value);
  void set_samples(unsigned value);
  void set_sampling_rate(double value);

  // mode
  LABE::OSC::MODE calc_mode(double time_per_division);

  // trigger
  void parse_trigger_mode();
  void trigger_osc_mode_dispatcher();
  void trigger_osc_mode_repeated();

  void find_trigger_point_loop();
  bool find_trigger_point();
  void create_trigger_frame();
  void find_trigger_timeout_timer();
  uint32_t calc_trigger_level_raw_bits(double trigger_level);

  void prefill_buffers_for_triggering();

  // Record
  void config_dma_cb_record();

  // Other
  void debug();

  // Data and conversion
  void fill_raw_osc_samp_buff_from_dma_buff();
  void parse_raw_osc_samp_buff();

  double conv_raw_osc_samp_to_actual_chan_value(uint32_t raw_osc_samp,
                                                unsigned channel);

  uint32_t extract_raw_osc_chan_samp_from_raw_osc_samp(uint32_t raw_osc_samp,
                                                       unsigned channel);

  uint32_t reconstruct_adc_data_from_raw_osc_chan_samp(
    uint32_t raw_osc_chan_samp,
    unsigned channel);

  uint32_t reconstruct_raw_osc_chan_samp_from_adc_data(uint32_t adc_data);

  double conv_adc_data_to_actual_value(uint32_t adc_data, unsigned channel);

  uint32_t conv_raw_osc_samp_to_recon_chan_adc_data(uint32_t raw_osc_samp,
                                                    unsigned channel);

  void reset_dma_process();
  void reset_uncached_rx_buffer();
  void record_raw_sample_buffer_metadata();

public:
  LAB_Oscilloscope(LAB& _LAB);
  ~LAB_Oscilloscope();

  // master controls
  void run();
  void stop();
  void frontend_run_stop(bool value);
  void backend_run_stop(bool value);
  void single();
  void do_measurements(bool value);

  // mode
  void mode(LABE::OSC::MODE mode);
  LABE::OSC::MODE mode();

  // vertical
  void channel_enable_disable(unsigned channel, bool enable);
  void voltage_per_division(unsigned channel, double value);
  double voltage_per_division(unsigned channel) const;
  void vertical_offset(unsigned channel, double value);
  double vertical_offset(unsigned channel) const;
  void coupling(unsigned channel, LABE::OSC::COUPLING coupling);
  LABE::OSC::COUPLING coupling(unsigned channel);
  void scaling(unsigned channel, LABE::OSC::SCALING scaling);
  LABE::OSC::SCALING scaling(unsigned channel) const;
  std::size_t channels() const;

  // horizontal
  void horizontal_offset(double value);
  double horizontal_offset() const;
  void time_per_division(double value);
  double time_per_division() const;
  void samples(unsigned value);
  unsigned samples() const;
  void sampling_rate(double value);
  double sampling_rate() const;
  unsigned samples_displayed() const;

  // trigger
  void trigger_mode(LABE::OSC::TRIG::MODE value);
  LABE::OSC::TRIG::MODE trigger_mode() const;
  void trigger_source(unsigned chan);
  unsigned trigger_source() const;
  void trigger_type(LABE::OSC::TRIG::TYPE value);
  LABE::OSC::TRIG::TYPE trigger_type() const;
  void trigger_condition(LABE::OSC::TRIG::CND value);
  LABE::OSC::TRIG::CND trigger_condition() const;
  void trigger_level(double value);
  double trigger_level() const;
  bool trigger_found() const;
  void trigger_serviced();

  // Record
  void record_start();
  void record_init();

  // query state
  bool is_running();
  bool is_channel_enabled(unsigned channel) const;
  bool has_enabled_channels() const;
  bool is_backend_running() const;
  bool is_frontend_running() const;
  LABE::OSC::STATUS status() const;

  // for display update
  void update_data_samples();

  // Data
  const LAB_Parent_Data_Oscilloscope& parent_data();

  // channel data getter
  double chan_voltage_per_division(unsigned channel) const;
  double chan_vertical_offset(unsigned channel) const;

  // raw buffer
  double raw_buffer_time_per_division() const;
  double raw_buffer_horizontal_offset() const;
  double raw_buffer_sampling_rate() const;

  const std::array<double, LABC::OSC::NUMBER_OF_SAMPLES>& chan_samples(
    unsigned channel) const;

public:
  Measurements& measurements();
  Calibration& calibration();
};

#endif
