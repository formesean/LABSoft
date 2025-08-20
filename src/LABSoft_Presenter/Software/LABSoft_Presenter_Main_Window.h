#ifndef LABSOFT_PRESENTER_MAIN_WINDOW_H
#define LABSOFT_PRESENTER_MAIN_WINDOW_H

#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>

#include "../LABSoft_Presenter_Unit.h"

class LABSoft_Presenter_Main_Window: public LABSoft_Presenter_Unit
{
  public:
    LABSoft_Presenter_Main_Window (LABSoft_Presenter& _LABSoft_Presenter);

    void cb_exit                  (Fl_Widget* w, void* data);
    void cb_help_about            (Fl_Widget* w, void* data);
    void cb_help_about_win_close  (Fl_Button* w, void* data);
};

#endif
