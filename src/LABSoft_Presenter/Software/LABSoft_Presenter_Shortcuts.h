#ifndef LABSOFT_PRESENTER_SHORTCUTS
#define LABSOFT_PRESENTER_SHORTCUTS

#include "../LABSoft_Presenter_Unit.h"
#include "../../LAB/Software/LAB_Shortcuts.h"

class LABSoft_Presenter_Shortcuts : public LABSoft_Presenter_Unit
{
  public:
    LABSoft_Presenter_Shortcuts (LABSoft_Presenter& _LABSoft_Presenter);

    void cb_show_window         ();
    void cb_apply               ();
    void cb_close               ();
};

#endif

// EOF
