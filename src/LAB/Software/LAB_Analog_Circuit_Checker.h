#ifndef LAB_ANALOG_CIRCUIT_CHECKER_H
#define LAB_ANALOG_CIRCUIT_CHECKER_H

#include <cmath>
#include <iostream>
#include <array>
#include <iomanip>
#include <algorithm>

#include "../LAB_Module.h"
#include "../../Utility/LAB_Constants.h"
#include "../../Utility/LAB_Definitions.h"

class LAB_Analog_Circuit_Checker : public LAB_Module
{
  public:
    LAB_Analog_Circuit_Checker(LAB& _lab);

    void      load_file                 (const std::string& path);

    double   compute_cross_correlation  ();
    double   compute_similarity         ();

  private:
    std::array<double, LABC::OSC::NUMBER_OF_SAMPLES> dummy_student_data;
    std::array<double, LABC::OSC::NUMBER_OF_SAMPLES> dummy_instructor_data;

    void     print_samples (const std::array<double, LABC::OSC::NUMBER_OF_SAMPLES>& instructor,
                            const std::array<double, LABC::OSC::NUMBER_OF_SAMPLES>& student);
};

#endif
