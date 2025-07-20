#ifndef LABSOFT_PRESENTER_FUNCTION_GENERATOR_H
#define LABSOFT_PRESENTER_FUNCTION_GENERATOR_H

#include <FL/Fl_Choice.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Input_Choice.H>

#include "LABSoft_Presenter_Unit.h"

class LABSoft_Presenter_Function_Generator : public LABSoft_Presenter_Unit
{
  private:
    void init_gui_values (); 

  public:
    LABSoft_Presenter_Function_Generator (LABSoft_Presenter& _LABSoft_Presenter);

    void update_gui_frequency_elements    ();

    void cb_run_stop        (Fl_Light_Button* w, long channel);
    void cb_wave_type       (Fl_Choice*       w, long channel);
    void cb_amplitude       (Fl_Input_Choice* w, long channel);
    void cb_frequency       (Fl_Input_Choice* w, long channel);
    void cb_period          (Fl_Input_Choice* w, long channel);
    void cb_vertical_offset (Fl_Input_Choice* w, long channel);
    void cb_phase           (Fl_Input_Choice* w, long channel);
};

#endif
