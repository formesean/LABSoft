#include "LAB_LABChecker_Digital.h"

#include <fstream>
#include <sstream>

#include "../../Utility/pugixml.hpp"
#include "../../Utility/LAB_Constants.h"

// #include "../../Utility/LAB_Encryptor.h"

// #include <chrono>
// #include <string>
// #include <iomanip>
// #include <sstream>
// #include <iostream>
// #include <stdexcept>
// #include <fstream>

LAB_LABChecker_Digital:: 
LAB_LABChecker_Digital ()
{

}

std::stringstream LAB_LABChecker_Digital:: 
create_circuit_checker_xml_stringstream (const std::vector<std::vector<char>>& inputs,
                                         const std::vector<std::vector<char>>& outputs)
{
  pugi::xml_document  doc;
  pugi::xml_node      root      = doc.append_child ("root");
  pugi::xml_node      metadata  = root.append_child ("metadata");

  metadata.append_child ("input_bits")
          .append_child (pugi::node_pcdata)
          .set_value    (std::to_string (inputs[0].size ()).c_str ());

  metadata.append_child ("output_bits")
          .append_child (pugi::node_pcdata)
          .set_value    (std::to_string (outputs[0].size ()).c_str ());   

  pugi::xml_node data = root.append_child ("data");

  for (size_t row = 0; row < inputs.size (); row++)
  {
    // 1. node
    pugi::xml_node data_pair    = data.append_child ("data_pair");

    // 2. input
    pugi::xml_node input_node   = data_pair.append_child ("input");

    input_node.append_child (pugi::node_pcdata).set_value 
      (std::string (inputs[row].begin (), inputs[row].end ()).c_str ());
    
    // 3. output
    pugi::xml_node output_node  = data_pair.append_child ("output");

    output_node.append_child (pugi::node_pcdata).set_value 
      (std::string (outputs[row].begin (), outputs[row].end ()).c_str ());
  }

  std::stringstream ss;

  doc.save (ss);

  return (ss);
}

void LAB_LABChecker_Digital:: 
encrypt_stringstream (std::stringstream& ss,
                      const std::string& password)
{
  // ss.str (LAB_Encryptor::encrypt_string (ss.str (), password));
}

void LAB_LABChecker_Digital:: 
save_stringstream_to_file (const std::stringstream& ss,
                           const std::string&       file_path)
{
  std::string path = file_path;
  
  // if (file_path.length () < extension.length () || file_path.compare (
  //   file_path.length () - extension.length (), extension.length (), 
  //   extension) != 0)
  // {
  //   path += LABC::LABSOFT::DIGITAL_CIRCUIT_CHECKER_FILENAME_EXTENSION;
  // }

  std::ofstream file (path);

  if (file.is_open ())
  {
    std::string str = ss.str ();

    file << str;

    file.close ();
  }
  else 
  {
    throw (std::runtime_error ("Error in saving circuit checker file."));
  }
}

void LAB_LABChecker_Digital:: 
create_circuit_checker_file (const std::vector<std::vector<char>>& inputs,
                             const std::vector<std::vector<char>>& outputs,
                             const std::string&                    file_path,
                             const char*                           password)
{
  std::stringstream ss = create_circuit_checker_xml_stringstream (inputs, outputs);
  
  // if (password)
  // {
  //   encrypt_stringstream (ss, std::string (password));
  // }

  save_stringstream_to_file (ss, file_path);
}

// EOF
