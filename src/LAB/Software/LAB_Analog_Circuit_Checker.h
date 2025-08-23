#ifndef LAB_ANALOG_CIRCUIT_CHECKER_H
#define LAB_ANALOG_CIRCUIT_CHECKER_H

#include <cmath>
#include <iostream>

#include "../LAB.h"
#include "../LAB_Module.h"

class LAB_Analog_Circuit_Checker : public LAB_Module
{
  public:
  LAB_Analog_Circuit_Checker(LAB& _lab);
  
  void      load_file    (const std::string& path);

  private:
};

#endif