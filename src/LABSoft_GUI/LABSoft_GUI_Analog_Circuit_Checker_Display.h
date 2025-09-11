#ifndef LABSOFT_GUI_ANALOG_CIRCUIT_CHECKER_DISPLAY_H
#define LABSOFT_GUI_ANALOG_CIRCUIT_CHECKER_DISPLAY_H

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

class LABSoft_GUI_Analog_Circuit_Checker_Display : public Fl_Group {

private:

    void init_child_widgets();
    void init_child_widgets_internal_display();


    LABSoft_GUI_Oscilloscope_Display* oscilloscope_display;
    const LABSoft_Presenter* m_presenter                                 = nullptr;
    LABSoft_GUI_Oscilloscope_Internal_Display*  m_internal_display       = nullptr;


public:
    using PixelPoints = std::array<std::vector<std::array<int, 2>>,
                      LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS>;

    // Constructor
    LABSoft_GUI_Analog_Circuit_Checker_Display(int X, int Y, int W, int H, const char* label = 0);

    void initialize_oscilloscope();
    // Destructor
    //~LABSoft_GUI_LABChecker_Analog_Checker_Display();


    // Optional methods
    void update_display();  // A method to update or interact with the oscilloscope
    void load_presenter       (const LABSoft_Presenter& presenter);
    void load_pixel_points    (const PixelPoints& pixel_points);
    void load_overlay_points  (const std::vector<std::array<int, 2>>& points, Fl_Color color = FL_RED, bool enabled = true);
    void voltage_per_division             (unsigned channel, double value);
    void time_per_division                (double value);
    void samples                          (unsigned value);
    void sampling_rate                    (double value);
    void set_frequency_view               (bool enabled, double sampling_rate);
    void horizontal_offset                (double value);
    unsigned display_width                () const;
    unsigned display_height               () const;
    void vertical_offset                  (unsigned channel, double value);
    void channel_enable_disable           (unsigned channel, bool state);

private:
    // Persistent storage so the internal display can safely reference pixel points
    PixelPoints m_pixel_points_storage{};
};

#endif
