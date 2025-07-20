#include "LAB_Utility_Functions.h"

#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

bool LABF::
is_equal (double a,
          double b,
          double epsilon)
{
  // https://stackoverflow.com/a/17341

  return (std::abs (a - b) < epsilon);
}

bool LABF:: 
is_greater_than (double value,
                 double reference,
                 double epsilon)
{
  // https://stackoverflow.com/a/253874

  return ((value - reference) > ((std::abs (value) < std::abs (reference) ? 
    std::abs (reference) : std::abs (value)) * epsilon));
}


bool LABF:: 
is_less_than (double value,
              double reference,
              double epsilon)
{
  // https://stackoverflow.com/a/253874

  return ((reference - value) > ((std::abs (value) < std::abs (reference) ? 
    std::abs (reference) : std::abs (value)) * epsilon));
}

bool LABF:: 
is_greater_than_or_equal_to (double value,
                             double reference,
                             double epsilon)
{
  return (is_greater_than (value, reference, epsilon) && is_equal (value, reference, epsilon));
}

bool LABF:: 
is_less_than_or_equal_to (double value,
                          double reference,
                          double epsilon)
{
  return (is_less_than (value, reference, epsilon) && is_equal (value, reference, epsilon));
}

bool LABF:: 
is_within_range (double value,
                 double min,
                 double max,
                 double epsilon)
{
  return (
    (is_less_than (value, max, epsilon)     && 
    is_greater_than (value, min, epsilon))  || 
    is_equal (value, min, epsilon)          || 
    is_equal (value, max, epsilon)
  );
}

double LABF::
normalize (double input, 
           double min_input, 
           double max_input, 
           double min_output, 
           double max_output)
{
  return (((input - min_input) / (max_input - min_input)) * 
    (max_output - min_output) + min_output);
}

std::string LABF::
get_now_timestamp ()
{
  // x.
  std::chrono::time_point<std::chrono::system_clock> current_time = 
    std::chrono::system_clock::now ();
  
  // x.
  std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
      current_time_ms = std::chrono::time_point_cast<std::chrono::milliseconds> (current_time);
  
  // x.
  std::time_t time_time_t = std::chrono::system_clock::to_time_t (current_time);

  // x. 
  std::tm* time_tm = std::localtime (&time_time_t);
  
  // x. skip milliseconds for now
  // int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>
  //   (current_time - current_time_ms).count ();
  
  std::stringstream ss;

  ss << std::put_time (time_tm, "%Y-%m-%d %H:%M:%S");
  // ss << "." << std::setw (3) << std::setfill ('0') << milliseconds;

  return (ss.str ());
}

void LABF::
str_ltrim (std::string& s)
{
  s.erase (s.begin (), std::find_if (s.begin (), s.end (), [] (unsigned char ch) {
    return (!std::isspace (ch));
  }));
}

void LABF::
str_rtrim (std::string& s)
{
  s.erase (std::find_if (s.rbegin (), s.rend (), [] (unsigned char ch) {
    return (!std::isspace (ch));
  }).base (), s.end ());
}

void LABF::
str_trim (std::string& s)
{
  str_ltrim (s);
  str_rtrim (s);
}

void LABF::
str_remove_whitespaces (std::string& s)
{
  s.erase (std::remove_if (s.begin (), s.end (), [] (unsigned char ch) {
      return (std::isspace (ch));
  }), s.end ());
}

bool LABF:: 
has_filename_this_extension (const std::string& filename,
                             const std::string& extension)
{
  // 1. take into account ".filename" or "filename" for extension argument
  size_t ext_dot_pos = extension.find_first_of (".");

  std::string ext;

  if (ext_dot_pos == std::string::npos)
  {
    ext = extension;
  }
  else 
  {
    ext = extension.substr (ext_dot_pos + 1);
  }

  // 2. actual comparison
  size_t dot_pos = filename.find_last_of (".");

  if (dot_pos == std::string::npos)
  {
    return (false);
  }
  else 
  {
    return (filename.substr (dot_pos + 1) == ext);
  }
}

std::string LABF::
get_filename_from_path (const std::string& path)
{
  // 1. Get position of the last separator slash
  size_t last_separator_pos = path.find_last_of ("/\\");

  std::string filename;

  if (last_separator_pos == std::string::npos)
  {
    filename = path;
  }
  else 
  {
    // 2. Extract the substring starting from the last separator
    filename = path.substr (last_separator_pos + 1);    
  }

  // 3. Get position of the last dot (before the extension)
  size_t last_dot_pos = filename.find_last_of (".");

  // 4. Extract the substring until before the last dot
  //    to get the actual filename
  filename = filename.substr (0, last_dot_pos);

  return (filename);
}

  
