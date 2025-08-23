#include "LAB_Analog_Circuit_Checker.h"

#include <cmath>
#include <iostream>

#include "LAB.h"

LAB_Analog_Circuit_Checker::
LAB_Analog_Circuit_Checker(LAB& _lab)
    : LAB_Module(_lab)
{
    

}
void LAB_Analog_Circuit_Checker:: 
load_file (const std::string& path)
{
  lab().m_Analog_Circuit_Checker.load_file(path);
}