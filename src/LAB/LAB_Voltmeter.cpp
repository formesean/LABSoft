#include "LAB_Voltmeter.h"

#include "LAB.h"

LAB_Voltmeter:: 
LAB_Voltmeter (LAB& _LAB)
  : LAB_Module (_LAB)
{

}

void LAB_Voltmeter:: 
load_oscilloscope_voltmeter_mode_settings ()
{
  lab ().m_Oscilloscope.trigger_mode    (LABE::OSC::TRIG::MODE::NONE); 
  lab ().m_Oscilloscope.samples         (LABC::OSC::MAX_SAMPLES);
  lab ().m_Oscilloscope.sampling_rate   (LABC::VOLTMETER::SAMPLING_RATE);
  lab ().m_Oscilloscope.do_measurements (true);

  for (int a = 0; a < LABC::VOLTMETER::NUMBER_OF_CHANNELS; a++)
  {
    lab ().m_Oscilloscope.coupling  (a, LABE::OSC::COUPLING::DC);
    lab ().m_Oscilloscope.scaling   (a, LABC::VOLTMETER::SCALING);
    lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (a, LABC::VOLTMETER::SCALING, 1.0);
  }

  lab ().m_Oscilloscope.do_measurements (true);
}

bool LAB_Voltmeter:: 
is_frontend_running () const
{
  return (m_parent_data.is_frontend_running);
}

bool LAB_Voltmeter:: 
is_backend_running () const
{
  return (m_parent_data.is_backend_running);
}

void LAB_Voltmeter:: 
run ()
{
  lab ().m_Oscilloscope .stop ();
  lab ().m_Voltmeter    .stop ();
  lab ().m_Ohmmeter     .stop ();

  //

  frontend_run_stop (true);
  backend_run_stop  (true);

  load_oscilloscope_voltmeter_mode_settings ();
}

void LAB_Voltmeter:: 
stop ()
{
  frontend_run_stop (false);
  backend_run_stop  (false);
}

void LAB_Voltmeter:: 
frontend_run_stop (bool value)
{
  m_parent_data.is_frontend_running = value;
}

void LAB_Voltmeter:: 
backend_run_stop (bool value)
{
  lab ().m_Oscilloscope.backend_run_stop (value);

  if (value) load_oscilloscope_voltmeter_mode_settings ();

  m_parent_data.is_backend_running = value;
}

double LAB_Voltmeter:: 
get_reading (unsigned chan)
{
  return (lab ().m_Oscilloscope.measurements ().avg (chan));
}