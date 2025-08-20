//teacher side
#ifndef LABSoft_Presenter_LABChecker_Analog_H
#define LABSoft_Presenter_LABChecker_Analog_H

#include "../LABSoft_Presenter_Unit.h"
#include <FL/Fl_Button.H>


class LABSoft_Presenter_LABChecker_Analog : public LABSoft_Presenter_Unit
{
  private:
    void load_gui                                           ();
    bool can_capture_signal                                 () const;
    void capture_oscilloscope_and_function_generator_data   ();
    //void update_gui_with_captured_data                      ();
    //void create_file                                        ();

  public:
    LABSoft_Presenter_LABChecker_Analog (LABSoft_Presenter& _LABSoft_Presenter);

    void update_display             ();
    void init_gui_callbacks         ();
    static void cb_capture_signal   (Fl_Button* w, void* data);
    //static void cb_create_file      (Fl_Button* w, void* data);
};

#endif
