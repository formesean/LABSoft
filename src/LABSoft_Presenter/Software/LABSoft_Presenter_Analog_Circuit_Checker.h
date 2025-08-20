//student side
#ifndef LABSoft_Presenter_Analog_Circuit_Checker_H
#define LABSoft_Presenter_Analog_Circuit_Checker_H

#include "../LABSoft_Presenter_Unit.h"


class LABSoft_Presenter_Analog_Circuit_Checker : public LABSoft_Presenter_Unit
{
  private:
    void load_gui ();

  public:
    LABSoft_Presenter_Analog_Circuit_Checker (LABSoft_Presenter& _LABSoft_Presenter);
    
    void update_display       ();
};

#endif