//student side
#ifndef LABSoft_Presenter_Analog_Circuit_Checker_H
#define LABSoft_Presenter_Analog_Circuit_Checker_H

#include "../LABSoft_Presenter_Unit.h"


class LABSoft_Presenter_Analog_Circuit_Checker : public LABSoft_Presenter_Unit
{
  private:
    void load_gui               ();
    void display_signals        ();
    void update_gui_display     ();

  public:
    LABSoft_Presenter_Analog_Circuit_Checker (LABSoft_Presenter& _LABSoft_Presenter);
    
    void update_display         ();
    void cb_load_file_analog    (Fl_Button* w, void* data);
    void cb_run_checker_analog  (Fl_Button* w, void* data);

};

#endif