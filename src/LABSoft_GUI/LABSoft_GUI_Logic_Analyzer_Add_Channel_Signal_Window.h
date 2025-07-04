#ifndef LABSOFT_GUI_LOGIC_ANALYZER_ADD_CHANNEL_SIGNAL_WINDOW
#define LABSOFT_GUI_LOGIC_ANALYZER_ADD_CHANNEL_SIGNAL_WINDOW

#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multi_Browser.H>

class LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window : public Fl_Window
{
  public:
    Fl_Input*         m_name;
    Fl_Button*        m_cancel;
    Fl_Multi_Browser* m_multi_browser;
    Fl_Button*        m_add;

  private:
    void init_children_guis ();
    void init_this_gui ();
    void hide_as_modal ();
    void reset_state ();
    void cb_multi_browser (Fl_Multi_Browser* w, void* data);
    void cb_add (Fl_Button* w, void* data);
    void cb_cancel (Fl_Button* w, void* data);
    void cb_window_close (LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window* w, void* data);
    
    static void cb_multi_browser_static (Fl_Multi_Browser* w, void* data);
    static void cb_add_static (Fl_Button* w, void* data);
    static void cb_cancel_static (Fl_Button* w, void* data);
    static void cb_window_close_static (LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window* w, void* data);

  public:
    LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window (int X, int Y, const char* label = 0);

    void show_as_modal ();
    void number_of_channels (unsigned value);
};

#endif 