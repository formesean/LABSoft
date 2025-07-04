#ifndef LAB_DEFAULTS_H
#define LAB_DEFAULTS_H

#include "LAB_Constants.h"
#include "LAB_Enumerations.h"

namespace LABD
{
  namespace OSC
  {
    // ADC info and conversions
    constexpr double                ADC_REFERENCE_VOLTAGE         = 5.0; // 5 volts
    constexpr double                CONVERSION_REFERENCE_VOLTAGE  = ADC_REFERENCE_VOLTAGE / 2.0;
    constexpr double                CONVERSION_CONSTANT           = CONVERSION_REFERENCE_VOLTAGE / 
                                                                    ((LABC::OSC::ADC_RESOLUTION_INT - 1) >> 1);
    // vertical
    constexpr bool                  IS_ENABLED                    = true;
    constexpr double                VOLTAGE_PER_DIVISION          = 1.0;
    constexpr double                VERTICAL_OFFSET               = 0.0;
    constexpr LABE::OSC::SCALING    SCALING                       = LABE::OSC::SCALING::HALF;
    constexpr LABE::OSC::COUPLING   COUPLING                      = LABE::OSC::COUPLING::DC;

    // horizontal
    constexpr unsigned              SAMPLES                       = LABC::OSC::NUMBER_OF_SAMPLES;
    constexpr double                TIME_PER_DIVISION             = 0.005; // 5 ms/div
    constexpr double                SAMPLING_RATE                 = SAMPLES / (TIME_PER_DIVISION * LABC::OSC::DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double                HORIZONTAL_OFFSET             = 0.0;
    
    // mode
    constexpr LABE::OSC::MODE       MODE                          = (TIME_PER_DIVISION < LABC::OSC::MIN_TIME_PER_DIVISION_SCREEN) ? 
                                                                    LABE::OSC::MODE::REPEATED : LABE::OSC::MODE::SCREEN;
    
    // trigger
    constexpr LABE::OSC::TRIG::MODE TRIGGER_MODE                  = LABE::OSC::TRIG::MODE::NONE;
    constexpr unsigned              TRIGGER_SOURCE                = 0; // Channel 1
    constexpr LABE::OSC::TRIG::TYPE TRIGGER_TYPE                  = LABE::OSC::TRIG::TYPE::EDGE;
    constexpr LABE::OSC::TRIG::CND  TRIGGER_CONDITION             = LABE::OSC::TRIG::CND::RISING;
    constexpr double                TRIGGER_LEVEL                 = 0.0;
    
    constexpr double                MAX_OSC_HARDWARE_TRIGGER_LEVEL  = CONVERSION_REFERENCE_VOLTAGE;
    constexpr double                MIN_OSC_HARDWARE_TRIGGER_LEVEL  = -1 * CONVERSION_REFERENCE_VOLTAGE;
  
    // calibration data
    extern const std::unordered_map <LABE::OSC::SCALING, double> VERTICAL_OFFSET_CORRECTOR;
    extern const std::unordered_map <LABE::OSC::SCALING, double> SCALING_CORRECTOR_TO_UNITY;
    extern const std::unordered_map <LABE::OSC::SCALING, double> SCALING_CORRECTOR_TO_ACTUAL; 
  };

  namespace OSC_DISPLAY
  {
    constexpr unsigned int BACKGROUND_COLOR = FL_BLACK; 
  };

  namespace LOGAN 
  {
    // Horizontal
    constexpr double    SAMPLING_RATE                   = 50; // Hz
    constexpr double    SAMPLING_PERIOD                 = 1.0 / SAMPLING_RATE;
    constexpr unsigned  SAMPLES                         = LABC::LOGAN::MAX_SAMPLES;
    constexpr double    TIME_PER_DIVISION               = SAMPLES / (SAMPLING_RATE * LABC::LOGAN::DISPLAY_NUMBER_OF_COLUMNS);  
    constexpr double    HORIZONTAL_OFFSET               = 0.0;
    
    // mode
    constexpr LABE::LOGAN::MODE MODE                    = (TIME_PER_DIVISION < LABC::LOGAN::MIN_TIME_PER_DIVISION_SCREEN) ? 
                                                          LABE::LOGAN::MODE::REPEATED : LABE::LOGAN::MODE::SCREEN;
    
    // trigger
    constexpr LABE::LOGAN::TRIG::CND TRIGGER_CONDITION  = LABE::LOGAN::TRIG::CND::IGNORE;
    constexpr LABE::LOGAN::TRIG::MODE TRIGGER_MODE      = LABE::LOGAN::TRIG::MODE::NONE;
    constexpr double CHECK_TRIGGER_SLEEP_PERIOD         = SAMPLING_RATE / 5.0;
  };

};

#endif