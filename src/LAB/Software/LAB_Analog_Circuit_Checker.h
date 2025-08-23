#ifndef LAB_ANALOG_CIRCUIT_CHECKER_H
#define LAB_ANALOG_CIRCUIT_CHECKER_H

#include <cmath>
#include <iostream>

#include "../LAB_Module.h"
#include "../../Utility/LAB_Constants.h"

class LAB_Analog_Circuit_Checker : public LAB_Module
{
  public:
  LAB_Analog_Circuit_Checker(LAB& _lab);

  void      load_file    (const std::string& path);

  private:
};

#endif
