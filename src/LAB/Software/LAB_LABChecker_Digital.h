#ifndef LAB_LABCHECKER_DIGITAL
#define LAB_LABCHECKER_DIGITAL

#include <string>
#include <vector>

class LAB_LABChecker_Digital
{
  private:
    std::stringstream create_circuit_checker_xml_stringstream (const std::vector<std::vector<char>>& inputs,
                                                               const std::vector<std::vector<char>>& outputs);

    void encrypt_stringstream       (std::stringstream& ss, const std::string& password);
    void save_stringstream_to_file  (const std::stringstream& ss, const std::string& file_path);

  public:
    LAB_LABChecker_Digital ();

    void create_circuit_checker_file (const std::vector<std::vector<char>>& inputs,
                                      const std::vector<std::vector<char>>& outputs,
                                      const std::string&                    file_pah,
                                      const char*                           password = 0);
};

#endif