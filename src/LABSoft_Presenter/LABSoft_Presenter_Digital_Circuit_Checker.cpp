#include "LABSoft_Presenter_Digital_Circuit_Checker.h"

#include <cstdio>
#include <string>

#include <FL/fl_ask.H>

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LAB_Utility_Functions.h"

LABSoft_Presenter_Digital_Circuit_Checker:: 
LABSoft_Presenter_Digital_Circuit_Checker (LABSoft_Presenter& _LABSoft_Presenter)
 : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{
   gui ().digital_circuit_checker_fl_output_results->value ("-");
}

void LABSoft_Presenter_Digital_Circuit_Checker:: 
display_results (const LAB_Digital_Circuit_Checker::ScoreData score_data)
{
  char str[50];

  std::snprintf (str, sizeof (str), "%d/%d - %2.2f%%", score_data.score_current,
    score_data.score_total, score_data.score_percent);

  m_presenter.gui ().digital_circuit_checker_fl_output_results->value (str);
}

void LABSoft_Presenter_Digital_Circuit_Checker:: 
update_gui_reset ()
{
  LABSoft_GUI& gui = m_presenter.gui ();

  gui.digital_circuit_checker_labchecker_gui_digital_output_table_table->reset ();
  gui.digital_circuit_checker_fl_output_selected_file->value ("");
  gui.digital_circuit_checker_fl_output_results->value ("");
}

void LABSoft_Presenter_Digital_Circuit_Checker:: 
cb_load_file (Fl_Button*  w, 
              void*       data)
{
  Fl_Native_File_Chooser chooser;

  chooser.title   ("Open Circuit Checker File");
  chooser.type    (Fl_Native_File_Chooser::BROWSE_FILE);
  chooser.filter  ("LAB Circuit Checker\t*.labdcc");

  //

  Fl_Output& selected_file = *(m_presenter.gui ().digital_circuit_checker_fl_output_selected_file);

  //

  try 
  {
    switch (chooser.show ())
    {
      // user picked file
      case (0):
      {
        std::string path (chooser.filename ());

        if (LABF::has_filename_this_extension (path, 
          LABC::LABSOFT::DIGITAL_CIRCUIT_CHECKER_FILENAME_EXTENSION))
        {
          update_gui_reset ();

          selected_file.value (LABF::get_filename_from_path (path).c_str ());

          m_presenter.lab ().m_Digital_Circuit_Checker.load_file (path);
        }
        else 
        {
          throw (std::runtime_error ("Invalid file selected."));
        }

        break;
      }

      // user cancelled
      case (1):
      {
        break;
      }

      // open failed
      case (-1):
      {
        break;
      }
    }
  }
  catch (const std::exception& e)
  {
    fl_message (e.what ());

    selected_file.value ("");
  }
}

void LABSoft_Presenter_Digital_Circuit_Checker:: 
cb_unload_file (Fl_Button*  w, 
                void*       data)
{
  m_presenter.lab ().m_Digital_Circuit_Checker.unload_file ();

  update_gui_reset ();
}

void LABSoft_Presenter_Digital_Circuit_Checker:: 
cb_run_checker (Fl_Button*  w,
                void*       data)
{
  try
  {
    m_presenter.lab ().m_Digital_Circuit_Checker.run_checker ();

    if (m_presenter.lab ().m_Digital_Circuit_Checker.are_results_ready ())
    {
      LAB_Digital_Circuit_Checker::ScoreData score_data = m_presenter.lab ().
        m_Digital_Circuit_Checker.get_score_data ();

      // Update results Fl_Output widget
      display_results (score_data);

      // Update table
      m_presenter.gui ().digital_circuit_checker_labchecker_gui_digital_output_table_table->
        display_results (score_data);

      fl_message ("Checking done.");
    }    
  }
  catch (const std::exception& e)
  {
    fl_message (e.what ());
  }
}

// EOF