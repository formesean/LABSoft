#include "LAB_Function_Generator.h"

#include "LAB.h"

LAB_Function_Generator:: 
LAB_Function_Generator (LAB& _LAB)
  : LAB_Module (_LAB),
    m_func_gen_ic       {LAB_AD9833   (LABC::PIN::FG::PWG_CS, 
                                        LABC::PIN::FG::PWG_MISO,
                                        LABC::PIN::FG::PWG_MOSI,
                                        LABC::PIN::FG::PWG_SCLK,
                                        LABC::FUNC_GEN::IC_FREQUENCY)}
    // m_digipot_amplitude {LAB_MCP4XXX  (LAB_MCP4XXX::PART_NUMBER::MCP4161,
    //                                     LAB_MCP4XXX::RESISTANCE_VERSION::_503,
    //                                     0),
    //                      LAB_MCP4XXX  (LAB_MCP4XXX::PART_NUMBER::MCP4161,
    //                                     LAB_MCP4XXX::RESISTANCE_VERSION::_503,
    //                                     1)},
    // m_digipot_offset    {LAB_MCP4XXX  (LAB_MCP4XXX::PART_NUMBER::MCP4161,
    //                                     LAB_MCP4XXX::RESISTANCE_VERSION::_103,
    //                                     2)}
{
  init_gpio_pins ();
}

LAB_Function_Generator:: 
~LAB_Function_Generator ()
{
  
}

void LAB_Function_Generator:: 
init_gpio_pins ()
{
  // m_LAB.rpi ().gpio.set (LABC::PIN::FG::DPOTS_SCLK,  AP::GPIO::FUNC::ALT4,   AP::GPIO::PULL::OFF);
  // m_LAB.rpi ().gpio.set (LABC::PIN::FG::DPOTS_MOSI,  AP::GPIO::FUNC::ALT4,   AP::GPIO::PULL::OFF);
  // m_LAB.rpi ().gpio.set (LABC::PIN::FG::DPOTS_MISO,  AP::GPIO::FUNC::ALT4,   AP::GPIO::PULL::DOWN);
  // m_LAB.rpi ().gpio.set (LABC::PIN::FG::DPOT0_CS,    AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::OFF,  1);
  // m_LAB.rpi ().gpio.set (LABC::PIN::FG::DPOT1_CS,    AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::OFF,  1);
  // m_LAB.rpi ().gpio.set (LABC::PIN::FG::DPOT2_CS,    AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::OFF,  1);
}

void LAB_Function_Generator::
set_Rf (unsigned  channel,
        double    value)
{
  // m_digipot_amplitude[2 * channel]      .resistance (0, value / 2.0);
  // m_digipot_amplitude[(2 * channel) + 1].resistance (0, value / 2.0);
}

void LAB_Function_Generator:: 
run (unsigned channel)
{
  m_func_gen_ic[channel].run ();

  m_parent_data.channel_data[channel].is_enabled = true;
}

void LAB_Function_Generator:: 
stop (unsigned channel)
{
  m_func_gen_ic[channel].stop ();

  m_parent_data.channel_data[channel].is_enabled = false;
}

void LAB_Function_Generator:: 
wave_type (unsigned                  channel,
           LABE::FUNC_GEN::WAVE_TYPE wave_type)
{
  AD9833::WAVE_TYPE type;
  
  switch (wave_type)
  {
    case (LABE::FUNC_GEN::WAVE_TYPE::SINE):
    {
      type = AD9833::WAVE_TYPE::SINE;
      break;
    }

    case (LABE::FUNC_GEN::WAVE_TYPE::TRIANGLE):
    {
      type = AD9833::WAVE_TYPE::TRIANGLE;
      break;
    }

    case (LABE::FUNC_GEN::WAVE_TYPE::SQUARE):
    {
      type = AD9833::WAVE_TYPE::SQUARE;
      break;
    }

    case (LABE::FUNC_GEN::WAVE_TYPE::DC):
    {
      this->wave_type (channel, LABE::FUNC_GEN::WAVE_TYPE::SINE);

      type = AD9833::WAVE_TYPE::DC;
      break;
    }
  }

  m_parent_data.channel_data[channel].wave_type = wave_type;
  m_func_gen_ic[channel].wave_type (type);
}

void LAB_Function_Generator:: 
amplitude (unsigned channel,
           double   value)
{
  m_parent_data.channel_data[channel].amplitude = value;

  // set_hw_amplitude (channel, value);
}

void LAB_Function_Generator:: 
frequency (unsigned channel,
           double   value)
{
  m_parent_data.channel_data[channel].frequency = value;
  m_parent_data.channel_data[channel].period    = 1.0 / value;

  m_func_gen_ic[channel].frequency (value);
}

void LAB_Function_Generator:: 
period (unsigned channel,
        double   value)
{
  m_parent_data.channel_data[channel].period    = value;
  m_parent_data.channel_data[channel].frequency = 1.0 / value;

  m_func_gen_ic[channel].period (value);
}

void LAB_Function_Generator:: 
phase (unsigned channel,
       double   value)
{
  m_parent_data.channel_data[channel].phase = value;

  m_func_gen_ic[channel].phase (value);
}

void LAB_Function_Generator:: 
vertical_offset (unsigned channel,
                 double   value)
{
  m_parent_data.channel_data[channel].vertical_offset = value;

  // set_hw_vertical_offset (channel, value);
}

void LAB_Function_Generator:: 
set_hw_amplitude (unsigned channel,
                  double   value)
{
  // https://en.wikibooks.org/wiki/Electronics/Electronics_Formulas/Op_Amp_Configurations
  // Inverting amplifier. Vout = (-Rf/Rin) * Vin
  // Rf = digipot0 + digipot1
  // Rin = 1000

  // double Rf = (value / (m_func_gen_ic[channel].amplitude ())) * 
  //               LABC::FUNC_GEN::R1_RESISTANCE;

  // std::cout << "value: " << value << std::endl;
  // std::cout << "amp: " << m_func_gen_ic[channel].amplitude () << std::endl;
  // std::cout << "Rf: " << Rf << std::endl;

  // if (Rf < (m_digipot_amplitude[0].min_resistance () * 2)) // 2 digipots per channel
  // {
  //   Rf = (m_digipot_amplitude[0].min_resistance () * 2);
  // }
  
  // set_Rf (channel, Rf);

  // m_parent_data.channel_data[channel].Rf = Rf;
  m_parent_data.channel_data[channel].amplitude = value;
}

void LAB_Function_Generator:: 
set_hw_vertical_offset (unsigned channel,
                        double   value)
{
  // https://en.wikipedia.org/wiki/Operational_amplifier_applications#Differential_amplifier_.28difference_amplifier.29
  //
  // double numerator = LABC::FUNC_GEN::ROFF_RESISTANCE * 
  //                     (value + ((m_parent_data.channel_data[channel].Rf * 
  //                     LABC::FUNC_GEN::V1) / LABC::FUNC_GEN::R1_RESISTANCE));

  // double denominator = LABC::FUNC_GEN::V2 * ((LABC::FUNC_GEN::R1_RESISTANCE * 
  //                       m_parent_data.channel_data[channel].Rf) / 
  //                       LABC::FUNC_GEN::R1_RESISTANCE);

  // double Rg = numerator / denominator;

  // m_digipot_offset[channel].resistance (0, Rg);

  // std::cout << "vertical offset resistance: " << value << std::endl;

  // m_digipot_offset[channel].resistance (0, value);
}

bool LAB_Function_Generator::
is_running ()
{
  return (m_parent_data.has_enabled_channels ());
}

LABE::FUNC_GEN::WAVE_TYPE LAB_Function_Generator::
wave_type (unsigned channel)
{
  return (m_parent_data.channel_data[channel].wave_type);
}

double LAB_Function_Generator::
amplitude (unsigned channel)
{
  return (m_parent_data.channel_data[channel].amplitude);
}

double LAB_Function_Generator::
frequency (unsigned channel)
{
  return (m_parent_data.channel_data[channel].frequency);
}

double LAB_Function_Generator::
period (unsigned channel)
{
  return (m_parent_data.channel_data[channel].period);
}

double LAB_Function_Generator::
phase (unsigned channel)
{
  return (m_parent_data.channel_data[channel].phase);
}

double LAB_Function_Generator::
vertical_offset (unsigned channel)
{
  return (m_parent_data.channel_data[channel].vertical_offset);
}

// EOF