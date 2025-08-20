#ifndef LABSOFT_GUI_OSCILLOSCOPE_CALIBRATION_TABLE_H
#define LABSOFT_GUI_OSCILLOSCOPE_CALIBRATION_TABLE_H

#include "LABSoft_GUI_Sheet.h"

class LABSoft_GUI_Oscilloscope_Calibration_Table : public LABSoft_GUI_Sheet
{
  public:
    LABSoft_GUI_Oscilloscope_Calibration_Table (int X, int Y, int W, int H, const char* label = 0);
   ~LABSoft_GUI_Oscilloscope_Calibration_Table ();
};

#endif