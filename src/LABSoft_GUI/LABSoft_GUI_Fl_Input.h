#ifndef LABSOFT_GUI_FL_INPUT_H
#define LABSOFT_GUI_FL_INPUT_H

#include "FL/Fl_Input.H"

class LABSoft_GUI_Fl_Input : public Fl_Input
{
  private:
    int   handle  (int e);
    void  draw    ();

  public: 
    LABSoft_GUI_Fl_Input (int X, int Y, int W, int H, const char* label = 0);
};

#endif