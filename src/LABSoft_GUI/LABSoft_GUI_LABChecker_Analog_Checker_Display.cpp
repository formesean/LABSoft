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

#include "LABSoft_GUI_LABChecker_Analog_Checker_Display.h"

LABSoft_GUI_LABChecker_Analog_Checker_Display::LABSoft_GUI_LABChecker_Analog_Checker_Display(
    int X, int Y, int W, int H, const char* label)
    : Fl_Group(X, Y, W, H, label)
{
  
    oscilloscope_display = new LABSoft_GUI_Oscilloscope_Display(260, 100, 920, 450, " ");
    this->add(oscilloscope_display);
    oscilloscope_display->show();

    oscilloscope_display->voltage_per_division(0, 1.0);
    oscilloscope_display->voltage_per_division(1, 1.0);

    oscilloscope_display->time_per_division(0.005);

    oscilloscope_display->samples(1024);
    oscilloscope_display->sampling_rate(2000.0);

    oscilloscope_display->update_display();
   

    end(); // Important: Close Fl_Group after adding children
}


void LABSoft_GUI_LABChecker_Analog_Checker_Display:: 
load_presenter (const LABSoft_Presenter& presenter)
{
  m_presenter = &presenter;

   oscilloscope_display->load_presenter(presenter);
}


void LABSoft_GUI_LABChecker_Analog_Checker_Display:: 
load_pixel_points (const LABSoft_GUI_LABChecker_Analog_Checker_Display::PixelPoints& pixel_points)
{
  oscilloscope_display->load_pixel_points (pixel_points);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display:: 
update_display ()
{
  oscilloscope_display->update_display ();
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
time_per_division(double value)
{
  oscilloscope_display->time_per_division(value);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
samples(unsigned value)
{
  oscilloscope_display->samples(value);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
sampling_rate(double value)
{
  oscilloscope_display->sampling_rate(value);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
voltage_per_division(unsigned channel, double value)
{
  oscilloscope_display->voltage_per_division(channel, value);
}

unsigned LABSoft_GUI_LABChecker_Analog_Checker_Display::
display_width() const
{
  return oscilloscope_display->display_width();
}

unsigned LABSoft_GUI_LABChecker_Analog_Checker_Display::
display_height() const
{
  return oscilloscope_display->display_height();
}

