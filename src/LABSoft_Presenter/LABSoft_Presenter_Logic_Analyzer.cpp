#include "LABSoft_Presenter_Logic_Analyzer.h"

#include <cmath>

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../Utility/LAB_Constants.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"
#include "../Utility/LABSoft_GUI_Label_Values.h"

LABSoft_Presenter_Logic_Analyzer:: 
LABSoft_Presenter_Logic_Analyzer (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{
  init ();
  init_gui_values ();
}

void LABSoft_Presenter_Logic_Analyzer:: 
init ()
{
  gui ().logic_analyzer_labsoft_gui_logic_analyzer_display->
    load_parent_data (lab ().m_Logic_Analyzer.parent_data ());
}

void LABSoft_Presenter_Logic_Analyzer:: 
init_gui_values ()
{
  LAB_Logic_Analyzer& logan = lab ().m_Logic_Analyzer;

  // Horizontal
  gui ().logic_analyzer_fl_input_choice_horizontal_offset->value (
    LABSoft_GUI_Label (logan.horizontal_offset ()).to_text (
    LABSoft_GUI_Label::UNIT::SECOND).c_str ()
  );

  gui ().logic_analyzer_fl_input_choice_time_per_division->value (
    LABSoft_GUI_Label (logan.time_per_division ()).to_text (
    LABSoft_GUI_Label::UNIT::SECOND_PER_DIVISION).c_str ()
  );

  gui ().logic_analyzer_fl_input_choice_samples->value (
    LABSoft_GUI_Label (logan.samples ()).to_text (
    LABSoft_GUI_Label::UNIT::NONE).c_str ()
  );

  gui ().logic_analyzer_fl_input_choice_sampling_rate->value (
    LABSoft_GUI_Label (logan.sampling_rate ()).to_text (
    LABSoft_GUI_Label::UNIT::HERTZ).c_str ()
  );

  // Display
  gui ().logic_analyzer_labsoft_gui_logic_analyzer_add_channel_signal_window->
    number_of_channels (LABC::LOGAN::NUMBER_OF_CHANNELS);

  for (unsigned chan = 0; chan < LABC::LOGAN::NUMBER_OF_CHANNELS; chan++)
  {
    gui ().logic_analyzer_labsoft_gui_logic_analyzer_display->add_channel (chan);
  }
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_run_stop (Fl_Light_Button* w,
             void*            data)
{
  if (w->value () == 0)
  {
    lab ().m_Logic_Analyzer.stop ();
  }
  else 
  {
    lab ().m_Logic_Analyzer.run ();
  }
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_single (Fl_Button* w,
           void*      data)
{
  gui ().logic_analyzer_fl_light_button_run_stop->clear ();

  lab ().m_Logic_Analyzer.single ();
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_horizontal_offset (Fl_Input_Choice *w,
                      void            *data)
{
  LABSoft_GUI_Label lbl (
    w->value (),
    lab ().m_Logic_Analyzer.horizontal_offset (),
    LABSoft_GUI_Label::UNIT::SECOND
  );

  lab ().m_Logic_Analyzer.horizontal_offset (lbl.actual_value ());

  update_gui_horizontal ();
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_time_per_division (Fl_Input_Choice *w,
                      void            *data)
{
  LABSoft_GUI_Label lbl (
    w->value (),
    lab ().m_Logic_Analyzer.time_per_division (),
    LABSoft_GUI_Label::UNIT::SECOND_PER_DIVISION
  );

  lab ().m_Logic_Analyzer.time_per_division (lbl.actual_value ());

  update_gui_horizontal ();
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_samples (Fl_Input_Choice*  w,
            void*             data)
{
  LABSoft_GUI_Label lbl (
    w->value (), 
    lab ().m_Logic_Analyzer.samples (),
    LABSoft_GUI_Label::UNIT::NONE
  );

  if (lbl.is_valid ())
  {
    lab ().m_Logic_Analyzer.samples (std::round (lbl.actual_value ()));
  }

  update_gui_horizontal ();
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_sampling_rate (Fl_Input_Choice*  w,
                  void*             data)
{
  LABSoft_GUI_Label lbl (
    w->value (),
    lab ().m_Logic_Analyzer.sampling_rate (),
    LABSoft_GUI_Label::UNIT::HERTZ
  );
  
  if (lbl.is_valid ())
  {
    lab ().m_Logic_Analyzer.sampling_rate (lbl.actual_value ());
  }

  update_gui_horizontal ();
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_trigger_mode (Fl_Choice* w, 
                 void*      data)
{
  std::string choice (w->text ());

  lab ().m_Logic_Analyzer.trigger_mode 
    (LABS_GUI_VALUES::LOGAN::TRIG_MODE_s[choice]);
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_trigger_condition (Fl_Menu_Button* w,
                      void*           data)
{
  ChanWidget* chan    = static_cast<ChanWidget*>(data);
  unsigned    channel = chan->channel ();

  LABE::LOGAN::TRIG::CND trig_cnd = 
    LABS_GUI_VALUES::LOGAN_DISPLAY::TRIG_CND_s.at (std::string (w->text ()));

  lab ().m_Logic_Analyzer.trigger_condition (channel, trig_cnd);
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_mode (Fl_Choice  *w,
                 void       *data)
{
  std::string str (w->text ());

  LABE::LOGAN::MODE mode;

  if (str == "Repeated")
  {
    mode = LABE::LOGAN::MODE::REPEATED;
  }
  else if (str == "Screen")
  {
    mode = LABE::LOGAN::MODE::SCREEN;
  }
  else if (str == "Record")
  {
    mode = LABE::LOGAN::MODE::RECORD;
  }
  else 
  {
    throw (std::runtime_error ("Invalid display mode input."));
  }

  lab ().m_Logic_Analyzer.mode (mode);

  update_gui_mode ();
}

void LABSoft_Presenter_Logic_Analyzer::
display_update_cycle ()
{
  if (lab ().m_Logic_Analyzer.is_running ())
  {
    lab ().m_Logic_Analyzer.update_data_samples ();

    gui ().logic_analyzer_labsoft_gui_logic_analyzer_display->
      update_display ();
  }
}

void LABSoft_Presenter_Logic_Analyzer:: 
update_gui_horizontal ()
{
  LABSoft_GUI_Label horizontal_offset  (lab ().m_Logic_Analyzer.horizontal_offset ());
  LABSoft_GUI_Label time_per_division  (lab ().m_Logic_Analyzer.time_per_division ());
  LABSoft_GUI_Label samples            (lab ().m_Logic_Analyzer.samples ());
  LABSoft_GUI_Label sampling_rate      (lab ().m_Logic_Analyzer.sampling_rate ());

  // 1. Horizontal Offset
  gui ().logic_analyzer_fl_input_choice_horizontal_offset->value (
    horizontal_offset.to_text (LABSoft_GUI_Label::UNIT::SECOND).c_str ()
  );

  // 2. Time per Division
  gui ().logic_analyzer_fl_input_choice_time_per_division->value (
    time_per_division.to_text (LABSoft_GUI_Label::UNIT::SECOND_PER_DIVISION).c_str ()
  );

  // 3. Samples
  gui ().logic_analyzer_fl_input_choice_samples->value (
    samples.to_text (LABSoft_GUI_Label::UNIT::NONE, 3).c_str ()
  );

  // 4. Sampling Rate
  gui ().logic_analyzer_fl_input_choice_sampling_rate->value (
    sampling_rate.to_text (LABSoft_GUI_Label::UNIT::HERTZ).c_str ()
  );

  // 6. Display Mode
  // gui ().logic_analyzer_fl_choice_mode->value (
  //   gui ().logic_analyzer_fl_choice_mode->find_index (
  //     (LABS_GUI_VALUES::OSC::MODE.at (lab ().m_Oscilloscope.mode ())).c_str ()
  //   )
  // );

  // 6. Time per Division Labels
  gui ().logic_analyzer_labsoft_gui_logic_analyzer_display->
    update_gui_time_per_division ();
}

void LABSoft_Presenter_Logic_Analyzer::
update_gui_mode ()
{
  LABE::LOGAN::MODE mode = lab ().m_Logic_Analyzer.mode ();

  if (mode == LABE::LOGAN::MODE::RECORD)
  {
    gui ().logic_analyzer_fl_light_button_run_stop->hide ();
    gui ().logic_analyzer_fl_button_single->hide ();
    gui ().logic_analyzer_fl_button_record->show ();
    gui ().logic_analyzer_fl_button_record_config->show ();
  }
  else 
  {
    gui ().logic_analyzer_fl_light_button_run_stop->show ();
    gui ().logic_analyzer_fl_button_single->show ();
    gui ().logic_analyzer_fl_button_record->hide ();
    gui ().logic_analyzer_fl_button_record_config->hide ();

    update_gui_horizontal ();

    gui ().logic_analyzer_labsoft_gui_logic_analyzer_display-> 
      update_gui_time_per_division ();
    
    // gui ().logic_analyzer_labsoft_logic_analyzer_display_group_display->
    //   update_gui_upper_left_info ();
  }
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_add_channel_selection (Fl_Menu_* w, void* data)
{
  gui ().logic_analyzer_labsoft_gui_logic_analyzer_add_channel_signal_window
    ->show_as_modal ();
}

void LABSoft_Presenter_Logic_Analyzer::
cb_add_channel_signal (LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window* w, void* data)
{
  Fl_Input&         i = *(w->m_name);
  Fl_Multi_Browser& b = *(w->m_multi_browser);

  LABSoft_GUI_Logic_Analyzer_Display& disp = 
    *(gui ().logic_analyzer_labsoft_gui_logic_analyzer_display);

  char      label[20];
  unsigned  added_count = 0;

  for (unsigned line = b.size (); line > 0; line--)
  {
    if ((b.selected (line)) == 1)
    {
      // create label
      if (*(i.value ()) == '\0')
      {
        std::snprintf (label, sizeof (label), "DIO %d", b.data (line));
      }
      else 
      {
        if (added_count == 0)
        {
          std::snprintf (label, sizeof (label), "%s", i.value ());
        }
        else 
        {
          std::snprintf (label, sizeof (label), "%s%d", i.value (), added_count + 1);
        }
      }

      // add channel
      disp.add_channel (reinterpret_cast<unsigned>(b.data (line)), label);

      // increment
      added_count++;
    }
  }
}

void LABSoft_Presenter_Logic_Analyzer:: 
cb_clear_channels (Fl_Menu_* w, void* data)
{
  gui ().logic_analyzer_labsoft_gui_logic_analyzer_display->clear_all_channels ();
}

// EOF