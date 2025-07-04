#ifndef LAB_UTILITY_FUNCTIONS_H
#define LAB_UTILITY_FUNCTIONS_H

#include <string>

namespace LABF
{
  // floating point comparisons
  bool  is_equal                    (double a, double b, double epsilon);
  bool  is_greater_than             (double value, double reference, double epsilon);
  bool  is_less_than                (double value, double reference, double epsilon);
  bool  is_greater_than_or_equal_to (double value, double reference, double epsilon);
  bool  is_less_than_or_equal_to    (double value, double reference, double epsilon);
  bool  is_within_range             (double value, double min, double max, double epsilon);
  
  // data manipulation
  double normalize (double input, double min_input, double max_input, double min_output, double max_output);
  
  // trim functions
  void str_ltrim              (std::string& s);
  void str_rtrim              (std::string& s);
  void str_trim               (std::string& s);
  void str_remove_whitespaces (std::string& s);

  // time functions
  std::string get_now_timestamp ();

  // filename functions
  bool        has_filename_this_extension (const std::string& filename, const std::string& extension);
  std::string get_filename_from_path      (const std::string& path);
};

#endif