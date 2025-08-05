#include "LAB_Shortcuts.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace LABE::SNM;

LAB_Shortcuts::
LAB_Shortcuts(LAB& lab)
  : LAB_Module(lab)
{
  initialize_defaults();
  load_config("data/software navigation/shortcuts.snm");
}

LAB_Shortcuts::
~LAB_Shortcuts()
{
  save_config("data/software navigation/shortcuts.snm");
}

void LAB_Shortcuts::
initialize_defaults()
{
  macro_key_defaults =
  {
    {1, {ACTION_TYPE::GOTO, TAB_ID::DIGITAL_CIRCUIT_CHECKER}},
    {2, {ACTION_TYPE::RUN, "Oscilloscope"}}
  };
  macro_key_map = macro_key_defaults;
}

bool LAB_Shortcuts::
has_key(int key_id) const
{
  return macro_key_map.find(key_id) != macro_key_map.end();
}

const MACRO_KEY_CONFIG& LAB_Shortcuts::
get_config(int key_id) const
{
  auto it = macro_key_map.find(key_id);
  return it != macro_key_map.end() ? it->second : macro_key_defaults.at(1);
}

void LAB_Shortcuts::
set_config(int key_id, const std::string& action_str, const std::string& value_str)
{
  ACTION_TYPE action = ACTION_TYPE::UNKNOWN;
  if (action_str == "GOTO") action = ACTION_TYPE::GOTO;
  else if (action_str == "RUN") action = ACTION_TYPE::RUN;

  if (action == ACTION_TYPE::GOTO)
  {
    for (const auto& [label, id] : tab_label_to_id)
    {
      if (label == value_str)
      {
        macro_key_map[key_id] = {action, std::variant<TAB_ID, std::string>(id)};
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

void LAB_Shortcuts::
load_config(const std::string& path)
{
  std::ifstream in(path);
  if (!in) return;

  macro_key_map.clear();
  std::string line;

  while (std::getline(in, line))
  {
    std::istringstream iss(line);
    int key_id;
    std::string action_str, value_str;
    if (iss >> key_id >> action_str && std::getline(iss, value_str))
    {
      if (!value_str.empty() && value_str[0] == ' ')
        value_str = value_str.substr(1);

      set_config(key_id, action_str, value_str);
    }
  }
}

void LAB_Shortcuts::
save_config(const std::string& path) const
{
  std::ofstream out(path);
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
          value_str = std::string(label);
          break;
        }
      }
    }
    else
    {
      value_str = std::get<std::string>(config.target);
    }

    out << key_id << " " << action_str << " " << value_str << "\n";
  }
}

// EOF
