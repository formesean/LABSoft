#ifndef LABSOFT_GUI_OSCILLOSCOPE_DISPLAY_H
#define LABSOFT_GUI_OSCILLOSCOPE_DISPLAY_H

#include <array>
#include <vector>

#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Enumerations.H>

#include "LABSoft_GUI_Fl_Slider.h"
#include "../Utility/LAB_Constants.h"
#include "../Utility/LAB_Enumerations.h"

class LABSoft_Presenter;
class LABSoft_GUI_Oscilloscope_Internal_Display;

class LABSoft_GUI_Oscilloscope_Display : public Fl_Group
{
  using PixelPoints = std::array<std::vector<std::array<int, 2>>, 
                      LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS>;

  private:
    std::array<double, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> m_voltage_per_division  = {0.0};
    std::array<double, LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS> m_vertical_offset       = {0.0};

    double    m_time_per_division = 0.0;
    double    m_horizontal_offset = 0.0;
    unsigned  m_samples           = 0;
    double    m_sampling_rate     = 0.0;
    unsigned  m_selected_channel  = 0;

    // connections
    const LABSoft_Presenter* m_presenter  = nullptr;

    // widgets
    Fl_Box*                                     m_status                    = nullptr;
    Fl_Box*                                     m_top_info                  = nullptr;
    LABSoft_GUI_Fl_Slider*                      m_horizontal_offset_slider  = nullptr;
    LABSoft_GUI_Fl_Slider*                      m_vertical_offset_slider    = nullptr;
    LABSoft_GUI_Fl_Slider*                      m_trigger_level_slider      = nullptr;
    LABSoft_GUI_Oscilloscope_Internal_Display*  m_internal_display          = nullptr; 

    std::array<
      Fl_Button*, 
      LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS
    > m_channel_selectors;

    std::array<
      Fl_Box*, 
      LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS + 1
    > m_time_per_division_labels;

    std::array<
      std::array<
        Fl_Box*, 
        LABC::OSC_DISPLAY::NUMBER_OF_ROWS + 1
      >, 
      LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS
    > m_voltage_per_division_labels;

    std::array<
      Fl_Box*,
      LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS
    > m_voltage_per_division_units;

  private:
    // widget functions
    void draw ();

    // init widgets
    void init_child_widgets                             ();
    void init_child_widgets_internal_display            ();
    void init_child_widgets_sliders                     ();
    void init_child_widgets_status                      ();
    void init_child_widgets_voltage_per_division_labels ();
    void init_child_widgets_voltage_per_division_units  (int X, unsigned chan);
    void init_child_widgets_time_per_division_labels    ();
    void init_child_widgets_channel_selectors           ();
    void init_child_widgets_top_info                    ();

    // callbacks
    static void cb_trigger_level_static     (Fl_Widget* w, void* data);
    static void cb_channel_selector_static  (Fl_Widget* w, void* data);

    // update gui
    void update_gui_top_info                ();
    void update_gui_voltage_per_division    ();
    void update_gui_voltage_per_division    (unsigned channel);
    void update_gui_time_per_division       ();
    void update_gui_trigger_level_slider    ();
    void update_gui_vertical_offset_slider  ();
    void update_gui_vertical_elements       ();
    void update_gui_horizontal_elements     ();
    
    // calc
    double calc_row_voltage_per_division  (unsigned channel, unsigned row);
    double calc_col_time_per_division     (unsigned col);

    void update_gui_voltage_per_division_labels ();
    void update_gui_voltage_per_division_labels (unsigned channel);
    void update_gui_time_per_division_labels    ();

  public:
    LABSoft_GUI_Oscilloscope_Display (int X, int Y, int W, int H, const char* label = 0);

    void load_presenter       (const LABSoft_Presenter& presenter);  
    void load_pixel_points    (const PixelPoints& pixel_points);
    void update_display       ();

    void mark_samples                     (bool state);
    void channel_enable_disable           (unsigned channel, bool state);
    void vertical_offset                  (unsigned channel, double value);
    void voltage_per_division             (unsigned channel, double value);
    void horizontal_offset                (double value);
    void time_per_division                (double value);
    void samples                          (unsigned value); 
    void sampling_rate                    (double value);
    void trigger_source                   (unsigned channel);
    void trigger_channel                  (unsigned channel);
    void trigger_level                    (double value);
    void status                           (LABE::OSC::STATUS status);

    void hide_voltage_per_division_labels (unsigned channel);
    void show_voltage_per_division_labels (unsigned channel);

    unsigned display_width  () const;
    unsigned display_height () const;
};

#endif