#ifndef LABSOFT_PRESENTER_LABCHECKER_DIGITAL
#define LABSOFT_PRESENTER_LABCHECKER_DIGITAL

class Fl_Input;
class Fl_Choice;
class Fl_Button;

#include "../LABSoft_Presenter_Unit.h"

class LABSoft_Presenter_LABChecker_Digital : public LABSoft_Presenter_Unit
{
  private:
    void init_gui ();
    void update_gui_digital_output_count (unsigned value);

    static void update_display (void* data);
    
  public:
    LABSoft_Presenter_LABChecker_Digital (LABSoft_Presenter& _LABSoft_Presenter);

    void cb_digital_input_bits    (Fl_Choice* w, void* data);
    void cb_digital_output_bits   (Fl_Choice* w, void* data);
    void cb_digital_output_count  (Fl_Input*  w, void* data);
    void cb_digital_create_file   (Fl_Button* w, void* data);
};

#endif