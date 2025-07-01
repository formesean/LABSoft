#include "LABSoft_Presenter_Oscilloscope.h"

#include <cmath>

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"
#include "../Utility/LABSoft_GUI_Label_Values.h"

LABSoft_Presenter_Oscilloscope::
LABSoft_Presenter_Oscilloscope (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit  (_LABSoft_Presenter)
{
  init_gui_values ();
}

void LABSoft_Presenter_Oscilloscope:: 
init_gui_values () const
{
  LAB_Oscilloscope& osc = lab ().m_Oscilloscope;

  // ========== 1. vertical ==========

  // 1.1. channel enable disable
  osc.is_channel_enabled (0) ? 
    gui ().oscilloscope_fl_light_button_channel_0_enable->set    ():
    gui ().oscilloscope_fl_light_button_channel_0_enable->clear  ();
  
  osc.is_channel_enabled (1) ? 
    gui ().oscilloscope_fl_light_button_channel_1_enable->set    ():
    gui ().oscilloscope_fl_light_button_channel_1_enable->clear  ();

  // 1.2. coupling
  osc.coupling (0) == LABE::OSC::COUPLING::AC ? 
    gui ().oscilloscope_fl_light_button_channel_0_ac_coupling->set   (): 
    gui ().oscilloscope_fl_light_button_channel_0_ac_coupling->clear ();

  osc.coupling (1) == LABE::OSC::COUPLING::AC ? 
    gui ().oscilloscope_fl_light_button_channel_1_ac_coupling->set   (): 
    gui ().oscilloscope_fl_light_button_channel_1_ac_coupling->clear ();

  // 1.3. scaling
  {
    LABSoft_GUI_Fl_Choice_With_Scroll* w;

    w = gui ().oscilloscope_labsoft_gui_fl_choice_with_scroll_channel_0_scaling;

    w->value (w->find_index (LABS_GUI_VALUES::OSC::SCALING_s.at (osc.scaling (0)).c_str ()));
  }
  {
    LABSoft_GUI_Fl_Choice_With_Scroll* w;

    w = gui ().oscilloscope_labsoft_gui_fl_choice_with_scroll_channel_1_scaling;
  
    w->value (w->find_index (LABS_GUI_VALUES::OSC::SCALING_s.at (osc.scaling (1)).c_str ()));
  } 
  
  // 1.4. voltage per division
  {
    LABSoft_GUI_Fl_Input_Choice_With_Scroll* w;

    w = gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_voltage_per_division;

    w->value (LABSoft_GUI_Label (osc.voltage_per_division (0)).
      to_text (LABSoft_GUI_Label::UNIT::VOLT_PER_DIVISION).c_str ());
  }
  {
    LABSoft_GUI_Fl_Input_Choice_With_Scroll* w;
    
    w = gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_voltage_per_division;

    w->value (LABSoft_GUI_Label (osc.voltage_per_division (1)).
      to_text (LABSoft_GUI_Label::UNIT::VOLT_PER_DIVISION).c_str ());
  }
  
  // 1.5. vertical offset
  {
    LABSoft_GUI_Fl_Input_Choice_With_Scroll* w;

    w = gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_vertical_offset;

    w->value (LABSoft_GUI_Label (osc.vertical_offset (0)).
      to_text (LABSoft_GUI_Label::UNIT::VOLT).c_str ());
  }
  {
    LABSoft_GUI_Fl_Input_Choice_With_Scroll* w;

    w = gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_vertical_offset;

    w->value (LABSoft_GUI_Label (osc.vertical_offset (1)).
      to_text (LABSoft_GUI_Label::UNIT::VOLT).c_str ());
  }
  
  // ========== 2. horizontal ==========
  update_gui_horizontal_elements ();

  // ========== 3. trigger ==========
  {
    LABSoft_GUI_Fl_Choice_With_Scroll* w;

    w = gui ().oscilloscope_fl_choice_trigger_mode;

    w->value (w->find_index (LABS_GUI_VALUES::OSC::TRIGGER_MODE_s.at
      (osc.trigger_mode ()).c_str ()));
  }
  {
    LABSoft_GUI_Fl_Choice_With_Scroll* w;

    w = gui ().oscilloscope_fl_choice_trigger_source;

    w->value (w->find_index (LABS_GUI_VALUES::OSC::TRIGGER_SOURCE_s.at
      (osc.trigger_source ()).c_str ()));
  } 
  {
    LABSoft_GUI_Fl_Choice_With_Scroll* w;

    w = gui ().oscilloscope_fl_choice_trigger_type;

    w->value (w->find_index (LABS_GUI_VALUES::OSC::TRIGGER_TYPE_s.at
      (osc.trigger_type ()).c_str ()));
  }
  {
    LABSoft_GUI_Fl_Choice_With_Scroll* w;

    w = gui ().oscilloscope_fl_choice_trigger_condition;

    w->value (w->find_index (LABS_GUI_VALUES::OSC::TRIGGER_CONDITION_s.at
      (osc.trigger_condition ()).c_str ()));
  }

  gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_trigger_level->value (
    LABSoft_GUI_Label (osc.trigger_level ()).to_text (
    LABSoft_GUI_Label::UNIT::VOLT).c_str ()
  );

  // ========== 4. mode ==========
  {
    LABSoft_GUI_Fl_Choice_With_Scroll* w;

    w = gui ().oscilloscope_labsoft_gui_fl_choice_with_scroll_mode;

    w->value (w->find_index (LABS_GUI_VALUES::OSC::MODE_s.at 
      (osc.mode ()).c_str ())); 
  }
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_panel_vertical_offset (unsigned channel) const
{
  LABSoft_GUI_Label label (
    lab ().m_Oscilloscope.vertical_offset (channel),
    LABSoft_GUI_Label::UNIT::VOLT 
  );

  if (channel)
  {
    gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_vertical_offset->
      value (label.to_text ().c_str ());
  }
  else 
  {
    gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_vertical_offset->
      value (label.to_text ().c_str ());
  }  
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_panel_voltage_per_division (unsigned channel) const
{
  LABSoft_GUI_Label label (
    lab ().m_Oscilloscope.voltage_per_division (channel),
    LABSoft_GUI_Label::UNIT::VOLT_PER_DIVISION
  );

  if (channel)
  {
    gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_1_voltage_per_division->
      value (label.to_text ().c_str ());
  }
  else 
  {
    gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_channel_0_voltage_per_division->
      value (label.to_text ().c_str ());
  } 
}

void LABSoft_Presenter_Oscilloscope::
update_gui_panel_horizontal_offset () const
{
  LABSoft_GUI_Label label (
    lab ().m_Oscilloscope.horizontal_offset (),
    LABSoft_GUI_Label::UNIT::SECOND
  );

  gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_horizontal_offset->
    value (label.to_text ().c_str ());
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_panel_time_per_division () const
{
  LABSoft_GUI_Label label (
    lab ().m_Oscilloscope.time_per_division (),
    LABSoft_GUI_Label::UNIT::SECOND_PER_DIVISION
  );
  
  gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_time_per_division->
    value (label.to_text ().c_str ());
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_panel_samples () const
{
  LABSoft_GUI_Label label (
    lab ().m_Oscilloscope.samples (),
    LABSoft_GUI_Label::UNIT::NONE
  );
  
  gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_samples->
    value (label.to_text ().c_str ());
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_panel_sampling_rate () const
{
  LABSoft_GUI_Label label (
    lab ().m_Oscilloscope.sampling_rate (),
    LABSoft_GUI_Label::UNIT::HERTZ
  );
  
  gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_sampling_rate->
    value (label.to_text ().c_str ());
}
  
void LABSoft_Presenter_Oscilloscope:: 
update_gui_panel_mode () const
{
  LABE::OSC::MODE mode = lab ().m_Oscilloscope.mode ();

  // 1.
  if (mode == LABE::OSC::MODE::RECORD)
  {
    gui ().oscilloscope_fl_light_button_run_stop->hide  ();
    gui ().oscilloscope_fl_button_single->hide          ();

    gui ().oscilloscope_fl_button_record->show          ();
    gui ().oscilloscope_fl_button_record_config->show   ();
  }
  else 
  {
    gui ().oscilloscope_fl_light_button_run_stop->show  ();
    gui ().oscilloscope_fl_button_single->show          ();

    gui ().oscilloscope_fl_button_record->hide          ();
    gui ().oscilloscope_fl_button_record_config->hide   ();
  }

  // 2.
  LABSoft_GUI_Fl_Choice_With_Scroll& w = 
    *(gui ().oscilloscope_labsoft_gui_fl_choice_with_scroll_mode);
  
  w.value (w.find_index (LABS_GUI_VALUES::OSC::MODE_s.at (mode).c_str ()));
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_panel_trigger_level () const
{
  LABSoft_GUI_Label label (
    lab ().m_Oscilloscope.trigger_level (),
    LABSoft_GUI_Label::UNIT::VOLT
  );

  gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_trigger_level->
    value (label.to_text ().c_str ());
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_vertical_offset (unsigned channel) const
{
  update_gui_panel_vertical_offset (channel);

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    vertical_offset (channel, lab ().m_Oscilloscope.vertical_offset (channel));

  lab ().m_Oscilloscope_Display.update_cached_values ();
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_voltage_per_division (unsigned channel) const
{ 
  update_gui_panel_voltage_per_division (channel);

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    voltage_per_division (channel, lab ().m_Oscilloscope.voltage_per_division (channel));

  lab ().m_Oscilloscope_Display.update_cached_values ();
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_horizontal_offset () const
{
  update_gui_panel_horizontal_offset ();

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    horizontal_offset (lab ().m_Oscilloscope.horizontal_offset ());  

  lab ().m_Oscilloscope_Display.update_cached_values ();
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_time_per_division () 
{
  update_gui_horizontal_elements ();  

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    time_per_division (lab ().m_Oscilloscope.time_per_division ());  

  lab ().m_Oscilloscope_Display.update_cached_values (); 
  
  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    mark_samples (lab ().m_Oscilloscope_Display.mark_samples ());
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_samples () 
{
  update_gui_horizontal_elements ();

  lab ().m_Oscilloscope_Display.update_cached_values ();

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    mark_samples (lab ().m_Oscilloscope_Display.mark_samples ());
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_sampling_rate () 
{
  update_gui_horizontal_elements ();

  lab ().m_Oscilloscope_Display.update_cached_values ();

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    mark_samples (lab ().m_Oscilloscope_Display.mark_samples ());
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_horizontal_elements () const
{
  update_gui_panel_horizontal_offset  ();
  update_gui_panel_time_per_division  ();
  update_gui_panel_samples            ();
  update_gui_panel_sampling_rate      ();
  update_gui_panel_mode               ();
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_mode () const
{
  update_gui_panel_mode ();
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_trigger_level () const
{
  update_gui_panel_trigger_level ();

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    trigger_level (lab ().m_Oscilloscope.trigger_level ());
}

void LABSoft_Presenter_Oscilloscope:: 
update_gui_trigger_panel () const
{
  if (lab ().m_Oscilloscope.trigger_mode () == LABE::OSC::TRIG::MODE::NONE)
  {
    gui ().oscilloscope_fl_choice_trigger_source->deactivate ();
    gui ().oscilloscope_fl_choice_trigger_type->deactivate ();
    gui ().oscilloscope_fl_choice_trigger_condition->deactivate ();
    gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_trigger_level->deactivate ();
  }
  else 
  {
    gui ().oscilloscope_fl_choice_trigger_source->activate ();
    gui ().oscilloscope_fl_choice_trigger_type->activate ();
    gui ().oscilloscope_fl_choice_trigger_condition->activate ();
    gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_trigger_level->activate ();
  }
}

void LABSoft_Presenter_Oscilloscope:: 
cb_run_stop (Fl_Light_Button* w, 
             void*            data)
{
  if (w->value ())
  {
    presenter ().m_Oscilloscope.stop_gui ();
    presenter ().m_Voltmeter   .stop_gui ();
    presenter ().m_Ohmmeter    .stop_gui ();

    //

    lab ().m_Oscilloscope.run ();

    presenter ().m_Oscilloscope.run_gui ();
  }
  else 
  {
    lab ().m_Oscilloscope.stop ();

    presenter ().m_Oscilloscope.stop_gui ();
  }
}

void LABSoft_Presenter_Oscilloscope:: 
run_gui ()
{
  lab ().m_Oscilloscope_Display.update_cached_values ();

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->
    mark_samples (lab ().m_Oscilloscope_Display.mark_samples ());

  gui ().oscilloscope_fl_light_button_run_stop->set ();
}

void LABSoft_Presenter_Oscilloscope::
stop_gui ()
{
  gui ().oscilloscope_fl_light_button_run_stop->clear ();
}

void LABSoft_Presenter_Oscilloscope:: 
cb_single (Fl_Button* w,
           void*      data)
{
  gui ().oscilloscope_fl_light_button_run_stop->clear ();

  lab ().m_Oscilloscope.single ();
}

void LABSoft_Presenter_Oscilloscope:: 
cb_channel_enable_disable (Fl_Light_Button* w,
                           long             channel)
{
  lab ().m_Oscilloscope.channel_enable_disable (channel, w->value ());

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->channel_enable_disable (channel, w->value ());
}

void LABSoft_Presenter_Oscilloscope::
cb_ac_coupling (Fl_Light_Button*  w, 
                long              channel)
{
  lab ().m_Oscilloscope.coupling (
    channel,
    w->value () ? LABE::OSC::COUPLING::AC : LABE::OSC::COUPLING::DC
  );
}

void LABSoft_Presenter_Oscilloscope:: 
cb_scaling (LABSoft_GUI_Fl_Choice_With_Scroll*  w,
            long                                channel)
{
  lab ().m_Oscilloscope.scaling (
    channel,
    LABS_GUI_VALUES::OSC::SCALING[std::string (w->text ())]
  );
}

void LABSoft_Presenter_Oscilloscope::
cb_vertical_offset (LABSoft_GUI_Fl_Input_Choice_With_Scroll*  w, 
                    long                                      channel)
{ 
  LABSoft_GUI_Label label (
    w->value (),
    lab ().m_Oscilloscope.vertical_offset (channel),
    LABSoft_GUI_Label::UNIT::VOLT
  );

  if (label.is_valid ())
  {
    lab ().m_Oscilloscope.vertical_offset (channel, label.actual_value ());
  }

  update_gui_vertical_offset (channel);
}

void LABSoft_Presenter_Oscilloscope::
cb_voltage_per_division (LABSoft_GUI_Fl_Input_Choice_With_Scroll* w, 
                        long                                      channel)
{
  LABSoft_GUI_Label label (
    w->value (),
    lab ().m_Oscilloscope.voltage_per_division (channel),
    LABSoft_GUI_Label::UNIT::VOLT_PER_DIVISION
  );

  if (label.is_valid ())
  {
    lab ().m_Oscilloscope.voltage_per_division (channel, label.actual_value ());
  }

  update_gui_voltage_per_division (channel);
}

void LABSoft_Presenter_Oscilloscope:: 
cb_vertical_offset_slider (LABSoft_GUI_Fl_Slider* w,
                           void*                  data)
{
  unsigned channel = reinterpret_cast<unsigned>(data);
}

// horizontal
void LABSoft_Presenter_Oscilloscope::
cb_horizontal_offset (LABSoft_GUI_Fl_Input_Choice_With_Scroll* w, 
                      void*                                    data) const
{
  LABSoft_GUI_Label label (
    w->value (),
    lab ().m_Oscilloscope.horizontal_offset (),
    LABSoft_GUI_Label::UNIT::SECOND
  );

  if (label.is_valid ())
  {
    lab ().m_Oscilloscope.horizontal_offset (label.actual_value ());
  }

  update_gui_horizontal_offset ();
}

void LABSoft_Presenter_Oscilloscope:: 
cb_time_per_division (LABSoft_GUI_Fl_Input_Choice_With_Scroll* w,
                      void*                                    data)
{
  LABSoft_GUI_Label label (
    w->value (),
    lab ().m_Oscilloscope.time_per_division (),
    LABSoft_GUI_Label::UNIT::SECOND_PER_DIVISION
  );

  if (label.is_valid ())
  {
    lab ().m_Oscilloscope.time_per_division (label.actual_value ());
  }
  
  update_gui_time_per_division ();
}

void LABSoft_Presenter_Oscilloscope:: 
cb_samples (Fl_Input_Choice*  w,
            void*             data)
{
  LABSoft_GUI_Label label (
    w->value (), 
    0, // 0 as reference kay whole numbers raman sad ang samples
    LABSoft_GUI_Label::UNIT::NONE
  );

  if (label.is_valid ())
  {
    lab ().m_Oscilloscope.samples (std::round (label.actual_value ()));
  }

  update_gui_samples ();
}

void LABSoft_Presenter_Oscilloscope:: 
cb_sampling_rate (Fl_Input_Choice*  w,
                  void*             data)
{
  LABSoft_GUI_Label label (
    w->value (),
    lab ().m_Oscilloscope.sampling_rate (),
    LABSoft_GUI_Label::UNIT::HERTZ
  );
  
  if (label.is_valid ())
  {
    lab ().m_Oscilloscope.sampling_rate (label.actual_value ());
  }

  update_gui_sampling_rate ();
}

void LABSoft_Presenter_Oscilloscope::
cb_mode (Fl_Choice* w,  
         void*      data)
{
  std::string str (w->text ());

  LABE::OSC::MODE mode;

  if (str == "Repeated")
  {
    mode = LABE::OSC::MODE::REPEATED;
  }
  else if (str == "Screen")
  {
    mode = LABE::OSC::MODE::SCREEN;
  }
  else if (str == "Record")
  {
    mode = LABE::OSC::MODE::RECORD;
  }
  else 
  {
    throw (std::runtime_error ("Invalid display mode input."));
  }

  lab ().m_Oscilloscope.mode (mode);

  update_gui_mode ();
}

void LABSoft_Presenter_Oscilloscope::
cb_trigger_mode (Fl_Choice* w,
                 void*      data)
{
  std::string choice (w->text ());

  LABE::OSC::TRIG::MODE mode = LABS_GUI_VALUES::OSC::TRIGGER_MODE.at (choice);

  lab ().m_Oscilloscope.trigger_mode (mode);

  update_gui_trigger_panel ();
}

void LABSoft_Presenter_Oscilloscope::
cb_trigger_source (Fl_Choice* w, 
                   void*      data)
{
  unsigned trigger_source = LABS_GUI_VALUES::OSC::TRIGGER_SOURCE.at (std::string (w->text ()));

  lab ().m_Oscilloscope.trigger_source (trigger_source);

  gui ().oscilloscope_labsoft_gui_oscilloscope_display->trigger_source (trigger_source);
}

void LABSoft_Presenter_Oscilloscope:: 
cb_trigger_type (Fl_Choice* w,
                 void*      data)
{
  LABE::OSC::TRIG::TYPE type;
  std::string choice (w->text ());

  if (choice == "Edge")
  {
    type = LABE::OSC::TRIG::TYPE::EDGE;
  }
  else if (choice == "Level")
  {
    type = LABE::OSC::TRIG::TYPE::LEVEL;
  }
  else 
  {
    throw (std::runtime_error ("Invalid trigger type input."));
  }
  
  lab ().m_Oscilloscope.trigger_type (type);
}

void LABSoft_Presenter_Oscilloscope:: 
cb_trigger_condition (Fl_Choice* w,
                      void*      data)
{
  LABE::OSC::TRIG::CND cnd;
  std::string choice (w->text ());

  if (choice == "Rising")
  {
    cnd = LABE::OSC::TRIG::CND::RISING;
  }
  else if (choice == "Falling")
  {
    cnd = LABE::OSC::TRIG::CND::FALLING;
  }
  else if (choice == "Either")
  {
    cnd = LABE::OSC::TRIG::CND::EITHER;
  }
  else 
  {
    throw (std::runtime_error ("Invalid trigger condition input."));
  }

  lab ().m_Oscilloscope.trigger_condition (cnd);
}

void LABSoft_Presenter_Oscilloscope::
cb_trigger_level (LABSoft_GUI_Fl_Input_Choice_With_Scroll* w, 
                  void*            data)
{
  LABSoft_GUI_Label label (
    w->value (),
    lab ().m_Oscilloscope.trigger_level (),
    LABSoft_GUI_Label::UNIT::VOLT
  );

  if (label.is_valid ())
  {
    lab ().m_Oscilloscope.trigger_level (label.actual_value ());
  }

  update_gui_trigger_level ();
}

void LABSoft_Presenter_Oscilloscope:: 
cb_trigger_level_slider (LABSoft_GUI_Fl_Slider* w, 
                         void*                  data)
{
  lab ().m_Oscilloscope.trigger_level (w->value ());

  LABSoft_GUI_Label lbl (lab ().m_Oscilloscope.trigger_level ());

  gui ().oscilloscope_labsoft_gui_fl_input_choice_with_scroll_trigger_level->value (
    lbl.to_text (LABSoft_GUI_Label::UNIT::VOLT).c_str ()
  );
}

void LABSoft_Presenter_Oscilloscope::
cb_record (Fl_Button*  w, 
           void*       data)
{

}

void LABSoft_Presenter_Oscilloscope::
cb_record_config (Fl_Button* w, 
                  void*      data)
{
  gui ().oscilloscope_fl_window_record_config->show ();
  gui ().main_fl_tabs->deactivate ();
}

void LABSoft_Presenter_Oscilloscope::
cb_record_config_start (Fl_Button*  w,
                        void*       data)
{

}

void LABSoft_Presenter_Oscilloscope::
cb_record_config_cancel (Fl_Button* w,
                         void*      data)
{
  gui ().oscilloscope_fl_window_record_config->hide ();
  gui ().main_fl_tabs->activate ();
}

void LABSoft_Presenter_Oscilloscope:: 
cb_export (Fl_Menu_Item*  w,
           void*          data)
{

}
    
void LABSoft_Presenter_Oscilloscope:: 
display_update_cycle ()
{ 
  if (lab ().m_Oscilloscope.is_backend_running ())
  {
    lab ().m_Oscilloscope.update_data_samples ();

    if (lab ().m_Oscilloscope.trigger_found ())
    {
      lab ().m_Oscilloscope.trigger_serviced ();
    }
  }

  if (lab ().m_Oscilloscope.is_frontend_running ())
  {
    presenter ().m_Oscilloscope_Display.update_display ();
  }
}

void LABSoft_Presenter_Oscilloscope:: 
cb_debug_measurements (Fl_Light_Button* w, void* data)
{
  lab ().m_Oscilloscope.do_measurements (w->value ());
}

// EOF