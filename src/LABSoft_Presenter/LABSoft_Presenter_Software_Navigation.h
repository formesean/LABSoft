#ifndef LABSOFT_PRESENTER_SOFTWARE_NAVIGATION_H
#define LABSOFT_PRESENTER_SOFTWARE_NAVIGATION_H

#include "LABSoft_Presenter_Unit.h"

#include <cstdio>

class LABSoft_Presenter_Software_Navigation : public LABSoft_Presenter_Unit
{
  private:

  public:
    LABSoft_Presenter_Software_Navigation (LABSoft_Presenter& _LABSoft_Presenter);

    void update_data_cycle ();
};

#endif

// EOF
