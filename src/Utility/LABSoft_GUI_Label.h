#ifndef LABSOFT_GUI_LABEL
#define LABSOFT_GUI_LABEL

#include <string>
#include <unordered_map>

class LABSoft_GUI_Label
{
  public:
    enum class UNIT
    {
      VOLT,
      VOLT_PER_DIVISION,
      SECOND,
      SECOND_PER_DIVISION,
      OHM,
      HERTZ,
      DEGREE,
      NONE,
      ANY
    };
  
  private:
    struct KeyLengths
    {
      unsigned shortest = 0;
      unsigned longest = 0;
    };

  private:
    bool        m_is_valid        = false;
    int         m_exponent        = 0;
    double      m_actual_value    = 0.0;
    double      m_coefficient     = 0.0;
    double      m_reference       = 0.0;
    std::string m_cleaned_string  = "";
    std::string m_unit_prefix     = "";
    UNIT        m_unit            = UNIT::NONE;

  private:
    static std::unordered_map<UNIT, std::string>  m_unit_to_unit_string;
    static std::unordered_map<std::string, UNIT>  m_unit_string_to_unit;
    static std::unordered_map<std::string, UNIT>  m_unit_string_lowercase_to_unit;
    static std::unordered_map<std::string, int>   m_unit_prefix_to_exponent;
    static KeyLengths                             m_keylengths_unit_prefix_to_exponent;

  private:
    bool        parse_input                             (double value);
    bool        parse_input                             (const std::string& str);
    bool        parse_input_double                      (double value);
    bool        parse_input_string                      (const std::string& str);
    int         conv_unit_prefix_to_exponent            (const std::string& unit_prefix) const;
    int         correct_modulo                          (int exponent, int modulo) const;
    double      calc_actual_value_with_given_reference  (double value, double reference) const;
    double      calc_actual_value                       (double coefficient, double exponent) const;
    double      calc_short_value                        () const;
    std::string find_unit_prefix_match                  (const std::string& str) const;
    std::string conv_exponent_to_unit_prefix            (int exponent) const;
    std::string string_with_no_whitespaces              (const std::string& str) const;
    UNIT        find_unit_match                         (const std::string& str);
    void        get_sci_coefficient_and_exponent        (double value, double& coefficient, int& exponent) const;

    static std::string string_to_lowercase (const std::string& str);

    template <typename Value>
    static KeyLengths 
    find_map_keylengths (const std::unordered_map<std::string, Value>& map);

    template <typename Key, typename Value>
    static std::unordered_map<Value, Key> 
    invert_map (const std::unordered_map<Key, Value>& map);

    template <typename Value>
    static std::unordered_map<std::string, Value> 
    lowercase_map (const std::unordered_map<std::string, Value>& map);  

  public:
    LABSoft_GUI_Label (double value, UNIT unit = UNIT::NONE);
    LABSoft_GUI_Label (const std::string& label, double reference = 0.0, UNIT unit = UNIT::ANY);

    bool        is_valid      () const;
    double      actual_value  () const;
    void        debug         () const;

    std::string to_text       (unsigned precision = 3) const;
    std::string to_text       (UNIT unit, unsigned precision = 3);
    std::string unit_prefix   () const;
};

#endif