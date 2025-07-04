#include "LABSoft_GUI_Fl_Input_Choice_With_Scroll.h"

LABSoft_GUI_Fl_Input_Choice_With_Scroll::
LABSoft_GUI_Fl_Input_Choice_With_Scroll (int         X, 
                                         int         Y, 
                                         int         W, 
                                         int         H, 
                                         const char* label)
  : Fl_Input_Choice (X, Y, W, H, label)
{

}

int LABSoft_GUI_Fl_Input_Choice_With_Scroll:: 
handle (int e)
{
  switch (e)
  {    
    case (FL_MOUSEWHEEL):
    {
      if (Fl::belowmouse () == this ||
          Fl::belowmouse () == this->input () || 
          Fl::belowmouse () == this->menubutton ())
      {
        cb_mouse_wheel (Fl::event_dy ());  

        return (1);
      }
    }

    default:
    {
      return (LABSoft_GUI_Fl_Input_Choice_With_Scroll::Fl_Input_Choice::handle (e));
    }
  }
}

void LABSoft_GUI_Fl_Input_Choice_With_Scroll:: 
cb_mouse_wheel (int scroll_amount)
{
  int next_index = find_next_index (scroll_amount);

  if (next_index < 0)
  {
    // do nothing
  }
  else 
  {
    value (next_index);

    do_callback ();
  }
}

int LABSoft_GUI_Fl_Input_Choice_With_Scroll:: 
find_next_index (int scroll_amount)
{
  if (menubutton ()->size () <= 1)
  {
    return (-1);
  }
  
  // 

  int curr_index = menubutton ()->find_index (input ()->value ());

  if (curr_index < 0)
  {
    return (-1);

    // std::pair<int, int> input_bounds_indexes;

    // try 
    // {
    //   input_bounds_indexes = find_input_bounds_indexes ();
    // }
    // catch (const std::exception& e)
    // {
    //   return (-1);
    // }
    
    // if (scroll_amount >= 0) // positive, scroll down
    // {
    //   return (input_bounds_indexes.first);
    // } 
    // else // negative, scroll up 
    // {
    //   return (input_bounds_indexes.second);
    //}
  }
  else 
  {
    int next_index = curr_index + scroll_amount;

    if (next_index < 0)
    {
      return (0);
    }
    else if (next_index > (menubutton ()->size () - 2))
    {
      return (menubutton ()->size () - 2);
    }
    else 
    {
      return (next_index);
    }
  }
}

void LABSoft_GUI_Fl_Input_Choice_With_Scroll:: 
do_scroll (int direction)
{
  
}