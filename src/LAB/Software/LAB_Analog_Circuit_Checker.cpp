#include "LAB_Analog_Circuit_Checker.h"

#include <cmath>
#include <iostream>

#include "../LAB.h"
#include "../../Utility/LAB_Constants.h"
#include "../../Utility/LAB_Definitions.h"
#include "../../Utility/LAB_Utility_Functions.h"

LAB_Analog_Circuit_Checker::
LAB_Analog_Circuit_Checker(LAB& _lab)
    : LAB_Module(_lab)
{
    create_dummy_data();
}

void LAB_Analog_Circuit_Checker::
load_file (const std::string& path)
{
  lab().m_Analog_Circuit_Checker.load_file(path);
}

void LAB_Analog_Circuit_Checker::
create_dummy_data()
{
  double frequency = 1000.0;
  double amplitude = 1.0;
  double offset = 0;
  double delay = 0.0001;
  double noise = (static_cast<double>(rand() % 100) - 50.0) / 1000.0;

  for (unsigned i = 0; i < LABC::OSC::NUMBER_OF_SAMPLES; ++i)
  {
    double time = static_cast<double>(i) / LABC::OSC::NUMBER_OF_SAMPLES;

    dummy_instructor_data[i] = amplitude * sin(2.0 * M_PI * frequency * (time - delay)) + offset + noise; // student channel
    dummy_student_data[i] = amplitude * sin(2.0 * M_PI * frequency * time) + offset + noise;              // instructor channel
  }

  // Print first 10 samples from both dummy data arrays
  std::cout << "\n=== DUMMY DATA SAMPLES ===" << std::endl;
  std::cout << "Student Data (first 10 samples):" << std::endl;
  for (unsigned i = 0; i < 10 && i < LABC::OSC::NUMBER_OF_SAMPLES; ++i)
  {
    std::cout << "  Sample[" << i << "]: " << dummy_student_data[i] << " V" << std::endl;
  }

  std::cout << "\nInstructor Data (first 10 samples):" << std::endl;
  for (unsigned i = 0; i < 10 && i < LABC::OSC::NUMBER_OF_SAMPLES; ++i)
  {
    std::cout << "  Sample[" << i << "]: " << dummy_instructor_data[i] << " V" << std::endl;
  }
  std::cout << "===============================" << std::endl << std::endl;
}
