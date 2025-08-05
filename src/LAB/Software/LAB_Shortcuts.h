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
    std::unordered_map<int, Config> macro_key_defaults;

  public:
    LAB_Shortcuts(LAB& lab);
    ~LAB_Shortcuts();

    void load_config(const std::string& path = "data/software navigation/shortcuts.snm");
    void save_config(const std::string& path = "data/software navigation/shortcuts.snm") const;

    const Config& get_config(int key_id) const;
    void set_config(int key_id, const std::string& action, const std::string& value);

    bool has_key(int key_id) const;

  private:
    void initialize_defaults();
};

#endif

// EOF
