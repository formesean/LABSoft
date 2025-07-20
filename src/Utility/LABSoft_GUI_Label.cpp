#include "LABSoft_GUI_Label.h"

#include <iomanip>
#include <cmath>
#include <sstream>
#include <algorithm>

// delete soon
#include <iostream>

// --- static member variables ---

std::unordered_map<LABSoft_GUI_Label::UNIT, std::string> LABSoft_GUI_Label::m_unit_to_unit_string = 
{
  {UNIT::VOLT               , "V"},
  {UNIT::VOLT_PER_DIVISION  , "V/div"},
  {UNIT::SECOND             , "s"},
  {UNIT::SECOND_PER_DIVISION, "s/div"},
  {UNIT::OHM                , "Ω"},
  {UNIT::HERTZ              , "Hz"},
  {UNIT::DEGREE             , "deg"},
  {UNIT::NONE               , ""},
  {UNIT::ANY                , ""}
};

std::unordered_map<std::string, int> LABSoft_GUI_Label::m_unit_prefix_to_exponent = 
{
  {"n", -9},
  {"u", -6},
  {"m", -3},
  {"",   0},
  {"k",  3},
  {"M",  6},
  {"G",  9},
};

std::unordered_map<std::string, LABSoft_GUI_Label::UNIT> LABSoft_GUI_Label::m_unit_string_to_unit = 
  invert_map (LABSoft_GUI_Label::m_unit_to_unit_string);

std::unordered_map<std::string, LABSoft_GUI_Label::UNIT> LABSoft_GUI_Label::m_unit_string_lowercase_to_unit =
  lowercase_map (m_unit_string_to_unit);

LABSoft_GUI_Label::KeyLengths LABSoft_GUI_Label::m_keylengths_unit_prefix_to_exponent = 
  find_map_keylengths (m_unit_prefix_to_exponent);

// ----------

LABSoft_GUI_Label::
LABSoft_GUI_Label (double                  value,
                   LABSoft_GUI_Label::UNIT unit)
{
  m_unit      = unit;
  m_is_valid  = parse_input (value);
}

LABSoft_GUI_Label:: 
LABSoft_GUI_Label (const std::string&            input,
                         double                  reference,
                         LABSoft_GUI_Label::UNIT unit)
  : m_cleaned_string (string_with_no_whitespaces (input))
{
  m_reference = reference;
  m_unit      = unit;
  m_is_valid  = parse_input (m_cleaned_string);

  // debug ();
}

bool LABSoft_GUI_Label:: 
parse_input (double value)
{
  return (parse_input_double (value));
}

bool LABSoft_GUI_Label:: 
parse_input (const std::string& str)
{
  try 
  {
    if (str.find_first_not_of ("0123456789.+-") == std::string::npos)
    {
      return (parse_input_double (std::stod (str)));
    }
    else 
    {
      return (parse_input_string (str));
    }
  }
  catch (const std::exception& e)
  {
    return (false);
  }
}

bool LABSoft_GUI_Label:: 
parse_input_double (double value)
{
  m_actual_value = calc_actual_value_with_given_reference (value, m_reference);

  get_sci_coefficient_and_exponent (m_actual_value, m_coefficient, m_exponent);

  m_unit_prefix = conv_exponent_to_unit_prefix (m_exponent);

  return (true);
}

bool LABSoft_GUI_Label:: 
parse_input_string (const std::string& str)
{
  size_t pos      = str.find_first_not_of ("0123456789.+-");
  m_coefficient   = std::stod (str.substr (0, pos));
  m_unit_prefix   = find_unit_prefix_match (str.substr (pos));
  m_unit          = find_unit_match (str.substr (pos + m_unit_prefix.length ()));
  m_exponent      = conv_unit_prefix_to_exponent (m_unit_prefix);
  m_actual_value  = calc_actual_value (m_coefficient, m_exponent);

  return (true);
}

int LABSoft_GUI_Label:: 
conv_unit_prefix_to_exponent (const std::string& unit_prefix) const
{
  return (m_unit_prefix_to_exponent.at (unit_prefix));
}

int LABSoft_GUI_Label::
correct_modulo (int exponent, 
                int modulo) const
{
  return (exponent < 0 ? ((exponent % modulo + modulo) % modulo) : (exponent %modulo));
}

std::string LABSoft_GUI_Label:: 
conv_exponent_to_unit_prefix (int exponent) const
{
  std::string unit_prefix;

  switch (exponent)
  {
    case -9:
    case -8:
    case -7:
      unit_prefix = "n";
      break;

    case -6:
    case -5:
    case -4:
      unit_prefix = "u";
      break;

    case -3:
    case -2:
    case -1:
      unit_prefix = "m";
      break;
    
    case 0:
    case 1:
    case 2:
      unit_prefix = "";
      break;  

    case 3:
    case 4:
    case 5:
      unit_prefix = "k";
      break;

    case 6:
    case 7:
    case 8:
      unit_prefix = "M";
      break;
    
    case 9:
    case 10:
    case 11:
      unit_prefix = "G";
      break;
  }

  return (unit_prefix);
}

std::string LABSoft_GUI_Label:: 
find_unit_prefix_match (const std::string& str) const
{
  KeyLengths& kl          = m_keylengths_unit_prefix_to_exponent; 
  std::string unit_prefix = ""; // defaults to no unit prefix (expressed as "")

  for (int a = 0; (a < kl.longest) && (a < str.length ()); a++)
  {
    std::string substr = str.substr (0, a + 1);

    std::unordered_map<std::string, int>::iterator it = 
      m_unit_prefix_to_exponent.find (substr);

    if (it != m_unit_prefix_to_exponent.end ())
    {
      unit_prefix = substr;
    }
  }

  return (unit_prefix);
}

LABSoft_GUI_Label::UNIT LABSoft_GUI_Label:: 
find_unit_match (const std::string& str) 
{
  std::string lower_str               = string_to_lowercase (str);
  LABSoft_GUI_Label::UNIT unit_match  = m_unit_string_lowercase_to_unit.at (lower_str);

  if ((m_unit == LABSoft_GUI_Label::UNIT::ANY) || (m_unit == unit_match))
  {
    return (unit_match);
  }
  else 
  {
    throw; // will be caught at parse_input (const std::string& str);
  }
}

std::string LABSoft_GUI_Label:: 
string_with_no_whitespaces (const std::string& str) const
{
  std::string temp = str;

  temp.erase (std::remove_if (temp.begin (), temp.end (), ::isspace), temp.end ());

  return (temp);
}

void LABSoft_GUI_Label:: 
get_sci_coefficient_and_exponent (double  value,
                                  double& coefficient,
                                  int&    exponent) const
{
  // 1. convert the value to a string representation of its scientific notation
  std::stringstream ss;
  ss << std::scientific << std::setprecision (12) << value;
  std::string str = ss.str (); 

  // 2. extract the coefficient and exponent from the string representation
  unsigned e_pos  = str.find_first_of ("Ee");
  coefficient     = std::stod (str.substr (0, e_pos));
  exponent        = std::stoi (str.substr (e_pos + 1));
}

double LABSoft_GUI_Label:: 
calc_actual_value_with_given_reference (double value,
                                        double reference) const
{
  if (reference == 0.0)
  {
    return (value);
  }
  else 
  {
    // 1.
    double  ref_coefficient = 0.0;
    int     ref_exponent    = 0;

    get_sci_coefficient_and_exponent (reference, ref_coefficient, ref_exponent);  

    // 2.
    std::string ref_unit_prefix = conv_exponent_to_unit_prefix (ref_exponent);

    // 3.
    int ref_pow_exponent = conv_unit_prefix_to_exponent (ref_unit_prefix);

    // 4.
    double actual_value = value * std::pow (10, ref_pow_exponent);   

    return (actual_value);
  }
}

double LABSoft_GUI_Label:: 
calc_actual_value (double coefficient,
                   double exponent) const
{
  return (coefficient * std::pow (10, exponent));
}

double LABSoft_GUI_Label:: 
calc_short_value () const
{
  return (m_coefficient * std::pow (10, correct_modulo (m_exponent, 3)));
}

template <typename Key, typename Value>
std::unordered_map<Value, Key> LABSoft_GUI_Label::
invert_map (const std::unordered_map<Key, Value>& map)
{
  std::unordered_map<Value, Key> new_map;

  for (const std::pair<Key, Value> entry : map)
  {
    new_map[entry.second] = entry.first;
  }

  return (new_map);
}

template <typename Value>
std::unordered_map<std::string, Value> LABSoft_GUI_Label::
lowercase_map (const std::unordered_map<std::string, Value>& map)
{
  std::unordered_map<std::string, Value> new_map;

  for (const std::pair<std::string, Value> entry : map)
  {
    new_map[LABSoft_GUI_Label::string_to_lowercase (entry.first)] = entry.second;
  }

  return (new_map);
}

template <typename Value>
LABSoft_GUI_Label::KeyLengths LABSoft_GUI_Label:: 
find_map_keylengths (const std::unordered_map<std::string, Value>& map)
{
  LABSoft_GUI_Label::KeyLengths keylengths;

  for (const std::pair<std::string, Value>& pair : map)
  {
    const std::string&  key     = pair.first;
          unsigned      length  = key.length ();

    if (length < keylengths.shortest)
    {
      keylengths.shortest = length;
    }

    if (length > keylengths.longest)
    {
      keylengths.longest = length;
    }
  }

  return (keylengths);
}

// setters
// void LABSoft_GUI_Label:: 
// unit (LABSoft_GUI_Label::UNIT unit)
// {
//   m_unit = unit;
// }

// getters
bool LABSoft_GUI_Label::
is_valid () const
{
  return (m_is_valid);
}

double LABSoft_GUI_Label:: 
actual_value () const
{
  return (m_actual_value);
}

std::string LABSoft_GUI_Label:: 
to_text (unsigned precision) const
{
  std::stringstream ss;

  ss  << std::setprecision (precision)
      << calc_short_value ()
      << " "
      << m_unit_prefix
      << m_unit_to_unit_string[m_unit];

  return (ss.str ());
}

std::string LABSoft_GUI_Label:: 
to_text (LABSoft_GUI_Label::UNIT unit,
         unsigned                precision)
{
  m_unit = unit;

  return (to_text (precision));
}

void LABSoft_GUI_Label:: 
debug () const
{
  std::cout << "----------\n";
  std::cout << "raw string      : " << m_cleaned_string         << "\n";
  std::cout << "is valid        : " << m_is_valid               << "\n";
  std::cout << "actual value    : " << m_actual_value           << "\n";
  std::cout << "coefficient     : " << m_coefficient            << "\n";
  std::cout << "exponent        : " << m_exponent               << "\n";
  std::cout << "unit prefix     : " << m_unit_prefix            << "\n";
  std::cout << "label type      : " << static_cast<int>(m_unit) << "\n";
  std::cout << "reference value : " << m_reference              << "\n";
  std::cout << "----------\n\n";
}

std::string LABSoft_GUI_Label:: 
unit_prefix () const
{
  return (m_unit_prefix);
}

std::string LABSoft_GUI_Label:: 
string_to_lowercase (const std::string& input)
{
  std::string lowercase_str = input;

  std::transform (
    lowercase_str.begin (),
    lowercase_str.end   (),
    lowercase_str.begin (),
    ::tolower
  );

  return (lowercase_str);
}