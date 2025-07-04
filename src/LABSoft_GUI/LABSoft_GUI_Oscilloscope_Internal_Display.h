#ifndef LABSOFT_GUI_OSCILLOSCOPE_INTERNAL_DISPLAY_H
#define LABSOFT_GUI_OSCILLOSCOPE_INTERNAL_DISPLAY_H

#include <array>
#include <vector>

#include <FL/Fl_Widget.H>
#include <FL/Enumerations.H>

#include "../Utility/LAB_Constants.h"

class LABSoft_Presenter;

class LABSoft_GUI_Oscilloscope_Internal_Display : public Fl_Widget
{
  using PixelPoints = std::array<std::vector<std::array<int, 2>>, 
                      LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS>;

  private:
    Fl_Color m_background_color = FL_BLACK;
    Fl_Color m_grid_color       = LABC::OSC_DISPLAY::GRID_COLOR;

    //
    std::array<bool, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> m_channel_enabled = {0};
    bool m_mark_samples = false;

    // cached values for display dimensions
    double  m_row_height                = 0;
    double  m_column_width              = 0;
    double  m_x_axis_minor_ticks_width  = 0;
    double  m_y_axis_minor_ticks_width  = 0;
    double  m_display_half_height       = 0;
    double  m_display_height_midline    = 0;

    // connections
    const LABSoft_Presenter* m_presenter     = nullptr;
    const PixelPoints*        m_pixel_points  = nullptr;
    
    // cached values for mouse drag horizontal offset change
    int     m_mouse_drag_start_x                  = 0;
    int     m_mouse_drag_start_y                  = 0;
    double  m_mouse_drag_start_horizontal_offset  = 0.0;
          
  private:
    // widget functions
    void    draw   ();
    int     handle (int event);
    void    resize (int x, int y, int w, int h);

    // draw functions
    void    draw_grid          ();
    void    draw_channels      (const PixelPoints& pixel_points);
    void    draw_sample_marker (int x, int y);
    void    calc_cached_values ();  
    double  calc_mouse_drag_time_per_division_delta_scaler (int drag_x) const;     

    // callbacks
    void    cb_mouse_click  (int x, int y);
    void    cb_mouse_drag   (int x);
    void    cb_mouse_wheel  (int direction);

  public:
    LABSoft_GUI_Oscilloscope_Internal_Display (int X, int Y, int W, int H, const char* label = 0);

    void    channel_enable_disable  (unsigned channel, bool state);
    void    load_presenter          (const LABSoft_Presenter& presenter);
    void    load_pixel_points       (const PixelPoints& pixel_points);
    double  row_height              () const;
    double  column_width            () const;
    void    mark_samples            (bool state);
    void    update_display          ();
};

#endif