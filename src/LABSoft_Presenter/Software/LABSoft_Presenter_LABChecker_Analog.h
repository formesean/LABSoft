// teacher side
#ifndef LABSOFT_PRESENTER_LABCHECKER_ANALOG_H
#define LABSOFT_PRESENTER_LABCHECKER_ANALOG_H

class Fl_Button;
class Fl_Check_Button;

#include "../LABSoft_Presenter_Unit.h"

class LABSoft_Presenter_LABChecker_Analog : public LABSoft_Presenter_Unit
{
private:
  void load_gui();
  bool can_capture_signal() const;
  void capture_oscilloscope_and_function_generator_data();
  void update_gui_with_captured_data();
  void sync_comparison_inputs_enabled();

public:
  LABSoft_Presenter_LABChecker_Analog(LABSoft_Presenter &_LABSoft_Presenter);

  void update_display();
  void init_gui_callbacks();
  static void cb_capture_signal(Fl_Button *w, void *data);
  void cb_analog_create_file(Fl_Button *w, void *data);
  static void cb_toggle_time_domain(Fl_Check_Button *w, void *data);
  static void cb_toggle_frequency_domain(Fl_Check_Button *w, void *data);
};

#endif
