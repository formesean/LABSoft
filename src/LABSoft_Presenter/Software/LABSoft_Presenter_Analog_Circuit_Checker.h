// student side
#ifndef LABSoft_Presenter_Analog_Circuit_Checker_H
#define LABSoft_Presenter_Analog_Circuit_Checker_H

#include <FL/Fl_Button.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <string>
#include <vector>

#include "../LABSoft_Presenter_Unit.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"
#include "../../Utility/LAB_Enumerations.h"

class LABSoft_Presenter_Analog_Circuit_Checker : public LABSoft_Presenter_Unit
{
  private:
    void load_gui                          ();
    void import_metadata                   ();
    void update_gui_analog_circuit_checker ();
    void update_gui_acc_comparison         ();
    void update_gui_oscilloscope           ();
    void update_gui_function_generator     ();
    void update_gui_display                ();
    void perform_time_domain_signal_analysis();

    struct ACC_Metadata
    {
      double   time_per_division   = 0.0;
      unsigned samples             = 0;
      double   sampling_rate       = 0.0;
      double   horizontal_offset   = 0.0;
      // Trigger settings
      unsigned trigger_mode        = 0;
      unsigned trigger_source      = 0;
      unsigned trigger_type        = 0;
      unsigned trigger_condition   = 0;
      double   trigger_level       = 0.0;

      struct ChannelMeta
      {
        std::string name;
        unsigned    samples         = 0;
        bool        coupling        = false;   // true=AC, false=DC
        unsigned    scaling         = 0;
        double      voltage_per_div = 0.0;
        double      vertical_offset = 0.0;
        bool        is_enabled      = false;
        double      scaling_corrector = 0.0;
        struct Measurements { double min=0.0, max=0.0, avg=0.0, trms=0.0; } measurements;
      };

      std::vector<ChannelMeta> channels;

      struct FunctionGenerator
      {
        bool     is_enabled = false;
        unsigned wave_type  = 0;
        double   frequency  = 0.0;
        double   period     = 0.0;
      } function_generator;

      struct Comparison
      {
        bool   time_domain            = true;
        bool   frequency_domain       = true;
        double time_similarity_threshold     = 0.90;
        double frequency_similarity_threshold = 0.90;
      } comparison;
    } m_metadata;


public:
  LABSoft_Presenter_Analog_Circuit_Checker(LABSoft_Presenter &_LABSoft_Presenter);

  void update_display();
  void cb_load_file_acc(Fl_Button *w, void *data);
  void cb_run_checker_acc(Fl_Button *w, void *data);

  // Accessor for imported metadata
  const ACC_Metadata &metadata() const { return m_metadata; }
};

#endif
