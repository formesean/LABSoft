#ifndef LABSOFT_GUI_FL_SLIDER_H
#define LABSOFT_GUI_FL_SLIDER_H

#include <FL/Fl_Slider.H>

class LABSoft_GUI_Fl_Slider : public Fl_Slider
{
  private:
    int   handle  (int e);
    void  draw    ();

  public:
    LABSoft_GUI_Fl_Slider (int X, int Y, int W, int H, const char* label = 0);
};

#endif