#include "LAB_Module.h"

#include "LAB.h"

LAB_Module:: 
LAB_Module (LAB& _LAB)
  : m_LAB (_LAB)
{

}

AikaPi& LAB_Module:: 
rpi () const
{
  return (m_LAB.rpi ());
}

LAB& LAB_Module:: 
lab () const
{
  return (m_LAB);
}