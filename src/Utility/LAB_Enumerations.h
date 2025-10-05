#ifndef LAB_ENUMERATIONS_H
#define LAB_ENUMERATIONS_H

#include <cstdint>
#include <unordered_map>
#include <variant>
#include <string_view>
#include <string>

namespace LABE
{
  namespace LAB
  {
    enum class INSTRUMENT
    {
      OSCILLOSCOPE,
      VOLTMETER,
      OHMMETER,
      FUNCTION_GENERATOR,
      POWER_SUPPLY,
      LOGIC_ANALYZER,
      DIGITAL_CIRCUIT_CHECKER,
      LABCHECKER_DIGITAL,
      ANALOG_CIRCUIT_CHECKER,
      LABCHECKER_ANALOG
    };
  };

  namespace DISPLAY
  {
    enum class COLOR : uint32_t
    {
      RED     = 0xFF000000,
      YELLOW  = 0xFFFF0000,
      GREEN   = 0x00FF0000
    };
  };

  namespace OSC
  {
    namespace TRIG
    {
      enum class MODE
      {
        NONE,
        NORMAL,
        AUTO
      };

      enum class TYPE
      {
        LEVEL,
        EDGE
      };

      enum class CND
      {
        RISING,
        FALLING,
        EITHER
      };
    };

    enum class BUFFER_COUNT
    {
      SINGLE,
      DOUBLE
    };

    enum class MODE
    {
      REPEATED,
      SCREEN,
      RECORD
    };

    enum class SCALING
    {
      DOUBLE,     // attenuator stage + x4 scaling at mux stage
      UNITY,      // software implemented, half scaling multiplied by 2
      HALF,       // attenuator stage + x1 scaling at mux stage
      FOURTH,     // attenuator stage + x0.5 scaling at mux stage0
      EIGHTH      // attenuator stage + x0.25 scaling at mux stage
    };

    enum class COUPLING
    {
      DC = 0,
      AC = 1
    };

    enum class STATUS
    {
      READY,
      STOP,
      CONFIG,
      AUTO,
      ARMED,
      TRIGGERED,
      DONE
    };

  };

  namespace FUNC_GEN
  {
    enum class WAVE_TYPE
    {
      SINE,
      TRIANGLE,
      SQUARE,
      SQUARE_HALF,
      SQUARE_FULL,
      DC
    };
  }

  namespace LOGAN
  {
    namespace TRIG
    {
      enum class MODE
      {
        NONE,
        NORMAL,
        AUTO
      };

      enum class CND
      {
        IGNORE,
        LOW,
        HIGH,
        RISING_EDGE,
        FALLING_EDGE,
        EITHER_EDGE
      };
    };

    enum class BUFFER_COUNT
    {
      SINGLE,
      DOUBLE
    };

    enum class MODE
    {
      REPEATED,
      SCREEN,
      RECORD
    };

    enum class STATUS
    {
      READY,
      STOP,
      CONFIG,
      AUTO,
      ARMED,
      TRIGGERED,
      DONE
    };
  }

  namespace SNM
  {
    enum class FOCUS_LEVEL
    {
      MENU,
      TAB,
      GROUP,
      WIDGET
    };

    enum class ACTION_TYPE
    {
      GOTO,
      RUN,
      UNKNOWN
    };

    inline const std::unordered_map<std::string_view, LABE::LAB::INSTRUMENT> tab_label_to_id =
    {
      { "Oscilloscope",            LABE::LAB::INSTRUMENT::OSCILLOSCOPE },
      { "Voltmeter",               LABE::LAB::INSTRUMENT::VOLTMETER },
      { "Ohmmeter",                LABE::LAB::INSTRUMENT::OHMMETER },
      { "Function Generator",      LABE::LAB::INSTRUMENT::FUNCTION_GENERATOR },
      { "Power Supply",            LABE::LAB::INSTRUMENT::POWER_SUPPLY },
      { "Logic Analyzer",          LABE::LAB::INSTRUMENT::LOGIC_ANALYZER },
      { "Digital Circuit Checker", LABE::LAB::INSTRUMENT::DIGITAL_CIRCUIT_CHECKER },
      { "LABChecker - Digital",    LABE::LAB::INSTRUMENT::LABCHECKER_DIGITAL },
      { "Analog Circuit Checker",  LABE::LAB::INSTRUMENT::ANALOG_CIRCUIT_CHECKER },
      { "LABChecker - Analog",     LABE::LAB::INSTRUMENT::LABCHECKER_ANALOG }
    };

    inline const std::unordered_map<LABE::LAB::INSTRUMENT, std::string_view> tab_id_to_label =
    {
      { LABE::LAB::INSTRUMENT::OSCILLOSCOPE,            "Oscilloscope" },
      { LABE::LAB::INSTRUMENT::VOLTMETER,               "Voltmeter" },
      { LABE::LAB::INSTRUMENT::OHMMETER,                "Ohmmeter" },
      { LABE::LAB::INSTRUMENT::FUNCTION_GENERATOR,      "Function Generator" },
      { LABE::LAB::INSTRUMENT::POWER_SUPPLY,            "Power Supply" },
      { LABE::LAB::INSTRUMENT::LOGIC_ANALYZER,          "Logic Analyzer" },
      { LABE::LAB::INSTRUMENT::DIGITAL_CIRCUIT_CHECKER, "Digital Circuit Checker" },
      { LABE::LAB::INSTRUMENT::LABCHECKER_DIGITAL,      "LABChecker - Digital" },
      { LABE::LAB::INSTRUMENT::ANALOG_CIRCUIT_CHECKER,  "Analog Circuit Checker" },
      { LABE::LAB::INSTRUMENT::LABCHECKER_ANALOG,       "LABChecker - Analog" }
    };

    struct MACRO_KEY_CONFIG
    {
      ACTION_TYPE action;
      std::variant<LABE::LAB::INSTRUMENT, std::string> target;
    };
  }
};

#endif
