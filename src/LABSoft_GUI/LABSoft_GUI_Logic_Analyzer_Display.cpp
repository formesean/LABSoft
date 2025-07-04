#include "LABSoft_GUI_Logic_Analyzer_Display.h"

#include <cmath>
#include <sstream>
#include <iostream>

#include "../Utility/LAB_Defaults.h"
#include "../Utility/LABSoft_GUI_Label.h"
#include "../Utility/LAB_Utility_Functions.h"
#include "../LABSoft_Presenter/LABSoft_Presenter.h"

Fl_Menu_Item ChanWidget::menu_m_fl_menu_button_trigger_mode[] = {
 {"X Ignore", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"0 Low", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"1 High", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"\342\226\262 Rise", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"\342\226\274 Fall", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {"\342\206\225 Edge", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

// ========== LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph ==========

LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph:: 
LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph (int X, int Y, int W, int H, const char* label)
  : Fl_Widget (X, Y, W, H, label)
{

}

LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph::
~LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph ()
{

}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph:: 
draw ()
{
  draw_box    (LOGAN_DISPLAY::GRAPH_BOX, LOGAN_DISPLAY::GRAPH_BG_COLOR);
  draw_signal ();
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph:: 
draw_signal ()
{
  std::vector<std::array<int, 2>>& pp = m_display_data->pixel_points[m_channel];

  if (m_display_data == nullptr || m_channel < 0 || pp.size () == 0)
  {
    return;
  }

  // =====

  fl_color (LOGAN_DISPLAY::GRAPH_LINE_COLOR);

  fl_line_style (
    LOGAN_DISPLAY::GRAPH_LINE_STYLE, 
    LOGAN_DISPLAY::GRAPH_LINE_WIDTH, 
    LOGAN_DISPLAY::GRAPH_LINE_DASHES
  );

  for (unsigned a = 0; a < (pp.size () - 1); a++)
  {
    fl_line (
      pp[a][0],
      pp[a][1] + m_graph_offset,
      pp[a + 1][0],
      pp[a + 1][1] + m_graph_offset
    );
  }

  fl_line_style (0);
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph:: 
load_display_data (DisplayData* display_data)
{
  m_display_data = display_data;
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph:: 
channel (int channel)
{
  m_channel = channel;
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph::
graph_offset (int offset)
{
  m_graph_offset = offset;
}

// ========== LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget ==========

LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget (int X, int Y, int W, int H, const char* label)
  : Fl_Group (X, Y, W, H, label)
{
  init_child_widgets ();
}

LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
~LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget ()
{

}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
init_child_widgets ()
{
  m_fl_group_channel_info = new Fl_Group (
    0, 
    0, 
    LOGAN_DISPLAY::CHANNEL_INFO_WIDTH, 
    LOGAN_DISPLAY::CHANNEL_HEIGHT
  );

  m_fl_group_channel_info->box    (FL_THIN_DOWN_BOX);
  m_fl_group_channel_info->color  (53);
  {
    {
      m_fl_box_dragger = new Fl_Button (
        0,
        0,
        LOGAN_DISPLAY::CHANNEL_DRAGGER_WIDTH,
        LOGAN_DISPLAY::CHANNEL_HEIGHT
      );

      m_fl_box_dragger->box (FL_GTK_UP_BOX);
      m_fl_box_dragger->color (53);
    }
    {
      m_fl_output_name = new Fl_Output (
        LOGAN_DISPLAY::CHANNEL_DRAGGER_WIDTH, 
        0, 
        LOGAN_DISPLAY::CHANNEL_NAME_WIDTH, 
        LOGAN_DISPLAY::CHANNEL_HEIGHT
      );
    }
    {
      m_fl_button_channel_info_setting = new Fl_Button  (
        LOGAN_DISPLAY::CHANNEL_DRAGGER_WIDTH +
        LOGAN_DISPLAY::CHANNEL_NAME_WIDTH, 
        0, 
        LOGAN_DISPLAY::CHANNEL_SETTING_WIDTH,
        LOGAN_DISPLAY::CHANNEL_HEIGHT
      );
      
      m_fl_button_channel_info_setting->box   (FL_GTK_UP_BOX);
      m_fl_button_channel_info_setting->color (53);
    }
    {
      m_fl_output_dio_pin = new Fl_Output (
        LOGAN_DISPLAY::CHANNEL_DRAGGER_WIDTH +
        LOGAN_DISPLAY::CHANNEL_NAME_WIDTH + 
        LOGAN_DISPLAY::CHANNEL_SETTING_WIDTH,
        0, 
        LOGAN_DISPLAY::CHANNEL_DIO_WIDTH, 
        LOGAN_DISPLAY::CHANNEL_HEIGHT
      );

      m_fl_output_dio_pin->color (79);
    }
    {
      m_fl_menu_button_trigger_mode = new Fl_Menu_Button  (
        LOGAN_DISPLAY::CHANNEL_DRAGGER_WIDTH +
        LOGAN_DISPLAY::CHANNEL_NAME_WIDTH + 
        LOGAN_DISPLAY::CHANNEL_SETTING_WIDTH + 
        LOGAN_DISPLAY::CHANNEL_DIO_WIDTH, 
        0, 
        LOGAN_DISPLAY::CHANNEL_TRIGGER_WIDTH, 
        LOGAN_DISPLAY::CHANNEL_HEIGHT, 
        "x"
      );

      m_fl_menu_button_trigger_mode->box      (FL_GTK_UP_BOX);
      m_fl_menu_button_trigger_mode->color    (53);
      m_fl_menu_button_trigger_mode->menu     (menu_m_fl_menu_button_trigger_mode);
      m_fl_menu_button_trigger_mode->callback (cb_fl_menu_button_trigger_mode_static, this);
    }
  }

  m_fl_group_channel_info->end ();  

  m_fl_widget_channel_graph = new LABSoft_GUI_Logic_Analyzer_Display_Channel_Graph (
    LOGAN_DISPLAY::CHANNEL_INFO_WIDTH,
    0, 
    LOGAN_DISPLAY::CHANNEL_GRAPH_WIDTH, 
    LOGAN_DISPLAY::CHANNEL_HEIGHT
  );

  m_fl_widget_channel_graph->box                (FL_THIN_DOWN_BOX);
  m_fl_widget_channel_graph->color              ((Fl_Color)53);
  m_fl_widget_channel_graph->load_display_data  (m_display_data);
  m_fl_widget_channel_graph->channel            (m_channel);

  end ();
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
draw ()
{
  draw_box ();
  draw_children ();
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
cb_fl_menu_button_trigger_mode (Fl_Widget* w, void* data)
{
  Fl_Menu_Button* menu_button = static_cast<Fl_Menu_Button*>(w);

  const char* new_label;
  const char* text = menu_button->text ();

  if (text == "X Ignore")
  {
    new_label = "X";
  }
  else if (text == "0 Low")
  {
    new_label = "0";
  }
  else if (text == "1 High")
  {
    new_label = "1";
  }
  else if (text == "\342\226\262 Rise")
  {
    new_label = "\342\226\262";
  }
  else if (text == "\342\226\274 Fall")
  {
    new_label = "\342\226\274";
  }
  else if (text == "\342\206\225 Edge")
  {
    new_label = "\342\206\225";
  }

  menu_button->copy_label (new_label);

  // parent hierarchy from fl menu button
  // m_fl_group_channel_info -> LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget ->
  // m_pack -> m_scroll -> Disp

  ChanWidget* chan = static_cast<ChanWidget*>(data);
  Disp*       disp = static_cast<Disp*>(chan->parent ()->parent ()->parent ());

  disp->presenter ().m_Logic_Analyzer.
    cb_trigger_condition (menu_button, static_cast<void*>(chan));
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
cb_fl_menu_button_trigger_mode_static (Fl_Widget* w, void* data)
{
  ChanWidget* chan_widget = static_cast<ChanWidget*>(data);

  chan_widget->cb_fl_menu_button_trigger_mode (w, data);
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
select ()
{
  m_is_selected = true;
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
deselect ()
{
  m_is_selected = false;
}

bool LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
selected ()
{
  return (m_is_selected);
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
load_display_data (DisplayData& display_data)
{
  m_display_data = &display_data;

  m_fl_widget_channel_graph->load_display_data (m_display_data);
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget::
channel (int channel)
{
  m_channel = channel;

  m_fl_widget_channel_graph->channel (m_channel);
}

int LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget:: 
channel () const
{
  return (m_channel);
}

void LABSoft_GUI_Logic_Analyzer_Display_Channel_Widget::
graph_offset (int offset)
{
  m_fl_widget_channel_graph->graph_offset (offset);
}

// ========== LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay ==========

LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay:: 
LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay (int X, int Y, int W, int H, const char* label)
  : Fl_Widget (X, Y, W, H, label)
{

}

LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay::
~LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay ()
{

}

void LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay:: 
draw ()
{
  draw_grid ();
}

void LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay:: 
draw_grid ()
{
  double col_width = static_cast<double>(w ()) / LABC::LOGAN::DISPLAY_NUMBER_OF_COLUMNS;

  fl_color (LOGAN_DISPLAY::GRID_COLOR);

  // Columns
  for (unsigned col = 0; col < LOGAN_DISPLAY::NUMBER_OF_COLUMNS; col++)
  {
    if (col == 0)
    {
      fl_line_style (FL_SOLID); 
    }
    else if (col == (LOGAN_DISPLAY::NUMBER_OF_COLUMNS / 2))
    {
      fl_line_style (FL_DASH);
    }
    else 
    {
      fl_line_style (FL_DOT);
    }

    fl_line (
      x () + (col * col_width), 
      y (), 
      x () + (col * col_width), 
      y () + h ()
    );
  }

  fl_line_style (0);
}

void LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay:: 
display_data (DisplayData& display_data)
{
  m_display_data = &display_data;
}

// ========== Disp ==========

LABSoft_GUI_Logic_Analyzer_Display::
LABSoft_GUI_Logic_Analyzer_Display (int X, int Y, int W, int H, const char* label)
  : Fl_Group (X, Y, W, H, label)
{
  init_child_widgets          ();
  reserve_pixel_points        ();
  calc_graph_base_line_coords ();
}

LABSoft_GUI_Logic_Analyzer_Display::
~LABSoft_GUI_Logic_Analyzer_Display ()
{

}

void LABSoft_GUI_Logic_Analyzer_Display:: 
init_child_widgets ()
{
  // ! The order of initialization here is important!
  // ! The order of initialization here is important!
  // ! The order of initialization here is important!

  // 1. fl_scroll
  m_scroll = new Fl_Scroll (
    x (),
    y () + LOGAN_DISPLAY::STATUS_HEIGHT,
    w (),
    h () - LOGAN_DISPLAY::STATUS_HEIGHT - 
      LOGAN_DISPLAY::TIME_PER_DIVISION_LABELS_STRIP_HEIGHT
  );
  
  m_scroll->type (Fl_Scroll::VERTICAL);
  {
    m_pack = new Fl_Pack (
      m_scroll->x (),
      m_scroll->y (),
      m_scroll->w (),
      m_scroll->h ()
    );

    m_pack->type (Fl_Pack::VERTICAL);
  }
  m_scroll->end ();

  m_overlay = new LABSoft_GUI_Logic_Analyzer_Display_Graph_Overlay (
    x () + LOGAN_DISPLAY::CHANNEL_INFO_WIDTH,
    y () + LOGAN_DISPLAY::STATUS_HEIGHT,
      LOGAN_DISPLAY::CHANNEL_GRAPH_WIDTH,
    h () - LOGAN_DISPLAY::STATUS_HEIGHT - 
      LOGAN_DISPLAY::TIME_PER_DIVISION_LABELS_STRIP_HEIGHT
  );

  m_overlay->display_data (m_display_data);

  // x. status
  init_child_widgets_status ();

  // x. time per division labels
  init_child_widgets_time_per_division_labels ();

  // x. top info
  init_child_widgets_top_info ();

  end ();
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
init_child_widgets_status ()
{
  m_status = new Fl_Box (
    m_overlay->x (),
    m_overlay->y () - LOGAN_DISPLAY::STATUS_HEIGHT,
    80,
    LOGAN_DISPLAY::STATUS_HEIGHT,
    "Ready"
  );

  m_status->box         (FL_BORDER_FRAME);
  m_status->color       (1);
  m_status->labelfont   (1);
  m_status->labelsize   (LOGAN_DISPLAY::AXIS_LABEL_SIZE);
  m_status->labelcolor  (FL_BLACK);
}

void LABSoft_GUI_Logic_Analyzer_Display::
init_child_widgets_time_per_division_labels ()
{
  unsigned  disp_internal_width = w () - LOGAN_DISPLAY::CHANNEL_INFO_WIDTH;
  double    col_width           = static_cast<double>(disp_internal_width) / 
                                  LABC::LOGAN::DISPLAY_NUMBER_OF_COLUMNS;

  for (unsigned col = 0; col <= m_time_per_division_labels.size (); col++)
  {
    double x_coord = x () + (LOGAN_DISPLAY::CHANNEL_INFO_WIDTH) + 
      (col * col_width);
    
    if (col == LABC::LOGAN::DISPLAY_NUMBER_OF_COLUMNS)
    {
      x_coord -= LOGAN_DISPLAY::TIME_PER_DIVSION_LABELS_LAST_OFFSET;
    }

    double y_coord = y () + h () - 
      LOGAN_DISPLAY::TIME_PER_DIVISION_LABELS_STRIP_HEIGHT + 
      LOGAN_DISPLAY::TIME_PER_DIVISION_LABELS_TOP_MARGIN;

    Fl_Box* box = new Fl_Box (
      x_coord,
      y_coord,
      5,
      5,
      "0.00 s"
    );

    box->labelcolor (LOGAN_DISPLAY::TIME_PER_DIVISION_LABELS_COLOR);
    box->labelsize  (LOGAN_DISPLAY::TIME_PER_DIVISION_LABELS_SIZE);
    box->align      (FL_ALIGN_TEXT_OVER_IMAGE);

    m_time_per_division_labels[col] = box;
  }
}

void LABSoft_GUI_Logic_Analyzer_Display::
init_child_widgets_top_info ()
{
  m_top_info = new Fl_Box (
    m_status->x () + m_status->w () + 10,
    m_status->y (),
    3,
    m_status->h (),
    "2000 samples"
  );

  m_top_info->box         (FL_NO_BOX);
  m_top_info->align       (FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  m_top_info->labelsize   (LOGAN_DISPLAY::AXIS_LABEL_SIZE);
  m_top_info->labelcolor  (FL_BLACK);
}

ChanWidget* LABSoft_GUI_Logic_Analyzer_Display::
create_channel_widget (unsigned channel, const char* name)
{
  ChanWidget* widget = new ChanWidget (
    0, 
    0, 
    LOGAN_DISPLAY::CHANNEL_INFO_WIDTH + m_display_data.graph_width,
    LOGAN_DISPLAY::CHANNEL_HEIGHT
  );

  char label[20];
  std::snprintf (label, sizeof (label), "DIO %d", channel);

  if (name == "\0" || name == nullptr)
  {
    widget->m_fl_output_name->value (label);
  }
  else 
  {
    widget->m_fl_output_name->value (name);
  }

  widget->m_fl_output_dio_pin->value  (label);
  widget->m_fl_output_dio_pin->color  (LOGAN_DISPLAY::CHANNEL_COLORS[channel]);
  widget->load_display_data           (m_display_data);
  widget->channel                     (channel);

  return (widget);
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
fill_pixel_points ()
{
  // if (m_parent_data->is_backend_running)
  // {
  //   fill_pixel_points_backend_running ();
  // }
  // else 
  // {
  //   fill_pixel_points_backend_stopped ();
  // }

  fill_pixel_points_backend_running ();
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
fill_pixel_points_backend_running ()
{
  LAB_Parent_Data_Logic_Analyzer& pdata = *m_parent_data;

  for (unsigned chan = 0; chan < pdata.channel_data.size (); chan++)
  {
    if (is_chan_present_in_chan_widget_array (chan))
    {
      LAB_Channel_Data_Logic_Analyzer& cdata  = pdata.channel_data[chan];
      std::vector<std::array<int, 2>>& pp     = m_display_data.pixel_points[chan];

      pp.clear ();

      bool  curr_samp, next_samp;
      int   next_x, i;

      if (pdata.samples >= m_display_data.graph_width)
      {
        double samp_skipper = (pdata.samples - 1) /
          static_cast<double>(m_display_data.graph_width - 1);
        
        for (i = 0; i < (m_display_data.graph_width - 1); i++)
        {
          curr_samp = cdata.samples[std::round (i * samp_skipper)];
          next_samp = cdata.samples[std::round ((i + 1) * samp_skipper)];
          next_x    = x () + LOGAN_DISPLAY::CHANNEL_INFO_WIDTH + i;

          calc_pp_coords (curr_samp, next_samp, next_x, i, pp);
        }
      }     
      else 
      {
        double pxl_skipper = static_cast<double>(m_display_data.graph_width - 1) / 
          (pdata.samples - 1);

        for (i = 0; i < (pdata.samples - 1); i++)
        {
          curr_samp = cdata.samples[i];
          next_samp = cdata.samples[i + 1];
          next_x    = x () + LOGAN_DISPLAY::CHANNEL_INFO_WIDTH + std::round ((i + 1) * pxl_skipper);

          calc_pp_coords (curr_samp, next_samp, next_x, i, pp);
        }
      }
    }
  }
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
fill_pixel_points_backend_stopped ()
{

}

void LABSoft_GUI_Logic_Analyzer_Display::
calc_pp_coords (bool      curr_samp, 
                bool      next_samp,
                int       next_x, 
                int       curr_index,
                std::vector<std::array<int, 2>>& pp)
{
  if (curr_index == 0)
  {
    pp.emplace_back (
      std::array<int, 2> {x () + LOGAN_DISPLAY::CHANNEL_INFO_WIDTH, 
        m_graph_base_line_coords[curr_samp]}
    );
  }
  else 
  {
    if (curr_samp == next_samp)
    {
      pp.emplace_back (
        std::array<int, 2> {next_x, m_graph_base_line_coords[next_samp]}
      );
    }
    else 
    {
      pp.emplace_back (
        std::array<int, 2> {next_x, m_graph_base_line_coords[next_samp ^ 1]}
      );

      pp.emplace_back (
        std::array<int, 2> {next_x, m_graph_base_line_coords[next_samp]}
      );
    }
  }
}

bool LABSoft_GUI_Logic_Analyzer_Display:: 
is_chan_present_in_chan_widget_array (unsigned channel) const
{
  for (const ChanWidget* w : m_channel_widgets)
  {
    if (channel == w->channel ())
    {
      return (true);
    }
  }

  return (false);
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
calc_graph_base_line_coords ()
{
  int main_y_offset = y () + LOGAN_DISPLAY::STATUS_HEIGHT + 
    (LOGAN_DISPLAY::CHANNEL_HEIGHT / 2);

  int graph_y_offset = ((LOGAN_DISPLAY::CHANNEL_HEIGHT * 
    LOGAN_DISPLAY::CHANNEL_GRAPH_PEAK_TO_PEAK_SPREAD) / (100.0)) / 2;

  m_graph_base_line_coords[0] = main_y_offset + graph_y_offset;
  m_graph_base_line_coords[1] = main_y_offset - graph_y_offset;
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
calc_chan_widget_graph_offset_last ()
{
  int graph_offset = (m_channel_widgets.size () - 1) * 
    LOGAN_DISPLAY::CHANNEL_HEIGHT;

  m_channel_widgets.back ()->graph_offset (graph_offset);
}

void LABSoft_GUI_Logic_Analyzer_Display::
reserve_pixel_points ()
{
  for (std::vector<std::array<int, 2>>& pp : m_display_data.pixel_points)
  {
    pp.reserve (LOGAN_DISPLAY::CHANNEL_GRAPH_WIDTH * 2);
  }
}

void LABSoft_GUI_Logic_Analyzer_Display::
update_gui_status ()
{
  switch (m_parent_data->status)
  {
    case (LABE::LOGAN::STATUS::READY):
    {
      m_status->copy_label ("Ready");
      m_status->color (1);

      break;
    }

    case (LABE::LOGAN::STATUS::STOP):
    {
      m_status->copy_label ("Stop");
      m_status->color (1);

      break;
    }

    case (LABE::LOGAN::STATUS::AUTO):
    {
      m_status->copy_label ("Auto");
      m_status->color (2);

      break;
    }

    case (LABE::LOGAN::STATUS::DONE):
    {
      m_status->copy_label ("Done");
      m_status->color (1);
    }

    case (LABE::LOGAN::STATUS::CONFIG):
    {
      m_status->copy_label ("Config");
      m_status->color (3);
    }
  }
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
update_gui_top_info ()
{
  LABSoft_GUI_Label lbl (m_parent_data->sampling_rate, LABSoft_GUI_Label::UNIT::HERTZ);

  std::stringstream ss;

  ss  << m_parent_data->samples 
      << " samples at "
      << lbl.to_text ()
      << " | "
      << LABF::get_now_timestamp ();

  m_top_info->copy_label (ss.str ().c_str ());
}

void LABSoft_GUI_Logic_Analyzer_Display::
draw ()
{
  if (m_parent_data->is_backend_running)
  {
    update_gui_top_info ();
  }

  update_gui_status ();

  draw_box      (FL_FLAT_BOX, LOGAN_DISPLAY::BG_COLOR);
  draw_children ();
  draw_box      (FL_BORDER_FRAME, 0);
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
load_parent_data (LAB_Parent_Data_Logic_Analyzer& pdata)
{
  m_parent_data = &pdata;

  update_gui_time_per_division ();
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
load_presenter (LABSoft_Presenter& presenter)
{
  m_LABSoft_Presenter = &presenter;
}

LABSoft_Presenter& LABSoft_GUI_Logic_Analyzer_Display:: 
presenter () const 
{
  return (*m_LABSoft_Presenter);
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
add_channel (unsigned channel, const char* name)
{
  ChanWidget* w = create_channel_widget (channel, name);
  
  m_pack->add (w);
  m_channel_widgets.push_back (w);

  calc_chan_widget_graph_offset_last ();

  redraw ();
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
clear_all_channels ()
{
  for (ChanWidget* ptr : m_channel_widgets)
  {
    delete ptr;
  }

  m_channel_widgets.clear ();

  redraw ();
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
update_gui_time_per_division ()
{
  double col_half = (LABC::OSC::DISPLAY_NUMBER_OF_COLUMNS / 2.0) * -1;

  for (unsigned a = 0; a < m_time_per_division_labels.size (); a++)
  {
    double col_tpd = (a + col_half) * (m_parent_data->time_per_division) + 
      (m_parent_data->horizontal_offset);

    LABSoft_GUI_Label lbl (col_tpd, LABSoft_GUI_Label::UNIT::SECOND);
    
    m_time_per_division_labels[a]->copy_label (lbl.to_text ().c_str ());
  }
}

void LABSoft_GUI_Logic_Analyzer_Display:: 
update_display ()
{
  fill_pixel_points ();

  redraw ();
}

// EOF