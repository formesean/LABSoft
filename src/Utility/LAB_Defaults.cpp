#include "LAB_Defaults.h"

const std::unordered_map <LABE::OSC::SCALING, double> LABD::OSC::VERTICAL_OFFSET_CORRECTOR = 
{
  {LABE::OSC::SCALING::DOUBLE , 0.0},
  {LABE::OSC::SCALING::UNITY  , 0.0},
  {LABE::OSC::SCALING::HALF   , 0.0},
  {LABE::OSC::SCALING::FOURTH , 0.0},
  {LABE::OSC::SCALING::EIGHTH , 0.0},
};

const std::unordered_map <LABE::OSC::SCALING, double> LABD::OSC::SCALING_CORRECTOR_TO_UNITY = 
{
  {LABE::OSC::SCALING::DOUBLE , 0.5},
  {LABE::OSC::SCALING::UNITY  , 1.0},
  {LABE::OSC::SCALING::HALF   , 2.0},
  {LABE::OSC::SCALING::FOURTH , 4.0},
  {LABE::OSC::SCALING::EIGHTH , 8.0},
};

const std::unordered_map <LABE::OSC::SCALING, double> LABD::OSC::SCALING_CORRECTOR_TO_ACTUAL = 
{
  {LABE::OSC::SCALING::DOUBLE , 2.0},
  {LABE::OSC::SCALING::UNITY  , 1.0},
  {LABE::OSC::SCALING::HALF   , 0.5},
  {LABE::OSC::SCALING::FOURTH , 0.25},
  {LABE::OSC::SCALING::EIGHTH , 0.125},
};  