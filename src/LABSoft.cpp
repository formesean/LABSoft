#include "LABSoft.h"

#include <FL/Fl.H>
#include <csignal>

// FLTK: Hitting the 'Escape' key closes windows - how do I prevent this?
// https://www.fltk.org/doc-1.3/FAQ.html

LABSoft::
LABSoft (int    argc,
         char** argv)
  : m_LABSoft_Presenter (m_LAB, m_LABSoft_GUI)
{
  // Register signal handlers to announce stop on Ctrl+C or termination
  static LAB* g_lab_ptr = nullptr;
  g_lab_ptr = &m_LAB;

  auto signal_handler = [](int)
  {
    if (g_lab_ptr && g_lab_ptr->m_Software_Navigation.is_snm_config_enabled())
    {
      g_lab_ptr->m_Software_Navigation.announce_program_stopping();
    }

    g_lab_ptr->m_Function_Generator.stop(0);
    g_lab_ptr->m_Function_Generator.stop(1);

    std::_Exit(0);
  };

  std::signal(SIGINT,  signal_handler);
  std::signal(SIGTERM, signal_handler);
#ifdef SIGBREAK
  std::signal(SIGBREAK, signal_handler);
#endif
  // initialize threading support
  Fl::lock ();

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
  // On application shutdown (GUI or process exit), announce stop if SNM is enabled
  if (m_LAB.m_Software_Navigation.is_snm_config_enabled())
  {
    m_LAB.m_Software_Navigation.announce_program_stopping();
  }
}
