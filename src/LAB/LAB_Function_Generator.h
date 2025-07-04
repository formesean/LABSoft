#ifndef LAB_FUNCTION_GENERATOR_H
#define LAB_FUNCTION_GENERATOR_H

#include "LAB_Module.h"
#include "../Utility/LAB_Constants.h"
#include "../Utility/LAB_Definitions.h"
#include "../Utility/LAB_Enumerations.h"
#include "../LAB_Libraries/LAB_AD9833.h"
#include "../LAB_Libraries/LAB_MCP4XXX.h"

class LAB_Function_Generator : public LAB_Module
{
  private: 
    LAB_AD9833  m_func_gen_ic       [LABC::FUNC_GEN::NUMBER_OF_CHANNELS];
    // LAB_MCP4XXX m_digipot_amplitude [2 * LABC::FUNC_GEN::NUMBER_OF_CHANNELS];
    // LAB_MCP4XXX m_digipot_offset    [LABC::FUNC_GEN::NUMBER_OF_CHANNELS];
  
  private:
    void init_gpio_pins ();
    void set_Rf         (unsigned channel, double value);
    
  public:
    LAB_Parent_Data_Function_Generator m_parent_data;

  public:
    LAB_Function_Generator      (LAB& _LAB);
   ~LAB_Function_Generator      ();

    void run                    (unsigned channel);
    void stop                   (unsigned channel);
    void wave_type              (unsigned channel, LABE::FUNC_GEN::WAVE_TYPE value);
    void amplitude              (unsigned channel, double value);
    void frequency              (unsigned channel, double value);
    void period                 (unsigned channel, double value);
    void phase                  (unsigned channel, double value);
    void vertical_offset        (unsigned channel, double value);
    void set_hw_amplitude       (unsigned channel, double value);
    void set_hw_vertical_offset (unsigned channel, double value);

    bool                      is_running      ();
    LABE::FUNC_GEN::WAVE_TYPE wave_type       (unsigned channel);
    double                    amplitude       (unsigned channel);
    double                    frequency       (unsigned channel);
    double                    period          (unsigned channel);
    double                    phase           (unsigned channel);
    double                    vertical_offset (unsigned channel);
};

#endif
