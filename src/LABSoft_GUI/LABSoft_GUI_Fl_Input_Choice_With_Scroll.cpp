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

int
LABSoft_GUI_Fl_Input_Choice_With_Scroll::
handle(int e)
{
  switch (e)
  {
    case FL_MOUSEWHEEL:
      if (Fl::belowmouse() == this ||
          Fl::belowmouse() == input() ||
          Fl::belowmouse() == menubutton())
      {
        cb_mouse_wheel(-Fl::event_dy());
        return 1;
      }
      break;
    default:
      return Fl_Input_Choice::handle(e);
  }

  return 0;
}

void
LABSoft_GUI_Fl_Input_Choice_With_Scroll::
cb_mouse_wheel(int scroll_amount)
{
  const Fl_Menu_Item* menu = menubutton()->menu();
  if (!menu) return;

  int next_index = find_next_index(scroll_amount);
  const Fl_Menu_Item* item = &menu[next_index];

  if (item && item->text)
  {
    input()->value(item->text);
    value(item->text);
    do_callback();
  }
}

int
LABSoft_GUI_Fl_Input_Choice_With_Scroll::
find_next_index(int scroll_amount)
{
  const Fl_Menu_Item* menu = menubutton()->menu();
  if (!menu) return 0;

  const char* current_value = input()->value();
  int curr_index = menubutton()->find_index(current_value);
  if (curr_index < 0) curr_index = 0;

  int max_index = menubutton()->size() - 1;
  int next_index = curr_index + scroll_amount;

  if (next_index < 0) next_index = 0;
  if (next_index > max_index) next_index = max_index;

  return next_index;
}

void LABSoft_GUI_Fl_Input_Choice_With_Scroll::do_scroll(int direction)
{
  cb_mouse_wheel(direction);
}

// EOF
