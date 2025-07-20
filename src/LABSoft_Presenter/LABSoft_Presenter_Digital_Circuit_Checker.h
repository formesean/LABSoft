#ifndef LABSOFT_PRESENTER_DIGITAL_CIRCUIT_CHECKER_H
#define LABSOFT_PRESENTER_DIGITAL_CIRCUIT_CHECKER_H

#include <string>

#include <FL/Fl_Button.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "LABSoft_Presenter_Unit.h"
#include "../LAB/LAB_Digital_Circuit_Checker.h"

class LABSoft_Presenter_Digital_Circuit_Checker : public LABSoft_Presenter_Unit
{
  private:
    void        display_results         (const LAB_Digital_Circuit_Checker::ScoreData score_data);
    void        update_gui_reset        (); 

  public:
    LABSoft_Presenter_Digital_Circuit_Checker (LABSoft_Presenter& _LABSoft_Presenter);

    void cb_load_file   (Fl_Button* w, void* data);
    void cb_unload_file (Fl_Button* w, void* data);
    void cb_run_checker (Fl_Button* w, void* data);
};

#endif

// EOF