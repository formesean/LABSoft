#include "LAB_Analog_Circuit_Checker.h"

#include "../LAB.h"
#include "../../Utility/LAB_Constants.h"
#include "../../Utility/LAB_Definitions.h"
#include "../../Utility/LAB_Utility_Functions.h"

LAB_Analog_Circuit_Checker::
LAB_Analog_Circuit_Checker(LAB& _lab)
    : LAB_Module(_lab)
{
  double Fs     = 40e3;
  double f      = 1000;
  double offset = 0.2;
  size_t delay  = 2;

  for (size_t n = 0; n < dummy_instructor_data.size(); n++)
  {
    double t = n / Fs;                                        // convert sample index to time

    size_t delayed_index = (n >= delay) ? (n - delay) : 0;    // shifts the student’s signal by delay samples
    double t_delayed = delayed_index / Fs;                    // convert delayed index to delayed time

    dummy_instructor_data[n] = std::sin(2 * M_PI * f * t);
    dummy_student_data[n]    = std::sin(2 * M_PI * f * t_delayed) + offset;
  }

  // Print samples
  print_samples(dummy_instructor_data, dummy_student_data);

  // Print similarity
  double sim = compute_similarity();
  std::cout << "\nSimilarity: " << sim << "%\n";
}

void LAB_Analog_Circuit_Checker::
load_file (const std::string& path)
{
  lab().m_Analog_Circuit_Checker.load_file(path);
}

double LAB_Analog_Circuit_Checker::
compute_cross_correlation ()
{
  const size_t N         = dummy_instructor_data.size(); // number of samples
  double       numerator = 0.0;                          // dot product of the two signal
  double       denom_x   = 0.0;                          // sum of squares of instructor signal
  double       denom_y   = 0.0;                          // sum of squares of student signal

  for (size_t n = 0; n < N; n++)
  {
    numerator += dummy_instructor_data[n] * dummy_student_data[n];      // multiply the instructor’s sample by the student’s sample
                                                                        // add it to the running total
                                                                        // numerator = measures alignment between signals

    denom_x   += dummy_instructor_data[n] * dummy_instructor_data[n];   // square the instructor’s sample
                                                                        // add it to the running total
                                                                        // denom_x = total signal energy of the instructor’s signal

    denom_y   += dummy_student_data[n]    * dummy_student_data[n];      // square the student's sample
                                                                        // add it to the running total
                                                                        // denom_y = total signal energy of the student's signal
  }

  if (denom_x == 0.0 || denom_y == 0.0) return 0.0; // avoid div by zero

  return numerator / std::sqrt(denom_x * denom_y); // normalized correlation
}


double LAB_Analog_Circuit_Checker::
compute_similarity()
{
    double corr = compute_cross_correlation();
    return corr * 100.0; // percentage
    // if corr =  1.0 ->  100% similar
    // if corr =  0.0 ->  0%   similar
    // if corr = -1.0 -> -100% (perfectly opposite)
}

void LAB_Analog_Circuit_Checker::
print_samples(const std::array<double, LABC::OSC::NUMBER_OF_SAMPLES>& instructor,
              const std::array<double, LABC::OSC::NUMBER_OF_SAMPLES>& student)
{
  constexpr size_t NUM_TO_PRINT = 10;

  std::cout << "Instructor Data (first 10): [ ";
  for (size_t i = 0; i < NUM_TO_PRINT; i++)
  {
    std::cout << std::fixed << std::setprecision(4) << instructor[i];
    if (i < NUM_TO_PRINT - 1) std::cout << ", ";
  }
  std::cout << " ]\n";

  std::cout << "Student Data (first 10):    [ ";
  for (size_t i = 0; i < NUM_TO_PRINT; i++)
  {
    std::cout << std::fixed << std::setprecision(4) << student[i];
    if (i < NUM_TO_PRINT - 1) std::cout << ", ";
  }
  std::cout << " ]\n";
}
