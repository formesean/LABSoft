#include "LAB_LABChecker_Analog.h"

#include <cmath>
#include <iostream>

#include "../LAB.h"
#include "../LAB_Oscilloscope.h"
#include "../../Utility/LAB_Utility_Functions.h"
#include "LAB_Oscilloscope_Display.h"




// bea: try to delete this

/* LAB_LABChecker_Analog::
LAB_LABChecker_Analog (
    int X, int Y, int W, int H, const char* label)
    : Fl_Widget(X, Y, W, H, label)
    , m_oscilloscope_data_available(false)
    //, m_current_mode(DisplayMode::CIRCUIT_ONLY)
{

}

void LAB_LABChecker_Analog::draw(){

fl_color(FL_BLACK);
fl_rectf(x(), y(), w(), h());

draw_grid();
//draw_circuit_elements();

if (m_osci_data_available){

  render_osci_overlay();
}

draw_labels();

}

void LAB_LABChecker_Analog::update_osci_data(const CircuitCheckerData& data){

    m_Osci_data = data.osci_data;
    m_osci_data_available = true;

    redraw();
}




void LAB_LABChecker_Analog::render_osci_overlay(){
  
  if(!m_osci_data_available)return;

  fl_color(FL_YELLOW);
  fl_line_style(FL_SOLID, 2);

  const auto& points = m_oscilloscope_data.waveform_points;
    for (size_t i = 1; i < points.size(); ++i) {
        fl_line(
            x() + points[i-1].first, y() + points[i-1].second,
            x() + points[i].first, y() + points[i].second
        );
    }
    
    // Reset line style
    fl_line_style(FL_SOLID, 1);

} */

LAB_LABChecker_Analog::
LAB_LABChecker_Analog (LAB&              _LAB,
                          LAB_Oscilloscope& _LAB_Oscilloscope)
  : LAB_Module  (_LAB),
    m_osc       (_LAB_Oscilloscope)
{

}

double LAB_LABChecker_Analog::
calc_time_per_division_delta_scaler () const
{
  if (m_osc.is_running ())
  {
    if (LABF::is_less_than (
      m_osc.time_per_division (),
      LABC::OSC::MIN_TIME_PER_DIVISION_NO_ZOOM,
      LABC::LABSOFT::EPSILON
    ))
    {
      return (LABC::OSC::MIN_TIME_PER_DIVISION_NO_ZOOM / m_osc.time_per_division ());
    }
    else 
    {
      return (1.0);
    }
  }
  else 
  {
    return (m_osc.raw_buffer_time_per_division () / m_osc.time_per_division ());
  }
}

LAB_LABChecker_Analog::DRAW_MODE LAB_LABChecker_Analog:: 
calc_draw_mode (double tpd_ds) const
{
  if (LABF::is_less_than (tpd_ds, 1.0, LABC::LABSOFT::EPSILON))
  {
    return (LAB_LABChecker_Analog::DRAW_MODE::SHRINK);
  }
  else if (LABF::is_greater_than (tpd_ds, 1.0, LABC::LABSOFT::EPSILON))
  {
    return (LAB_LABChecker_Analog::DRAW_MODE::STRETCH);
  }
  else
  {
    return (LAB_LABChecker_Analog::DRAW_MODE::FIT);
  }
}

unsigned LAB_LABChecker_Analog::
calc_samples_to_display () const
{
  double new_samples_to_display;
  
  if (m_osc.is_running ())
  {
    new_samples_to_display  = (m_osc.sampling_rate () * m_osc.time_per_division () * 
                              LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS);
  }
  else 
  {
    new_samples_to_display  = (m_osc.raw_buffer_sampling_rate () * m_osc.time_per_division () * 
                              LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS);
  }

  if (LABF::is_less_than (new_samples_to_display, 1.0, LABC::LABSOFT::EPSILON))
  {
    return (1);
  }
  else 
  {
    return (std::round (new_samples_to_display));
  }
}

unsigned LAB_LABChecker_Analog:: 
calc_graphing_area_width (double   tpd_ds,
                          unsigned display_width) const
{
  double new_draw_window_width = display_width * tpd_ds;

  if (LABF::is_less_than (new_draw_window_width, 2.0, LABC::LABSOFT::EPSILON))
  {
    return (2);
  }
  else 
  {
    return (std::round (new_draw_window_width));
  }    
}

double LAB_LABChecker_Analog:: 
calc_x_coord_scaling (unsigned graphing_area_width,
                      unsigned number_of_samples) const
{
  return (static_cast<double>(graphing_area_width - 1) / (number_of_samples - 1));
}

int LAB_LABChecker_Analog:: 
calc_x_coord_start_offset (unsigned graphing_area_width,
                           unsigned display_width) const
{  
  return (std::round ((static_cast<double>(display_width) - graphing_area_width) / 2.0));
}

int LAB_LABChecker_Analog:: 
calc_horizontal_offset_start_offset (unsigned display_width) const
{
  if (m_osc.is_running ())
  {
    return (0);
  }
  else 
  {
    double horiz_off_delta  = m_osc.horizontal_offset () - 
                              m_osc.raw_buffer_horizontal_offset ();                            

    double horiz_off_scaler = display_width / (m_osc.time_per_division () * 
                              LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS);

    return (std::round (horiz_off_delta * horiz_off_scaler * -1));
  }
}

int LAB_LABChecker_Analog:: 
calc_mid_sample_to_center_offset () const
{
  return (std::round ((m_graphing_area_width / m_osc.samples ()) / (-2.0)));
}

LAB_LABChecker_Analog::ChanDoubles LAB_LABChecker_Analog::
calc_sample_y_scaler () const
{
  LAB_LABChecker_Analog::ChanDoubles doubles;

  for (int chan = 0; chan < doubles.size (); chan++)
  {
    doubles[chan] = (m_height / 2.0) / ((m_rows / 2.0) * m_osc.voltage_per_division (chan));
  }

  return (doubles);
}

bool LAB_LABChecker_Analog:: 
calc_mark_samples (double   tpd_ds, 
                   unsigned samples_to_display) const
{
  return ((samples_to_display <= LABC::OSC_DISPLAY::SAMPLE_MARKING_THRESHOLD) &&
    (LABF::is_greater_than (tpd_ds, 0.5, LABC::LABSOFT::EPSILON)));
}

int LAB_LABChecker_Analog:: 
calc_sample_x_coord (unsigned index) const
{
  return (std::round ((index * m_x_coord_scaling) + m_x_coord_start_offset + 
    m_horizontal_offset_start_offset + m_mid_sample_to_center_offset));
}

int LAB_LABChecker_Analog::
calc_sample_y_coord (double   sample, 
                     unsigned channel) const
{
  double samp_with_offset = sample + m_vertical_offset[channel];

  return (std::round (m_display_height_midline - (samp_with_offset * m_sample_y_scaler[channel])));
}

LAB_LABChecker_Analog::ChanDoubles LAB_LABChecker_Analog:: 
calc_vertical_offset () const
{
  LAB_LABChecker_Analog::ChanDoubles doubles;

  for (int chan = 0; chan < m_vertical_offset.size (); chan++)
  {
    doubles[chan] = m_osc.vertical_offset (chan);
  }

  return (doubles);
}

void LAB_LABChecker_Analog:: 
resize_pixel_points (PixelPoints& pixel_points,
                     unsigned     size) 
{
  for (int a = 0; a < m_pixel_points.size (); a++)
  {
    m_pixel_points[a].resize (size);
  }
}

void LAB_LABChecker_Analog:: 
update_cached_display_values ()
{
  m_column_width            = m_width / m_columns;
  m_display_height_midline  = m_height / 2.0; 
}

void LAB_LABChecker_Analog:: 
display_parameters (unsigned width,
                    unsigned height,
                    unsigned rows,
                    unsigned columns)
{
  m_width   = width;
  m_height  = height;
  m_rows    = rows;
  m_columns = columns;

  update_cached_display_values  ();
  update_cached_values          ();
}

void LAB_LABChecker_Analog::
update_cached_values ()
{
  double tpd_ds                     = 
  m_time_per_division_delta_scaler  = calc_time_per_division_delta_scaler ();
  m_draw_mode                       = calc_draw_mode                      (tpd_ds);
  m_samples_to_display              = calc_samples_to_display             ();
  m_graphing_area_width             = calc_graphing_area_width            (tpd_ds, m_width);
  m_x_coord_scaling                 = calc_x_coord_scaling                (m_graphing_area_width, m_osc.samples ());
  m_x_coord_start_offset            = calc_x_coord_start_offset           (m_graphing_area_width, m_width);
  m_horizontal_offset_start_offset  = calc_horizontal_offset_start_offset (m_width);
  m_mid_sample_to_center_offset     = calc_mid_sample_to_center_offset    ();
  m_sample_y_scaler                 = calc_sample_y_scaler                ();
  m_mark_samples                    = calc_mark_samples                   (tpd_ds, m_samples_to_display);
  m_vertical_offset                 = calc_vertical_offset                ();

  resize_pixel_points (m_pixel_points, m_osc.samples ());

  // debug ();
}

void LAB_LABChecker_Analog::
debug () const
{
  std::cout << "m_draw_mode                     : " << static_cast<int>(m_draw_mode) << "\n";
  std::cout << "m_time_per_division_delta_scaler: " << m_time_per_division_delta_scaler << "\n";
  std::cout << "m_graphing_area_width           : " << m_graphing_area_width << "\n";
  std::cout << "m_x_coord_scaling               : " << m_x_coord_scaling << "\n";
  std::cout << "m_x_coord_start_offset          : " << m_x_coord_start_offset << "\n";
  std::cout << "m_horizontal_offset_start_offset: " << m_horizontal_offset_start_offset << "\n";
  std::cout << "m_mid_sample_to_center_offset   : " << m_mid_sample_to_center_offset << "\n";
  std::cout << "m_mark_samples                  : " << m_mark_samples << "\n";
  std::cout << "m_samples_to_display            : " << m_samples_to_display << "\n\n";
}

void LAB_LABChecker_Analog:: 
update_pixel_points ()
{
  for (int chan = 0; chan < m_pixel_points.size (); chan++)
  {
    if (m_osc.is_channel_enabled (chan))
    {
      std::vector<std::array<int, 2>>& pp = m_pixel_points[chan];

      if (m_draw_mode == LAB_LABChecker_Analog::DRAW_MODE::STRETCH)
      {
        int prev      = calc_sample_x_coord (0);
        int curr      = prev;
        bool left_ok  = false;
        bool right_ok = false;

        for (int i = 0; i < pp.size (); i++)
        {
          //
          curr = calc_sample_x_coord (i);

          // 1. register left and right post display pixel hooks
          if ((curr >= 0 && prev < 0) && !left_ok)
          {
            pp[i - 1][0] = calc_sample_x_coord (i - 1);
        
            left_ok = true;
          }

          if ((curr >= m_width && prev >= m_width) && !right_ok)
          {
            pp[i - 1][0] = calc_sample_x_coord (i - 1);
      
            right_ok = true;
          }

          // 2.
          if (curr < 0)
          {
            pp[i][0] = -100;
          }
          else if (curr >= m_width)
          {
            pp[i][0] = m_width + 100;
          }
          else 
          {
            pp[i][0] = calc_sample_x_coord (i);
          }

          prev = curr;
          //

          double sample = m_osc.chan_samples  (chan)[i];
          pp[i][1]      = calc_sample_y_coord (sample, chan);
        }
      }
      else 
      {
        for (int i = 0; i < pp.size (); i++)
        {
          double sample = m_osc.chan_samples (chan)[i];

          pp[i][0] = calc_sample_x_coord (i);
          pp[i][1] = calc_sample_y_coord (sample, chan);
        }
      }
    }
  }
}

const LAB_LABChecker_Analog::PixelPoints& LAB_LABChecker_Analog:: 
pixel_points () const
{
  return (m_pixel_points);
}

bool LAB_LABChecker_Analog:: 
mark_samples () const
{
  return (m_mark_samples);
}


