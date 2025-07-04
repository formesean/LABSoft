#include "LABSoft_GUI_Oscilloscope_Display.h"

#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "../Utility/LABSoft_GUI_Label.h"
#include "../Utility/LAB_Utility_Functions.h"
#include "../LABSoft_Presenter/LABSoft_Presenter.h"
#include "LABSoft_GUI_Oscilloscope_Internal_Display.h"
#include "../Utility/LABSoft_GUI_Label_Values.h"

LABSoft_GUI_Oscilloscope_Display:: 
LABSoft_GUI_Oscilloscope_Display (int         X, 
                                  int         Y, 
                                  int         W, 
                                  int         H, 
                                  const char* label)
  : Fl_Group (X, Y, W, H, label)
{
  init_child_widgets  ();
}

void LABSoft_GUI_Oscilloscope_Display:: 
load_presenter (const LABSoft_Presenter& presenter)
{
  m_presenter = &presenter;

  m_internal_display->load_presenter (presenter);
}

void LABSoft_GUI_Oscilloscope_Display:: 
load_pixel_points (const LABSoft_GUI_Oscilloscope_Display::PixelPoints& pixel_points)
{
  m_internal_display->load_pixel_points (pixel_points);
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets ()
{
  // ! The order of initialization here is important!
  // ! The order of initialization here is important!
  // ! The order of initialization here is important!

  init_child_widgets_internal_display             ();
  init_child_widgets_sliders                      ();  
  init_child_widgets_status                       ();
  init_child_widgets_voltage_per_division_labels  ();
  init_child_widgets_time_per_division_labels     ();
  init_child_widgets_channel_selectors            ();
  init_child_widgets_top_info                     ();

  end ();
}

void LABSoft_GUI_Oscilloscope_Display:: 
draw ()
{
  draw_box      (FL_THIN_DOWN_BOX, LABC::OSC_DISPLAY::BACKGROUND_COLOR);
  draw_children ();
}

// update gui functions

void LABSoft_GUI_Oscilloscope_Display:: 
update_display ()
{
  m_internal_display->update_display ();
}

void LABSoft_GUI_Oscilloscope_Display::
trigger_source (unsigned channel)
{

}

void LABSoft_GUI_Oscilloscope_Display:: 
trigger_channel (unsigned channel)
{
  m_selected_channel = channel;

  for (unsigned a = 0; a < m_channel_selectors.size (); a++)
  {
    (a == channel) ?  m_channel_selectors[a]->set   () : 
                      m_channel_selectors[a]->clear ();
  }

  update_gui_vertical_offset_slider ();
}

void LABSoft_GUI_Oscilloscope_Display::
trigger_level (double value)
{

}

void LABSoft_GUI_Oscilloscope_Display:: 
status (LABE::OSC::STATUS status)
{
  m_status->copy_label (LABS_GUI_VALUES::OSC::STATUS_s.at (status).c_str ());

  switch (status)
  {
    case (LABE::OSC::STATUS::READY):
    case (LABE::OSC::STATUS::STOP):
    case (LABE::OSC::STATUS::DONE):
    {
      m_status->color (1);
      break;
    }

    case (LABE::OSC::STATUS::AUTO):
    {
      m_status->color (2);

      break;
    }

    case (LABE::OSC::STATUS::CONFIG):
    {
      m_status->color (3);
    }
  }
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets_internal_display ()
{
  m_internal_display = new LABSoft_GUI_Oscilloscope_Internal_Display (
    x () + LABC::OSC_DISPLAY::INTERNAL_DISPLAY_LEFT_MARGIN,
    y () + LABC::OSC_DISPLAY::INTERNAL_DISPLAY_TOP_MARGIN, 
    w () - LABC::OSC_DISPLAY::INTERNAL_DISPLAY_LEFT_MARGIN - 
      LABC::OSC_DISPLAY::INTERNAL_DISPLAY_RIGHT_MARGIN,
    h () - LABC::OSC_DISPLAY::INTERNAL_DISPLAY_TOP_MARGIN - 
      LABC::OSC_DISPLAY::INTERNAL_DISPLAY_BOTTOM_MARGIN
  );
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets_sliders ()
{
  const LABSoft_GUI_Oscilloscope_Internal_Display& disp = *m_internal_display;
   
  // 2.1
  m_vertical_offset_slider = new LABSoft_GUI_Fl_Slider (disp.x (), disp.y (), LABC::OSC_DISPLAY::SLIDER_WIDTH, disp.h ());
  m_vertical_offset_slider->box    (FL_NO_BOX);
  m_vertical_offset_slider->color  (3);
  m_vertical_offset_slider->bounds (0, disp.h ());
  m_vertical_offset_slider->value  (disp. h () / 2.0);
  m_vertical_offset_slider->step   (disp.h (), LABC::OSC_DISPLAY::NUMBER_OF_ROWS * LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS);

  // 2.2 trigger level
  m_trigger_level_slider = new LABSoft_GUI_Fl_Slider (disp.x () + disp.w () - LABC::OSC_DISPLAY::SLIDER_WIDTH, 
    disp.y (), LABC::OSC_DISPLAY::SLIDER_WIDTH, disp.h ());
  m_trigger_level_slider->box       (FL_NO_BOX);
  m_trigger_level_slider->color     (3);
  m_trigger_level_slider->bounds    (0, disp.h ());
  m_trigger_level_slider->value     (disp.h () / 2.0);
  m_trigger_level_slider->step      (disp.h (), LABC::OSC_DISPLAY::NUMBER_OF_ROWS * LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS);
  m_trigger_level_slider->callback  (cb_trigger_level_static);  

  // 2.3 horizontal offset
  m_horizontal_offset_slider = new LABSoft_GUI_Fl_Slider (disp.x (), disp.y (), disp.w (), LABC::OSC_DISPLAY::SLIDER_WIDTH);
  m_horizontal_offset_slider->type   (1); // horizontal slider
  m_horizontal_offset_slider->box    (FL_NO_BOX);
  m_horizontal_offset_slider->color  (3);
  m_horizontal_offset_slider->bounds (0, disp.w ());
  m_horizontal_offset_slider->value  (disp. w () / 2.0);
  m_horizontal_offset_slider->step   (disp.w (), LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS * LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS);

  // remove for now para di mag conflict sa v offset ug trig level sliders
  m_horizontal_offset_slider->deactivate ();
  m_horizontal_offset_slider->hide ();
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets_status ()
{
  m_status = new Fl_Box (
    m_internal_display->x (),
    m_internal_display->y () - LABC::OSC_DISPLAY::STATUS_HEIGHT,
    LABC::OSC_DISPLAY::STATUS_WIDTH,
    LABC::OSC_DISPLAY::STATUS_HEIGHT,
    "Ready"
  );

  m_status->box         (FL_BORDER_FRAME);
  m_status->color       (1);
  m_status->labelfont   (1);
  m_status->labelsize   (LABC::OSC_DISPLAY::TOP_INFO_LABEL_SIZE);
  m_status->labelcolor  (FL_WHITE);
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets_voltage_per_division_labels ()
{
  for (unsigned chan = 0; chan < m_voltage_per_division_labels.size (); chan++)
  {
    int X = 
      m_internal_display->x () - 
      (LABC::OSC_DISPLAY::Y_AXIS_LABEL_HORIZONTAL_INTRASPACE *
      m_voltage_per_division_labels.size ()) + 
      (LABC::OSC_DISPLAY::Y_AXIS_LABEL_HORIZONTAL_INTRASPACE *
      chan);
    
    for (unsigned row = 0; row < m_voltage_per_division_labels[chan].size (); row++)
    {
      int Y = std::round (m_internal_display->y () + (row * m_internal_display->row_height ()));

      Fl_Box* box = new Fl_Box (X, Y, 3, 20, "0");

      box->labelcolor (LABC::OSC_DISPLAY::CHANNEL_COLORS[chan]);
      box->labelsize  (LABC::OSC_DISPLAY::AXIS_LABEL_SIZE);
      box->align      (FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);

      m_voltage_per_division_labels[chan][row] = box;
    }

    m_voltage_per_division_labels[chan].front ()->align (FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);
    m_voltage_per_division_labels[chan].back ()->align  (FL_ALIGN_TOP_LEFT);

    init_child_widgets_voltage_per_division_units (X, chan);
  }
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets_voltage_per_division_units (int      X,
                                               unsigned chan)
{
  char label[20];
  std::snprintf (label, sizeof (label), "C%d mV", chan + 1);

  Fl_Box* box = new Fl_Box (X, m_internal_display->y (), 3, 20, "C0 mV");

  box->copy_label (label);
  box->labelcolor (LABC::OSC_DISPLAY::CHANNEL_COLORS[chan]);
  box->labelsize  (LABC::OSC_DISPLAY::AXIS_LABEL_SIZE);
  box->align      (FL_ALIGN_TOP_LEFT);

  m_voltage_per_division_units[chan] = box;
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets_time_per_division_labels ()
{
  for (unsigned a = 0; a < m_time_per_division_labels.size (); a++)
  {
    Fl_Box* box = new Fl_Box (
      m_internal_display->x () + (a * m_internal_display->column_width ()),
      m_internal_display->y () + m_internal_display->h (),
      3,
      20
    );

    box->labelcolor (LABC::OSC_DISPLAY::X_AXIS_LABEL_COLOR);
    box->labelsize  (LABC::OSC_DISPLAY::AXIS_LABEL_SIZE);

    m_time_per_division_labels[a] = box;
  }

  m_time_per_division_labels.back ()->align (FL_ALIGN_LEFT);
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets_channel_selectors ()
{
  for (unsigned a = 0; a < m_channel_selectors.size (); a++)
  {
    Fl_Button* butt = new Fl_Button (
      m_status->x () + m_status->w () + (a * LABC::OSC_DISPLAY::STATUS_HEIGHT),
      m_status->y (),
      LABC::OSC_DISPLAY::STATUS_HEIGHT,
      LABC::OSC_DISPLAY::STATUS_HEIGHT
    );

    char label[20];
    std::snprintf (label, sizeof (label), "C%d", a + 1);

    butt->box              (FL_THIN_UP_BOX);
    butt->color            (FL_BLACK);
    butt->selection_color  (FL_BLACK);
    butt->labelsize        (LABC::OSC_DISPLAY::AXIS_LABEL_SIZE);
    butt->labelcolor       (LABC::OSC_DISPLAY::CHANNEL_COLORS[a]);
    butt->copy_label       (label);
    butt->user_data        (reinterpret_cast<void*>(a));
    butt->callback         (cb_channel_selector_static);

    m_channel_selectors[a] = butt;
  }
}

void LABSoft_GUI_Oscilloscope_Display:: 
init_child_widgets_top_info ()
{
  m_top_info = new Fl_Box (
    m_status->x () + m_status->w () + 10 + 
      (LABC::OSC_DISPLAY::STATUS_HEIGHT * m_channel_selectors.size ()),
    m_status->y (),
    3,
    m_status->h ()
  );

  m_top_info->box         (FL_NO_BOX);
  m_top_info->align       (FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  m_top_info->labelsize   (LABC::OSC_DISPLAY::TOP_INFO_LABEL_SIZE);
  m_top_info->labelcolor  (FL_WHITE);
}

void LABSoft_GUI_Oscilloscope_Display:: 
cb_trigger_level_static (Fl_Widget* w,
                         void*      data)
{
  LABSoft_GUI_Oscilloscope_Display* disp = 
    static_cast<LABSoft_GUI_Oscilloscope_Display*>(w->parent ());
}

void LABSoft_GUI_Oscilloscope_Display:: 
cb_channel_selector_static (Fl_Widget* w,
                            void*      data)
{
  LABSoft_GUI_Oscilloscope_Display& disp = 
    *(static_cast<LABSoft_GUI_Oscilloscope_Display*>(w->parent ()));

  unsigned channel = reinterpret_cast<int>(data);

  // disp.select_channel (channel);
}

void LABSoft_GUI_Oscilloscope_Display:: 
update_gui_voltage_per_division_labels ()
{
  for (unsigned chan = 0; chan < m_voltage_per_division_labels.size (); chan++)
  {
    update_gui_voltage_per_division_labels (chan);
  }
}

void LABSoft_GUI_Oscilloscope_Display:: 
update_gui_voltage_per_division_labels (unsigned channel)
{
  std::array<Fl_Box*, LABC::OSC_DISPLAY::NUMBER_OF_ROWS + 1>& labels = 
    m_voltage_per_division_labels.at (channel);
  
  for (unsigned row = 0; row < labels.size (); row++)
  {
    double row_vpd = calc_row_voltage_per_division (channel, row);

    LABSoft_GUI_Label label (row_vpd);

    char w_label[20];
    std::snprintf (w_label, sizeof (w_label), "%3.3f", label.actual_value ());      

    labels[row]->copy_label (w_label);
    labels[row]->show ();

    if (row == 0)
    { 
      std::snprintf (w_label, sizeof (w_label), "C%d %sV", channel + 1,
        label.unit_prefix ().c_str ());

      Fl_Box& w = *(m_voltage_per_division_units[channel]);

      w.show        ();
      w.copy_label  (w_label);
    }
  }
}

void LABSoft_GUI_Oscilloscope_Display:: 
update_gui_time_per_division_labels ()
{
  for (unsigned col = 0; col < m_time_per_division_labels.size (); col++)
  {
    double col_tpd = calc_col_time_per_division (col);

    LABSoft_GUI_Label label (col_tpd, LABSoft_GUI_Label::UNIT::SECOND);
    
    m_time_per_division_labels[col]->copy_label (label.to_text ().c_str ());
  }
}

void LABSoft_GUI_Oscilloscope_Display:: 
update_gui_top_info ()
{
  LABSoft_GUI_Label label (m_sampling_rate, LABSoft_GUI_Label::UNIT::HERTZ);

  std::stringstream ss;

  ss  << m_samples 
      << " samples at "
      << label.to_text ()
      << " | "
      << LABF::get_now_timestamp ();
  
  m_top_info->copy_label (ss.str ().c_str ());
}

void LABSoft_GUI_Oscilloscope_Display:: 
update_gui_vertical_elements ()
{
  // update_gui_voltage_per_division   ();
  // update_gui_trigger_level_slider   ();
  // update_gui_vertical_offset_slider ();
}

void LABSoft_GUI_Oscilloscope_Display:: 
update_gui_horizontal_elements ()
{
  // update_gui_time_per_division ();
}

void LABSoft_GUI_Oscilloscope_Display:: 
update_gui_trigger_level_slider ()
{
  // unsigned trigger_source = m_osc->trigger_source ();

  // // 1. update color
  // m_trigger_level_slider->selection_color 
  //   (LABC::OSC_DISPLAY::CHANNEL_COLORS[trigger_source]);
  
  // // 2. calc values
  // double max_tpd_raw  = m_osc->chan_voltage_per_division (trigger_source) * 
  //   LABC::OSC_DISPLAY::NUMBER_OF_ROWS_HALF;
  // double min_tpd_raw  = -max_tpd_raw;
  // double max_bounds   = max_tpd_raw - m_osc->vertical_offset (trigger_source);
  // double min_bounds   = min_tpd_raw - m_osc->vertical_offset (trigger_source);

  // // 3. set values
  // // if fl_slider is vertical, minimum is above, max is below
  // m_trigger_level_slider->bounds  (max_bounds, min_bounds);
  // m_trigger_level_slider->value   (m_osc->trigger_level ());
}

void LABSoft_GUI_Oscilloscope_Display:: 
update_gui_vertical_offset_slider ()
{
  // if (m_osc->has_enabled_channels ())
  // {
  //   unsigned trigger_source = m_osc->trigger_source ();
    
  //   m_vertical_offset_slider->show ();
  //   m_vertical_offset_slider->selection_color 
  //     (LABC::OSC_DISPLAY::CHANNEL_COLORS[m_selected_channel]);
    
  //   // 2. calc values
  //   double max_tpd_raw  = m_osc->chan_voltage_per_division (trigger_source) * 
  //     LABC::OSC_DISPLAY::NUMBER_OF_ROWS_HALF;
  //   double min_tpd_raw  = -max_tpd_raw;
  //   double max_bounds   = max_tpd_raw - m_osc->vertical_offset (trigger_source);
  //   double min_bounds   = min_tpd_raw - m_osc->vertical_offset (trigger_source);

  //   // 3. set values
  //   // if fl_slider is vertical, minimum is above, max is below
  //   m_vertical_offset_slider->bounds  (max_bounds, min_bounds);
  //   m_vertical_offset_slider->value   (m_osc->trigger_level ());
  // }
}

double LABSoft_GUI_Oscilloscope_Display:: 
calc_row_voltage_per_division (unsigned channel,
                               unsigned row)
{
  int index = static_cast<int>(row) - 
              static_cast<int>(LABC::OSC_DISPLAY::NUMBER_OF_ROWS_HALF);
  
  double row_vpd = (-index * m_voltage_per_division[channel]) - m_vertical_offset[channel];
  
  return (row_vpd);
}

double LABSoft_GUI_Oscilloscope_Display:: 
calc_col_time_per_division (unsigned col)
{
  int index = static_cast<int>(col) -
              static_cast<int>(LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS_HALF);
  
  double col_tpd = (index * m_time_per_division) + m_horizontal_offset;

  return (col_tpd);
}

void LABSoft_GUI_Oscilloscope_Display:: 
mark_samples (bool state) 
{
  m_internal_display->mark_samples (state);
}

void LABSoft_GUI_Oscilloscope_Display:: 
channel_enable_disable (unsigned channel,
                        bool     state)
{
  m_internal_display->channel_enable_disable (channel, state);
}

void LABSoft_GUI_Oscilloscope_Display:: 
vertical_offset (unsigned channel,
                 double   value)
{
  m_vertical_offset.at (channel) = value;

  update_gui_voltage_per_division_labels (channel);
}

void LABSoft_GUI_Oscilloscope_Display::
voltage_per_division (unsigned channel, 
                      double   value)
{
  m_voltage_per_division.at (channel) = value;

  update_gui_voltage_per_division_labels (channel);
}

void LABSoft_GUI_Oscilloscope_Display:: 
horizontal_offset (double value)
{
  m_horizontal_offset = value;

  update_gui_time_per_division_labels ();
}

void LABSoft_GUI_Oscilloscope_Display::
time_per_division (double value)
{
  m_time_per_division = value;

  update_gui_time_per_division_labels ();
}

void LABSoft_GUI_Oscilloscope_Display:: 
samples (unsigned value)
{
  m_samples = value;
}

void LABSoft_GUI_Oscilloscope_Display:: 
sampling_rate (double value)
{
  m_sampling_rate = value;
}

unsigned LABSoft_GUI_Oscilloscope_Display::
display_width () const
{
  return (m_internal_display->w ());
}

unsigned LABSoft_GUI_Oscilloscope_Display::
display_height () const
{
  return (m_internal_display->h ());
}

void LABSoft_GUI_Oscilloscope_Display::
hide_voltage_per_division_labels (unsigned channel)
{
  m_voltage_per_division_units.at (channel)->hide ();

  std::array<Fl_Box*, LABC::OSC_DISPLAY::NUMBER_OF_ROWS + 1>& labels = 
    m_voltage_per_division_labels.at (channel);

  for (unsigned row = 0; row < labels.size (); row++)
  {
    labels[row]->hide ();
  }
}

void LABSoft_GUI_Oscilloscope_Display::
show_voltage_per_division_labels (unsigned channel)
{
  m_voltage_per_division_units.at (channel)->hide ();

  std::array<Fl_Box*, LABC::OSC_DISPLAY::NUMBER_OF_ROWS + 1>& labels = 
    m_voltage_per_division_labels.at (channel);

  for (unsigned row = 0; row < labels.size (); row++)
  {
    labels[row]->show ();
  }
}
