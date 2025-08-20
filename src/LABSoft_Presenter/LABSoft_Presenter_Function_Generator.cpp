#include "LABSoft_Presenter_Function_Generator.h"

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"
#include "../Utility/LAB_Utility_Functions.h"
#include "../Utility/LABSoft_GUI_Label_Values.h"

LABSoft_Presenter_Function_Generator::
LABSoft_Presenter_Function_Generator (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{
  init_gui_values ();
}

void LABSoft_Presenter_Function_Generator:: 
init_gui_values ()
{
  LAB_Function_Generator& gen = m_presenter.lab ().m_Function_Generator;
  LABSoft_GUI&            gui = m_presenter.gui ();

  // Channel 0
  Fl_Choice& w = *(gui.function_generator_fl_choice_wave_type);

  w.value (w.find_index (LABS_GUI_VALUES::FUNC_GEN::WAVE_TYPE.at 
    (gen.wave_type (0)).c_str ()));

  // gui.function_generator_fl_input_choice_amplitude->value (
  //   LABSoft_GUI_Label (gen.amplitude (0)).to_text 
  //   (LABSoft_GUI_Label::UNIT::VOLT).c_str ()
  // );

  gui.function_generator_fl_input_choice_frequency->value (
    LABSoft_GUI_Label (gen.frequency (0)).to_text 
    (LABSoft_GUI_Label::UNIT::HERTZ).c_str ()
  );

  gui.function_generator_fl_input_choice_period->value (
    LABSoft_GUI_Label (gen.period (0)).to_text
    (LABSoft_GUI_Label::UNIT::SECOND).c_str ()
  );

  // gui.function_generator_fl_input_choice_phase->value (
  //   LABSoft_GUI_Label (gen.phase (0)).to_text 
  //   (LABSoft_GUI_Label::UNIT::DEGREE).c_str ()
  // );

  // gui.function_generator_fl_input_choice_vertical_offset->value (
  //   LABSoft_GUI_Label (gen.vertical_offset (0)).to_text
  //   (LABSoft_GUI_Label::UNIT::VOLT).c_str ()
  // );
}

void LABSoft_Presenter_Function_Generator:: 
update_gui_frequency_elements ()
{
  m_presenter.gui ().function_generator_fl_input_choice_frequency->
    value (LABSoft_GUI_Label (m_presenter.lab ().m_Function_Generator.frequency (0)).
    to_text (LABSoft_GUI_Label::UNIT::HERTZ).c_str ());
  
  m_presenter.gui ().function_generator_fl_input_choice_period->
    value (LABSoft_GUI_Label (m_presenter.lab ().m_Function_Generator.period (0)).
    to_text (LABSoft_GUI_Label::UNIT::SECOND).c_str ());
}

void LABSoft_Presenter_Function_Generator::
cb_run_stop (Fl_Light_Button* w, 
             long             channel)
{
  if (w->value () == 0)
  {
    m_presenter.lab ().m_Function_Generator.stop (channel);
  }
  else
  {
    m_presenter.lab ().m_Function_Generator.run (channel);
  }
}

void LABSoft_Presenter_Function_Generator::
cb_wave_type (Fl_Choice* w, 
              long       channel)
{
  std::string label (w->text ());
  LABE::FUNC_GEN::WAVE_TYPE wave_type;

  if (label == "Sine")
  {
    wave_type = LABE::FUNC_GEN::WAVE_TYPE::SINE;
  }
  else if (label == "Triangle")
  {
    wave_type = LABE::FUNC_GEN::WAVE_TYPE::TRIANGLE;
  }
  else if (label == "Square")
  {
    wave_type = LABE::FUNC_GEN::WAVE_TYPE::SQUARE;
  }
  else if (label == "DC")
  {
    wave_type = LABE::FUNC_GEN::WAVE_TYPE::DC;
  }
 
  m_presenter.lab ().m_Function_Generator.wave_type (channel, wave_type);
}

void LABSoft_Presenter_Function_Generator:: 
cb_amplitude (Fl_Input_Choice* w, 
              long             channel)
{
  LABSoft_GUI_Label lbl (
    w->value (),
    m_presenter.lab ().m_Function_Generator.amplitude (channel),
    LABSoft_GUI_Label::UNIT::VOLT
  );

  if (lbl.is_valid ())
  {
    if (LABF::is_within_range (
      lbl.actual_value (),
      LABC::FUNC_GEN::MIN_AMPLITUDE,
      LABC::FUNC_GEN::MAX_AMPLITUDE,
      LABC::LABSOFT::EPSILON
    ))
    {
      m_presenter.lab ().m_Function_Generator.amplitude (channel, lbl.actual_value ());
    }
  }

  w->value (LABSoft_GUI_Label (m_presenter.lab ().m_Function_Generator.amplitude (channel)).
    to_text (LABSoft_GUI_Label::UNIT::VOLT).c_str ());
}


void LABSoft_Presenter_Function_Generator:: 
cb_frequency (Fl_Input_Choice *w, 
              long             channel)
{
  LABSoft_GUI_Label lbl (
    w->value (),
    m_presenter.lab ().m_Function_Generator.frequency (channel),
    LABSoft_GUI_Label::UNIT::HERTZ
  );

  if (lbl.is_valid ())
  {
    if (LABF::is_within_range (
      lbl.actual_value (), 
      LABC::FUNC_GEN::MIN_FREQUENCY,
      LABC::FUNC_GEN::MAX_FREQUENCY,
      LABC::LABSOFT::EPSILON
    ))
    {
      m_presenter.lab ().m_Function_Generator.frequency (channel, lbl.actual_value ());
    }
  }

  update_gui_frequency_elements ();
}

void LABSoft_Presenter_Function_Generator:: 
cb_period (Fl_Input_Choice* w, 
           long             channel)
{
  LABSoft_GUI_Label lbl (
    w->value (),
    m_presenter.lab ().m_Function_Generator.period (channel),
    LABSoft_GUI_Label::UNIT::SECOND
  );
  
  if (lbl.is_valid ())
  {
    if (LABF::is_within_range (
      lbl.actual_value (), 
      LABC::FUNC_GEN::MIN_PERIOD,
      LABC::FUNC_GEN::MAX_PERIOD,
      LABC::LABSOFT::EPSILON
    ))
    {
      m_presenter.lab ().m_Function_Generator.period (channel, lbl.actual_value ());
    }
  }

  update_gui_frequency_elements ();
}

void LABSoft_Presenter_Function_Generator:: 
cb_phase (Fl_Input_Choice* w, 
          long             channel)
{
  LABSoft_GUI_Label lbl (
    w->value (),
    m_presenter.lab ().m_Function_Generator.phase (channel),
    LABSoft_GUI_Label::UNIT::DEGREE
  );

  if (lbl.is_valid ())
  {
    if (LABF::is_within_range (
      lbl.actual_value (), 
      LABC::FUNC_GEN::MIN_PHASE,
      LABC::FUNC_GEN::MAX_PHASE,
      LABC::LABSOFT::EPSILON
    ))
    {
      m_presenter.lab ().m_Function_Generator.phase (channel, lbl.actual_value ());
    }
  }

  w->value (LABSoft_GUI_Label (m_presenter.lab ().m_Function_Generator.phase (channel)).
    to_text (LABSoft_GUI_Label::UNIT::DEGREE).c_str ());
}


void LABSoft_Presenter_Function_Generator:: 
cb_vertical_offset (Fl_Input_Choice*  w, 
                    long              channel)
{
  LABSoft_GUI_Label lbl (
    w->value (),
    m_presenter.lab ().m_Function_Generator.vertical_offset (channel),
    LABSoft_GUI_Label::UNIT::VOLT
  );

  if (lbl.is_valid ())
  {
    if (LABF::is_within_range (
      lbl.actual_value (), 
      LABC::FUNC_GEN::MIN_VERTICAL_OFFSET,
      LABC::FUNC_GEN::MAX_VERTICAL_OFFSET,
      LABC::LABSOFT::EPSILON
    ))
    {
      m_presenter.lab ().m_Function_Generator.vertical_offset (channel, lbl.actual_value ());
    }
  }

  w->value (LABSoft_GUI_Label (m_presenter.lab ().m_Function_Generator.vertical_offset (channel)).
    to_text (LABSoft_GUI_Label::UNIT::VOLT).c_str ());
}

// EOF




