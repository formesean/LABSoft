#ifndef LABS_GUI_VALUES_H
#define LABS_GUI_VALUES_H

#include <string>
#include <unordered_map>

#include "LAB_Enumerations.h"

namespace LABS_GUI_VALUES
{
  namespace MAIN
  {
    static std::unordered_map<std::string, LABE::LAB::INSTRUMENT> INSTRUMENT = 
    {
      {"Oscilloscope",            LABE::LAB::INSTRUMENT::OSCILLOSCOPE},
      {"Voltmeter",               LABE::LAB::INSTRUMENT::VOLTMETER},
      {"Function Generator",      LABE::LAB::INSTRUMENT::FUNCTION_GENERATOR},
      {"Logic Analyzer",          LABE::LAB::INSTRUMENT::LOGIC_ANALYZER},
      {"Digital Circuit Checker", LABE::LAB::INSTRUMENT::DIGITAL_CIRCUIT_CHECKER},
      {"LABChecker - Digital",    LABE::LAB::INSTRUMENT::LABCHECKER_DIGITAL}
    };
  };

  namespace OSC
  {
    static std::unordered_map<std::string, LABE::OSC::SCALING> SCALING = 
    {
      {"x2"     , LABE::OSC::SCALING::DOUBLE},
      {"x1"     , LABE::OSC::SCALING::UNITY},
      {"x0.5"   , LABE::OSC::SCALING::HALF},
      {"x0.25"  , LABE::OSC::SCALING::FOURTH},
      {"x0.125" , LABE::OSC::SCALING::EIGHTH}
    };

    static std::unordered_map<LABE::OSC::SCALING, std::string> SCALING_s = 
    {
      {LABE::OSC::SCALING::DOUBLE   , "x2"},
      {LABE::OSC::SCALING::UNITY    , "x1"},
      {LABE::OSC::SCALING::HALF     , "x0.5"},
      {LABE::OSC::SCALING::FOURTH   , "x0.25"},
      {LABE::OSC::SCALING::EIGHTH   , "x0.125"}
    };

    static std::unordered_map<std::string, LABE::OSC::MODE> MODE = 
    {
      {"Screen",    LABE::OSC::MODE::SCREEN},
      {"Repeated",  LABE::OSC::MODE::REPEATED}
    };

    static std::unordered_map<LABE::OSC::MODE, std::string> MODE_s = 
    {
      {LABE::OSC::MODE::SCREEN,   "Screen"},
      {LABE::OSC::MODE::REPEATED, "Repeated"}
    };

    static std::unordered_map<std::string, LABE::OSC::TRIG::MODE> TRIGGER_MODE = 
    {
      {"None",    LABE::OSC::TRIG::MODE::NONE},
      {"Normal",  LABE::OSC::TRIG::MODE::NORMAL},
      {"Auto",    LABE::OSC::TRIG::MODE::AUTO}
    };

    static std::unordered_map<LABE::OSC::TRIG::MODE, std::string> TRIGGER_MODE_s = 
    {
      {LABE::OSC::TRIG::MODE::NONE  , "None"},
      {LABE::OSC::TRIG::MODE::NORMAL, "Normal"},
      {LABE::OSC::TRIG::MODE::AUTO  , "Auto"}
    };

    static std::unordered_map<std::string, unsigned> TRIGGER_SOURCE = 
    {
      {"Channel 1", 0},
      {"Channel 2", 1}
    };

    static std::unordered_map<unsigned, std::string> TRIGGER_SOURCE_s = 
    {
      {0, "Channel 1"},
      {1, "Channel 2"}
    };

    static std::unordered_map<std::string, LABE::OSC::TRIG::TYPE> TRIGGER_TYPE = 
    {
      {"Edge",  LABE::OSC::TRIG::TYPE::EDGE},
      {"Level", LABE::OSC::TRIG::TYPE::LEVEL}
    };

    static std::unordered_map<LABE::OSC::TRIG::TYPE, std::string> TRIGGER_TYPE_s = 
    {
      {LABE::OSC::TRIG::TYPE::EDGE, "Edge"},
    };

    static std::unordered_map<std::string, LABE::OSC::TRIG::CND> TRIGGER_CONDITION = 
    {
      {"Rising",  LABE::OSC::TRIG::CND::RISING},
      {"Falling", LABE::OSC::TRIG::CND::FALLING},
      {"Either",  LABE::OSC::TRIG::CND::EITHER},
    };

    static std::unordered_map<LABE::OSC::TRIG::CND, std::string> TRIGGER_CONDITION_s = 
    {
      {LABE::OSC::TRIG::CND::RISING,  "Rising"},
      {LABE::OSC::TRIG::CND::FALLING, "Falling"},
      {LABE::OSC::TRIG::CND::EITHER,  "Either"},
    };

    static std::unordered_map<LABE::OSC::STATUS, std::string> STATUS_s = 
    {
      {LABE::OSC::STATUS::READY,  "Ready"},
      {LABE::OSC::STATUS::STOP,   "Stop"},
      {LABE::OSC::STATUS::AUTO,   "Auto"},
      {LABE::OSC::STATUS::DONE,   "Done"},
      {LABE::OSC::STATUS::CONFIG, "Config"},
    };
  };

  namespace FUNC_GEN
  {
    static std::unordered_map<LABE::FUNC_GEN::WAVE_TYPE, std::string> WAVE_TYPE = 
    {
      {LABE::FUNC_GEN::WAVE_TYPE::DC,       "DC"},
      {LABE::FUNC_GEN::WAVE_TYPE::SINE,     "Sine"},
      {LABE::FUNC_GEN::WAVE_TYPE::SQUARE,   "Square"},
      {LABE::FUNC_GEN::WAVE_TYPE::TRIANGLE, "Triangle"}
    };
  };

  namespace LOGAN
  {
    static std::unordered_map<std::string, LABE::LOGAN::TRIG::MODE> TRIG_MODE_s = 
    {
      {"None",    LABE::LOGAN::TRIG::MODE::NONE},
      {"Normal",  LABE::LOGAN::TRIG::MODE::NORMAL},
      {"Auto",    LABE::LOGAN::TRIG::MODE::AUTO}
    };
  };

  namespace LOGAN_DISPLAY
  {
    static std::unordered_map<std::string, LABE::LOGAN::TRIG::CND> TRIG_CND_s = 
    {
      {"X Ignore",          LABE::LOGAN::TRIG::CND::IGNORE},
      {"0 Low",             LABE::LOGAN::TRIG::CND::LOW},
      {"1 High",            LABE::LOGAN::TRIG::CND::HIGH},
      {"\342\226\262 Rise", LABE::LOGAN::TRIG::CND::RISING_EDGE},
      {"\342\226\274 Fall", LABE::LOGAN::TRIG::CND::FALLING_EDGE},
      {"\342\206\225 Edge", LABE::LOGAN::TRIG::CND::EITHER_EDGE},
    };
  };
}

#endif