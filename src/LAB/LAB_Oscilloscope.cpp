#include "LAB_Oscilloscope.h"

#include <cmath>
#include <cstring>

#include "LAB.h"
#include "../Utility/LAB_Utility_Functions.h"

#include "Software/LAB_Data_Measurer.h"
#include "Software/LAB_Data_Measurer.cpp"

// delete soon
#include <iostream>
#include <bitset>

LAB_Oscilloscope:: 
LAB_Oscilloscope (LAB& _LAB)
  : LAB_Module      (_LAB),
    m_measurements  (this->m_parent_data),
    m_calibration   (this->m_parent_data)
{
  init_spi        ();
  init_pwm        ();
  init_gpio_pins  ();
  init_dma        (); 
  init_state      ();
}

LAB_Oscilloscope::
~LAB_Oscilloscope ()
{
  rpi ().dma.stop  (LABC::DMA::CHAN::OSC_RX);
  rpi ().dma.stop  (LABC::DMA::CHAN::OSC_TX);
  rpi ().dma.stop  (LABC::DMA::CHAN::PWM_PACING);

  rpi ().dma.reset (LABC::DMA::CHAN::OSC_RX);
  rpi ().dma.reset (LABC::DMA::CHAN::OSC_TX);
  rpi ().dma.reset (LABC::DMA::CHAN::PWM_PACING);
}

void LAB_Oscilloscope:: 
init_spi ()
{
  rpi ().spi.clear_fifo ();
  rpi ().spi.reg        (AP::SPI::DC, (8 << 24) | (4 << 16) | (8 << 8) | 1);
  rpi ().spi.frequency  (LABC::SPI::FREQUENCY, LABC::LAB::GPU_CORE_CLOCK_FREQ);

  rpi ().gpio.set (LABC::PIN::OSC::ADC_CS,   AP::GPIO::FUNC::ALT0, AP::GPIO::PULL::OFF);
  rpi ().gpio.set (LABC::PIN::OSC::ADC_MISO, AP::GPIO::FUNC::ALT0, AP::GPIO::PULL::DOWN);
  rpi ().gpio.set (LABC::PIN::OSC::ADC_MOSI, AP::GPIO::FUNC::ALT0, AP::GPIO::PULL::OFF);
  rpi ().gpio.set (LABC::PIN::OSC::ADC_SCLK, AP::GPIO::FUNC::ALT0, AP::GPIO::PULL::OFF);
}

void LAB_Oscilloscope:: 
init_pwm ()
{
  rpi ().cm.pwm.frequency  (LABC::CLKMAN::FREQUENCY);

  rpi ().pwm.reg           (AP::PWM::DMAC, (1 << 31) | (8 << 8) | (1 << 0));
  rpi ().pwm.use_fifo      (LABC::PWM::DMA_PACING_CHAN, true);
  rpi ().pwm.algo          (LABC::PWM::DMA_PACING_CHAN, AP::PWM::ALGO::MARKSPACE);
  rpi ().pwm.frequency     (LABC::PWM::DMA_PACING_CHAN, LABD::OSC::SAMPLING_RATE);
  rpi ().pwm.duty_cycle    (LABC::PWM::DMA_PACING_CHAN, 50.0);

  // PWM pin (BCM pin 12) is in use by digital circuit checker
  // rpi ().gpio.set (LABC::PIN::PWM, AP::GPIO::FUNC::ALT0, AP::GPIO::PULL::DOWN);
}

void LAB_Oscilloscope::
init_gpio_pins ()
{
  // scaling
  for (int chan = 0; chan < m_parent_data.channel_data.size (); chan++)
  {
    for (int a = 0; a < 2; a++)
    {
      rpi ().gpio.set (
        LABC::PIN::OSC::MUX[chan][a],
        AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::DOWN,
        1
      );
    }
  }

  // coupling
  for (int chan = 0; chan < m_parent_data.channel_data.size (); chan++)
  {
    bool enable = static_cast<int>(m_parent_data.channel_data[chan].coupling);

    rpi ().gpio.set (LABC::PIN::OSC::RELAY[chan], 
      AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::DOWN, enable);
  }
}

void LAB_Oscilloscope::
init_dma ()
{
  config_dma_cb ();

  AikaPi::Uncached&          un = m_uncached_memory;
  LAB_DMA_Data_Oscilloscope& dd = *(static_cast<LAB_DMA_Data_Oscilloscope*>(un.virt ()));

  rpi ().dma.start (LABC::DMA::CHAN::OSC_TX,     un.bus (&dd.cbs[6]));
  rpi ().dma.start (LABC::DMA::CHAN::OSC_RX,     un.bus (&dd.cbs[0]));
  rpi ().dma.start (LABC::DMA::CHAN::PWM_PACING, un.bus (&dd.cbs[7]));
}

void LAB_Oscilloscope:: 
init_state ()
{
  for (int a = 0; a < m_parent_data.channel_data.size (); a++)
  {
    scaling   (a, m_parent_data.channel_data[a].scaling);
    coupling  (a, m_parent_data.channel_data[a].coupling);
  }

  time_per_division (m_parent_data.time_per_division);
}

void LAB_Oscilloscope:: 
config_dma_cb ()
{
  m_uncached_memory.map_uncached_mem (LABC::OSC::VC_MEM_SIZE);

  LAB_DMA_Data_Oscilloscope& uncached_dma_data = 
    *(static_cast<LAB_DMA_Data_Oscilloscope*>(m_uncached_memory.virt ()));

  LAB_DMA_Data_Oscilloscope new_uncached_dma_data = 
  {
    .cbs = 
    {
      // SPI RX - double buffer
      // block 0
      {
        LABC::DMA::TI::OSC_RX,
        rpi ().spi.bus   (AP::SPI::FIFO),
        m_uncached_memory.bus (&uncached_dma_data.rxd[0]),
        static_cast<uint32_t> (sizeof (uint32_t) * LABD::OSC::SAMPLES),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[1]),
        0
      },
      // block 1
      {
        LABC::DMA::TI::OSC_RX | AP::DMA::TI_DATA::INTEN,
        rpi ().spi.bus   (AP::SPI::CS),
        m_uncached_memory.bus (&uncached_dma_data.status[0]),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[2]),
        0
      },
      // block 2
      {
        LABC::DMA::TI::OSC_RX,
        rpi ().spi.bus   (AP::SPI::FIFO),
        m_uncached_memory.bus (&uncached_dma_data.rxd[1]),
        static_cast<uint32_t> (sizeof (uint32_t) * LABD::OSC::SAMPLES),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[3]),
        0
      },
      // block 3
      {
        LABC::DMA::TI::OSC_RX | AP::DMA::TI_DATA::INTEN,
        rpi ().spi.bus   (AP::SPI::CS),
        m_uncached_memory.bus (&uncached_dma_data.status[1]),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[0]),
        0
      },

      // SPI RX - single buffer
      //
      {
        LABC::DMA::TI::OSC_RX,
        rpi ().spi.bus   (AP::SPI::FIFO),
        m_uncached_memory.bus (&uncached_dma_data.rxd[0]),
        static_cast<uint32_t> (sizeof (uint32_t) * LABD::OSC::SAMPLES),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[5]),
        0
      },
      // 5
      {
        LABC::DMA::TI::OSC_RX | AP::DMA::TI_DATA::INTEN,
        rpi ().spi.bus   (AP::SPI::CS),
        m_uncached_memory.bus (&uncached_dma_data.status[0]),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[4]),
        0
      },

      // SPI TX
      // 6
      {
        LABC::DMA::TI::OSC_TX,
        m_uncached_memory.bus (&uncached_dma_data.txd),
        rpi ().spi.bus   (AP::SPI::FIFO),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[6]),
        0
      },

      // PWM Pacing
      // 7
      {
        LABC::DMA::TI::OSC_PWM_PACING,
        m_uncached_memory.bus (&uncached_dma_data.pwm_duty_cycle),
        rpi ().pwm.bus   (AP::PWM::FIF1),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[8]),
        0
      },
      // 8
      {
        LABC::DMA::TI::OSC_PWM_PACING,
        m_uncached_memory.bus (&uncached_dma_data.spi_cs_fifo_reset),
        rpi ().spi.bus   (AP::SPI::CS),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[9]),
        0
      },
      // 9
      {
        LABC::DMA::TI::OSC_PWM_PACING,
        m_uncached_memory.bus (&uncached_dma_data.spi_samp_size),
        rpi ().spi.bus   (AP::SPI::DLEN),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[10]),
        0
      },
      // 10
      {
        LABC::DMA::TI::OSC_PWM_PACING,
        m_uncached_memory.bus (&uncached_dma_data.spi_cs),
        rpi ().spi.bus   (AP::SPI::CS),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[7]),
        0
      },     
    },   

    .spi_samp_size      = sizeof (uint32_t),    
    .spi_cs             = (1 <<  7) | // SPI CS TA
                          (1 << 11) | // SPI CS ADCS
                          (1 <<  8) | // SPI CS DMAEN
                          (0 <<  3) | // SPI POLARITY
                          (0 <<  2) | // SPI PHASE
                          LABC::OSC::ADC_SPI_CHIP_ENABLE,
    .spi_cs_fifo_reset  = 0x00000030,
    .pwm_duty_cycle     = 0x0,
    .txd                = 0x0000'0000'ffff
  };

  std::memcpy (
    &uncached_dma_data, 
    &new_uncached_dma_data, 
    sizeof (new_uncached_dma_data)
  );
}

void LAB_Oscilloscope:: 
run ()
{
  lab ().m_Oscilloscope .stop ();
  lab ().m_Voltmeter    .stop ();
  lab ().m_Ohmmeter     .stop ();

  //

  frontend_run_stop (true);
  backend_run_stop  (true);

  reload_settings ();
}

void LAB_Oscilloscope:: 
stop ()
{
  frontend_run_stop (false);
  backend_run_stop  (false);
}

void LAB_Oscilloscope:: 
frontend_run_stop (bool value)
{
  m_parent_data.status = value ? LABE::OSC::STATUS::AUTO : LABE::OSC::STATUS::STOP;

  m_parent_data.is_frontend_running = value;
}

void LAB_Oscilloscope:: 
backend_run_stop (bool value)
{
  value ? rpi ().pwm.start  (LABC::PWM::DMA_PACING_CHAN): 
          rpi ().pwm.stop   (LABC::PWM::DMA_PACING_CHAN);

  m_parent_data.is_backend_running = value;
}

void LAB_Oscilloscope:: 
single ()
{
  do_measurements (false);

  if (!is_backend_running ())
  {
    backend_run_stop (false);
  }
  
  // reset DMA OSC RX chan interrupt flag
  rpi ().dma.clear_interrupt (LABC::DMA::CHAN::OSC_RX);

  m_parent_data.single = true;
}

void LAB_Oscilloscope::
do_measurements (bool value)
{
  m_parent_data.do_measurements = value;
}

void LAB_Oscilloscope:: 
status (LABE::OSC::STATUS _STATUS)
{
  m_parent_data.status = _STATUS;
}

void LAB_Oscilloscope:: 
reload_settings ()
{
  trigger_mode    (m_parent_data.trigger_mode);
  samples         (m_parent_data.samples);
  sampling_rate   (m_parent_data.sampling_rate);

  for (int a = 0; a < m_parent_data.channel_data.size (); a++)
  {
    coupling  (a, m_parent_data.channel_data[a].coupling);
    scaling   (a, m_parent_data.channel_data[a].scaling);

    lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
      a, 
      LABC::VOLTMETER::SCALING, 
      calibration ().scaling_corrector_to_actual (a, LABC::VOLTMETER::SCALING)
    );
    
    lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (
      a,
      LABC::OHMMETER::SCALING,
      calibration ().scaling_corrector_to_actual (a, LABC::OHMMETER::SCALING)
    );
  }

  //
  do_measurements (false);
}

bool LAB_Oscilloscope:: 
is_running ()
{
  return (m_parent_data.is_backend_running && m_parent_data.is_frontend_running);
}

bool LAB_Oscilloscope:: 
has_enabled_channels () const
{
  return (m_parent_data.has_enabled_channels ());
}

void LAB_Oscilloscope::
channel_enable_disable (unsigned channel,
                        bool     enable)
{
  if (channel < m_parent_data.channel_data.size ())
  {
    m_parent_data.channel_data[channel].is_enabled = enable;
  }
}

void LAB_Oscilloscope:: 
voltage_per_division (unsigned  channel, 
                      double    value)
{
  if (LABF::is_within_range (value, LABC::OSC::MIN_VOLTAGE_PER_DIVISION, 
    LABC::OSC::MAX_VOLTAGE_PER_DIVISION, LABC::LABSOFT::EPSILON))
  {
    m_parent_data.channel_data[channel].voltage_per_division = value;

    reset_dma_process ();
  }
}

void LAB_Oscilloscope:: 
vertical_offset (unsigned channel, 
                 double   value)
{
  m_parent_data.channel_data[channel].vertical_offset = value;
}

void LAB_Oscilloscope:: 
coupling (unsigned            channel,
          LABE::OSC::COUPLING coupling)
{
  if (channel < m_parent_data.channel_data.size ())
  {
    switch (coupling)
    {
      case (LABE::OSC::COUPLING::DC):
      {
        rpi ().gpio.set (
          LABC::PIN::OSC::RELAY[channel],
          AP::GPIO::FUNC::OUTPUT,
          AP::GPIO::PULL::OFF,
          0
        );

        break;
      }

      case (LABE::OSC::COUPLING::AC):
      {
        rpi ().gpio.set (
          LABC::PIN::OSC::RELAY[channel],
          AP::GPIO::FUNC::INPUT,
          AP::GPIO::PULL::OFF
        );
        
        break;
      }
    }

    // rpi ().gpio.set (
    //   LABC::PIN::OSC::RELAY[channel],
    //   AP::GPIO::FUNC::OUTPUT, 
    //   AP::GPIO::PULL::OFF,
    //   (coupling == LABE::OSC::COUPLING::AC) ? 1 : 0
    // );

    m_parent_data.channel_data[channel].coupling = coupling;
  }
}

void LAB_Oscilloscope:: 
scaling (unsigned           channel, 
         LABE::OSC::SCALING scaling)
{
  int mux_a = 0;

  switch (scaling)
  {
    case (LABE::OSC::SCALING::DOUBLE):
    {
      mux_a = 0;
      break;
    }

    case (LABE::OSC::SCALING::UNITY):
    case (LABE::OSC::SCALING::HALF):
    {
      mux_a = 1;
      break;
    }

    case (LABE::OSC::SCALING::FOURTH):
    {
      mux_a = 2;
      break;
    }

    case (LABE::OSC::SCALING::EIGHTH):
    {
      mux_a = 3;
      break;
    }
  }

  for (int a = 0; a < 2; a++)
  {
    rpi ().gpio.write (LABC::PIN::OSC::MUX[channel][a], 
      (mux_a >> a) & 0x1);
  }

  m_parent_data.channel_data[channel].scaling = scaling;

  // // add software scaling compensation code here
  // switch (scaling)
  // {
  //   case (LABE::OSC::SCALING::DOUBLE):
  //   {
  //     m_parent_data.channel_data[channel].scaling_corrector = 1.0;
  //     break;
  //   }

  //   case (LABE::OSC::SCALING::UNITY):
  //   {
  //     m_parent_data.channel_data[channel].scaling_corrector =
  //       m_parent_data.channel_data[channel].calibration.half_scaling_to_unity_corrector;

  //     break;
  //   }

  //   case (LABE::OSC::SCALING::HALF):
  //   {
  //     m_parent_data.channel_data[channel].scaling_corrector = 1.0;
  //     break;
  //   }

  //   case (LABE::OSC::SCALING::FOURTH):
  //   {
  //     m_parent_data.channel_data[channel].scaling_corrector = 1.0;
  //     break;
  //   }

  //   case (LABE::OSC::SCALING::EIGHTH):
  //   {
  //     m_parent_data.channel_data[channel].scaling_corrector = 1.0;
  //     break;
  //   }
  // }
}

LABE::OSC::SCALING LAB_Oscilloscope:: 
scaling (unsigned channel) const
{
  return (m_parent_data.channel_data[channel].scaling);
}

unsigned LAB_Oscilloscope:: 
channels () const
{
  return (m_parent_data.channel_data.size ());
}

LABE::OSC::COUPLING LAB_Oscilloscope:: 
coupling (unsigned channel)
{
  return (m_parent_data.channel_data[channel].coupling);
}


void LAB_Oscilloscope:: 
update_data_samples ()
{
  switch (m_parent_data.trigger_mode)
  {
    case (LABE::OSC::TRIG::MODE::NONE):
    {
      fill_raw_osc_samp_buff_from_dma_buff  ();
      parse_raw_osc_samp_buff               ();

      break;
    }

    case (LABE::OSC::TRIG::MODE::NORMAL):
    {
      if (m_parent_data.trigger_frame_ready)
      {
        parse_raw_osc_samp_buff ();
      } 

      break;
    }

    case (LABE::OSC::TRIG::MODE::AUTO):
    {
      if (m_parent_data.auto_mode_frame_ready)
      {
        parse_raw_osc_samp_buff ();
      }

      break;
    }
  }

  if (m_parent_data.single)
  {
    m_parent_data.single = false;

    stop ();
  }
}

void LAB_Oscilloscope:: 
fill_raw_osc_samp_buff_from_dma_buff ()
{
  LAB_DMA_Data_Oscilloscope& dma_data = *(
    static_cast<LAB_DMA_Data_Oscilloscope*>(m_uncached_memory.virt ())
  );

  switch (m_parent_data.mode)
  {
    case (LABE::OSC::MODE::REPEATED):
    {
      for (int buff = 0; buff < 2; buff++)
      {
        if (dma_data.status[buff])
        {
          std::memcpy (
            m_parent_data.raw_data_buffer.data (),
            const_cast<const void*>(static_cast<volatile void*>(dma_data.rxd[buff])),
            sizeof (uint32_t) * m_parent_data.samples
          );

          // std::cout << std::bitset <32> (dma_data.rxd[buff][10]) << "\n";
        }

        // Check if the other buffer is also full.
        // If it is, then we have a buffer overflow (both buffers full).
        if (dma_data.status[buff ^ 1])
        {
          // std::cout << "OVERFLOW!" << "\n";
          m_parent_data.buffer_overflow_count++;

          dma_data.status[0] = dma_data.status[1] = 0;

          break;
        }
        else 
        {
          dma_data.status[buff] = 0;
        }
      }

      break;
    }

    case (LABE::OSC::MODE::SCREEN):
    {
      std::memcpy (
        m_parent_data.raw_data_buffer.data (),
        const_cast<const void*>(static_cast<volatile void*>(dma_data.rxd[0])),
        sizeof (uint32_t) * m_parent_data.samples
      );

      break;
    }
  }
}

void LAB_Oscilloscope:: 
parse_raw_osc_samp_buff ()
{
  record_raw_sample_buffer_metadata ();

  LAB_Parent_Data_Oscilloscope& pdata = m_parent_data;  

  for (unsigned samp = 0; samp < pdata.samples; samp++)
  {
    // DEBUG!
    // if (samp == 0)
    // {
    //   uint32_t  rsamp = pdata.raw_data_buffer[samp];
    //   uint16_t  chan1 = (rsamp & 0xFFFF);
    //   uint16_t  chan2 = (rsamp & 0xFFFF0000) >> 16; 

    //   std::cout << "raw spi dma buffer bits : " << std::bitset<32> (rsamp) << "\n";
    //   std::cout << "chan 1 adc              : " << std::bitset<12> (reconstruct_adc_data_from_raw_osc_chan_samp (chan1, 0)) << "\n";
    //   std::cout << "chan 2 adc              : " << std::bitset<12> (reconstruct_adc_data_from_raw_osc_chan_samp (chan2, 1)) << "\n";
    //   std::cout << "act chan 1              : " << conv_raw_osc_samp_to_actual_chan_value (rsamp, 0) << "\n";
    //   std::cout << "act chan 2              : " << conv_raw_osc_samp_to_actual_chan_value (rsamp, 1) << "\n";

    //   std::cout << std::endl;
    // }

    int chan = 0;

    for (chan = 0; chan < pdata.channel_data.size (); chan++)
    {
      if (!is_channel_enabled (chan))
      {
        continue;
      }

      pdata.channel_data[chan].samples[samp] = 
        conv_raw_osc_samp_to_actual_chan_value (pdata.raw_data_buffer[samp], chan);

      // FOR DEBUG TO GENERATE DUMMY SINE DATA
      // SINE
      // double y = (0.5 * sin (2 * 3.141592653 * (1.0 / 0.005) * (samp / 16'000.0))) + 0.001;
      
      // RANDOM NOISE -2 to +2
      // double y = 0.5 * sin (2 * 3.141592653 * (1.0 / 0.005) * (samp / 2000.0) * rpi ().st.low ());

      // DC
      // double y = 0;

      // double x = 2.5 * sin (2.0 * 3.141592 * (10.0 / 2000.0) * (samp));
      // pdata.channel_data[chan].samples[samp] =  (y * m_parent_data.channel_data[chan].scaling_corrector) + 0.322;
    }
  }

  if (pdata.do_measurements)
  {
    for (int chan = 0; chan < pdata.channel_data.size(); chan++)
    {
      if (is_channel_enabled (chan))
      {
        std::tuple<double, double, double> min_max_avg = 
          LAB_Data_Measurer::min_max_avg (pdata.channel_data[chan].samples);
        
        pdata.channel_data[chan].measurements.min   = std::get<0> (min_max_avg);
        pdata.channel_data[chan].measurements.max   = std::get<1> (min_max_avg);
        pdata.channel_data[chan].measurements.avg   = std::get<2> (min_max_avg);
        pdata.channel_data[chan].measurements.trms  = LAB_Data_Measurer::true_rms<double> (pdata.channel_data[chan].samples);

        // std::cout << "chan " << chan << "min: " <<  pdata.channel_data[chan].measurements.min   << "\n";
        // std::cout << "chan " << chan << "max: " <<  pdata.channel_data[chan].measurements.max   << "\n";
        // std::cout << "chan " << chan << "avg: " <<  pdata.channel_data[chan].measurements.avg   << "\n";
        // std::cout << "chan " << chan << "trms: " << pdata.channel_data[chan].measurements.trms  << "\n";  
      }
    }
  }

  if (m_parent_data.trigger_frame_ready)
  {
    m_parent_data.trigger_frame_ready = false;
  }
}

double LAB_Oscilloscope::
conv_raw_osc_samp_to_actual_chan_value (uint32_t raw_osc_samp,
                                        unsigned channel)
{
  uint32_t  raw_osc_chan_samp = extract_raw_osc_chan_samp_from_raw_osc_samp (raw_osc_samp, channel);
  uint32_t  adc_data          = reconstruct_adc_data_from_raw_osc_chan_samp (raw_osc_chan_samp, channel);
  double    actual_value      = conv_adc_data_to_actual_value               (adc_data, channel);

  return (actual_value);
}

uint32_t LAB_Oscilloscope::
extract_raw_osc_chan_samp_from_raw_osc_samp (uint32_t raw_osc_samp,
                                             unsigned channel)
{
  return ((raw_osc_samp >> (LABC::OSC::RAW_DATA_BIT_SHIFT_COUNT * channel)) & 
    LABC::OSC::RAW_DATA_POST_SHIFT_MASK);
}

uint32_t LAB_Oscilloscope:: 
reconstruct_adc_data_from_raw_osc_chan_samp (uint32_t raw_osc_chan_samp,
                                             unsigned channel)
{
  // MCP33111
  // uint32_t recon_adc_data =    ((raw_osc_chan_samp & 0x007F) << 5) | ((raw_osc_chan_samp & 0xF800) >> 11);
  //          recon_adc_data <<=  1;

  // ADS7883
  uint32_t recon_adc_data = ((raw_osc_chan_samp & 0xFE00) >> 9) | ((raw_osc_chan_samp & 0x001F) << 7);
 
  return (recon_adc_data);
}

uint32_t LAB_Oscilloscope:: 
reconstruct_raw_osc_chan_samp_from_adc_data (uint32_t adc_data)
{
  // MCP33111
  //          adc_data                >>= 1;
  // uint32_t recon_raw_osc_chan_samp =   ((adc_data & 0x0FE0) >> 5) | ((adc_data & 0x001F) << 11);

  // ADS7883
  uint32_t recon_raw_osc_chan_samp = ((adc_data & 0x007F) << 9) | ((adc_data & 0x0F800) >> 7);


  return (recon_raw_osc_chan_samp);
}

double LAB_Oscilloscope:: 
conv_adc_data_to_actual_value (uint32_t adc_data,
                               unsigned channel)
{
  LAB_Channel_Data_Oscilloscope cdata = m_parent_data.channel_data[channel];

  bool      sign                  = (adc_data >> (LABC::OSC::ADC_RESOLUTION_BITS - 1)) & 0x1;
  uint32_t  adc_data_without_sign = adc_data & ((LABC::OSC::ADC_RESOLUTION_INT - 1) >> 1);
  double    actual_value_absolute = adc_data_without_sign * cdata.calibration.conversion_constant;
  double    actual_value          = sign ? actual_value_absolute : (actual_value_absolute - 
                                    cdata.calibration.conversion_reference_voltage);

  if (m_parent_data.is_calibration_enabled)
  {
    actual_value += cdata.calibration.vertical_offset_corrector.at    (cdata.scaling);
    actual_value *= cdata.calibration.scaling_corrector_to_unity.at   (cdata.scaling);
    actual_value *= cdata.calibration.scaling_corrector_to_actual.at  (cdata.scaling);
  }

  return (actual_value);
}

uint32_t LAB_Oscilloscope:: 
conv_raw_osc_samp_to_recon_chan_adc_data (uint32_t raw_osc_samp,
                                          unsigned channel)
{
  uint32_t raw_osc_chan_samp = extract_raw_osc_chan_samp_from_raw_osc_samp (raw_osc_samp, channel);

  return (reconstruct_adc_data_from_raw_osc_chan_samp (raw_osc_chan_samp, channel));
}

void LAB_Oscilloscope:: 
reset_dma_process ()
{
  LAB_DMA_Data_Oscilloscope& dma_data = *(static_cast<LAB_DMA_Data_Oscilloscope*>
    (m_uncached_memory.virt ()));

  bool is_dma_running = rpi ().dma.is_running (LABC::DMA::CHAN::PWM_PACING);

  // 1. check if DMA is running. if it is, pause it
  if (is_dma_running)
  {
    rpi ().dma.pause (LABC::DMA::CHAN::PWM_PACING);
  }

  // 2. reset the DMA engine to the first control block, depending on the buffer
  switch (mode ())
  {
    case (LABE::OSC::MODE::REPEATED): // dual buffer
    {
      rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX,
        m_uncached_memory.bus (&dma_data.cbs[0]));

      break;
    }

    case (LABE::OSC::MODE::SCREEN): // single buffer
    {
      rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX,
        m_uncached_memory.bus (&dma_data.cbs[4]));
      
      break;
    }
  }

  // 3. abort the current control block
  rpi ().dma.abort (LABC::DMA::CHAN::OSC_RX);

  // 4. reset the DMA status flags
  dma_data.status[0] = dma_data.status[1] = 0;

  // 5. reset the 2D DMA OSC RX buffer array
  reset_uncached_rx_buffer ();

  // 6. run DMA again if it was running
  if (is_dma_running)
  {
    rpi ().dma.run (LABC::DMA::CHAN::PWM_PACING);
  }
}

void LAB_Oscilloscope:: 
reset_uncached_rx_buffer ()
{
  LAB_DMA_Data_Oscilloscope& dma_data = *(static_cast<LAB_DMA_Data_Oscilloscope*>
    (m_uncached_memory.virt ()));

  // std::memset (
  //   const_cast<void*>(static_cast<volatile void*>(dma_data.rxd)),
  //   reconstruct_raw_osc_chan_samp_from_adc_data (LABC::OSC::ADC_RESOLUTION_INT / 2),
  //   sizeof (dma_data.rxd)
  // );
}

void LAB_Oscilloscope:: 
record_raw_sample_buffer_metadata ()
{
  LAB_Parent_Data_Oscilloscope& p = m_parent_data;

  p.horizontal_offset_raw_buffer  = p.horizontal_offset;
  p.time_per_division_raw_buffer  = p.time_per_division;
  p.sampling_rate_raw_buffer      = p.sampling_rate; 
}

double LAB_Oscilloscope::
calc_time_per_division (unsigned  samples, 
                        double    sampling_rate) const
{
  return (samples / (sampling_rate * LABC::OSC::DISPLAY_NUMBER_OF_COLUMNS));
}

double LAB_Oscilloscope:: 
calc_sampling_rate (unsigned  samples,
                    double    time_per_division) const
{
  double new_sampling_rate = samples / (time_per_division * LABC::OSC::DISPLAY_NUMBER_OF_COLUMNS);

  if (LABF::is_greater_than (
    new_sampling_rate,
    LABC::OSC::MAX_SAMPLING_RATE,
    LABC::LABSOFT::EPSILON
  ))
  {
    return (LABC::OSC::MAX_SAMPLING_RATE);
  }
  else 
  {
    return (new_sampling_rate);
  }
}

void LAB_Oscilloscope:: 
set_time_per_division (double value)
{
  m_parent_data.time_per_division = value;

  set_mode (calc_mode (value));

  reset_dma_process ();
}

void LAB_Oscilloscope:: 
set_samples (unsigned value)
{
  m_parent_data.samples = value;

  LAB_DMA_Data_Oscilloscope& uncached_dma_data = 
    *(static_cast<LAB_DMA_Data_Oscilloscope*>(m_uncached_memory.virt ()));
  
  uncached_dma_data.cbs[0].txfr_len = static_cast<uint32_t>(sizeof (uint32_t) * value);
  uncached_dma_data.cbs[2].txfr_len = static_cast<uint32_t>(sizeof (uint32_t) * value);
  uncached_dma_data.cbs[4].txfr_len = static_cast<uint32_t>(sizeof (uint32_t) * value);
}

void LAB_Oscilloscope:: 
set_sampling_rate (double value)
{
  // 1. change the source frequency of the PWM peripheral
  rpi ().pwm.frequency (LABC::PWM::DMA_PACING_CHAN, value);

  // 2. set the duty cycle in the DMA data to 50%
  LAB_DMA_Data_Oscilloscope& dma_data = *(static_cast<LAB_DMA_Data_Oscilloscope*>
    (m_uncached_memory.virt ()));

  dma_data.pwm_duty_cycle = (rpi ().pwm.range (LABC::PWM::DMA_PACING_CHAN)) / 2.0;

  // 3. store the sampling rate
  m_parent_data.sampling_rate = value;
}

LABE::OSC::MODE LAB_Oscilloscope:: 
calc_mode (double time_per_division)
{
  if (LABF::is_less_than (
    time_per_division,
    LABC::OSC::MIN_TIME_PER_DIVISION_SCREEN,
    LABC::LABSOFT::EPSILON
  ))
  {
    return (LABE::OSC::MODE::REPEATED);
  }
  else 
  {
    return (m_parent_data.last_mode_before_repeated);
  }
}

// --- Horizontal ---

void LAB_Oscilloscope:: 
horizontal_offset (double value)
{
  if (LABF::is_within_range (
    value,
    LABC::OSC::MIN_HORIZONTAL_OFFSET,
    LABC::OSC::MAX_HORIZONTAL_OFFSET,
    LABC::LABSOFT::EPSILON
  ))
  {
    m_parent_data.horizontal_offset = value;

    // add functionality here soon
  }
}

double LAB_Oscilloscope:: 
horizontal_offset () const
{
  return (m_parent_data.horizontal_offset);
}

void LAB_Oscilloscope:: 
time_per_division (double value)
{
  if (LABF::is_within_range (
    value,
    LABC::OSC::MIN_TIME_PER_DIVISION,
    LABC::OSC::MAX_TIME_PER_DIVISION,
    LABC::LABSOFT::EPSILON
  ))
  {
    double new_sampling_rate = calc_sampling_rate (m_parent_data.samples, value);

    set_sampling_rate     (new_sampling_rate);
    set_time_per_division (value);
  }
}

void LAB_Oscilloscope:: 
samples (unsigned value)
{
  if (!(LABF::is_within_range (value, LABC::OSC::MIN_SAMPLES, 
    LABC::OSC::MAX_SAMPLES_RECORDING, LABC::LABSOFT::EPSILON)))
  {
    return;
  }

  if ((m_parent_data.mode != LABE::OSC::MODE::RECORD) && 
    (value > LABC::OSC::MAX_SAMPLES))
  {
    return;
  }

  // 

  set_samples           (value);
  set_time_per_division (calc_time_per_division (value, m_parent_data.sampling_rate));
}

void LAB_Oscilloscope:: 
sampling_rate (double value)
{
  if (LABF::is_within_range (
    value,
    LABC::OSC::MIN_SAMPLING_RATE,
    LABC::OSC::MAX_SAMPLING_RATE, 
    LABC::LABSOFT::EPSILON
  ))
  {
    set_time_per_division (calc_time_per_division (m_parent_data.samples, value));
    set_sampling_rate     (value);
  }
}

// --- Trigger ---

void LAB_Oscilloscope:: 
parse_trigger_mode ()
{
  switch (m_parent_data.trigger_mode)
  {
    case (LABE::OSC::TRIG::MODE::NONE):
    {
      if (m_thread_trigger.joinable ())
      {
        m_parent_data.trigger_enabled = false;

        m_thread_trigger.join ();
      }

      break;
    }

    case (LABE::OSC::TRIG::MODE::AUTO):
    {
      if (!m_thread_trigger.joinable ())
      {
        m_parent_data.trigger_enabled = true;

        m_thread_trigger = std::thread (&LAB_Oscilloscope::find_trigger_point_loop, this);
      }

      break;
    }

    case (LABE::OSC::TRIG::MODE::NORMAL):
    {
      if (!m_thread_trigger.joinable ())
      {
        m_parent_data.trigger_enabled = true;

        m_thread_trigger = std::thread (&LAB_Oscilloscope::find_trigger_point_loop, this);
      }

      break;
    }
  }
}

void LAB_Oscilloscope:: 
trigger_osc_mode_dispatcher ()
{
  // switch (m_parent_data.mode)
  // {
  //   case (LABE::OSC::MODE::REPEATED):
  //   {
  //     trigger_osc_mode_repeated ();

  //     break;
  //   }

  //   case (LABE::OSC::MODE::SCREEN):
  //   {
  //     trigger_osc_mode_screen ();
      
  //     break;
  //   }
  // }
}

void LAB_Oscilloscope:: 
find_trigger_point_loop ()
{
  LAB_DMA_Data_Oscilloscope& dma_data = *(static_cast<LAB_DMA_Data_Oscilloscope*>
    (m_uncached_memory.virt ()));

  // ----------
  
  // Cached variables

  // Regs
  volatile uint32_t* int_status     = rpi ().dma.MemoryMap::reg (AP::DMA::INT_STATUS);
  volatile uint32_t* curr_conblk_ad = rpi ().dma.reg (LABC::DMA::CHAN::OSC_RX, AP::DMA::CONBLK_AD);

  // Vars
  uint32_t  buff0_cbs_addr[2] = {m_uncached_memory.bus (&dma_data.cbs[0]),
                                 m_uncached_memory.bus (&dma_data.cbs[1])};
  uint32_t  buff1_cbs_addr[2] = {m_uncached_memory.bus (&dma_data.cbs[2]),
                                 m_uncached_memory.bus (&dma_data.cbs[3])};
  
  // ----------

  // sync_find_trigger_point_loop ();

  status (LABE::OSC::STATUS::CONFIG);

  switch (m_parent_data.mode)
  {
    case (LABE::OSC::MODE::REPEATED):
    {
      // 1. Pause osc RX DMA channel
      rpi ().dma.pause (LABC::DMA::CHAN::OSC_RX);

      // 2. Set the contents of its next control block register
      //    to point to the first control block
      rpi ().dma.reg (LABC::DMA::CHAN::OSC_RX, AP::DMA::NEXTCONBK,
        m_uncached_memory.bus (&dma_data.cbs[0]));
      
      // 3. Abort current control block
      rpi ().dma.reg_wbits (LABC::DMA::CHAN::OSC_RX, AP::DMA::CS, 1, 30);

      std::this_thread::sleep_for (std::chrono::microseconds (5));

      // 4. Unpause oscilloscope RX DMA channel
      rpi ().dma.run (LABC::DMA::CHAN::OSC_RX);  

      // 5. Wait until first buffer is filled
      while (*curr_conblk_ad == buff0_cbs_addr[0] || *curr_conblk_ad == buff0_cbs_addr[1]);

      // 6. Wait until second buffer is filled
      while (*curr_conblk_ad == buff1_cbs_addr[0] || *curr_conblk_ad == buff1_cbs_addr[1]); 

      // 7. Reset osc RX DMA interrupt
      rpi ().dma.MemoryMap::reg_wbits (AP::DMA::INT_STATUS, 0, LABC::DMA::CHAN::OSC_RX);

      break;
    }

    case (LABE::OSC::MODE::SCREEN):
    {
      break;
    }
  }
  
  // ----------

  status (LABE::OSC::STATUS::ARMED);
  
  while (m_parent_data.trigger_enabled)
  {
    if (!m_parent_data.trigger_found)
    {
      // 1. Check the oscilloscope RX DMA channel interrupt if it is asserted.
      //    If it is, this means that a buffer was just fully written to.
      if (rpi ().dma.interrupt (LABC::DMA::CHAN::OSC_RX))
      {
        // 2. Store the current control block running in the oscilloscope RX DMA 
        //    engine. This is to know what buffer (0 or 1) was just filled.
        uint32_t curr_conblk_ad = *(rpi ().dma.reg (LABC::DMA::CHAN::OSC_RX, AP::DMA::CONBLK_AD));

        // 3. Reset the interrupt
        rpi ().dma.clear_interrupt (LABC::DMA::CHAN::OSC_RX);

        // 4. Store a copy of the uncached receive buffers
        std::memcpy (
          m_parent_data.trig_buffs.pre_trigger.data (),
          const_cast<const void*>(static_cast<const volatile void*>(dma_data.rxd)),
          sizeof (uint32_t) * LABC::OSC::NUMBER_OF_CHANNELS * LABC::OSC::NUMBER_OF_SAMPLES
        );

        // 5. Identify the buffer that was filled 
        if (curr_conblk_ad == buff0_cbs_addr[0] || curr_conblk_ad == buff0_cbs_addr[1])
        {
          m_parent_data.trigger_buffer_index = 1;
        }
        else if (curr_conblk_ad == buff1_cbs_addr[0] || curr_conblk_ad == buff1_cbs_addr[1])
        {
          m_parent_data.trigger_buffer_index = 0;
        }

        // 6. Search for trigger
        if (find_trigger_point ())
        {
          create_trigger_frame ();
        }
      }
    }
  }
}

inline bool LAB_Oscilloscope:: 
find_trigger_point ()
{
  switch (m_parent_data.trig_type)
  {
    case (LABE::OSC::TRIG::TYPE::LEVEL):
    {
      for (int a = 0; a < m_parent_data.trig_buffs.pre_trigger[m_parent_data.
        trigger_buffer_index].size (); a += m_parent_data.find_trig_sample_skip)
      {
        uint32_t samp  = conv_raw_osc_samp_to_recon_chan_adc_data (
          m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index][a], 
          m_parent_data.trigger_source
        );

        if (samp >= m_parent_data.trigger_level_raw_bits)
        {
          m_parent_data.trig_index = a;
          m_parent_data.trigger_found = true;

          return (true);
        }
      }

      break;
    }

    case (LABE::OSC::TRIG::TYPE::EDGE):
    {
      uint32_t prev = conv_raw_osc_samp_to_recon_chan_adc_data (
        m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index][0],
        m_parent_data.trigger_source
      );

      switch (m_parent_data.trig_condition)
      {
        case (LABE::OSC::TRIG::CND::RISING):
        {
          for (int a = 1; a < m_parent_data.trig_buffs.pre_trigger[m_parent_data.
            trigger_buffer_index].size (); a += m_parent_data.find_trig_sample_skip)
          {
            uint32_t samp = conv_raw_osc_samp_to_recon_chan_adc_data (
              m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index][a],
              m_parent_data.trigger_source
            );

            if ((samp >= m_parent_data.trigger_level_raw_bits) && (prev < m_parent_data.trigger_level_raw_bits))
            {
              m_parent_data.trig_index = a;
              m_parent_data.trigger_found = true;

              return (true);
            }
            else 
            {
              prev = samp;
            }
          }

          break;
        }

        case (LABE::OSC::TRIG::CND::FALLING):
        {
          for (int a = 1; a < m_parent_data.trig_buffs.pre_trigger[m_parent_data.
            trigger_buffer_index].size (); a += m_parent_data.find_trig_sample_skip)
          {
            uint32_t samp = conv_raw_osc_samp_to_recon_chan_adc_data (
              m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index][a],
              m_parent_data.trigger_source
            );

            if ((samp <= m_parent_data.trigger_level_raw_bits) && (prev > m_parent_data.trigger_level_raw_bits))
            {
              m_parent_data.trig_index = a;
              m_parent_data.trigger_found = true;

              return (true);
            }
            else 
            {
              prev = samp;
            }
          }

          break;
        }

        case (LABE::OSC::TRIG::CND::EITHER):
        {
          for (int a = 1; a < m_parent_data.trig_buffs.pre_trigger[m_parent_data.
            trigger_buffer_index].size (); a += 4)
          {
            uint32_t samp = conv_raw_osc_samp_to_recon_chan_adc_data (
              m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index][a],
              m_parent_data.trigger_source
            );

            if (((samp <= m_parent_data.trigger_level_raw_bits) && (prev > m_parent_data.trigger_level_raw_bits)) ||
                ((samp >= m_parent_data.trigger_level_raw_bits) && (prev < m_parent_data.trigger_level_raw_bits)))
            {
              m_parent_data.trig_index = a;
              m_parent_data.trigger_found = true;

              return (true);
            }
            else 
            {
              prev = samp;
            }
          }

          break;
        }
      }

      break;
    }
  }

  return (false);
}

inline void LAB_Oscilloscope:: 
create_trigger_frame ()
{
  static constexpr unsigned samp_half         = LABC::OSC::NUMBER_OF_SAMPLES / 2;
  static constexpr unsigned samp_half_index   = samp_half - 1;
  static LAB_DMA_Data_Oscilloscope& dma_data  = *(static_cast<LAB_DMA_Data_Oscilloscope*>
                                                  (m_uncached_memory.virt ()));

  if (m_parent_data.trig_index >= samp_half_index)
  {
    unsigned  copy_count_0  = samp_half,
              copy_count_1  = LABC::OSC::NUMBER_OF_SAMPLES - 1 - m_parent_data.trig_index,
              copy_count_2  = samp_half - copy_count_1;
    
    std::memcpy (
      m_parent_data.raw_data_buffer.data (),
      m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index].data () 
        + (m_parent_data.trig_index - samp_half_index),
      sizeof (uint32_t) * copy_count_0
    );

    std::memcpy (
      m_parent_data.raw_data_buffer.data () + (copy_count_0),
      m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index].data () 
        + (m_parent_data.trig_index + 1),
      sizeof (uint32_t) * copy_count_1
    );

    while (!((*(rpi ().dma.MemoryMap::reg (AP::DMA::INT_STATUS)) >> LABC::DMA::CHAN::OSC_RX) & 0x1));

    std::memcpy (
      m_parent_data.raw_data_buffer.data () + (copy_count_0 + copy_count_1),
      const_cast<const void*>(static_cast<const volatile void*>(dma_data.rxd[m_parent_data.trigger_buffer_index ^ 1])),
      sizeof (uint32_t) * copy_count_2
    );

    rpi ().dma.MemoryMap::reg_wbits (AP::DMA::INT_STATUS, 0, LABC::DMA::CHAN::OSC_RX);
  }
  else if (m_parent_data.trig_index < samp_half_index)
  {
    unsigned  copy_count_2 = samp_half,
              copy_count_1 = m_parent_data.trig_index + 1,
              copy_count_0 = samp_half - copy_count_1;
                  
    std::memcpy (
      m_parent_data.raw_data_buffer.data (),
      m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index ^ 1].data () 
        + (LABC::OSC::NUMBER_OF_SAMPLES - copy_count_0),
      sizeof (uint32_t) * copy_count_0
    );

    std::memcpy (
      m_parent_data.raw_data_buffer.data () + (copy_count_0),
      m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index].data (),
      sizeof (uint32_t) * copy_count_1
    );

    std::memcpy (
      m_parent_data.raw_data_buffer.data () + (copy_count_0 + copy_count_1),
      m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index].data () + 
        (m_parent_data.trig_index + 1),
      sizeof (uint32_t) * copy_count_2
    );
  }

  m_parent_data.trigger_frame_ready = true;
}

void LAB_Oscilloscope:: 
find_trigger_timeout_timer ()
{
  // uint32_t start    = rpi ().st.low ();
  // uint32_t timeout  = start + (LABC::OSC::) 
  // while (m_parent_data.trigger_mode == LABE::OSC::TRIG::MODE::AUTO)
  // {
  //   {
  //     uint32_t start    = rpi ().st.low ();
  //     uint32_t timeout  = 0;

  //     while (!m_parent_data.trigger_found)
  //     {
  //       if (rpi ().st.low () >= timeout)
  //       {
  //         m_parent_data.find_trig_timeout = true;
  //       }

  //       while (!m_parent_data.trigger_found);

  //       m_parent_data.trigger_found = false;
  //       m_parent_data.find_trig_timeout = false;
  //     }
  //   }
  // }
}

uint32_t LAB_Oscilloscope:: 
calc_trigger_level_raw_bits (double trigger_level)
{
  if (LABF::is_within_range (
    trigger_level, 
    LABD::OSC::MIN_OSC_HARDWARE_TRIGGER_LEVEL,
    LABD::OSC::MAX_OSC_HARDWARE_TRIGGER_LEVEL,
    LABC::LABSOFT::EPSILON
  ))
  {
    return (reconstruct_raw_osc_chan_samp_from_adc_data (std::round (trigger_level)));
  }
  else 
  {
    return (0xFFFFFFFF);
  }
}

void LAB_Oscilloscope:: 
prefill_buffers_for_triggering ()
{
  LAB_DMA_Data_Oscilloscope& dma_data = 
    *(static_cast<LAB_DMA_Data_Oscilloscope*>(m_uncached_memory.virt ()));

  volatile uint32_t* curr_conblk_ad = rpi ().dma.reg 
    (LABC::DMA::CHAN::OSC_RX, AP::DMA::CONBLK_AD);

  uint32_t  buff0_cbs_addr[2] = {m_uncached_memory.bus (&dma_data.cbs[0]),
                                 m_uncached_memory.bus (&dma_data.cbs[1])};
  uint32_t  buff1_cbs_addr[2] = {m_uncached_memory.bus (&dma_data.cbs[2]),
                                 m_uncached_memory.bus (&dma_data.cbs[3])};
  
  // 1. pause oscilloscope RX DMA channel
  rpi ().dma.pause (LABC::DMA::CHAN::OSC_RX);

  // 2. set the content of next control block register 
  // to point to the first control block
  rpi ().dma.reg (LABC::DMA::CHAN::OSC_RX, AP::DMA::NEXTCONBK,
    m_uncached_memory.bus (&dma_data.cbs[0]));

  // 3. abort the current control block
  rpi ().dma.abort (LABC::DMA::CHAN::OSC_RX);

  std::this_thread::sleep_for (std::chrono::microseconds (5));

  // 4. unpause the RX DMA channel
  rpi ().dma.run (LABC::DMA::CHAN::OSC_RX);

  // 5. wait until first buffer is filled
  while (*curr_conblk_ad == buff0_cbs_addr[0] || *curr_conblk_ad == buff0_cbs_addr[1]);

  // 6. wait until second buffer is filled
  while (*curr_conblk_ad == buff1_cbs_addr[0] || *curr_conblk_ad == buff1_cbs_addr[1]); 

  // 7. reset the RX DMA channel interrupt
  rpi ().dma.MemoryMap::reg_wbits (AP::DMA::INT_STATUS, 0, LABC::DMA::CHAN::OSC_RX);
}

void LAB_Oscilloscope:: 
config_dma_cb_record ()
{
  // LAB_DMA_Data_Oscilloscope dma_data = 
  // {
  //   .cbs = 
  //   {
  //     LABC::DMA::TI::OSC_RX,
  //     rpi ().spi.bus (AP::SPI::FIFO),

  //     static_cast<uint32_t> (sizeof (uint32_t) * m_parent_data.samples),
  //     0,
  //     0,
  //     0
  //   }
  // }
}

void LAB_Oscilloscope:: 
debug ()
{
  std::cout << "*** Oscilloscope ***" << "\n";
  std::cout << "horizontal_offset : " << m_parent_data.horizontal_offset  << "\n";
  std::cout << "time_per_division : " << m_parent_data.time_per_division  << "\n";
  std::cout << "samples           : " << m_parent_data.samples            << "\n";
  std::cout << "sampling_rate     : " << m_parent_data.sampling_rate      << "\n";
  std::cout << "mode              : " << static_cast<int>(m_parent_data.mode) << "\n";;
  std::cout << "********************" << "\n\n";
}

void LAB_Oscilloscope:: 
trigger_mode (LABE::OSC::TRIG::MODE value)
{
  m_parent_data.trigger_mode = value;

  parse_trigger_mode ();
}

LABE::OSC::TRIG::MODE LAB_Oscilloscope:: 
trigger_mode () const
{
  return (m_parent_data.trigger_mode);
}

void LAB_Oscilloscope:: 
trigger_source (unsigned chan)
{
  if (chan >= 0 && chan <= LABC::OSC::NUMBER_OF_CHANNELS)
  {
    m_parent_data.trigger_source = chan;
  }
}

unsigned LAB_Oscilloscope:: 
trigger_source () const
{
  return (m_parent_data.trigger_source);
}

void LAB_Oscilloscope::
trigger_type (LABE::OSC::TRIG::TYPE value)
{
  m_parent_data.trig_type = value;
}

LABE::OSC::TRIG::TYPE LAB_Oscilloscope:: 
trigger_type () const
{
  return (m_parent_data.trig_type);
}

void LAB_Oscilloscope::
trigger_condition (LABE::OSC::TRIG::CND value)
{
  m_parent_data.trig_condition = value;
}

LABE::OSC::TRIG::CND LAB_Oscilloscope::
trigger_condition () const
{
  return (m_parent_data.trig_condition);
}

void LAB_Oscilloscope:: 
trigger_level (double value)
{
  if (LABF::is_within_range (
    value,
    LABC::OSC::MIN_TRIGGER_LEVEL,
    LABC::OSC::MAX_TRIGGER_LEVEL,
    LABC::LABSOFT::EPSILON
  ))
  {
    m_parent_data.trigger_level           = value;
    m_parent_data.trigger_level_raw_bits  = calc_trigger_level_raw_bits (value);
  }
}

double LAB_Oscilloscope::    
trigger_level () const
{
  return (m_parent_data.trigger_level);
}

bool LAB_Oscilloscope::            
trigger_found () const
{
  return (m_parent_data.trigger_found);
}

void LAB_Oscilloscope::                 
trigger_serviced ()
{
  m_parent_data.trigger_found = false;
}

void LAB_Oscilloscope::
record_start ()
{
  // record_init ();

  // // x. Stop PWM
  // stop ();

  // // x. Pause OSC RX DMA engine
  // rpi ().dma.pause (LABC::DMA::CHAN::OSC_RX);

  // // x. 
  // rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX, c)



  // // x. Start PWM
  // run ();

  // // x. Copy raw samples from DMA buffer
  // std::memcpy (
  //   m_parent_data.recording_raw_sample_buffer.data (),
  //   const_cast<const void*>(static_cast<volatile void*>(&(dma_data.rxd))),
  //   sizeof (uint32_t) * m_parent_data.samples
  // );

  // // x.
}

void LAB_Oscilloscope:: 
record_init ()
{
  // x. 
  m_parent_data.recording_raw_sample_buffer.reserve (LABC::OSC::MAX_SAMPLES_RECORDING);
}

// --- Mode ---

void LAB_Oscilloscope::   
mode (LABE::OSC::MODE mode)
{
  switch (mode)
  {
    case (LABE::OSC::MODE::REPEATED):
    {
      break;
    }

    case (LABE::OSC::MODE::SCREEN):
    {
      if (time_per_division () < LABC::OSC::MIN_TIME_PER_DIVISION_SCREEN)
      {
        time_per_division (LABC::OSC::MIN_TIME_PER_DIVISION_SCREEN);
      }

      break;
    }

    case (LABE::OSC::MODE::RECORD):
    {
      stop ();
      break;
    }
  }

  set_mode (mode);
}

LABE::OSC::MODE LAB_Oscilloscope:: 
mode ()
{
  return (m_parent_data.mode);
}

void LAB_Oscilloscope:: 
set_mode (LABE::OSC::MODE mode)
{
  if (m_parent_data.mode != mode)
  {
    if (LABF::is_greater_than_or_equal_to (
      time_per_division (),
      LABC::OSC::MIN_TIME_PER_DIVISION_SCREEN,
      LABC::LABSOFT::EPSILON
    ))
    {
      m_parent_data.last_mode_before_repeated = mode;
    }

    switch (mode)
    {
      case (LABE::OSC::MODE::REPEATED):
      {
        dma_buffer_count  (LABE::OSC::BUFFER_COUNT::DOUBLE);
        break;
      }

      case (LABE::OSC::MODE::SCREEN):
      {
        dma_buffer_count  (LABE::OSC::BUFFER_COUNT::SINGLE);
        break;
      }
    }

    m_parent_data.mode = mode; 
  }
}

void LAB_Oscilloscope:: 
dma_buffer_count (LABE::OSC::BUFFER_COUNT buffer_count)
{
  bool is_running = false; 

  LAB_DMA_Data_Oscilloscope& dma_data = *(static_cast<LAB_DMA_Data_Oscilloscope*>
    (m_uncached_memory.virt ()));
  
  // 1. Pause PWM pacing if running
  if (rpi ().dma.is_running (LABC::DMA::CHAN::PWM_PACING))
  {
    is_running = true;
    rpi ().dma.pause (LABC::DMA::CHAN::PWM_PACING);
  }

  // 2. Assign next control block depending on buffer
  if (buffer_count == LABE::OSC::BUFFER_COUNT::SINGLE)
  { 
    rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX, m_uncached_memory.bus (&dma_data.cbs[4]));
  }
  else if (buffer_count == LABE::OSC::BUFFER_COUNT::DOUBLE)
  {
    rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX, m_uncached_memory.bus (&dma_data.cbs[0]));
  }

  // 3. Abort the current control block 
  rpi ().dma.abort (LABC::DMA::CHAN::OSC_RX);

  // 4. Clean buffer status
  dma_data.status[0] = dma_data.status[1] = 0;

  // 5. Run DMA channel if it was running
  if (is_running)
  {
    rpi ().dma.run (LABC::DMA::CHAN::PWM_PACING);
  }
}

bool LAB_Oscilloscope:: 
is_channel_enabled (unsigned channel) const
{
  return (m_parent_data.channel_data[channel].is_enabled);
}

// Getters
bool LAB_Oscilloscope:: 
is_frontend_running () const
{
  return (m_parent_data.is_frontend_running);
}

bool LAB_Oscilloscope:: 
is_backend_running () const
{
  return (m_parent_data.is_backend_running);
}

LABE::OSC::STATUS LAB_Oscilloscope:: 
status () const
{
  return (m_parent_data.status);
}

double LAB_Oscilloscope::  
voltage_per_division (unsigned channel) const
{
  return (m_parent_data.channel_data[channel].voltage_per_division);
}

double LAB_Oscilloscope:: 
vertical_offset (unsigned channel) const
{
  return (m_parent_data.channel_data[channel].vertical_offset);
}

double LAB_Oscilloscope:: 
time_per_division () const
{
  return (m_parent_data.time_per_division);
}

unsigned LAB_Oscilloscope:: 
samples () const
{
  return (m_parent_data.samples);
}

double LAB_Oscilloscope:: 
sampling_rate () const
{
  return (m_parent_data.sampling_rate);
}

const LAB_Parent_Data_Oscilloscope& LAB_Oscilloscope:: 
parent_data ()
{
  return (m_parent_data);
}

const std::array<double, LABC::OSC::NUMBER_OF_SAMPLES>& LAB_Oscilloscope:: 
chan_samples (unsigned channel) const
{
  return (m_parent_data.channel_data[channel].samples);
}

double LAB_Oscilloscope:: 
chan_voltage_per_division (unsigned channel) const
{
  return (m_parent_data.channel_data[channel].voltage_per_division);
}

double LAB_Oscilloscope:: 
chan_vertical_offset (unsigned channel) const 
{
  return (m_parent_data.channel_data[channel].vertical_offset);
}

double LAB_Oscilloscope:: 
raw_buffer_time_per_division () const
{
  return (m_parent_data.time_per_division_raw_buffer);
}

double LAB_Oscilloscope:: 
raw_buffer_horizontal_offset () const 
{
  return (m_parent_data.horizontal_offset_raw_buffer);
}

double LAB_Oscilloscope:: 
raw_buffer_sampling_rate () const
{
  return (m_parent_data.sampling_rate_raw_buffer);
}

// LAB_Oscilloscope::Measurements

LAB_Oscilloscope::Measurements:: 
Measurements (LAB_Parent_Data_Oscilloscope& _LAB_Parent_Data_Oscilloscope)
  : m_pdata (_LAB_Parent_Data_Oscilloscope)
{

}

double LAB_Oscilloscope::Measurements:: 
avg (unsigned channel)
{
  return (m_pdata.channel_data[channel].measurements.avg);
}

double LAB_Oscilloscope::Measurements:: 
max (unsigned channel)
{
  return (m_pdata.channel_data[channel].measurements.max);
}

double LAB_Oscilloscope::Measurements:: 
min (unsigned channel)
{
  return (m_pdata.channel_data[channel].measurements.min);
}

double LAB_Oscilloscope::Measurements:: 
trms (unsigned channel)
{
  return (m_pdata.channel_data[channel].measurements.trms);
}

LAB_Oscilloscope::Measurements& LAB_Oscilloscope:: 
measurements ()
{
  return (m_measurements);
}

// LAB_Oscilloscope::Calibration

LAB_Oscilloscope::Calibration:: 
Calibration (LAB_Parent_Data_Oscilloscope& _LAB_Parent_Data_Oscilloscope)
  : m_pdata (_LAB_Parent_Data_Oscilloscope)
{

}

void LAB_Oscilloscope::Calibration:: 
enable ()
{
  m_pdata.is_calibration_enabled = true;
}

void LAB_Oscilloscope::Calibration:: 
disable ()
{
  m_pdata.is_calibration_enabled = false;
}

bool LAB_Oscilloscope::Calibration:: 
is_enabled () const
{
  return (m_pdata.is_calibration_enabled);
}

void LAB_Oscilloscope::Calibration:: 
adc_reference_voltage (unsigned channel, double value)
{
  LAB_Calibration_Data_Oscilloscope& calib = m_pdata.channel_data[channel].calibration;

  calib.adc_reference_voltage         = value;
  calib.conversion_reference_voltage  = calib.adc_reference_voltage / 2.0;
  calib.conversion_constant           = calib.conversion_reference_voltage / 
                                        ((LABC::OSC::ADC_RESOLUTION_INT - 1) >> 1); 
}

void LAB_Oscilloscope::Calibration:: 
vertical_offset_corrector (unsigned          channel, 
                          LABE::OSC::SCALING scaling,
                          double             value)
{
  m_pdata.channel_data.at (channel).calibration.
    vertical_offset_corrector.at (scaling) = value;
}

void LAB_Oscilloscope::Calibration:: 
scaling_corrector_to_unity (unsigned           channel, 
                            LABE::OSC::SCALING scaling,
                            double             value)
{
  m_pdata.channel_data.at (channel).calibration.
    scaling_corrector_to_unity.at (scaling) = value;
}

void LAB_Oscilloscope::Calibration:: 
scaling_corrector_to_actual (unsigned          channel, 
                                    LABE::OSC::SCALING scaling,
                                    double             value)
{
  m_pdata.channel_data.at (channel).calibration.
    scaling_corrector_to_actual.at (scaling) = value;
}

double LAB_Oscilloscope::Calibration:: 
adc_reference_voltage (unsigned channel) const
{
  return (m_pdata.channel_data.at (channel).calibration.adc_reference_voltage);
}

double LAB_Oscilloscope::Calibration:: 
vertical_offset_corrector (unsigned           channel, 
                           LABE::OSC::SCALING scaling) const
{
  return (m_pdata.channel_data.at (channel).calibration.
    vertical_offset_corrector.at (scaling));
}

double LAB_Oscilloscope::Calibration:: 
scaling_corrector_to_unity (unsigned           channel, 
                            LABE::OSC::SCALING scaling) const
{
  return (m_pdata.channel_data.at (channel).calibration.
    scaling_corrector_to_unity.at (scaling));
}      

double LAB_Oscilloscope::Calibration:: 
scaling_corrector_to_actual (unsigned           channel, 
                             LABE::OSC::SCALING scaling) const
{
  return (m_pdata.channel_data.at (channel).calibration.
    scaling_corrector_to_actual.at (scaling));
}  

LAB_Oscilloscope::Calibration& LAB_Oscilloscope:: 
calibration ()
{
  return (m_calibration);
}

// EOF