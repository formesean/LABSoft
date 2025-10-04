#include "LABSoft_Presenter_Main_Window.h"

#include <FL/Fl.H>

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"

LABSoft_Presenter_Main_Window::
LABSoft_Presenter_Main_Window (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{

}

void LABSoft_Presenter_Main_Window::
cb_exit (Fl_Widget* w,
                   void*      data)
{
  if (Fl::modal () == m_presenter.gui ().main_fl_window)
  {
    // Announce stop to SNM before hiding, if configured
    if (m_presenter.lab().m_Software_Navigation.is_snm_config_enabled())
    {
      m_presenter.lab().m_Software_Navigation.announce_program_stopping();
    }
    m_presenter.gui ().main_fl_window->hide ();
  }
}

void LABSoft_Presenter_Main_Window::
cb_help_about (Fl_Widget* w, void* data)
{
  m_presenter.gui ().main_fl_window->deactivate ();
  m_presenter.gui ().main_fl_window_about->show ();
}

void LABSoft_Presenter_Main_Window::
cb_help_about_win_close (Fl_Button* w, void* data)
{
  m_presenter.gui ().main_fl_window_about->hide ();
  m_presenter.gui ().main_fl_window->activate ();
}
