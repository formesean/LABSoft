#include "LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window.h"

#include <cstdio>

#include <FL/Fl.H>

LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window (int          X,
                                               int          Y,
                                               const char*  label)
  : Fl_Window (X, Y, label)
{
  init_children_guis ();
  init_this_gui ();
} 

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
init_children_guis ()
{
  m_name = new Fl_Input (70, 20, 120, 25, "Name: ");
  m_name->maximum_size (20);

  m_multi_browser = new Fl_Multi_Browser (15, 60, 210, 240);
  m_multi_browser->when (FL_WHEN_CHANGED | FL_WHEN_NOT_CHANGED);
  m_multi_browser->callback ((Fl_Callback*)(cb_multi_browser_static));

  m_add = new Fl_Button (90, 315, 60, 30, "Add"); 
  m_add->box(FL_GTK_UP_BOX);
  m_add->color ((Fl_Color)(53));
  m_add->callback ((Fl_Callback*)(cb_add_static));
  m_add->deactivate ();

  m_cancel = new Fl_Button (165, 315, 60, 30, "Cancel");
  m_cancel->box (FL_GTK_UP_BOX);
  m_cancel->color ((Fl_Color)(53));
  m_cancel->callback ((Fl_Callback*)(cb_cancel_static));

  end ();
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
init_this_gui ()
{
  copy_label ("Add signal");

  size (240, 360);

  //callback (cb_window_close);
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
hide_as_modal ()
{
  hide ();
  clear_modal_states ();
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
reset_state ()
{
  m_name->value ("");
  m_multi_browser->deselect ();
  m_add->deactivate ();
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window::
cb_multi_browser (Fl_Multi_Browser* w, void* data)
{
  LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window& win = 
    *(reinterpret_cast<LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window*>(w->parent ()));

  for (int line = w->size (); line > 0; line--)
  {
    if ((w->selected (line)) == 1)
    {
      win.m_add->activate ();

      break;
    }

    win.m_add->deactivate ();
  }
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window::
cb_add (Fl_Button* w, void* data)
{
  hide_as_modal ();

  (reinterpret_cast<LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window*>
    (w->parent ()))->do_callback ();

  reset_state ();
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window::
cb_cancel (Fl_Button* w, void* data)
{
  hide_as_modal ();
}

void cb_window_close (LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window* w, void* data)
{

}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window::
cb_multi_browser_static (Fl_Multi_Browser* w, void* data)
{
  (reinterpret_cast<LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window*>(w->parent ()))->
    cb_multi_browser (w, data);
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
cb_add_static (Fl_Button* w, void* data)
{
  (reinterpret_cast<LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window*>(w->parent ()))->
    cb_add (w, data);
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
cb_cancel_static (Fl_Button* w, void* data)
{
  (reinterpret_cast<LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window*>(w->parent ()))->
    cb_cancel (w, data);
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
cb_window_close_static (LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window* w, void* data)
{
  //hide_as_modal ();
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
show_as_modal ()
{
  set_modal   ();
  show        ();
}

void LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window:: 
number_of_channels (unsigned value)
{
  char label[20];

  m_multi_browser->clear ();

  if (value > 0)
  {
    for (int a = (value - 1); a >= 0; a--)
    {
      std::snprintf (label, sizeof (label), "DIO %d", a);

      m_multi_browser->add (label, reinterpret_cast<void*>(a));
    }
  }
}