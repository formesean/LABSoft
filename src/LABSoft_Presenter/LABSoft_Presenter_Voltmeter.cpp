#include "LABSoft_Presenter_Voltmeter.h"

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"

LABSoft_Presenter_Voltmeter:: 
LABSoft_Presenter_Voltmeter (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{
  init_gui_values ();
}

void LABSoft_Presenter_Voltmeter:: 
init_gui_values ()
{
  gui ().voltmeter_fl_output_chan_0_dc  ->value ("- . - -");
  gui ().voltmeter_fl_output_chan_0_trms->value ("- . - -");
  gui ().voltmeter_fl_output_chan_0_max ->value ("- . - -");
  gui ().voltmeter_fl_output_chan_0_min ->value ("- . - -");

  gui ().voltmeter_fl_output_chan_1_dc  ->value ("- . - -");
  gui ().voltmeter_fl_output_chan_1_trms->value ("- . - -");
  gui ().voltmeter_fl_output_chan_1_max ->value ("- . - -");
  gui ().voltmeter_fl_output_chan_1_min ->value ("- . - -");
}

void LABSoft_Presenter_Voltmeter:: 
run_gui ()
{
  gui ().voltmeter_fl_light_button_run_stop->set ();
}

void LABSoft_Presenter_Voltmeter::
stop_gui ()
{
  gui ().voltmeter_fl_light_button_run_stop->clear ();
}

void LABSoft_Presenter_Voltmeter:: 
cb_run_stop  (Fl_Light_Button* w, 
              void*            data)
{
  if (w->value ())
  {
    presenter ().m_Oscilloscope.stop_gui ();
    presenter ().m_Voltmeter   .stop_gui ();
    presenter ().m_Ohmmeter    .stop_gui ();

    //

    lab ().m_Voltmeter.run ();

    presenter ().m_Voltmeter.run_gui ();    
  }
  else 
  {
    lab ().m_Voltmeter.stop ();

    presenter ().m_Voltmeter.stop_gui ();   
  }
}

void LABSoft_Presenter_Voltmeter:: 
display_update_cycle ()
{
  if (lab ().m_Voltmeter.is_frontend_running ())
  {
    LABSoft_GUI_Label chan_0_dc   (lab ().m_Oscilloscope.measurements ().avg  (0), LABSoft_GUI_Label::UNIT::VOLT);
    LABSoft_GUI_Label chan_0_min  (lab ().m_Oscilloscope.measurements ().min  (0), LABSoft_GUI_Label::UNIT::VOLT);
    LABSoft_GUI_Label chan_0_max  (lab ().m_Oscilloscope.measurements ().max  (0), LABSoft_GUI_Label::UNIT::VOLT);
    LABSoft_GUI_Label chan_0_trms (lab ().m_Oscilloscope.measurements ().trms (0), LABSoft_GUI_Label::UNIT::VOLT);

    LABSoft_GUI_Label chan_1_dc   (lab ().m_Oscilloscope.measurements ().avg  (1), LABSoft_GUI_Label::UNIT::VOLT);
    LABSoft_GUI_Label chan_1_min  (lab ().m_Oscilloscope.measurements ().min  (1), LABSoft_GUI_Label::UNIT::VOLT);
    LABSoft_GUI_Label chan_1_max  (lab ().m_Oscilloscope.measurements ().max  (1), LABSoft_GUI_Label::UNIT::VOLT);
    LABSoft_GUI_Label chan_1_trms (lab ().m_Oscilloscope.measurements ().trms (1), LABSoft_GUI_Label::UNIT::VOLT);

    gui ().voltmeter_fl_output_chan_0_dc  ->value (chan_0_dc.to_text    ().c_str ());
    gui ().voltmeter_fl_output_chan_0_min ->value (chan_0_min.to_text   ().c_str ());
    gui ().voltmeter_fl_output_chan_0_max ->value (chan_0_max.to_text   ().c_str ());
    gui ().voltmeter_fl_output_chan_0_trms->value (chan_0_trms.to_text  ().c_str ());

    gui ().voltmeter_fl_output_chan_1_dc  ->value (chan_1_dc.to_text    ().c_str ());
    gui ().voltmeter_fl_output_chan_1_min ->value (chan_1_min.to_text   ().c_str ());
    gui ().voltmeter_fl_output_chan_1_max ->value (chan_1_max.to_text   ().c_str ());
    gui ().voltmeter_fl_output_chan_1_trms->value (chan_1_trms.to_text  ().c_str ());
  } 
}

// EOF