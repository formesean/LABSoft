#ifndef LABSOFT_GUI_WAVEFORM_DISPLAY_H
#define LABSOFT_GUI_WAVEFORM_DISPLAY_H

#include <array>
#include <vector>

#include <FL/Fl_Widget.H>
#include <FL/Enumerations.H>

template <unsigned N>
class LABSoft_GUI_Waveform_Display : public Fl_Widget
{
  private:
    struct Channel_Info
    {
      bool      is_enabled = false;
      Fl_Color  line_color = FL_WHITE;

      std::vector<std::array<int, 2>> pixel_points;
    };

  private:
    std::vector<Channel_Info> m_channel_info;
    Fl_Color                  m_background_color = FL_BLACK;
  
  private:
    void draw ();
  
  public:
    LABSoft_GUI_Waveform_Display (int X, int Y, int W, int H, const char* label = 0);

    void background_color (Fl_Color value);
    void line_color       (Fl_Color value);
    void channels         (unsigned value);

    unsigned                                      channels      () const;
    std::vector<std::vector<std::array<int, 2>>>  pixel_points  () const;
}

#endif