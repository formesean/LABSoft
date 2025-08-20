#ifndef LABSOFT_PRESENTER_OSCILLOSCOPE_DISPLAY_H
#define LABSOFT_PRESENTER_OSCILLOSCOPE_DISPLAY_H

#include "../LABSoft_Presenter_Unit.h"

class LABSoft_Presenter_Oscilloscope_Display : public LABSoft_Presenter_Unit
{
  private:
    void load_gui ();

  public:
    LABSoft_Presenter_Oscilloscope_Display (LABSoft_Presenter& _LABSoft_Presenter);

    void cb_horizontal_offset (double value);
    void cb_mouse_wheel       (int direction) const;
    void update_display       ();
};

#endif