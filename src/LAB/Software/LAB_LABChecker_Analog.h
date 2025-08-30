#ifndef LAB_LABCHECKER_ANALOG_H
#define LAB_LABCHECKER_ANALOG_H

#include <string>
#include <sstream>
#include "../../Utility/LAB_Definitions.h"

class LAB_LABChecker_Analog
{

private:
  void encrypt_stringstream(std::stringstream &ss,
                            const std::string &password);

  void save_stringstream_to_file(const std::stringstream &ss,
                                 const std::string &file_path);

public:
  LAB_LABChecker_Analog();

  std::stringstream create_circuit_checker_xml_string_with_data(
      const LAB_Channel_Data_Oscilloscope &osc,
      const LAB_Parent_Data_Oscilloscope &osc_data,
      const LAB_Channel_Data_Function_Generator &func_gen);

  void create_circuit_checker_file(
      const std::string &file_path,
      const LAB_Channel_Data_Oscilloscope &osc,
      const LAB_Parent_Data_Oscilloscope &osc_data,
      const LAB_Channel_Data_Function_Generator &func_gen,
      const char *password = nullptr);
};

#endif // LAB_LABCHECKER_ANALOG_H
