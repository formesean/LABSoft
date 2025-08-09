
#ifndef LABSOFT_PRESENTER_ANALOG_CIRCUIT_CHECKER_H
#define LABSOFT_PRESENTER_ANALOG_CIRCUIT_CHECKER_H

#include "../LABSoft_Presenter_Unit.h"
#include <FL/Fl_Button.H>


class LABSoft_Presenter_Analog_Circuit_Checker : public LABSoft_Presenter_Unit
{
  private:
    void load_gui                                           ();
    bool can_capture_signal                                 () const;
    void capture_oscilloscope_and_function_generator_data   ();

  public:
    LABSoft_Presenter_Analog_Circuit_Checker (LABSoft_Presenter& _LABSoft_Presenter);
    
    void update_display             ();
    void init_gui_callbacks         ();
    static void cb_capture_signal   (Fl_Button* w, void* data);
};

#endif