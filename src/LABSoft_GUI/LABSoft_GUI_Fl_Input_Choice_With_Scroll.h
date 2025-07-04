#ifndef LABSOFT_GUI_FL_INPUT_CHOICE_WITH_SCROLL
#define LABSOFT_GUI_FL_INPUT_CHOICE_WITH_SCROLL

#include <utility>

#include "FL/Fl_Input_Choice.H"

class LABSoft_GUI_Fl_Input_Choice_With_Scroll : public Fl_Input_Choice
{
  private:
    int                 handle                      (int e);
    void                cb_mouse_wheel              (int scroll_amount);
    int                 find_next_index             (int scroll_amount);

  public: 
    LABSoft_GUI_Fl_Input_Choice_With_Scroll (int X, int Y, int W, int H, const char* label = 0);

    void do_scroll (int direction);
};

#endif