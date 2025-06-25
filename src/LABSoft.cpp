#include "LABSoft.h"

#include <FL/Fl.H>

// FLTK: Hitting the 'Escape' key closes windows - how do I prevent this?
// https://www.fltk.org/doc-1.3/FAQ.html

LABSoft::
LABSoft (int    argc,
         char** argv)
  : m_LABSoft_Presenter (m_LAB, m_LABSoft_GUI)
{
  // initialize threading support
  Fl::lock ();

  // start Software Navigation
  m_LAB.m_Software_Navigation.start_navigation();

  // https://www.fltk.org/doc-1.3/classFl__Double__Window.html#details
  // It is highly recommended that you put the following code before the first show() of any window in your program:
  Fl::visual (FL_DOUBLE | FL_INDEX);

  // show main window
  m_LABSoft_GUI.main_fl_window->show ();

  // run main fltk loop
  Fl::run();
}

LABSoft::
~LABSoft()
{
  // stop navigation before destroying
  m_LAB.m_Software_Navigation.stop_navigation();
}
