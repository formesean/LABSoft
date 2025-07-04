#include "LABSoft_GUI_Fl_Slider.h"

LABSoft_GUI_Fl_Slider:: 
LABSoft_GUI_Fl_Slider (int X, int Y, int W, int H, const char* label)
  : Fl_Slider (X, Y, W, H, label)
{

}

int LABSoft_GUI_Fl_Slider:: 
handle (int e)
{
  switch (e)
  {
    default:
    {
      return (LABSoft_GUI_Fl_Slider::Fl_Slider::handle (e));
    }
  }
}

void LABSoft_GUI_Fl_Slider:: 
draw ()
{
  Fl_Slider::draw ();
}
