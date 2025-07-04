#ifndef LAB_OSCILLOSCOPE_H
#define LAB_OSCILLOSCOPE_H

#include <thread>

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"
#include "../Utility/LAB_Enumerations.h"

class LAB_Oscilloscope : public LAB_Module
{
  private:
    std::thread                   m_thread_trigger;
    std::thread                   m_find_trigger_timer;
    AikaPi::Uncached              m_uncached_memory;   
    LAB_Parent_Data_Oscilloscope  m_parent_data; 

  private:
    // Setup
    void init_spi       ();
    void init_pwm       ();
    void init_gpio_pins ();
    void init_dma       ();
    void init_state     ();
    void config_dma_cb  ();

    // Master Controls
    void                master_run_stop                 (bool value);

    // State 
    void                status                          (LABE::OSC::STATUS _STATUS);

    // Mode
    void                set_mode                        (LABE::OSC::MODE mode);
    void                dma_buffer_count                (LABE::OSC::BUFFER_COUNT buffer_count);
  
    // Horizontal 
    double              calc_time_per_division          (unsigned samples, double sampling_rate);
    double              calc_samples_displayed          (double sampling_rate, double time_per_division);
    double              calc_sampling_rate              (unsigned samples, double time_per_division);
    void                set_time_per_division           (unsigned samples, double sampling_rate);
    void                set_time_per_division           (double value);
    void                set_samples                     (unsigned value);
    void                set_sampling_rate               (double value);
    
    // mode
    LABE::OSC::MODE     calc_mode                       (double time_per_division);    
  
    // trigger 
    void                parse_trigger_mode              ();
    void                trigger_osc_mode_dispatcher     ();
    void                trigger_osc_mode_repeated       ();

    void                find_trigger_point_loop         ();
    bool                find_trigger_point              ();
    void                create_trigger_frame            ();
    void                find_trigger_timeout_timer      ();
    uint32_t            calc_trigger_level_raw_bits     (double trigger_level);

    void                prefill_buffers_for_triggering  ();

    // Record
    void                config_dma_cb_record            ();

    // Other
    bool                is_raw_buffer_being_written     (unsigned buff);
    void                clear_dma_interrupt_flag        (unsigned dma_chan);

    // Data and conversion
    void                fill_raw_osc_samp_buff_from_dma_buff          ();
    void                parse_raw_osc_samp_buff                         ();
    constexpr double    conv_raw_chan_adc_bits_to_actual_value          (uint32_t raw_buff_samp, unsigned channel);
    constexpr uint32_t  extract_raw_chan_adc_bits_from_raw_buff_samp    (uint32_t raw_buff_samp, unsigned channel);
    constexpr uint32_t  arrange_raw_chan_adc_bits                       (uint32_t raw_chan_bits, unsigned channel);
    constexpr double    conv_arranged_raw_chan_adc_bits_to_actual_value (uint32_t arranged_bits_abs_val, bool arranged_bits_sign);    
    constexpr uint32_t  reverse_arrange_raw_chan_adc_bits               (uint32_t arranged_bits);
    constexpr uint32_t  conv_raw_buff_get_arranged_bits                 (uint32_t sample, unsigned channel);
    void                reset_dma_process                               ();
    void                reset_uncached_rx_buffer                        ();

  public:   
    LAB_Oscilloscope (LAB& _LAB);
   ~LAB_Oscilloscope ();   

    // master controls
    void                  run                     ();
    void                  stop                    ();  
    void                  osc_core_run_stop       (bool value);
    void                  osc_frontend_run_stop   (bool value);
    void                  single                  ();

    // mode 
    void                  mode                    (LABE::OSC::MODE mode);
    LABE::OSC::MODE       mode                    ();

    // vertical
    void                  channel_enable_disable  (unsigned channel, bool enable);
    void                  voltage_per_division    (unsigned channel, double value);
    double                voltage_per_division    (unsigned channel);
    void                  vertical_offset         (unsigned channel, double value);
    double                vertical_offset         (unsigned channel) const;
    void                  coupling                (unsigned channel, LABE::OSC::COUPLING coupling);
    LABE::OSC::COUPLING   coupling                (unsigned channel);
    void                  scaling                 (unsigned channel, LABE::OSC::SCALING scaling);
    LABE::OSC::SCALING    scaling                 (unsigned channel) const;
    unsigned              channels                () const;

    // horizontal
    void                  horizontal_offset       (double value);
    double                horizontal_offset       () const;
    void                  time_per_division       (double value);
    double                time_per_division       () const;
    void                  samples                 (unsigned value);
    unsigned              samples                 () const;
    void                  sampling_rate           (double value);
    double                sampling_rate           () const;
    
    // trigger 
    void                  trigger_mode            (LABE::OSC::TRIG::MODE value);
    LABE::OSC::TRIG::MODE trigger_mode            () const;
    void                  trigger_source          (unsigned chan);
    unsigned              trigger_source          () const;
    void                  trigger_type            (LABE::OSC::TRIG::TYPE value);
    LABE::OSC::TRIG::TYPE trigger_type            () const;
    void                  trigger_condition       (LABE::OSC::TRIG::CND value);
    LABE::OSC::TRIG::CND  trigger_condition       () const;
    void                  trigger_level           (double value);
    double                trigger_level           () const;
    bool                  trigger_found           () const;
    void                  trigger_serviced        ();

    // Record
    void                  record_start            ();
    void                  record_init             ();

    // query state
    bool                  is_running              ();
    bool                  is_channel_enabled      (unsigned channel) const;
    bool                  has_enabled_channels    () const;
    bool                  is_backend_running      () const;
    bool                  is_frontend_running     () const;
    LABE::OSC::STATUS     status                  () const;

    // for display update
    void                  update_data_samples       (); 
    
    // Data
    const LAB_Parent_Data_Oscilloscope&                     parent_data     ();


    // calibration
    void    adc_reference_voltage (double value);
    double  adc_reference_voltage () const;

    // channel data getter
    double chan_voltage_per_division  (unsigned channel) const;
    double chan_vertical_offset       (unsigned channel) const;
    
    const std::array<double, LABC::OSC::NUMBER_OF_SAMPLES>& chan_samples (unsigned channel) const;
};

#endif
