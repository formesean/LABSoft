#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <string>
#include <unordered_map>

#include <FL/fl_draw.H>
#include <FL/Enumerations.H>

#include "../../lib/AikaPi/AikaPi.h"
#include "../../lib/AD9833/AD9833.h"
#include "../../lib/MCP4XXX/MCP4XXX.h"
#include "../../lib/MCP23S17/MCP23S17.h"

#include "LAB_Enumerations.h"

namespace LABC
{
  namespace LAB
  {
    constexpr uint32_t GPU_CORE_CLOCK_FREQ = 250'000'000.0; // 250 MHz
  };

  namespace LABSOFT
  {
    constexpr double EPSILON = 1e-12;

    constexpr float DISPLAY_UPDATE_RATE = 1.0 / 15.0; // ideally 20 FPS

    enum class FL_TAB_GROUP_COLOR : uint32_t
    {
      GREEN   = 63,
      DEFAULT = 54
    };

    constexpr const char* DIGITAL_CIRCUIT_CHECKER_FILENAME_EXTENSION  = ".labdcc";
    constexpr const char* CALIBRATION_FILENAME_EXTENSION              = ".labcalibration";
    constexpr const char* DEFAULT_CALIBRATION_FILE_PATH               = "../data/default.labcalibration";
    constexpr const char* FACTORY_CALIBRATION_FILE_PATH               = "../data/FACTORY.labcalibration";
  }

  namespace PIN
  {
    // Yes, GPIO pins 5 and 19 are used in Function Generator.
    // These are not used in Function Gen MISOs for digipots.
    // I'm relying on the fact that LAB_Logic_Analyzer is the
    // last to be initialized, after Function Generator.
    // So LAB_Logic_Analyzer's GPIO pin init is called last.

    constexpr unsigned LOGAN[] = {17, 18, 5, 26};
  };

  namespace DMA
  {
    namespace TI
    {
      constexpr uint32_t OSC_PWM_PACING  = (AP::DMA::TI_DATA::PERMAP (AP::DMA::PERIPH_DREQ::PWM))
                                            | AP::DMA::TI_DATA::DEST_DREQ
                                            | AP::DMA::TI_DATA::WAIT_RESP
                                            | AP::DMA::TI_DATA::INTEN;

      constexpr uint32_t OSC_TX          = (AP::DMA::TI_DATA::PERMAP (AP::DMA::PERIPH_DREQ::SPI_TX))
                                            | AP::DMA::TI_DATA::DEST_DREQ
                                            | AP::DMA::TI_DATA::WAIT_RESP
                                            | AP::DMA::TI_DATA::SRC_INC
                                            | AP::DMA::TI_DATA::INTEN;

      constexpr uint32_t OSC_RX          = (AP::DMA::TI_DATA::PERMAP (AP::DMA::PERIPH_DREQ::SPI_RX))
                                            | AP::DMA::TI_DATA::SRC_DREQ
                                            | AP::DMA::TI_DATA::DEST_INC
                                            | AP::DMA::TI_DATA::WAIT_RESP;

      constexpr uint32_t LOGAN_STORE     = (AP::DMA::TI_DATA::PERMAP (AP::DMA::PERIPH_DREQ::PWM))
                                            | AP::DMA::TI_DATA::SRC_DREQ
                                            | AP::DMA::TI_DATA::DEST_INC
                                            | AP::DMA::TI_DATA::WAIT_RESP;
    };

    namespace CHAN
    {
      // DMA channels in-use after reboot (3B+): 2, 3, 4, 6

      constexpr unsigned PWM_PACING       = 7;
      constexpr unsigned OSC_RX           = 8;
      constexpr unsigned OSC_TX           = 9;
      constexpr unsigned LOGAN_GPIO_STORE = 10;
    };
  };

  namespace SPI
  {
    // Please note that LABSoft requires the core clock to be fixed to
    // 250 MHz in boot/config.txt. This affects the divisor setting.
    constexpr double FREQUENCY = 12'500'000;
  }

  namespace PWM
  {
    constexpr int DMA_PACING_CHAN = 0;
  };

  namespace CLKMAN
  {
    constexpr double FREQUENCY = 100'000'000.0;
  };

  namespace DISPLAY
  {
    constexpr int DISPLAY_STATUS_BOX_TYPE = FL_BORDER_FRAME;

    static constexpr uint32_t CHAN_COLORS[] = {
      3, // yellow
      6, // cyan
      2, // green
      5, // purple
    };
  };

  namespace OSC
  {
    // channels and buffer size
    constexpr unsigned  NUMBER_OF_CHANNELS              = 2;
    constexpr unsigned  NUMBER_OF_SAMPLES               = 2000;
    constexpr unsigned  SAMPLE_SIZE                     = sizeof (uint32_t);
    constexpr unsigned  NUMBER_OF_BUFFERS               = 2;
    constexpr unsigned  BUFFER_SIZE                     = SAMPLE_SIZE * NUMBER_OF_SAMPLES;
    constexpr unsigned  VC_MEM_SIZE                     = AP::RPI::PAGE_SIZE + (NUMBER_OF_CHANNELS * NUMBER_OF_BUFFERS * BUFFER_SIZE);

    // ADC info and conversions
    constexpr unsigned  ADC_RESOLUTION_BITS             = 12; // MCP33111
    constexpr unsigned  ADC_RESOLUTION_INT              = 1 << ADC_RESOLUTION_BITS;
    constexpr unsigned  ADC_SPI_CHIP_ENABLE             = 0;

    constexpr uint32_t  RAW_DATA_MASK                   = 0xF0FF;
    constexpr uint32_t  RAW_DATA_BIT_SHIFT_COUNT        = (SAMPLE_SIZE * 8) / NUMBER_OF_CHANNELS;
    constexpr uint32_t  RAW_DATA_POST_SHIFT_MASK        = (1 << RAW_DATA_BIT_SHIFT_COUNT) - 1;

    // vertical
    constexpr unsigned  DISPLAY_NUMBER_OF_ROWS          = 10;
    constexpr double    DISPLAY_NUMBER_OF_ROWS_HALF     = DISPLAY_NUMBER_OF_ROWS / 2.0;
    constexpr double    MAX_VOLTAGE_PER_DIVISION        = 5.0;    // 5 V
    constexpr double    MIN_VOLTAGE_PER_DIVISION        = 0.0001; // 100 uV
    constexpr double    MAX_VERTICAL_OFFSET             = MAX_VOLTAGE_PER_DIVISION *
                                                          (DISPLAY_NUMBER_OF_ROWS / 2.0); // 25 V
    constexpr double    MIN_VERTICAL_OFFSET             = -1.0 * MAX_VERTICAL_OFFSET; // -25 V

    // horizontal
    constexpr unsigned  DISPLAY_NUMBER_OF_COLUMNS       = 10;
    constexpr double    MAX_SAMPLING_RATE               = 200'000.0;              // Hz
    constexpr double    MIN_SAMPLING_RATE               = 1.0;                    // Hz
    constexpr double    MAX_SAMPLES                     = NUMBER_OF_SAMPLES;
    constexpr double    MAX_SAMPLES_RECORDING           = 1'000'000.0;
    constexpr double    MIN_SAMPLES                     = 20;
    constexpr double    MIN_SAMPLES_DISPLAY             = 1;
    constexpr double    MAX_TIME_PER_DIVISION           = MAX_SAMPLES / (MIN_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double    MAX_TIME_PER_DIVISION_RECORDING = MAX_SAMPLES_RECORDING / (MIN_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double    MIN_TIME_PER_DIVISION           = MIN_SAMPLES_DISPLAY / (MAX_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double    MIN_TIME_PER_DIVISION_SCREEN    = 1.0 / DISPLAY_NUMBER_OF_COLUMNS;
    constexpr double    MIN_TIME_PER_DIVISION_NO_ZOOM   = MAX_SAMPLES / (MAX_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double    MAX_HORIZONTAL_OFFSET           = MAX_SAMPLES / MIN_SAMPLING_RATE;
    constexpr double    MIN_HORIZONTAL_OFFSET           = (-1) * MAX_HORIZONTAL_OFFSET;

    // trigger
    constexpr double    MAX_TRIGGER_LEVEL               = MAX_VERTICAL_OFFSET;
    constexpr double    MIN_TRIGGER_LEVEL               = MIN_VERTICAL_OFFSET;
    constexpr double    FIND_TRIGGER_TIMEOUT            = 2; // seconds

    // calibration
    constexpr unsigned  NUMBER_OF_CALIBRATION_PARAMETERS  = 13;
  };

  namespace OSC_DISPLAY
  {
    constexpr unsigned  NUMBER_OF_CHANNELS                  = OSC::NUMBER_OF_CHANNELS;
    constexpr unsigned  NUMBER_OF_ROWS                      = OSC::DISPLAY_NUMBER_OF_ROWS;
    constexpr unsigned  NUMBER_OF_COLUMNS                   = OSC::DISPLAY_NUMBER_OF_COLUMNS;
    constexpr unsigned  NUMBER_OF_ROWS_HALF                 = NUMBER_OF_ROWS / 2;
    constexpr unsigned  NUMBER_OF_COLUMNS_HALF              = NUMBER_OF_COLUMNS / 2;
    constexpr unsigned  MID_COLUMN_INDEX                    = (NUMBER_OF_COLUMNS / 2) - 1;
    constexpr unsigned  MID_ROW_INDEX                       = (NUMBER_OF_ROWS / 2) - 1;
    constexpr unsigned  NUMBER_OF_MINOR_TICKS               = 10;
    constexpr unsigned  NUMBER_OF_MINOR_TICKS_HALF          = NUMBER_OF_MINOR_TICKS / 2;
    constexpr unsigned  X_AXIS_MAJOR_TICK_LENGTH            = 20;
    constexpr unsigned  X_AXIS_SEMI_MAJOR_TICK_LENGTH       = X_AXIS_MAJOR_TICK_LENGTH / 2;
    constexpr unsigned  X_AXIS_MINOR_TICK_LENGTH            = X_AXIS_SEMI_MAJOR_TICK_LENGTH / 2;
    constexpr unsigned  Y_AXIS_MAJOR_TICK_LENGTH            = 12;
    constexpr unsigned  Y_AXIS_SEMI_MAJOR_TICK_LENGTH       = Y_AXIS_MAJOR_TICK_LENGTH / 2;
    constexpr unsigned  Y_AXIS_MINOR_TICK_LENGTH            = Y_AXIS_SEMI_MAJOR_TICK_LENGTH / 2;
    constexpr unsigned  SAMPLE_MARKING_THRESHOLD            = 50;
    constexpr unsigned  SAMPLE_MARKING_AMPLITUDE            = 6;
    constexpr unsigned  SAMPLE_MARKING_THICKNESS            = 2;
    constexpr unsigned  INTERNAL_DISPLAY_LEFT_MARGIN        = 80;
    constexpr unsigned  INTERNAL_DISPLAY_TOP_MARGIN         = 20;
    constexpr unsigned  INTERNAL_DISPLAY_RIGHT_MARGIN       = 10;
    constexpr unsigned  INTERNAL_DISPLAY_BOTTOM_MARGIN      = 20;
    constexpr unsigned  AXIS_LABEL_SIZE                     = 9;
    constexpr unsigned  TOP_INFO_LABEL_SIZE                 = 12;
    constexpr unsigned  Y_AXIS_LABEL_HORIZONTAL_INTRASPACE  = 40;
    constexpr unsigned  Y_AXIS_UNIT_LABEL_SIZE              = 9;
    constexpr unsigned  SLIDER_WIDTH                        = 10;
    constexpr unsigned  STATUS_HEIGHT                       = 20;
    constexpr unsigned  STATUS_WIDTH                        = 80;
    constexpr uint32_t  GRID_COLOR                          = 46; // gray
    constexpr uint32_t  BACKGROUND_COLOR                    = FL_BLACK;
    constexpr uint32_t  X_AXIS_LABEL_COLOR                  = FL_WHITE;

    static constexpr uint32_t CHANNEL_COLORS[] = {
      3, // yellow
      6, // cyan
      2, // green
      5, // purple
    };
  };

  namespace OSC_DISPLAY_GROUP
  {
    constexpr int Y_LABEL_INTERSPACE          = 40; // spacing between columns of y-axis labels
    constexpr int Y_LABEL_UNIT_LEFT_MARGIN    = 10;
    constexpr int Y_LABEL_UNIT_BOTTOM_MARGIN  = 17; // padding of voltage unit of y-axis labels from top of grid
    constexpr int X_LABEL_INTRASPACE          = 18;
    constexpr int AXIS_LABEL_SIZE             = 9;

    constexpr int X_LABEL_COLOR               = FL_WHITE;
    constexpr int Y_LABEL_COLOR               = FL_FOREGROUND_COLOR;
    constexpr int DEFAULT_LABEL_COLOR         = FL_WHITE;
    constexpr int BACKGROUND_COLOR            = FL_BLACK;
  };

  namespace VOLTMETER
  {
    constexpr unsigned            NUMBER_OF_CHANNELS  = OSC::NUMBER_OF_CHANNELS;
    constexpr double              SAMPLING_RATE       = 40'000.0; // Hz
    constexpr LABE::OSC::SCALING  SCALING             = LABE::OSC::SCALING::FOURTH;
  };

  namespace OHMMETER
  {
    constexpr LABE::OSC::SCALING  SCALING             = LABE::OSC::SCALING::DOUBLE;
    constexpr unsigned            NUMBER_OF_CHANNELS  = 1;
    constexpr double              R1_RESISTOR_VALUE   = 750'000.0; // 20k ohms
    constexpr double              VIN_VOLTAGE_VALUE   = 12.0; // +5.0 volts
  };

  namespace FUNC_GEN
  {
    constexpr unsigned                  NUMBER_OF_CHANNELS      = 1;
    constexpr double                    MAX_AMPLITUDE_DIGI_POT  = MCP4XXX::MAX_AMPLITUDE; // V
    constexpr double                    MIN_AMPLITUDE_DIGI_POT  = MCP4XXX::MIN_AMPLITUDE; // V
    constexpr double                    MAX_AMPLITUDE           = MCP4XXX::MAX_VDD_VOLTAGE_WRT_VSS; // V
    constexpr double                    MIN_AMPLITUDE           = 0.1; // 100 mV
    constexpr double                    MAX_FREQUENCY           = AD9833::MAX_FREQUENCY;  // Hz
    constexpr double                    MIN_FREQUENCY           = AD9833::MIN_FREQUENCY;  // Hz
    constexpr double                    MAX_PERIOD              = 1.0 / MIN_FREQUENCY;    // s
    constexpr double                    MIN_PERIOD              = 1.0 / MAX_FREQUENCY;    // s
    constexpr double                    MAX_PHASE               = 360.0;                  // degrees
    constexpr double                    MIN_PHASE               = -360.0;                 // degrees
    constexpr double                    MAX_VERTICAL_OFFSET     = 10075;                    // V
    constexpr double                    MIN_VERTICAL_OFFSET     = 75;                   // V

    constexpr LABE::FUNC_GEN::WAVE_TYPE WAVE_TYPE               = LABE::FUNC_GEN::WAVE_TYPE::SINE;
    constexpr double                    AMPLITUDE               = 1.0;
    constexpr double                    FREQUENCY               = 1'000.0;
    constexpr double                    PERIOD                  = 1.0 / FREQUENCY;
    constexpr double                    PHASE                   = 0.0;
    constexpr double                    VERTICAL_OFFSET         = 0.0;

    constexpr double                    IC_FREQUENCY            = 50'000.0;  // Hz

    // see differential amplifier diagram here
    // https://en.wikipedia.org/wiki/Operational_amplifier_applications#Differential_amplifier_.28difference_amplifier.29

    constexpr double                    R1_RESISTANCE           = 1'000.0;  // ohms
    constexpr double                    RF_RESISTANCE           = 1'000.0;  // ohms
    constexpr double                    V1                      = 5.0;      // V
    constexpr double                    V2                      = 5.0;      // V
    constexpr double                    ROFF_RESISTANCE         = static_cast<uint32_t>
                                                                    (MCP4XXX::RESISTANCE_VERSION::_103);
                                                                 // total offset digi pot resistance
  };

  namespace LOGAN
  {
    // General
    constexpr unsigned                NUMBER_OF_CHANNELS              = sizeof (PIN::LOGAN) / sizeof (PIN::LOGAN[0]);
    constexpr unsigned                MAX_NUMBER_OF_SAMPLES           = 2'000;
    constexpr unsigned                SAMPLE_SIZE                     = sizeof (uint32_t); // bytes

    // Uncached Oscilloscope DMA Data Info
    constexpr unsigned                NUMBER_OF_BUFFERS               = 2;
    constexpr unsigned                BUFFER_LENGTH                   = SAMPLE_SIZE * MAX_NUMBER_OF_SAMPLES;
    constexpr unsigned                VC_MEM_SIZE                     = AP::RPI::PAGE_SIZE + (NUMBER_OF_CHANNELS * NUMBER_OF_BUFFERS * BUFFER_LENGTH);

    // Horizontal
    constexpr unsigned                DISPLAY_NUMBER_OF_COLUMNS       = 10;
    constexpr double                  MAX_SAMPLING_RATE               = 100;  // Hz
    constexpr double                  MIN_SAMPLING_RATE               = 0.1;        // Hz
    constexpr unsigned                MAX_SAMPLES                     = MAX_NUMBER_OF_SAMPLES;
    constexpr unsigned                MAX_SAMPLES_RECORDING           = 1'000'000;
    constexpr unsigned                MIN_SAMPLES                     = 2;
    constexpr double                  MAX_TIME_PER_DIVISION           = MAX_SAMPLES / (MIN_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double                  MAX_TIME_PER_DIVISION_RECORDING = MAX_SAMPLES_RECORDING / (MIN_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double                  MIN_TIME_PER_DIVISION           = MIN_SAMPLES / (MAX_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double                  MIN_TIME_PER_DIVISION_SCREEN    = 1.0 / DISPLAY_NUMBER_OF_COLUMNS;
    constexpr double                  MIN_TIME_PER_DIVISION_NO_ZOOM   = MAX_NUMBER_OF_SAMPLES / (MAX_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
    constexpr double                  MAX_HORIZONTAL_OFFSET           = MAX_SAMPLES / MIN_SAMPLING_RATE;
    constexpr double                  MIN_HORIZONTAL_OFFSET           = (-1) * MAX_HORIZONTAL_OFFSET;

    // Trigger
    constexpr double                  FIND_TRIGGER_TIMEOUT            = 2; // seconds

    // Display
    constexpr double                  MIN_TIME_PER_DIV_NO_ZOOM        = MAX_NUMBER_OF_SAMPLES / (MAX_SAMPLING_RATE * DISPLAY_NUMBER_OF_COLUMNS);
  };

  namespace DIGITAL_CIRCUIT_CHECKER
  {
    constexpr MCP23S17::PORT  INPUT_PORT    = MCP23S17::PORT::A; // pov sa expander
    constexpr MCP23S17::PORT  OUTPUT_PORT   = MCP23S17::PORT::B; // pov sa expander
    constexpr double          IC_FREQUENCY  = 100'000.0; // Hz
  };

  namespace PIN
  {
    namespace OSC
    {
      constexpr unsigned ADC_SCLK = 11;
      constexpr unsigned ADC_MOSI = 10;
      constexpr unsigned ADC_MISO = 9;
      constexpr unsigned ADC_CS   = 8;

      // multiplexer
      // {A0, A1}
      constexpr unsigned MUX[LABC::OSC::NUMBER_OF_CHANNELS][2] =
      {
        {27, 22},
        {23, 24}
      };

      // relay enable/disable
      constexpr unsigned RELAY[LABC::OSC::NUMBER_OF_CHANNELS] = {14, 15};
    };

    namespace FG
    {
      constexpr unsigned PWG_CS      = 13;
      constexpr unsigned PWG_MISO    = 5;
      constexpr unsigned PWG_MOSI    = 4;
      constexpr unsigned PWG_SCLK    = 6;
    };

    namespace DCC
    {
      constexpr unsigned BUFFER_OE  = 7;
      constexpr unsigned XPAND_CS   = 25;
      constexpr unsigned XPAND_MISO = 12;
      constexpr unsigned XPAND_MOSI = 3;
      constexpr unsigned XPAND_SCLK = 2;
    };

    namespace SNM
    {
      // SPI configuration
      constexpr unsigned BAUD_RATE   = 20000000;
      constexpr unsigned CS_PIN      = 16;
      constexpr unsigned MISO_PIN    = 19;
      constexpr unsigned MOSI_PIN    = 20;
      constexpr unsigned SCLK_PIN    = 21;

      constexpr unsigned TRANSFER_SIZE    = 2;
    }
  };
};

#endif
