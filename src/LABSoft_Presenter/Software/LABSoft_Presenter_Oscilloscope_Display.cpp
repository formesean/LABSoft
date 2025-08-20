#include "LABSoft_Presenter_Oscilloscope_Display.h"

#include "../../LAB/LAB.h"
#include "../LABSoft_Presenter.h"
#include "../../LABSoft_GUI/LABSoft_GUI.h"

LABSoft_Presenter_Oscilloscope_Display:: 
LABSoft_Presenter_Oscilloscope_Display (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{
  load_gui ();
}

void LABSoft_Presenter_Oscilloscope_Display:: 
load_gui ()
{
  LAB_Oscilloscope&                 osc           = lab ().m_Oscilloscope;
  LAB_Oscilloscope_Display&         osc_disp      = lab ().m_Oscilloscope_Display;
  LABSoft_GUI_Oscilloscope_Display& osc_disp_gui  = *(gui ().oscilloscope_labsoft_gui_oscilloscope_display);
  
  osc_disp_gui.channel_enable_disable (0, osc.is_channel_enabled    (0));
  osc_disp_gui.channel_enable_disable (0, osc.is_channel_enabled    (1));
  osc_disp_gui.voltage_per_division   (0, osc.voltage_per_division  (0));
  osc_disp_gui.voltage_per_division   (1, osc.voltage_per_division  (1));
  osc_disp_gui.vertical_offset        (1, osc.vertical_offset       (1));
  osc_disp_gui.vertical_offset        (1, osc.vertical_offset       (1));
  osc_disp_gui.horizontal_offset      (osc.horizontal_offset        ( ));
  osc_disp_gui.time_per_division      (osc.time_per_division        ( ));
  osc_disp_gui.load_presenter         (m_presenter);
  osc_disp_gui.load_pixel_points      (osc_disp.pixel_points ());

  osc_disp.display_parameters (
    osc_disp_gui.display_width (),
    osc_disp_gui.display_height (),
    LABC::OSC_DISPLAY::NUMBER_OF_ROWS,
    LABC::OSC_DISPLAY::NUMBER_OF_COLUMNS
  );
}

void LABSoft_Presenter_Oscilloscope_Display:: 
cb_horizontal_offset (double value)
{

}

void LABSoft_Presenter_Oscilloscope_Display:: 
cb_mouse_wheel (int direction) const
{

}

void LABSoft_Presenter_Oscilloscope_Display:: 
update_display ()
{
  LAB_Oscilloscope_Display&         osc_disp      = lab ().m_Oscilloscope_Display; 
  LABSoft_GUI_Oscilloscope_Display& osc_disp_gui  = *(gui ().oscilloscope_labsoft_gui_oscilloscope_display);

  osc_disp    .update_pixel_points  ();
  osc_disp_gui.update_display       ();
}