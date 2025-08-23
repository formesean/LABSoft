//student side
#include "LABSoft_Presenter_Analog_Circuit_Checker.h"

#include <FL/fl_ask.H>

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LAB_Utility_Functions.h"

LABSoft_Presenter_Analog_Circuit_Checker::LABSoft_Presenter_Analog_Circuit_Checker(LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit(_LABSoft_Presenter)
{
  load_gui();
}

void LABSoft_Presenter_Analog_Circuit_Checker::load_gui()
{
  LABSoft_GUI_LABChecker_Analog_Checker_Display& analog_disp_gui = *(gui ().analog_labsoft_gui_analog_checker_display);

   analog_disp_gui.load_presenter         (m_presenter);
  
}

void LABSoft_Presenter_Analog_Circuit_Checker::
display_signals ()
{
  
}

void LABSoft_Presenter_Analog_Circuit_Checker::
update_gui_display () 
{
  LABSoft_GUI& gui = m_presenter.gui ();

  gui.analog_labsoft_gui_analog_checker_display; // to be edited. 
}

void LABSoft_Presenter_Analog_Circuit_Checker::
cb_load_file_analog (Fl_Button*  w, 
                     void*       data)
{
  Fl_Native_File_Chooser chooser;

  chooser.title   ("Open Analog Circuit Checker Template File");
  chooser.type    (Fl_Native_File_Chooser::BROWSE_FILE);
  chooser.filter  ("LAB Analog Circuit Checker\t*.labacc");

  Fl_Output& selected_file = *(m_presenter.gui ().analog_circuit_checker_fl_output_selected_file);

  try
  {
    switch (chooser.show ())
    {
      case (0):
      {
        std::string path (chooser.filename ());

        if (LABF::has_filename_this_extension(path,
          LABC::LABSOFT::ANALOG_CIRCUIT_CHECKER_FILENAME_EXTENSION))
          {
            update_gui_display ();

            selected_file.value (LABF::get_filename_from_path(path).c_str());

            m_presenter.lab ().m_Analog_Circuit_Checker.load_file (path);
          }
      }
    }
  }
  catch(const std::exception& e)
  {
    
  }
  
}
  
