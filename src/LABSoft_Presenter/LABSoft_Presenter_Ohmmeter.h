#ifndef LABSOFT_PRESENTER_OHMMETER_H
#define LABSOFT_PRESENTER_OHMMETER_H

#include <FL/Fl_Light_Button.H>

#include "LABSoft_Presenter_Unit.h"

class LABSoft_Presenter_Ohmmeter : public LABSoft_Presenter_Unit
{
  private:
    void init_gui_values    ();
    
  public:
    LABSoft_Presenter_Ohmmeter (LABSoft_Presenter& _LABSoft_Presenter);

    void run_gui ();
    void stop_gui ();

    void cb_run_stop          (Fl_Light_Button *w, void *data);
    void display_update_cycle ();
};

#endif