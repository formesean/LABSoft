#ifndef LAB_LOGIC_ANALYZER_H
#define LAB_LOGIC_ANALYZER_H

#include <thread>

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"
#include "../Utility/LAB_Enumerations.h"

class LAB_Logic_Analyzer : public LAB_Module
{
  private:
    AikaPi::Uncached                m_uncached_memory;
    std::thread                     m_thread_find_trigger;
    std::thread                     m_thread_get_samples;
    LAB_Parent_Data_Logic_Analyzer  m_parent_data;

  private:
    // Setup
    void              init_gpio_pins          ();
    void              init_dma                ();
    void              init_interrupts         ();
    void              config_dma_cb           ();  

    // Mode
    LABE::LOGAN::MODE calc_mode               (double time_per_division) const;
    void              set_mode                (LABE::LOGAN::MODE mode);
    void              dma_buffer_count        (LABE::LOGAN::BUFFER_COUNT buffer_count);

    // Horizontal
    void              set_samples             (unsigned value);
    void              set_time_per_division   (double value);
    void              set_time_per_division   (unsigned samples, double sampling_rate);
    void              set_sampling_rate       (double value);
    double            calc_time_per_division  (unsigned samples, double sampling_rate);
    double            calc_sampling_rate      (unsigned samples, double time_per_division);
    double            calc_sample_count       (double sampling_rate, double time_per_division);

    // Trigger 
    void              set_trigger_condition   (unsigned gpio_pin, LABE::LOGAN::TRIG::CND condition);
    void              parse_trigger_mode      ();
    void              find_trigger_point_loop ();
    bool              check_if_triggered      (uint32_t event_detect_status_register_value);
    void              create_trigger_frame    ();
    void              cache_trigger_condition (unsigned channel, LABE::LOGAN::TRIG::CND condition);

    // Data and conversion
    void reset_dma_process                      ();
    void fill_raw_sample_buffer_from_dma_buffer ();
    void parse_raw_sample_buffer                ();
    void get_samples_loop                       ();

  public:
    LAB_Logic_Analyzer                  (LAB& _LAB);
   ~LAB_Logic_Analyzer                  ();

    // master controls
    void              run               ();
    void              stop              ();
    void              single            ();
    
    // State
    bool is_running                     () const;

    // Mode
    void              mode              (LABE::LOGAN::MODE mode);
    LABE::LOGAN::MODE mode              ();
  
    // Horizontal
    void              horizontal_offset (double value);
    double            horizontal_offset () const;
    void              time_per_division (double value);
    double            time_per_division () const;
    void              samples           (unsigned value);
    unsigned          samples           () const;
    void              sampling_rate     (double value);
    double            sampling_rate     () const;

    // Get data/samples
    void              update_data_samples ();
    double            calc_samp_count   (double time_per_div, unsigned osc_disp_num_cols);
    double            calc_samp_rate    (double time_per_div, unsigned osc_disp_num_cols);
    LABE::LOGAN::MODE calc_mode         (double time_per_division);

    // Trigger  
    void              trigger_mode      (LABE::LOGAN::TRIG::MODE value);
    void              trigger_condition (unsigned channel, LABE::LOGAN::TRIG::CND condition);

    // Data 
    LAB_Parent_Data_Logic_Analyzer& parent_data ();
};

#endif