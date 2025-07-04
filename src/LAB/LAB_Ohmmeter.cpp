#include "LAB_Ohmmeter.h"

#include "LAB.h"
#include "../Utility/LAB_Utility_Functions.h"

// delete soon
#include <iostream>

LAB_Ohmmeter:: 
LAB_Ohmmeter (LAB& _LAB)
  : LAB_Module    (_LAB),
    m_calibration (this->m_parent_data)
{

}

bool LAB_Ohmmeter:: 
is_frontend_running () const
{
  return (m_parent_data.is_frontend_running);
}

bool LAB_Ohmmeter:: 
is_backend_running  () const
{
  return (m_parent_data.is_backend_running);
}

void LAB_Ohmmeter:: 
run ()
{
  lab ().m_Oscilloscope .stop ();
  lab ().m_Voltmeter    .stop ();
  lab ().m_Ohmmeter     .stop ();

  //

  frontend_run_stop (true);
  backend_run_stop  (true);
}

void LAB_Ohmmeter:: 
stop ()
{
  frontend_run_stop (false);
  backend_run_stop  (false);
}

void LAB_Ohmmeter:: 
frontend_run_stop (bool value)
{
  m_parent_data.is_frontend_running = value;
}

void LAB_Ohmmeter:: 
backend_run_stop (bool value)
{
  lab ().m_Voltmeter.backend_run_stop (value);

  if (value)
  {
    for (int a = 0; a < LABC::VOLTMETER::NUMBER_OF_CHANNELS; a++)
    {
      lab ().m_Oscilloscope.coupling  (a, LABE::OSC::COUPLING::DC);
      lab ().m_Oscilloscope.scaling   (a, LABC::OHMMETER::SCALING);
      lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (a, LABC::OHMMETER::SCALING, 1.0);
    }
  }

  m_parent_data.is_backend_running = value;
}

double LAB_Ohmmeter:: 
get_reading (unsigned channel) 
{
  double num = lab ().m_Voltmeter.get_reading (0) * m_calibration.r1 (0);
  double den = m_calibration.vref (0) - lab ().m_Voltmeter.get_reading (0);
  double ret = num / den;

  // std::cout << "lab ().m_Voltmeter.get_reading (0): " << lab ().m_Voltmeter.get_reading (0) << "\n";
  // std::cout << "m_calibration.r1 (0): " << m_calibration.r1 (0) << "\n";
  // std::cout << "m_calibration.vref (0: " << m_calibration.vref (0) << "\n\n";

  // return ((LABF::is_less_than (ret, 0.0, LABC::LABSOFT::EPSILON)) ? 0.0 : ret); 

  return (ret);
}

LAB_Ohmmeter::Calibration::
Calibration (LAB_Parent_Data_Ohmmeter& _LAB_Parent_Data_Ohmmeter)
  : m_pdata (_LAB_Parent_Data_Ohmmeter)
{
 
}

void LAB_Ohmmeter::Calibration:: 
enable ()
{
  m_pdata.is_calibration_enabled = true;
}

void LAB_Ohmmeter::Calibration:: 
disable ()
{
  m_pdata.is_calibration_enabled = false;
}

bool LAB_Ohmmeter::Calibration:: 
is_enabled () const
{
  return (m_pdata.is_calibration_enabled);
}

void LAB_Ohmmeter::Calibration:: 
r1 (unsigned channel, double value)
{
  m_pdata.channel_data[channel].calibration.r1 = value;
}

void LAB_Ohmmeter::Calibration:: 
vref (unsigned channel, double value)
{
  m_pdata.channel_data[channel].calibration.vref = value;
}

double LAB_Ohmmeter::Calibration:: 
r1 (unsigned channel) const
{
  return (m_pdata.channel_data[channel].calibration.r1);
}

double LAB_Ohmmeter::Calibration:: 
vref (unsigned channel) const
{
  return (m_pdata.channel_data[channel].calibration.vref);
}

LAB_Ohmmeter::Calibration& LAB_Ohmmeter::
calibration () 
{
  return (m_calibration);
}

    