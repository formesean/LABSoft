#include "LABSoft_GUI_Oscilloscope_Internal_Display.h"

#include <cmath>

// delete soon
#include <iostream>

#include <FL/Fl.H>

#include "../LABSoft_Presenter/LABSoft_Presenter.h"

LABSoft_GUI_Oscilloscope_Internal_Display::
LABSoft_GUI_Oscilloscope_Internal_Display (int         X,
                                           int         Y,
                                           int         W,
                                           int         H,
                                           const char* label)
  : Fl_Widget (X, Y, W, H, label)
{
  calc_cached_values ();
}

void LABSoft_GUI_Oscilloscope_Internal_Display:: 
draw ()
{
  draw_box  (FL_FLAT_BOX, m_background_color);
  draw_grid ();

  if (m_pixel_points != nullptr)
  {
    draw_channels (*m_pixel_points);
  }
}

int LABSoft_GUI_Oscilloscope_Internal_Display::
handle (int event)
{
  switch (event)
  {
    case (FL_ENTER):
    {
      return (1);
    }

    case (FL_MOUSEWHEEL):
    {
      if (Fl::belowmouse () == this)
      {
        m_presenter->m_Oscilloscope_Display.cb_mouse_wheel (Fl::event_dy ());

        return (1);
      }
      else 
      {
        return (0);
      }
    }

    case (FL_PUSH):
    {
      cb_mouse_click (Fl::event_x (), Fl::event_y ());

      return (1);
    }

    case (FL_DRAG):
    {
      cb_mouse_drag (Fl::event_x ());

      return (1);
    }

    default:
    {
      return (0);
    }
  }
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
resize (int x,
        int y,
        int w,
        int h)
{
  Fl_Widget::resize (x, y, w, h);

  calc_cached_values ();
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
draw_grid ()
{
  // 1. set line color
  fl_color (m_grid_color);

  // 2. draw rows
  for (unsigned row = 0; row < (LABC::OSC_DISPLAY::NUMBER_OF_ROWS - 1); row++)
  {
    (row == LABC::OSC_DISPLAY::MID_ROW_INDEX) ? fl_line_style (FL_DASH) : 
                                                fl_line_style (FL_DOT);

    int Y = std::round ((row + 1) * m_row_height) + y ();

    fl_line (x (), Y, x () + w (), Y);
  }

  // 3. draw columns
  for (unsigned col = 0; col < (LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS - 1); col++)
  {
    (col == LABC::OSC_DISPLAY::MID_COLUMN_INDEX) ?  fl_line_style (FL_DASH) : 
                                                    fl_line_style (FL_DOT);

    int X = std::round ((col + 1) * m_column_width) + x ();

    fl_line (X, y (), X, y () + h ());
  }

  // 4. draw upper and lower x-axis ticks
  fl_line_style (FL_SOLID, 0, NULL);
 
  for (int a = 0; a < 2; a++)
  {
    for (int X = x (); X < (x () + w ()); X += m_column_width)
    {
      for (int i = 0; i < LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS; i++)
      {
        int length = (i == LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS_HALF) ?
          LABC::OSC_DISPLAY::X_AXIS_SEMI_MAJOR_TICK_LENGTH :
          LABC::OSC_DISPLAY::X_AXIS_MINOR_TICK_LENGTH;
        
        int x = X + (i * m_x_axis_minor_ticks_width);

        (a == 0) ?  fl_line (x, y (), x, y () + length) : 
                    fl_line (x, y () + h () - length, x, y () + h ());
      }

      int length = LABC::OSC_DISPLAY::X_AXIS_MAJOR_TICK_LENGTH;

      (a == 0) ?  fl_line (X, y (), X, y () + length) : 
                  fl_line (X, y () + h () - length, X, y () + h ());
    }
  }

  // 5. draw left and right y-axis ticks
  fl_line_style (FL_SOLID, 0, NULL);
 
  for (int a = 0; a < 2; a++)
  {
    for (int Y = y (); Y < (y () + h ()); Y += m_row_height)
    {
      for (int i = 0; i < LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS; i++)
      {
        int length = (i == LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS_HALF) ?
          LABC::OSC_DISPLAY::Y_AXIS_SEMI_MAJOR_TICK_LENGTH :
          LABC::OSC_DISPLAY::Y_AXIS_MINOR_TICK_LENGTH;
        
        int y = Y + (i * m_y_axis_minor_ticks_width);

        (a == 0) ?  fl_line (x (), y, x () + length, y) :
                    fl_line (x () + w () - length, y, x () + w (), y);
      }

      int length = LABC::OSC_DISPLAY::Y_AXIS_MAJOR_TICK_LENGTH;

      (a == 0) ?  fl_line (x (), Y, x () + length, Y) :
                  fl_line (x () + w () - length, Y, x () + w (), Y);
    }
  }

  // 6. Draw border box
  draw_box (FL_BORDER_FRAME, LABC::OSC_DISPLAY::GRID_COLOR);

  // 7. Reset line color and style
  fl_color      (0);
  fl_line_style (FL_SOLID, 0, NULL);
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
draw_channels (const LABSoft_GUI_Oscilloscope_Internal_Display::PixelPoints& pixel_points)
{
  fl_push_clip (x (), y (), w (), h ());

  for (unsigned chan = 0; chan < pixel_points.size (); chan++)
  {
    if (m_channel_enabled[chan])
    {
      const std::vector<std::array<int, 2>>& pp = pixel_points[chan];

      fl_color (LABC::OSC_DISPLAY::CHANNEL_COLORS[chan]);

      if (m_mark_samples)
      {
        for (unsigned i = 0; i < (pp.size () - 1); i++)
        {
          fl_line (
            pp[i][0]      + x (), 
            pp[i][1]      + y (), 
            pp[i + 1][0]  + x (), 
            pp[i + 1][1]  + y ()
          );

          draw_sample_marker  (pp[i][0] + x (), pp[i][1] + y ());
        }
      }
      else 
      {       
        for (int i = 0; i < (pp.size () - 1); i++)
        {         
          fl_line (
            pp[i][0]      + x (), 
            pp[i][1]      + y (), 
            pp[i + 1][0]  + x (), 
            pp[i + 1][1]  + y ()
          );
        }
      }
    }
  }

  fl_color      (0);
  fl_line_style (0);
  fl_pop_clip   ();
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
draw_sample_marker (int x,
                    int y)
{
  // vertical
  fl_line (
    x,
    y + LABC::OSC_DISPLAY::SAMPLE_MARKING_AMPLITUDE,
    x,
    y - LABC::OSC_DISPLAY::SAMPLE_MARKING_AMPLITUDE
  );

  // horizontal
  fl_line (
    x + LABC::OSC_DISPLAY::SAMPLE_MARKING_AMPLITUDE,
    y,
    x - LABC::OSC_DISPLAY::SAMPLE_MARKING_AMPLITUDE,
    y
  );
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
calc_cached_values ()
{
  m_row_height                = static_cast<double>(h ()) / LABC::OSC_DISPLAY::NUMBER_OF_ROWS;
  m_column_width              = static_cast<double>(w ()) / LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS;

  m_x_axis_minor_ticks_width  = m_column_width / LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS;
  m_y_axis_minor_ticks_width  = m_row_height / LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS;

  m_display_half_height       = h () / 2.0;
  m_display_height_midline    = std::round (y () + m_display_half_height);
}

double LABSoft_GUI_Oscilloscope_Internal_Display::
calc_mouse_drag_time_per_division_delta_scaler (int drag_x) const
{
  return ((m_mouse_drag_start_x - drag_x) / m_x_axis_minor_ticks_width);
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
cb_mouse_wheel (int direction)
{
  // m_presenter->m_Oscilloscope.cb_display_time_per_division (direction);
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
cb_mouse_click (int x, int y)
{
  // m_mouse_drag_start_x                  = x;
  // m_mouse_drag_start_y                  = y;
  // m_mouse_drag_start_horizontal_offset  = m_horizontal_offset;
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
cb_mouse_drag (int x)
{
  // double drag_delta_scaler  = calc_mouse_drag_time_per_division_delta_scaler (x);
  // double drag_delta_time    = drag_scaler * (m_time_per_division / 
  //                             LABC::OSC_DISPLAY::NUMBER_OF_MINOR_TICKS);
  // double new_horiz_offset   = drag_delta_time + m_mouse_drag_start_horizontal_offset;

  // if (LABF::is_equal (horiz_off, 0.0, LABC::LABSOFT::EPSILON))
  // {
  //   new_horiz_offset = 0.0;
  // }

  // m_presenter->m_Oscilloscope_Display.cb_horizontal_offset 
  //   (static_cast<void*>(&new_horiz_offset));
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
channel_enable_disable (unsigned channel,
                        bool     state)
{
  m_channel_enabled.at (channel) = state;
}

void LABSoft_GUI_Oscilloscope_Internal_Display::
load_presenter (const LABSoft_Presenter& presenter)
{
  m_presenter = &presenter;
}

void LABSoft_GUI_Oscilloscope_Internal_Display:: 
load_pixel_points (const LABSoft_GUI_Oscilloscope_Internal_Display::PixelPoints& pixel_points)
{
  m_pixel_points = &pixel_points;
}

double LABSoft_GUI_Oscilloscope_Internal_Display:: 
row_height () const
{
  return (m_row_height);
}

double LABSoft_GUI_Oscilloscope_Internal_Display:: 
column_width () const 
{
  return (m_column_width);
}

void LABSoft_GUI_Oscilloscope_Internal_Display:: 
mark_samples (bool state)
{
  m_mark_samples = state;
}

void LABSoft_GUI_Oscilloscope_Internal_Display:: 
update_display ()
{
  redraw ();
}