cmake_minimum_required(VERSION 3.18)

project(LABSoft
  VERSION 2.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_executable(LABSoft
  main.cpp
  LABSoft.cpp

  LAB/LAB_Module.cpp
  LAB/LAB.cpp
  LAB/LAB_Oscilloscope.cpp
  LAB/LAB_Voltmeter.cpp
  LAB/LAB_Ohmmeter.cpp
  LAB/LAB_Function_Generator.cpp
  LAB/LAB_Logic_Analyzer.cpp
  LAB/LAB_Digital_Circuit_Checker.cpp
  LAB/LAB_Software_Navigation.cpp

  LAB/Software/LAB_Exporter.cpp
  LAB/Software/LAB_Calibration.cpp
  LAB/Software/LAB_Data_Measurer.cpp
  LAB/Software/LAB_LABChecker_Digital.cpp
  LAB/Software/LAB_Oscilloscope_Display.cpp
  LAB/Software/LAB_Shortcuts.cpp
  LAB/Software/LAB_LABChecker_Analog.cpp
  LAB/Software/LAB_Analog_Circuit_Checker.cpp

  LABSoft_Presenter/LABSoft_Presenter_Unit.cpp
  LABSoft_Presenter/LABSoft_Presenter.cpp
  LABSoft_Presenter/LABSoft_Presenter_Oscilloscope.cpp
  LABSoft_Presenter/LABSoft_Presenter_Voltmeter.cpp
  LABSoft_Presenter/LABSoft_Presenter_Ohmmeter.cpp
  LABSoft_Presenter/LABSoft_Presenter_Function_Generator.cpp
  LABSoft_Presenter/LABSoft_Presenter_Logic_Analyzer.cpp
  LABSoft_Presenter/LABSoft_Presenter_Digital_Circuit_Checker.cpp
  LABSoft_Presenter/LABSoft_Presenter_Software_Navigation.cpp

  LABSoft_Presenter/Software/LABSoft_Presenter_Exporter.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Calibration.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Main_Window.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_LABChecker_Digital.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Oscilloscope_Display.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Shortcuts.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Analog_Circuit_Checker.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_LABChecker_Analog.cpp


  LABSoft_GUI/LABSoft_GUI.cpp
  LABSoft_GUI/LABSoft_GUI_Fl_Input.cpp
  LABSoft_GUI/LABSoft_GUI_Fl_Slider.cpp
  LABSoft_GUI/LABSoft_GUI_Sheet.cpp
  LABSoft_GUI/LABSoft_GUI_Exporter_Data_Table.cpp
  LABSoft_GUI/LABSoft_GUI_Oscilloscope_Display.cpp
  LABSoft_GUI/LABSoft_GUI_Fl_Choice_With_Scroll.cpp
  LABSoft_GUI/LABSoft_GUI_Logic_Analyzer_Display.cpp
  LABSoft_GUI/LABSoft_GUI_Fl_Input_Choice_With_Scroll.cpp
  LABSoft_GUI/LABSoft_GUI_Oscilloscope_Internal_Display.cpp
  LABSoft_GUI/LABSoft_GUI_Oscilloscope_Calibration_Table.cpp
  LABSoft_GUI/LABSoft_GUI_LABChecker_Digital_Input_Table.cpp
  LABSoft_GUI/LABSoft_GUI_LABChecker_Digital_Output_Table.cpp
  LABSoft_GUI/LABSoft_GUI_Logic_Analyzer_Add_Channel_Signal_Window.cpp
  LABSoft_GUI/LABSoft_GUI_LABChecker_Analog_Checker_Display.cpp
  LABSoft_GUI/LABSoft_GUI_Analog_Circuit_Checker_Display.cpp


  LAB_Libraries/LAB_AD9833.cpp
  LAB_Libraries/LAB_MCP4XXX.cpp
  LAB_Libraries/LAB_MCP23S17.cpp

  Utility/pugixml.cpp
  Utility/LAB_Defaults.cpp
  Utility/LAB_Constants.h
  Utility/LABSoft_GUI_Label.cpp
  Utility/LAB_Utility_Functions.cpp
  # Utility/LAB_Encryptor.cpp

  ../lib/AikaPi/AikaPi.cpp
  ../lib/AD9833/AD9833.cpp
  ../lib/MCP4XXX/MCP4XXX.cpp
  ../lib/MCP23S17/MCP23S17.cpp
)

target_link_libraries(LABSoft
  PRIVATE fltk
  PRIVATE Threads::Threads
  PRIVATE dl
  PRIVATE m
  PRIVATE fontconfig
  PRIVATE X11
  PRIVATE Xext
  PRIVATE Xft
  PRIVATE Xinerama
  PRIVATE Xcursor
  PRIVATE Xrender
  PRIVATE Xfixes
)
