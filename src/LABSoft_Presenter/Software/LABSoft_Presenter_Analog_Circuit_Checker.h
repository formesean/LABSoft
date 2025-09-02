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
  void load_gui();
  void display_signals();
  void update_gui_display();
  void log_metadata_to_terminal() const;

  std::vector<double> instructor_data;
  std::vector<double> dummy_student_data;

  // Imported metadata container (non-signal data only)
  struct ACC_Metadata
  {
    // Oscilloscope-like global settings
    double time_per_division = 0.0;
    unsigned samples = 0;
    double sampling_rate = 0.0;
    double horizontal_offset = 0.0;

    struct ChannelMeta
    {
      std::string name;
      unsigned samples = 0;
      bool coupling = false; // true=AC, false=DC (as provided by loader)
      unsigned scaling = 0;
      double voltage_per_div = 0.0;
      double vertical_offset = 0.0;
      bool is_enabled = false;
      double scaling_corrector = 0.0;
      struct Measurements
      {
        double min = 0.0, max = 0.0, avg = 0.0, trms = 0.0;
      } measurements;
    };

    std::vector<ChannelMeta> channels;

    struct FunctionGenerator
    {
      bool is_enabled = false;
      unsigned wave_type = 0;
      double frequency = 0.0;
      double period = 0.0;
      // Optional fields if available in loader; default to 0
      double amplitude = 0.0;
      double vertical_offset = 0.0;
      double phase = 0.0;
    } function_generator;

    struct Comparison
    {
      bool time_domain = true;
      bool frequency_domain = true;
      double similarity_threshold = 0.90; // 0..1
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
