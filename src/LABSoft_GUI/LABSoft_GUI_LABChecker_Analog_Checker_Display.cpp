#include "LABSoft_GUI_Oscilloscope_Display.h"

#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

#include "../Utility/LABSoft_GUI_Label.h"
#include "../Utility/LAB_Utility_Functions.h"
#include "../LABSoft_Presenter/LABSoft_Presenter.h"
#include "LABSoft_GUI_Oscilloscope_Internal_Display.h"
#include "../Utility/LABSoft_GUI_Label_Values.h"

#include "LABSoft_GUI_LABChecker_Analog_Checker_Display.h"

LABSoft_GUI_LABChecker_Analog_Checker_Display::LABSoft_GUI_LABChecker_Analog_Checker_Display
(int X, int Y, int W, int H, const char* label)
    : Fl_Group(X, Y, W, H, label)
{
  oscilloscope_display = new LABSoft_GUI_Oscilloscope_Display(260, 100, 920, 450, " ");
  this->add(oscilloscope_display);
  oscilloscope_display->show();
  oscilloscope_display->voltage_per_division(0, 1.0);
  oscilloscope_display->voltage_per_division(1, 1.0);
  oscilloscope_display->time_per_division(0.005);
  end();
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
voltage_per_division(unsigned channel, double value)
{
  if (oscilloscope_display) oscilloscope_display->voltage_per_division(channel, value);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
time_per_division(double value)
{
  if (oscilloscope_display) oscilloscope_display->time_per_division(value);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
samples(unsigned value)
{
  if (oscilloscope_display) oscilloscope_display->samples(value);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
sampling_rate(double value)
{
 if (oscilloscope_display) oscilloscope_display->sampling_rate(value);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
horizontal_offset(double value)
{
  if (oscilloscope_display) oscilloscope_display->horizontal_offset(value);
}

unsigned LABSoft_GUI_LABChecker_Analog_Checker_Display::
display_width() const
{
  if (oscilloscope_display) return oscilloscope_display->display_width();
  return w();
}

unsigned LABSoft_GUI_LABChecker_Analog_Checker_Display::
display_height() const
{
  if (oscilloscope_display) return oscilloscope_display->display_height();
  return h();
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
vertical_offset(unsigned channel, double value)
{
  if (oscilloscope_display) oscilloscope_display->vertical_offset(channel, value);
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
enable_channels(bool enable)
{
  if (oscilloscope_display)
  {
    try
    {
      oscilloscope_display->channel_enable_disable(0, enable);
      oscilloscope_display->channel_enable_disable(1, enable);
    }
    catch (...)
    {
    }
  }
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
update_display()
{
  if (oscilloscope_display)
  {
  try
  {
    oscilloscope_display->update_display();
  }
  catch (...)
  {
  }
  }
}


void LABSoft_GUI_LABChecker_Analog_Checker_Display::
load_presenter(const LABSoft_Presenter& presenter)
{
  m_presenter = &presenter;
  if (oscilloscope_display)
  {
    oscilloscope_display->load_presenter(presenter);
    oscilloscope_display->update_display();
  }
}

void LABSoft_GUI_LABChecker_Analog_Checker_Display::
load_pixel_points(const PixelPoints& pixel_points)
{
  if (oscilloscope_display)
  {
    try
    {
      m_pixel_points_storage = pixel_points;

      oscilloscope_display->load_pixel_points(m_pixel_points_storage);
      oscilloscope_display->update_display();
    }
    catch (...)
    {
    }
  }
}

// EOF
