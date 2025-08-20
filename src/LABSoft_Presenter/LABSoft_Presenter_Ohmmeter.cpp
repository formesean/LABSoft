#include "LABSoft_Presenter_Ohmmeter.h"

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"

LABSoft_Presenter_Ohmmeter:: 
LABSoft_Presenter_Ohmmeter (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{
  init_gui_values ();
}

void LABSoft_Presenter_Ohmmeter:: 
init_gui_values ()
{
  gui ().ohmmeter_fl_output_resistance->value ("- . - -");
}

void LABSoft_Presenter_Ohmmeter:: 
run_gui ()
{
  gui ().ohmmeter_fl_light_button_run_stop->set  ();
}

void LABSoft_Presenter_Ohmmeter:: 
stop_gui ()
{
  gui ().ohmmeter_fl_light_button_run_stop->clear ();
}

void LABSoft_Presenter_Ohmmeter:: 
cb_run_stop  (Fl_Light_Button* w, 
              void*            data)
{
  if (w->value ())
  {
    presenter ().m_Oscilloscope.stop_gui ();
    presenter ().m_Voltmeter   .stop_gui ();
    presenter ().m_Ohmmeter    .stop_gui ();

    //

    lab ().m_Ohmmeter.run ();

    presenter ().m_Ohmmeter.run_gui ();    
  }
  else 
  {
    lab ().m_Ohmmeter.stop ();

    presenter ().m_Ohmmeter.stop_gui ();    
  }
}

void LABSoft_Presenter_Ohmmeter:: 
display_update_cycle ()
{
  if (lab ().m_Ohmmeter.is_frontend_running ())
  {
    LABSoft_GUI_Label reading (
      lab ().m_Ohmmeter.get_reading (0), 
      LABSoft_GUI_Label::UNIT::OHM
    );

    gui ().ohmmeter_fl_output_resistance->value (reading.to_text ().c_str ());
  }
}

// EOF