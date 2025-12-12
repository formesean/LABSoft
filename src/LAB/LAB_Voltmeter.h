#ifndef LAB_VOLTMETER_H
#define LAB_VOLTMETER_H

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"

class LAB_Voltmeter : public LAB_Module
{
  private:
    LAB_Parent_Data_Voltmeter m_parent_data;

    // Snapshot of oscilloscope state so that Voltmeter can be non-intrusive.
    struct OscilloscopeStateSnapshot
    {
      bool                          is_valid           = false;
      bool                          do_measurements    = false;
      double                        sampling_rate      = 0.0;
      unsigned                      samples            = 0;
      LABE::OSC::TRIG::MODE         trigger_mode       = LABE::OSC::TRIG::MODE::NONE;

      // Per-channel vertical configuration and calibration
      struct ChannelState
      {
        LABE::OSC::COUPLING                         coupling  = LABE::OSC::COUPLING::DC;
        LABE::OSC::SCALING                          scaling   = LABE::OSC::SCALING::HALF;
        double                                      vertical_offset = 0.0;
        double                                      voltage_per_div = 0.0;
        double                                      scaling_corrector_to_actual = 1.0;
      };

      std::array<ChannelState, LABC::VOLTMETER::NUMBER_OF_CHANNELS> channels;
    };

    OscilloscopeStateSnapshot m_saved_osc_state;

  private:
    void save_oscilloscope_state   ();
    void restore_oscilloscope_state();
    void load_oscilloscope_voltmeter_mode_settings ();

  public:
    LAB_Voltmeter (LAB& _LAB);

    bool    is_frontend_running () const;
    bool    is_backend_running  () const;
    void    run                 ();
    void    stop                ();
    void    frontend_run_stop   (bool value);
    void    backend_run_stop    (bool value);
    double  get_reading         (unsigned chan);
};

#endif
