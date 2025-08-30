#include "LAB_LABChecker_Analog.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

#include "../../Utility/pugixml.hpp"
#include "../../Utility/LAB_Definitions.h"

LAB_LABChecker_Analog::LAB_LABChecker_Analog()
{
}

static std::stringstream create_circuit_checker_xml_string_with_data_impl(
    const std::vector<LAB_Channel_Data_Oscilloscope> &osc_channels,
    const LAB_Parent_Data_Oscilloscope &osc_data,
    const LAB_Channel_Data_Function_Generator &func_gen)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(6);

  ss << "<?xml version=\"1.0\"?>\n";
  ss << "<root>\n";

  // Oscilloscope metadata and channels
  ss << "  <oscilloscope>\n";
  // Save horizontal and trigger state
  ss << "    <osc_mode>" << static_cast<int>(osc_data.mode) << "</osc_mode>\n";
  ss << "    <horizontal_offset>" << osc_data.horizontal_offset << "</horizontal_offset>\n";
  ss << "    <time_per_division>" << osc_data.time_per_division << "</time_per_division>\n";
  ss << "    <samples>" << osc_data.samples << "</samples>\n";
  ss << "    <sampling_rate>" << osc_data.sampling_rate << "</sampling_rate>\n";
  ss << "    <trigger_mode>" << static_cast<int>(osc_data.trigger_mode) << "</trigger_mode>\n";
  ss << "    <trigger_source>" << static_cast<int>(osc_data.trigger_source) << "</trigger_source>\n";
  ss << "    <trigger_type>" << static_cast<int>(osc_data.trig_type) << "</trigger_type>\n";
  ss << "    <trigger_condition>" << static_cast<int>(osc_data.trig_condition) << "</trigger_condition>\n";
  ss << "    <trigger_level>" << osc_data.trigger_level << "</trigger_level>\n";

  // Channels block (persist per-channel settings and samples)
  ss << "    <channels>\n";
  for (size_t i = 0; i < osc_channels.size(); ++i)
  {
    const auto &osc = osc_channels[i];
    ss << "      <channel index=\"" << i << "\">\n";
    ss << "        <is_enabled>" << osc.is_enabled << "</is_enabled>\n";
    ss << "        <voltage_per_division>" << osc.voltage_per_division << "</voltage_per_division>\n";
    ss << "        <vertical_offset>" << osc.vertical_offset << "</vertical_offset>\n";
    ss << "        <scaling>" << static_cast<int>(osc.scaling) << "</scaling>\n";
    ss << "        <coupling>" << static_cast<int>(osc.coupling) << "</coupling>\n";

    // Measurements
    ss << "        <measurements>\n";
    ss << "          <min>" << osc.measurements.min << "</min>\n";
    ss << "          <max>" << osc.measurements.max << "</max>\n";
    ss << "          <avg>" << osc.measurements.avg << "</avg>\n";
    ss << "          <trms>" << osc.measurements.trms << "</trms>\n";
    ss << "        </measurements>\n";

    // Samples
    if (i == 1)
    {
      ss << "        <samples>\n";
      for (const auto &s : osc.samples)
        ss << "          <sample>" << s << "</sample>\n";
      ss << "        </samples>\n";
    }
    ss << "      </channel>\n";
  }
  ss << "    </channels>\n";

  ss << "  </oscilloscope>\n";

  // Function Generator
  ss << "  <function_generator>\n";
  ss << "    <is_enabled>" << func_gen.is_enabled << "</is_enabled>\n";
  ss << "    <wave_type>" << static_cast<int>(func_gen.wave_type) << "</wave_type>\n";
  ss << "    <frequency>" << func_gen.frequency << "</frequency>\n";
  ss << "    <period>" << func_gen.period << "</period>\n";
  ss << "    <amplitude>" << func_gen.amplitude << "</amplitude>\n";
  ss << "    <vertical_offset>" << func_gen.vertical_offset << "</vertical_offset>\n";
  ss << "    <phase>" << func_gen.phase << "</phase>\n";
  ss << "    <Rf>" << func_gen.Rf << "</Rf>\n";
  ss << "  </function_generator>\n";

  ss << "</root>\n";

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

void LAB_LABChecker_Analog::create_circuit_checker_file(
    const std::string &file_path,
    const LAB_Parent_Data_Oscilloscope &osc_data,
    const LAB_Channel_Data_Function_Generator &func_gen,
    const char *password)
{
  // Build channels array from current oscilloscope parent data
  std::vector<LAB_Channel_Data_Oscilloscope> osc_channels;
  osc_channels.reserve(osc_data.channel_data.size());
  for (const auto &src : osc_data.channel_data)
    osc_channels.push_back(src);

  std::stringstream ss = create_circuit_checker_xml_string_with_data_impl(osc_channels, osc_data, func_gen);

  // if (password)
  //     encrypt_stringstream(ss, std::string(password));

  save_stringstream_to_file(ss, file_path);
}
