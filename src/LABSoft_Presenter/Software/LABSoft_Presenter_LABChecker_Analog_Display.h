
#ifndef LABSOFT_PRESENTER_LABCHECKER_ANALOG_DISPLAY_H
#define LABSOFT_PRESENTER_LABCHECKER_ANALOG_DISPLAY_H

#include "../LABSoft_Presenter_Unit.h"


class LABSoft_Presenter_LABChecker_Analog_Display : public LABSoft_Presenter_Unit
{
  private:
    void load_gui ();

  public:
    LABSoft_Presenter_LABChecker_Analog_Display (LABSoft_Presenter& _LABSoft_Presenter);
    
    void update_display       ();
};

#endif