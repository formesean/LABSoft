#include "LAB_Data_Measurer.h"

#include <cmath>
#include <iostream>

#include "../../Utility/LAB_Constants.h"
#include "../../Utility/LAB_Utility_Functions.h"

template <typename T, std::size_t N>
std::tuple<T, T, double> LAB_Data_Measurer:: 
min_max_avg (const std::array<T, N>& arr) 
{
  double min = 0;
  double max = 0;
  double avg = 0;
  double sum = 0;

  for (const auto& x : arr)
  {
    sum += x;

    if (LABF::is_greater_than (x, max, LABC::LABSOFT::EPSILON))
    {
      max = x;
    }

    if (LABF::is_less_than (x, min, LABC::LABSOFT::EPSILON))
    {
      min = x;
    }
  }

  avg = sum / arr.size ();

  return (std::make_tuple (min, max, avg));
}

template <typename T, std::size_t N>
double LAB_Data_Measurer:: 
true_rms (const std::array<T, N>& arr) 
{
  double sum = 0;

  for (const auto& x : arr)
  {
    sum += std::pow (x, 2);
  }

  sum /= arr.size ();

  return (std::sqrt (sum));
}