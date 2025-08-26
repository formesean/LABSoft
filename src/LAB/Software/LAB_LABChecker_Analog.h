#ifndef LAB_LABCHECKER_ANALOG_H
#define LAB_LABCHECKER_ANALOG_H

#include <sstream>
#include <string>
#include <vector>

class LAB_LABChecker_Analog
{
public:
  LAB_LABChecker_Analog();

  std::stringstream create_circuit_checker_xml_stringstream();

  void save_stringstream_to_file(const std::stringstream &ss,
                                 const std::string &file_path);

  void create_circuit_checker_file(const std::string &file_path,
                                   const char *password = nullptr);

private:
  void encrypt_stringstream(std::stringstream &ss,
                            const std::string &password);
};

#endif
