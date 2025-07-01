#include "LABSoft_Presenter_Software_Navigation.h"

#include "../LAB/LAB.h"
#include "LABSoft_Presenter.h"
#include "../LABSoft_GUI/LABSoft_GUI.h"
#include "../Utility/LABSoft_GUI_Label.h"

LABSoft_Presenter_Software_Navigation::
LABSoft_Presenter_Software_Navigation (LABSoft_Presenter& _LABSoft_Presenter)
{

}

void
LABSoft_Presenter_Software_Navigation::
update_data_cycle()
{
  auto data = lab().m_Software_Navigation.update_spi_data();
  printf("Type: %u, Action: %u, Value: %u\n", data[0], data[1], data[2]);
}

// EOF
