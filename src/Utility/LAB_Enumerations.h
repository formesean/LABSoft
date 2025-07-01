#ifndef LAB_ENUMERATIONS_H
#define LAB_ENUMERATIONS_H

namespace LABE
{
  namespace LAB
  {
    enum class INSTRUMENT
    {
      OSCILLOSCOPE,
      VOLTMETER,
      FUNCTION_GENERATOR,
      LOGIC_ANALYZER,
      DIGITAL_CIRCUIT_CHECKER,
      LABCHECKER_DIGITAL
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
        AUTO,
        NORMAL
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
        AUTO,
        NORMAL
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
};

#endif