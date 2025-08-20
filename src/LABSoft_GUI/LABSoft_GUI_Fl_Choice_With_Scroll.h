#ifndef LABSOFT_GUI_FL_CHOICE_WITH_SCROLL_H
#define LABSOFT_GUI_FL_CHOICE_WITH_SCROLL_H

#include "FL/Fl_Choice.H"

class LABSoft_GUI_Fl_Choice_With_Scroll : public Fl_Choice
{
  private:
    int   handle          (int e);
    void  cb_mouse_wheel  (int scroll_amount);

  public: 
    LABSoft_GUI_Fl_Choice_With_Scroll (int X, int Y, int W, int H, const char* label = 0);
};

#endif