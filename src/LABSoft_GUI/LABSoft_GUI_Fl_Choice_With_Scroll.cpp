#include "LABSoft_GUI_Fl_Choice_With_Scroll.h"

#include "FL/Fl.H"

LABSoft_GUI_Fl_Choice_With_Scroll:: 
LABSoft_GUI_Fl_Choice_With_Scroll (int          X,
                                   int          Y,
                                   int          W,
                                   int          H,
                                   const char*  label)
  : Fl_Choice (X, Y, W, H, label)
{

}

int LABSoft_GUI_Fl_Choice_With_Scroll:: 
handle (int e)
{
  switch (e)
  {    
    case (FL_MOUSEWHEEL):
    {
      if (Fl::belowmouse () == this)
      {
        cb_mouse_wheel (Fl::event_dy ());

        return (1);
      }
    }

    default:
    {
      return (LABSoft_GUI_Fl_Choice_With_Scroll::Fl_Choice::handle (e));
    }
  }
}

void LABSoft_GUI_Fl_Choice_With_Scroll:: 
cb_mouse_wheel (int scroll_amount)
{
  int index = value () + scroll_amount;
  
  if (index < size () && index >= 0)
  {
    value (index);

    do_callback ();
  }
}

