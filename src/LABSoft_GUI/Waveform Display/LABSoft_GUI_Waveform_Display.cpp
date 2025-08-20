#include "LABSoft_GUI_Waveform_Display.h"

template <unsigned N>
LABSoft_GUI_Waveform_Display:: 
LABSoft_GUI_Waveform_Display (int         X,
                              int         Y,
                              int         W,
                              int         H,
                              const char* label)
  : Fl_Widget (X, Y, W, H, label),
    m_pixel_points {{{0, 0}}}
{

}

void LABSoft_GUI_Waveform_Display:: 
draw ()
{
  for (const Channel_Info& chan : m_channel_info)
  {
    if (chan.is_enabled)
    {
      for (int i = 0; i < (chan.pixel_points.size () - 1); i++)
      {
        fl_line 
      }
    }
  }
}

void LABSoft_GUI_Waveform_Display::
background_color (Fl_Color value)
{
  m_background_color = value;
}

void LABSoft_GUI_Waveform_Display::
line_color (Fl_Color value)
{
  m_line_color = value;
}

void LABSoft_GUI_Waveform_Display::
channels (unsigned value)
{
  m_pixel_points.resize (value);
}

unsigned LABSoft_GUI_Waveform_Display:: 
channels () const
{
  return (m_channels);
}

std::vector<std::vector<std::array<int, 2>>> LABSoft_GUI_Waveform_Display:: 
pixel_points () const
{
  return (m_pixel_points);
}