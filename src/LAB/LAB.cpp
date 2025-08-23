#include "LAB.h"

#include "../../lib/AikaPi/AikaPi.h"

LAB::LAB ()
: m_LAB_Core                (AikaPi::get_instance ()),
  m_Oscilloscope            (*this),
  m_Voltmeter               (*this),
  m_Ohmmeter                (*this),
  m_Function_Generator      (*this),
  m_Logic_Analyzer          (*this),
  m_Digital_Circuit_Checker (*this),
  m_Exporter                (*this),
  m_Oscilloscope_Display    (*this, m_Oscilloscope),
  m_Calibration             (*this),
  m_Software_Navigation     (*this),
  m_Shortcuts               (*this),
  m_LABChecker_Analog       (*this, m_Oscilloscope),
  m_Analog_Circuit_Checker  (*this)
{

}

AikaPi& LAB::
rpi () const
{
  return (m_LAB_Core);
}
