#ifndef LAB_DATA_MEASURER_H
#define LAB_DATA_MEASURER_H

#include <array>
#include <tuple>
#include <cstddef>

class LAB_Data_Measurer
{
  public:
    template <typename T, std::size_t N>
    static std::tuple<T, T, double> min_max_avg (const std::array<T, N>& arr);

    template <typename T, std::size_t N>
    static double true_rms (const std::array<T, N>& arr);
};

#endif