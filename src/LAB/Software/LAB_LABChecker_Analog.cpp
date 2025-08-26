#include "LAB_LABChecker_Analog.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "../../Utility/pugixml.hpp"
#include "../../Utility/LAB_Constants.h"

// #include "../../Utility/LAB_Encryptor.h"

LAB_LABChecker_Analog::LAB_LABChecker_Analog()
{
}

std::stringstream LAB_LABChecker_Analog::
create_circuit_checker_xml_stringstream()
{
  pugi::xml_document doc;
  pugi::xml_node root = doc.append_child("root");

  // --- metadata ---
  pugi::xml_node metadata = root.append_child("metadata");

  metadata.append_child("channels").append_child(pugi::node_pcdata).set_value("2");
  metadata.append_child("samples").append_child(pugi::node_pcdata).set_value("2000");
  metadata.append_child("sampling_rate").append_child(pugi::node_pcdata).set_value("40000");
  metadata.append_child("time_per_division").append_child(pugi::node_pcdata).set_value("0.005");
  metadata.append_child("horizontal_offset").append_child(pugi::node_pcdata).set_value("0");
  metadata.append_child("trigger_mode").append_child(pugi::node_pcdata).set_value("0");
  metadata.append_child("trigger_source").append_child(pugi::node_pcdata).set_value("0");
  metadata.append_child("trigger_type").append_child(pugi::node_pcdata).set_value("1");
  metadata.append_child("trigger_condition").append_child(pugi::node_pcdata).set_value("0");
  metadata.append_child("trigger_level").append_child(pugi::node_pcdata).set_value("0");

  // --- data ---
  pugi::xml_node data = root.append_child("data");

  // Channel 1
  {
    pugi::xml_node data_pair = data.append_child("data_pair");
    data_pair.append_child("input").append_child(pugi::node_pcdata).set_value("Channel 1");

    pugi::xml_node output = data_pair.append_child("output");
    output.append_child("samples").append_child(pugi::node_pcdata).set_value("2000");
    output.append_child("coupling").append_child(pugi::node_pcdata).set_value("0");
    output.append_child("scaling").append_child(pugi::node_pcdata).set_value("2");
    output.append_child("voltage_per_division").append_child(pugi::node_pcdata).set_value("1");
    output.append_child("vertical_offset").append_child(pugi::node_pcdata).set_value("0");
    output.append_child("is_enabled").append_child(pugi::node_pcdata).set_value("1");
    output.append_child("scaling_corrector").append_child(pugi::node_pcdata).set_value("1");

    pugi::xml_node meas = output.append_child("measurements");
    meas.append_child("min").append_child(pugi::node_pcdata).set_value("0");
    meas.append_child("max").append_child(pugi::node_pcdata).set_value("0");
    meas.append_child("avg").append_child(pugi::node_pcdata).set_value("0");
    meas.append_child("trms").append_child(pugi::node_pcdata).set_value("0");

    pugi::xml_node samples = output.append_child("first_10_samples");
    for (int i = 0; i < 10; i++)
    {
      samples.append_child("sample").append_child(pugi::node_pcdata).set_value("1.88324");
    }
  }

  // Channel 2
  {
    pugi::xml_node data_pair = data.append_child("data_pair");
    data_pair.append_child("input").append_child(pugi::node_pcdata).set_value("Channel 2");

    pugi::xml_node output = data_pair.append_child("output");
    output.append_child("samples").append_child(pugi::node_pcdata).set_value("2000");
    output.append_child("coupling").append_child(pugi::node_pcdata).set_value("0");
    output.append_child("scaling").append_child(pugi::node_pcdata).set_value("2");
    output.append_child("voltage_per_division").append_child(pugi::node_pcdata).set_value("1");
    output.append_child("vertical_offset").append_child(pugi::node_pcdata).set_value("0");
    output.append_child("is_enabled").append_child(pugi::node_pcdata).set_value("0");
    output.append_child("scaling_corrector").append_child(pugi::node_pcdata).set_value("1");

    pugi::xml_node meas = output.append_child("measurements");
    meas.append_child("min").append_child(pugi::node_pcdata).set_value("0");
    meas.append_child("max").append_child(pugi::node_pcdata).set_value("0");
    meas.append_child("avg").append_child(pugi::node_pcdata).set_value("0");
    meas.append_child("trms").append_child(pugi::node_pcdata).set_value("0");

    pugi::xml_node samples = output.append_child("first_10_samples");
    for (int i = 0; i < 10; i++)
    {
      samples.append_child("sample").append_child(pugi::node_pcdata).set_value("0");
    }
  }

  // Function Generator
  {
    pugi::xml_node data_pair = data.append_child("data_pair");
    data_pair.append_child("input").append_child(pugi::node_pcdata).set_value("Function Generator Ch1");

    pugi::xml_node output = data_pair.append_child("output");
    output.append_child("wave_type").append_child(pugi::node_pcdata).set_value("0");
    output.append_child("frequency").append_child(pugi::node_pcdata).set_value("1000");
    output.append_child("period").append_child(pugi::node_pcdata).set_value("0.001");
  }

  std::stringstream ss;
  doc.save(ss);
  return ss;
}

void LAB_LABChecker_Analog::
encrypt_stringstream(std::stringstream &ss,
                     const std::string &password)
{
  // ss.str(LAB_Encryptor::encrypt_string(ss.str(), password));
}

void LAB_LABChecker_Analog::
save_stringstream_to_file(const std::stringstream &ss,
                          const std::string &file_path)
{
  std::ofstream file(file_path);
  if (file.is_open())
  {
    file << ss.str();
    file.close();
  }
  else
  {
    throw(std::runtime_error("Error in saving analog circuit checker file."));
  }
}

void LAB_LABChecker_Analog::
create_circuit_checker_file(const std::string &file_path,
                            const char *password)
{
  std::stringstream ss = create_circuit_checker_xml_stringstream();

  // if (password)
  // {
  //   encrypt_stringstream(ss, std::string(password));
  // }

  save_stringstream_to_file(ss, file_path);
}
