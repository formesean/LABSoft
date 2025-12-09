#include "LABSoft_Presenter_Shortcuts.h"

#include <iostream> // For logging

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../Utility/LAB_Constants.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"
#include "../../Utility/LAB_Utility_Functions.h"

using LABE::SNM::ACTION_TYPE;
using LABE::SNM::tab_id_to_label;

LABSoft_Presenter_Shortcuts::LABSoft_Presenter_Shortcuts(LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
}

void LABSoft_Presenter_Shortcuts::
cb_show_window()
{
  lab().m_Shortcuts.load_config();

  const auto& config1 = lab().m_Shortcuts.get_config(1);
  const auto& config2 = lab().m_Shortcuts.get_config(2);

  // -- Set KEY 1 Action --
  if (config1.action == ACTION_TYPE::GOTO)
    gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_1_action->value(0); // "Go to"
  else if (config1.action == ACTION_TYPE::RUN)
    gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_1_action->value(1); // "Run"

  // -- Set KEY 1 Target --
  {
    std::string label1;
    if (std::holds_alternative<LABE::LAB::INSTRUMENT>(config1.target)) {
      auto it = LABE::SNM::tab_id_to_label.find(std::get<LABE::LAB::INSTRUMENT>(config1.target));
      if (it != LABE::SNM::tab_id_to_label.end())
        label1 = std::string(it->second);
    } else {
      label1 = std::get<std::string>(config1.target);
    }

    Fl_Choice* choice = gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_1_value;
    for (int i = 0; i < choice->size(); ++i) {
      if (label1 == choice->text(i)) {
        choice->value(i);
        break;
      }
    }
  }

  // -- Set KEY 2 Action --
  if (config2.action == ACTION_TYPE::GOTO)
    gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_2_action->value(0); // "Go to"
  else if (config2.action == ACTION_TYPE::RUN)
    gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_2_action->value(1); // "Run"

  // -- Set KEY 2 Target --
  {
    std::string label2;
    if (std::holds_alternative<LABE::LAB::INSTRUMENT>(config2.target)) {
      auto it = LABE::SNM::tab_id_to_label.find(std::get<LABE::LAB::INSTRUMENT>(config2.target));
      if (it != LABE::SNM::tab_id_to_label.end())
        label2 = std::string(it->second);
    } else {
      label2 = std::get<std::string>(config2.target);
    }

    Fl_Choice* choice = gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_2_value;
    for (int i = 0; i < choice->size(); ++i) {
      if (label2 == choice->text(i)) {
        choice->value(i);
        break;
      }
    }
  }

  // -- Open the window --
  if (gui().main_fl_window_shortcuts) {
    gui().main_fl_window_shortcuts->set_modal();
    gui().main_fl_window_shortcuts->show();
  }
}

void LABSoft_Presenter_Shortcuts::
cb_apply()
{
  fl_message_title ("Software Navigation Shortcuts");

  switch (fl_choice ("Do you want to apply the shortcuts?", "No", "Yes", 0))
  {
    case 1: // Yes
    {
      // Key 1
      const int action_index_1 = gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_1_action->value();
      const char* action_str_1 = (action_index_1 == 0) ? "GOTO" : "RUN";

      const int value_index_1 = gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_1_value->value();
      const char* value_str_1 = gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_1_value->text(value_index_1);

      lab().m_Shortcuts.set_config(1, action_str_1, value_str_1);

      // Key 2
      const int action_index_2 = gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_2_action->value();
      const char* action_str_2 = (action_index_2 == 0) ? "GOTO" : "RUN";

      const int value_index_2 = gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_2_value->value();
      const char* value_str_2 = gui().shortcuts_labsoft_gui_fl_choice_with_scroll_key_2_value->text(value_index_2);

      lab().m_Shortcuts.set_config(2, action_str_2, value_str_2);

      // Save the updated config to file
      lab().m_Shortcuts.save_config();

      fl_message("Shortcuts applied.");

      break;
    }

    case 0: // No
    {
      break;
    }
  }
}

void LABSoft_Presenter_Shortcuts::
cb_close()
{
  gui().main_fl_window_shortcuts->clear_modal_states();
  gui().main_fl_window_shortcuts->hide();
}

// EOF
