#include "LABSoft_GUI_Oscilloscope_Calibration_Table.h"

#include "../Utility/LAB_Constants.h"

// delete soon
#include <iostream>

LABSoft_GUI_Oscilloscope_Calibration_Table:: 
LABSoft_GUI_Oscilloscope_Calibration_Table (int X,
                                            int Y,
                                            int W,
                                            int H,
                                            const char* label)
  : LABSoft_GUI_Sheet (X, Y, W, H, label)
{
  dimensions (LABC::OSC::NUMBER_OF_CALIBRATION_PARAMETERS,
    LABC::OSC::NUMBER_OF_CHANNELS);

  col_header (1);
  row_header (1);

  row_header_width  (150);
  row_height_all    (50);

  range (0, 1).value ("Channel 1");
  range (0, 2).value ("Channel 2");
  range (1, 0).value ("ADC Vref");
  range (2, 0).value ("Vertical Offset\nCorrector");
  range (3, 0).value ("Scaling Corrector\nto Unity");
  range (4, 0).value ("Scaling Corrector\nto Actual");

  std::cout << "rows: " << data ().size () << "\n";
  std::cout << "cols: " << data ()[0].size () << "\n\n";

  std::cout << "[0][0]: " << data ()[0][0].text << "\n";
  std::cout << "[0][1]: " << data ()[0][1].text << "\n";
  std::cout << "[0][2]: " << data ()[0][2].text << "\n";

  std::cout << "[1][0]: " << data ()[1][0].text << "\n";
  std::cout << "[1][1]: " << data ()[1][1].text << "\n";
  std::cout << "[1][2]: " << data ()[1][2].text << "\n";

  std::cout << "[2][0]: " << data ()[2][0].text << "\n";
  std::cout << "[2][1]: " << data ()[2][1].text << "\n";
  std::cout << "[2][2]: " << data ()[2][2].text << "\n";

  std::cout << "[3][0]: " << data ()[3][0].text << "\n";
  std::cout << "[3][1]: " << data ()[3][1].text << "\n";
  std::cout << "[3][2]: " << data ()[3][2].text << "\n";
}

LABSoft_GUI_Oscilloscope_Calibration_Table::
~LABSoft_GUI_Oscilloscope_Calibration_Table ()
{

}