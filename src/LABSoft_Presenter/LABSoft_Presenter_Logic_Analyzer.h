#ifndef LABSOFT_PRESENTER_LOGIC_ANALYZER_H
#define LABSOFT_PRESENTER_LOGIC_ANALYZER_H

#include <thread>

#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Light_Button.H>

#include "LABSoft_Presenter_Unit.h"
#include "../LABSoft_GUI/LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window.h"

class LABSoft_Presenter_Logic_Analyzer : public LABSoft_Presenter_Unit
{
  private:
    std::thread* m_thread_update_display;
  
  private:
    void init             ();
    void init_gui_values  ();
    
  public:
    LABSoft_Presenter_Logic_Analyzer (LABSoft_Presenter& _LABSoft_Presenter);

    // master controls
    void cb_run_stop              (Fl_Light_Button* w, void* data);
    void cb_single                (Fl_Button*       w, void* data);

    // horizontal
    void cb_horizontal_offset     (Fl_Input_Choice* w, void* data);
    void cb_time_per_division     (Fl_Input_Choice* w, void* data);
    void cb_samples               (Fl_Input_Choice* w, void* data);
    void cb_sampling_rate         (Fl_Input_Choice* w, void* data);
    
    // Trigger
    void cb_trigger_mode          (Fl_Choice* w, void* data);
    void cb_trigger_condition     (Fl_Menu_Button* w, void *data);

    // Mode
    void cb_mode                  (Fl_Choice* w, void* data);

    // Display update cycle
    void display_update_cycle     ();
    void update_gui_horizontal    ();
    void update_gui_mode          ();

    void cb_add_channel_selection (Fl_Menu_* w, void* data);
    void cb_add_channel_signal    (LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window* w, void* data);
    void cb_clear_channels        (Fl_Menu_* w, void* data);
};

#endif