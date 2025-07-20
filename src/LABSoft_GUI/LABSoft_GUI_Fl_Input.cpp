#include "LABSoft_GUI_Fl_Input.h"

LABSoft_GUI_Fl_Input:: 
LABSoft_GUI_Fl_Input (int X, int Y, int W, int H, const char* label)
  : Fl_Input (X, Y, W, H, label)
{
  when (FL_WHEN_RELEASE | FL_WHEN_ENTER_KEY);
}

int LABSoft_GUI_Fl_Input:: 
handle (int e)
{
  switch (e)
  {
    default:
    {
      return (Fl_Input::handle (e));
    }
  }
}

void LABSoft_GUI_Fl_Input:: 
draw ()
{
  Fl_Input::draw ();
}
