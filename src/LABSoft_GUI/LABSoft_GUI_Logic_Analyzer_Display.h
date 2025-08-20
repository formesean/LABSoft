#ifndef LABSOFT_GUI_LOGIC_ANALYZER_DISPLAY_H
#define LABSOFT_GUI_LOGIC_ANALYZER_DISPLAY_H

#include <array>
#include <vector>
#include <functional>

#include <FL/Fl_Pack.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Button.H>

#include "../Utility/LAB_Definitions.h"
#include "../Utility/LAB_Defaults.h"

class LABSoft_Presenter;

using ChanGraph   = class LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph;
using ChanWidget  = class LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget;
using DispOverlay = class LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay;
using Disp        = class LABSoft_GUI_Logic_Analyzer_Display;

using PixelPoints = std::array<std::vector<std::array<int, 2>>, 
                      LABC::LOGAN::NUMBER_OF_CHANNELS>;

namespace LOGAN_DISPLAY
{
  constexpr int       CHANNEL_HEIGHT                         = 60;
  constexpr int       CHANNEL_DRAGGER_WIDTH                  = 20;
  constexpr int       CHANNEL_NAME_WIDTH                     = 90;
  constexpr int       CHANNEL_SETTING_WIDTH                  = 30;
  constexpr int       CHANNEL_DIO_WIDTH                      = 60;
  constexpr int       CHANNEL_TRIGGER_WIDTH                  = 40;
  constexpr int       CHANNEL_INFO_WIDTH                     = CHANNEL_DRAGGER_WIDTH + 
                                                                CHANNEL_NAME_WIDTH + 
                                                                CHANNEL_SETTING_WIDTH + 
                                                                CHANNEL_DIO_WIDTH + 
                                                                CHANNEL_TRIGGER_WIDTH;

  constexpr int DISPLAY_WIDTH = 1180;
  constexpr int CHANNEL_WIDTH = DISPLAY_WIDTH;
  constexpr int CHANNEL_GRAPH_WIDTH = CHANNEL_WIDTH - CHANNEL_INFO_WIDTH;
  constexpr unsigned AXIS_LABEL_SIZE = 12;

  constexpr unsigned  STATUS_HEIGHT                 = 20; 
  constexpr unsigned  DISPLAY_STATUS_BOX_HEIGHT             = STATUS_HEIGHT;
  constexpr unsigned  DISPLAY_STATUS_BOX_WIDTH              = 90;
  constexpr unsigned  CHANNEL_BUTTON_HEIGHT                 = 60;
  constexpr unsigned  CHANNEL_BUTTON_WIDTH                  = 180;
  constexpr unsigned  INTERNAL_DISPLAY_BG_COLOR             = FL_WHITE;

  constexpr unsigned  TIME_PER_DIVISION_LABELS_STRIP_HEIGHT = 30;
  constexpr unsigned  TIME_PER_DIVISION_LABELS_TOP_MARGIN   = 5;
  constexpr unsigned  TIME_PER_DIVISION_LABELS_COLOR        = FL_FOREGROUND_COLOR;
  constexpr unsigned  TIME_PER_DIVISION_LABELS_SIZE         = 10;
  constexpr unsigned  TIME_PER_DIVSION_LABELS_LAST_OFFSET   = 20;

  static constexpr uint32_t CHANNEL_COLORS[] = 
  {
    212, // light pink
    60, // green
    152, // violet
    122 // brick?
  };

  
  constexpr double    CHANNEL_GRAPH_PEAK_TO_PEAK_SPREAD       = 60.0; // in percent, with the graph widget height as max

  constexpr Fl_Boxtype  GRAPH_BOX         = FL_THIN_DOWN_BOX;
  constexpr unsigned    GRAPH_BG_COLOR    = 7;
  constexpr unsigned    GRAPH_LINE_COLOR  = FL_BLACK;
  constexpr int         GRAPH_LINE_STYLE  = FL_SOLID;
  constexpr int         GRAPH_LINE_WIDTH         = 2;
  constexpr char*       GRAPH_LINE_DASHES = 0;

  constexpr unsigned  NUMBER_OF_COLUMNS        = 10;
  constexpr int       BG_COLOR                 = FL_WHITE;
  constexpr int       GROUP_NUMBER_OF_CHANNELS = LABC::LOGAN::NUMBER_OF_CHANNELS;
  constexpr int       GRID_COLOR               = FL_BLACK;

  constexpr int       STATUS_WIDTH                          = 90;
};

struct DisplayData
{
  PixelPoints pixel_points;
  unsigned    graph_width = LOGAN_DISPLAY::CHANNEL_GRAPH_WIDTH;
};

// ========== LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph ==========

class LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph : public Fl_Widget
{
  private:
    int           m_channel       = -1;
    int           m_graph_offset  = 0;
    DisplayData*  m_display_data  = nullptr;
    
  private:
    void draw               ();
    void draw_signal        ();
    void calc_graph_offset  ();
  
  public:
    LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph (int X, int Y, int W, int H, const char* label = 0);
   ~LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph ();
   
    void load_display_data  (DisplayData* display_data);
    void channel            (int channel);
    void graph_offset       (int offset);
};

// ========== LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget ==========

class LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget : public Fl_Group
{
  private:
    bool            m_is_selected   = false;
    int             m_channel       = -1;
    DisplayData*    m_display_data  = nullptr;

  public:
    Fl_Group*                                         m_fl_group_channel_info;
    Fl_Button*                                        m_fl_box_dragger;
    Fl_Output*                                        m_fl_output_name;
    Fl_Button*                                        m_fl_button_channel_info_setting;
    Fl_Output*                                        m_fl_output_dio_pin;
    Fl_Menu_Button*                                   m_fl_menu_button_trigger_mode;
    LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph* m_fl_widget_channel_graph;
    
    static Fl_Menu_Item menu_m_fl_menu_button_trigger_mode[];

  private:
    void init_child_widgets ();
    void draw ();
    void cb_fl_menu_button_trigger_mode (Fl_Widget* w, void* data);

    static void cb_fl_menu_button_trigger_mode_static (Fl_Widget* w, void* data);

  public:
    LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget (int X, int Y, int W, int H, const char* label = 0);
   ~LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget ();

    void  select            ();
    void  deselect          ();
    bool  selected          ();
    void  load_display_data (DisplayData& display_data);
    void  channel           (int channel);
    int   channel           () const;
    void  graph_offset      (int offset);
};

// ========== LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay ==========

class LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay : public Fl_Widget 
{
  private:
    DisplayData* m_display_data;

  private:
    void draw ();
    void draw_grid ();

  public:
    LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay (int X, int Y, int W, int H, const char* label = 0);
   ~LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay ();

   void display_data (DisplayData& display_data);
};

// ========== LABSoft_GUI_Logic_Analyzer_Display ==========

class LABSoft_GUI_Logic_Analyzer_Display : public Fl_Group
{
  private:
    // data
    DisplayData                     m_display_data;
    LAB_Parent_Data_Logic_Analyzer* m_parent_data;
    LABSoft_Presenter*             m_LABSoft_Presenter;

    // gui
    Fl_Pack*                        m_pack;           
    DispOverlay*                    m_overlay;
    Fl_Scroll*                      m_scroll;  
    std::vector<ChanWidget*>        m_channel_widgets;
    std::array<int, 2>              m_graph_base_line_coords;
    Fl_Box*                         m_status;
    Fl_Box*                         m_top_info;
    
    std::array<
      Fl_Box*,
      LABC::LOGAN::DISPLAY_NUMBER_OF_COLUMNS
    > m_time_per_division_labels;

  private:
    void        create_channel_widget                       ();
    ChanWidget* create_channel_widget                       (unsigned channel, const char* name);
    void        draw                                        ();
    void        calc_graph_base_line_coords                 ();
    void        calc_pp_coords                              (bool curr_samp, bool next_samp, int next_x, int curr_index, std::vector<std::array<int, 2>>& pixel_points);
    void        calc_chan_widget_graph_offset_last          ();
    void        fill_pixel_points                           ();
    void        fill_pixel_points_backend_running           ();
    void        fill_pixel_points_backend_stopped           ();
    void        init_child_widgets                          ();
    void        init_child_widgets_status                   ();
    void        init_child_widgets_time_per_division_labels ();
    void        init_child_widgets_top_info                 ();
    bool        is_chan_present_in_chan_widget_array        (unsigned channel) const;
    void        reserve_pixel_points                        ();
    void        update_gui_status                           ();
    void        update_gui_top_info                         ();

  public:
    LABSoft_GUI_Logic_Analyzer_Display (int X, int Y, int W, int H, const char* label = 0);
   ~LABSoft_GUI_Logic_Analyzer_Display ();

    void load_parent_data             (LAB_Parent_Data_Logic_Analyzer& pdata);
    void load_presenter              (LABSoft_Presenter& presenter);
    LABSoft_Presenter& presenter    () const;
    void add_channel                  (unsigned channel, const char* name = 0);
    void clear_all_channels           ();
    void update_gui_time_per_division ();
    void update_display               ();
};

#endif