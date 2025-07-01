#include "LABSoft_Presenter_Software_Navigation.h"

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"

LABSoft_Presenter_Software_Navigation::
LABSoft_Presenter_Software_Navigation (LABSoft_Presenter& _LABSoft_Presenter)
  : LABSoft_Presenter_Unit (_LABSoft_Presenter)
{

}

void
LABSoft_Presenter_Software_Navigation::
update_data_cycle()
{
  auto data = lab().m_Software_Navigation.update_spi_data();

  if (data[0] == 0 && data[1] == 0 && data[2] == 0)
    return;

  printf("Type: %u, Action: %u, Value: %u\n", data[0], data[1], data[2]);

  if (data[1] == 3 && data[2] == 0)
  {
    int state = gui().oscilloscope_fl_light_button_run_stop->value();
    printf("osc run btn state: %d\n", !state);

    if (!state)
    {
      presenter ().m_Oscilloscope.stop_gui ();
      presenter ().m_Voltmeter   .stop_gui ();
      presenter ().m_Ohmmeter    .stop_gui ();
      lab ().m_Oscilloscope.run ();
      presenter ().m_Oscilloscope.run_gui ();
    }
    else
    {
      lab ().m_Oscilloscope.stop ();
      presenter ().m_Oscilloscope.stop_gui ();
    }
  }
}

// EOF
