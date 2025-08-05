#ifndef LAB_SHORTCUTS_H
#define LAB_SHORTCUTS_H

#include "../LAB_Module.h"
#include "../../Utility/LAB_Enumerations.h"

#include <unordered_map>
#include <string>
#include <variant>

class LAB_Shortcuts : public LAB_Module
{
  using Config = LABE::SNM::MACRO_KEY_CONFIG;

  private:
    std::unordered_map<int, Config> macro_key_map;
    std::string                     config_path;

  public:
    LAB_Shortcuts  (LAB& lab, const std::string& config_file = "../data/software_navigation/shortcuts.snm");
    ~LAB_Shortcuts ();

    void          load_config ();
    void          save_config () const;

    const Config& get_config (int key_id) const;
    void          set_config (int key_id, const std::string& action, const std::string& value);

    bool          has_key    (int key_id) const;
};

#endif

// EOF
