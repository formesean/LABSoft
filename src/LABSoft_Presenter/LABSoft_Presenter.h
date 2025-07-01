#ifndef LABSOFT_PRESENTER_H
#define LABSOFT_PRESENTER_H

#include <FL/Fl_Tabs.H>

#include "LABSoft_Presenter_Oscilloscope.h"
#include "LABSoft_Presenter_Voltmeter.h"
#include "LABSoft_Presenter_Ohmmeter.h"
#include "LABSoft_Presenter_Function_Generator.h"
#include "LABSoft_Presenter_Logic_Analyzer.h"
#include "LABSoft_Presenter_Digital_Circuit_Checker.h"
#include "LABSoft_Presenter_Software_Navigation.h"

#include "Software/LABSoft_Presenter_Exporter.h"
#include "Software/LABSoft_Presenter_Calibration.h"
#include "Software/LABSoft_Presenter_Main_Window.h"
#include "Software/LABSoft_Presenter_LABChecker_Digital.h"
#include "Software/LABSoft_Presenter_Oscilloscope_Display.h"

class LABSoft_Presenter
{
  private:
    LAB&          m_LAB;
    LABSoft_GUI&  m_LABSoft_GUI;

  public:
    // Hardware
    LABSoft_Presenter_Oscilloscope            m_Oscilloscope;
    LABSoft_Presenter_Ohmmeter                m_Ohmmeter;
    LABSoft_Presenter_Voltmeter               m_Voltmeter;
    LABSoft_Presenter_Function_Generator      m_Function_Generator;
    LABSoft_Presenter_Logic_Analyzer          m_Logic_Analyzer;
    LABSoft_Presenter_Digital_Circuit_Checker m_Digital_Circuit_Checker;
    LABSoft_Presenter_LABChecker_Digital      m_LABChecker_Digital;
    LABSoft_Presenter_Software_Navigation     m_Software_Navigation

    // Software/GUI
    LABSoft_Presenter_Main_Window             m_Main_Window;
    LABSoft_Presenter_Exporter                m_Exporter;
    LABSoft_Presenter_Calibration             m_Calibration;
    LABSoft_Presenter_Oscilloscope_Display    m_Oscilloscope_Display;

  private:
    void load_presenter_to_gui ();
    void update_gui_tab_colors  ();

    static void update_display (void *data);

  public:
    LABSoft_Presenter (LAB& _LAB, LABSoft_GUI& _LABSoft_GUI);

    void cb_tabs (Fl_Group* w, void* data);

    LAB&          lab () const;
    LABSoft_GUI&  gui () const;
};

#endif
