cmake_minimum_required(VERSION 3.18)

project(LABSoft
  VERSION 1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(FLTK REQUIRED)

add_library(LABSoft_Libs STATIC
  ../lib/AikaPi/AikaPi.cpp
  ../lib/AD9833/AD9833.cpp
  ../lib/MCP4XXX/MCP4XXX.cpp
  ../lib/MCP23S17/MCP23S17.cpp
  ../lib/pugixml/pugixml.cpp
)

# Disable precision loss errors for AikaPi.cpp
set_source_files_properties(../lib/AikaPi/AikaPi.cpp PROPERTIES COMPILE_FLAGS "-Wno-error=conversion")

target_compile_options(LABSoft_Libs PRIVATE -O2)

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

  LAB/Software/LAB_Exporter.cpp
  LAB/Software/LAB_Calibration.cpp
  LAB/Software/LAB_Data_Measurer.cpp
  LAB/Software/LAB_LABChecker_Digital.cpp
  LAB/Software/LAB_Oscilloscope_Display.cpp

  LABSoft_Presenter/LABSoft_Presenter_Unit.cpp
  LABSoft_Presenter/LABSoft_Presenter.cpp
  LABSoft_Presenter/LABSoft_Presenter_Oscilloscope.cpp
  LABSoft_Presenter/LABSoft_Presenter_Voltmeter.cpp
  LABSoft_Presenter/LABSoft_Presenter_Ohmmeter.cpp
  LABSoft_Presenter/LABSoft_Presenter_Function_Generator.cpp
  LABSoft_Presenter/LABSoft_Presenter_Logic_Analyzer.cpp
  LABSoft_Presenter/LABSoft_Presenter_Digital_Circuit_Checker.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Exporter.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Calibration.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Main_Window.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_LABChecker_Digital.cpp
  LABSoft_Presenter/Software/LABSoft_Presenter_Oscilloscope_Display.cpp

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

  LAB_Libraries/LAB_AD9833.cpp
  LAB_Libraries/LAB_MCP4XXX.cpp
  LAB_Libraries/LAB_MCP23S17.cpp

  Utility/LAB_Defaults.cpp
  Utility/LAB_Constants.h
  Utility/LABSoft_GUI_Label.cpp
  Utility/LAB_Utility_Functions.cpp
  # Utility/LAB_Encryptor.cpp
)

target_compile_options(LABSoft PRIVATE
  -g  # Enable debugging symbols
  -Wall
  -Wextra
  -Wpedantic
  -Werror
  -Wno-error=unused-variable
  -Wno-error=unused-parameter
  -Wconversion
  -Wshadow
  -Wstrict-aliasing=2
  -Wnull-dereference
  $<$<CONFIG:Debug>:-O0> # Disable optimizations in Debug mode
  $<$<CONFIG:Release>:-O2> # Enable optimizations in Release mode
)

set_source_files_properties(LABSoft_GUI/LABSoft_GUI.cpp PROPERTIES COMPILE_FLAGS "-Wno-error=cast-function-type -Wno-error=shadow")

target_include_directories(LABSoft PRIVATE 
  ${FLTK_INCLUDE_DIRS}
  ../lib 
)

target_link_libraries(LABSoft 
  PRIVATE LABSoft_Libs
  PRIVATE ${FLTK_LIBRARIES}
  PRIVATE Threads::Threads
)
