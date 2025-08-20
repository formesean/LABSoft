#include "LAB_Shortcuts.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace LABE::SNM;

std::string trim(const std::string& str)
{
  size_t first = str.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) return "";

  size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, (last - first + 1));
}

LAB_Shortcuts::
LAB_Shortcuts(LAB& lab, const std::string& config_file)
  : LAB_Module(lab), config_path(config_file)
{
  load_config();
}

LAB_Shortcuts::
~LAB_Shortcuts()
{
  save_config();
}

void LAB_Shortcuts::
load_config()
{
  std::ifstream in(config_path);
  if (!in) return;

  macro_key_map.clear();
  std::string line;

  while (std::getline(in, line))
  {
    std::istringstream iss(line);
    std::string key_str, action_str, value_str;

    if (!std::getline(iss, key_str, ',')) continue;
    if (!std::getline(iss, action_str, ',')) continue;
    if (!std::getline(iss, value_str)) continue;

    const int key_id = std::stoi(key_str);
    set_config(key_id, action_str, trim(value_str));
  }
}

void LAB_Shortcuts::
save_config() const
{
  std::ofstream out(config_path);
  if (!out) return;

  for (const auto& [key_id, config] : macro_key_map)
  {
    std::string action_str;
    switch (config.action)
    {
      case ACTION_TYPE::GOTO: action_str = "GOTO"; break;
      case ACTION_TYPE::RUN:  action_str = "RUN"; break;
      default:                action_str = "UNKNOWN"; break;
    }

    std::string value_str;
    if (std::holds_alternative<TAB_ID>(config.target))
    {
      for (const auto& [label, id] : tab_label_to_id)
      {
        if (id == std::get<TAB_ID>(config.target))
        {
          value_str = label;
          break;
        }
      }
    }
    else
    {
      value_str = std::get<std::string>(config.target);
    }

    out << key_id << "," << action_str << "," << value_str << "\n";
  }
}

const MACRO_KEY_CONFIG& LAB_Shortcuts::
get_config(int key_id) const
{
  auto it = macro_key_map.find(key_id);
  if (it != macro_key_map.end()) return it->second;

  static const MACRO_KEY_CONFIG fallback{ACTION_TYPE::UNKNOWN, ""};
  return fallback;
}

void LAB_Shortcuts::
set_config(int key_id, const std::string& action_str, const std::string& value_str_raw)
{
  const std::string value_str = trim(value_str_raw);

  ACTION_TYPE action = ACTION_TYPE::UNKNOWN;
  if (action_str == "GOTO") action = ACTION_TYPE::GOTO;
  else if (action_str == "RUN") action = ACTION_TYPE::RUN;

  if (action == ACTION_TYPE::GOTO)
  {
    for (const auto& [label, id] : tab_label_to_id)
    {
      if (std::string(label) == value_str)
      {
        macro_key_map[key_id] = {action, id};
        return;
      }
    }
    macro_key_map[key_id] = {ACTION_TYPE::UNKNOWN, value_str};
  }
  else
  {
    macro_key_map[key_id] = {action, value_str};
  }
}

bool LAB_Shortcuts::
has_key(int key_id) const
{
  return macro_key_map.find(key_id) != macro_key_map.end();
}

// EOF
