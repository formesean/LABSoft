#include "LAB_Logic_Analyzer.h"

#include <bitset>
#include <thread>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "LAB.h"
#include "../Utility/LAB_Utility_Functions.h"

LAB_Logic_Analyzer::
LAB_Logic_Analyzer (LAB& _LAB)
  : LAB_Module (_LAB)
{
  // init_pwm (); // or maybe init_pcm () ???
  init_gpio_pins  ();
  init_dma        ();

  // m_LAB.rpi ().gpio.set_event_detect (LABC::PIN::LOGIC_ANALYZER[1],
  //   AP::GPIO::EVENT::RISING_EDGE, true);
}

LAB_Logic_Analyzer::
~LAB_Logic_Analyzer ()
{

}

void LAB_Logic_Analyzer:: 
init_gpio_pins ()
{
  for (unsigned chan = 0; chan < (m_parent_data.channel_data.size ()); chan++)
  {
    m_LAB.rpi ().gpio.set (LABC::PIN::LOGAN[chan], AP::GPIO::FUNC::INPUT,
      AP::GPIO::PULL::DOWN);
  }
}

void LAB_Logic_Analyzer:: 
init_dma ()
{
  config_dma_cb ();

  AikaPi::Uncached&            un = m_uncached_memory;
  LAB_DMA_Data_Logic_Analyzer& dd = *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(un.virt ()));

  m_LAB.rpi ().dma.start (LABC::DMA::CHAN::LOGAN_GPIO_STORE, un.bus (&dd.cbs[0]));
}

void LAB_Logic_Analyzer:: 
init_interrupts ()
{
  for (unsigned chan = 0; chan < LABC::LOGAN::NUMBER_OF_CHANNELS; chan++)
  {
    m_LAB.rpi ().gpio.clear_all_event_detect (LABC::PIN::LOGAN[chan]);
  }

  m_LAB.rpi ().gpio.clear_event_detect_status ();
}

void LAB_Logic_Analyzer::
config_dma_cb ()
{
  m_uncached_memory.map_uncached_mem (LABC::LOGAN::VC_MEM_SIZE);

  LAB_DMA_Data_Logic_Analyzer& uncached_dma_data =
    *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(m_uncached_memory.virt ()));

  LAB_DMA_Data_Logic_Analyzer new_uncached_dma_data  = 
  {
    .cbs = 
    { 
      // Double buffer
      // 0
      {
        LABC::DMA::TI::LOGAN_STORE,
        m_LAB.rpi ().gpio.bus  (AP::GPIO::GPLEV0),
        m_uncached_memory.bus (&uncached_dma_data.rxd[0]),
        static_cast<uint32_t> (sizeof (uint32_t) * LABC::LOGAN::MAX_NUMBER_OF_SAMPLES),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[1]),
        0
      },
      // 1
      {
        LABC::DMA::TI::LOGAN_STORE | AP::DMA::TI_DATA::INTEN,
        m_uncached_memory.bus (&uncached_dma_data.buffer_ok_flag),
        m_uncached_memory.bus (&uncached_dma_data.status[0]),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[2]),
        0
      },
      // 2
      {
        LABC::DMA::TI::LOGAN_STORE,
        m_LAB.rpi ().gpio.bus  (AP::GPIO::GPLEV0),
        m_uncached_memory.bus (&uncached_dma_data.rxd[1]),
        static_cast<uint32_t> (sizeof (uint32_t) * LABC::LOGAN::MAX_NUMBER_OF_SAMPLES),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[3]),
        0
      },
      // 3
      {
        LABC::DMA::TI::LOGAN_STORE | AP::DMA::TI_DATA::INTEN,
        m_uncached_memory.bus (&uncached_dma_data.buffer_ok_flag),
        m_uncached_memory.bus (&uncached_dma_data.status[1]),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[0]),
        0
      },

      // Single buffer
      // 4
      {
        LABC::DMA::TI::LOGAN_STORE,
        m_LAB.rpi ().gpio.bus  (AP::GPIO::GPLEV0),
        m_uncached_memory.bus (&uncached_dma_data.rxd[0]),
        static_cast<uint32_t> (sizeof (uint32_t) * LABC::LOGAN::MAX_NUMBER_OF_SAMPLES),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[5]),
        0
      },
      // 5
      {
        LABC::DMA::TI::LOGAN_STORE | AP::DMA::TI_DATA::INTEN,
        m_uncached_memory.bus (&uncached_dma_data.buffer_ok_flag),
        m_uncached_memory.bus (&uncached_dma_data.status[0]),
        sizeof (uint32_t),
        0,
        m_uncached_memory.bus (&uncached_dma_data.cbs[4]),
        0
      },
    },
  };

  std::memcpy (
    &uncached_dma_data, 
    &new_uncached_dma_data, 
    sizeof (new_uncached_dma_data)
  );
}

void LAB_Logic_Analyzer:: 
run ()
{
  // m_LAB.rpi ().pwm.start (LABC::PWM::DMA_PACING_CHAN);

  // ==========

  m_parent_data.is_backend_running  = true;
  m_parent_data.is_frontend_running = true;

  m_thread_get_samples = std::thread (&LAB_Logic_Analyzer::get_samples_loop, this);

  m_parent_data.status = LABE::LOGAN::STATUS::AUTO;
}

void LAB_Logic_Analyzer:: 
stop ()
{
  // m_LAB.rpi ().pwm.stop (LABC::PWM::DMA_PACING_CHAN);

  // ==========

  m_parent_data.is_backend_running  = false;
  m_parent_data.is_frontend_running = false;

  m_thread_get_samples.join ();

  m_parent_data.status = LABE::LOGAN::STATUS::STOP;
}

void LAB_Logic_Analyzer:: 
single ()
{
  if (!is_running ())
  {
    run ();
  }

  m_parent_data.single = true;
}

bool LAB_Logic_Analyzer:: 
is_running () const
{
  return (m_parent_data.is_backend_running && m_parent_data.is_frontend_running);
}

void LAB_Logic_Analyzer:: 
fill_raw_sample_buffer_from_dma_buffer ()
{
  LAB_DMA_Data_Logic_Analyzer& dma_data = 
    *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(m_uncached_memory.virt ()));

  switch (mode ())
  {
    case (LABE::LOGAN::MODE::SCREEN):
    {
      std::memcpy (
        m_parent_data.raw_data_buffer.data (),
        const_cast<const void*>(static_cast<volatile void*>(dma_data.rxd[0])),
        sizeof (uint32_t) * m_parent_data.samples
      );

      break;
    }

    case (LABE::LOGAN::MODE::REPEATED):
    {
      for (unsigned buff = 0; buff < 2; buff++)
      {
        if (dma_data.status[buff])
        {
          std::memcpy (
            m_parent_data.raw_data_buffer.data (),
            const_cast<const void*>(static_cast<volatile void*>(dma_data.rxd[buff])),
            sizeof (uint32_t) * m_parent_data.samples
          );
        }

        if (dma_data.status[buff ^ 1])
        {
          dma_data.status[0] = dma_data.status[1] = 0;

          break;
        }

        dma_data.status[buff] = 0;
      }
    }
  }
}

void LAB_Logic_Analyzer:: 
parse_raw_sample_buffer ()
{
  LAB_Parent_Data_Logic_Analyzer& pdata = m_parent_data;

  for (unsigned samp = 0; samp < pdata.samples; samp++)
  {
    for (unsigned chan = 0; chan < pdata.channel_data.size (); chan++)
    {
      pdata.channel_data[chan].samples[samp] = 
        (pdata.raw_data_buffer[samp] >> LABC::PIN::LOGAN[chan]) & 0x1;    

      // // For debug
      // if (samp == 0 && chan == 0)
      // {
      //   std::cout << std::bitset <32> (pdata.raw_data_buffer[samp]) << "\n";
      // }  
    }
  }

  if (m_parent_data.trigger_frame_ready)
  {
    m_parent_data.trigger_frame_ready = false;
  }
}

void LAB_Logic_Analyzer:: 
get_samples_loop ()
{
  LAB_DMA_Data_Logic_Analyzer& dma_data = 
    *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(m_uncached_memory.virt ()));

  while (m_parent_data.is_backend_running)
  {
    // for (int a = 0; a < 2; a++)
    // {
    //   for (int b = 0; b < LABC::LOGAN::MAX_NUMBER_OF_SAMPLES; b++)
    //   {
    //     dma_data.rxd[a][b] = m_LAB.rpi ().gpio.level ();

    //     if (!m_parent_data.is_backend_running)
    //     {
    //       return;
    //     }

    //     std::this_thread::sleep_for (std::chrono::duration<double, 
    //       std::milli>((m_parent_data.sampling_period)));
    //   }
    // }

    // ==========

    for (int a = 0; a < 1; a++)
    {
      for (int b = 0; b < LABC::LOGAN::MAX_NUMBER_OF_SAMPLES; b++)
      {
        dma_data.rxd[a][b] = m_LAB.rpi ().gpio.level ();

        if (!m_parent_data.is_backend_running)
        {
          return;
        }

        std::this_thread::sleep_for (std::chrono::duration<double, 
          std::milli>((m_parent_data.sampling_period)));
      }
    }
  }
}

LABE::LOGAN::MODE LAB_Logic_Analyzer:: 
calc_mode (double time_per_division) const 
{
  if (time_per_division < LABC::LOGAN::MIN_TIME_PER_DIVISION_SCREEN)
  {
    return (LABE::LOGAN::MODE::REPEATED);
  }
  else 
  {
    return (m_parent_data.last_mode_before_repeated);
  }
}

void LAB_Logic_Analyzer:: 
set_mode (LABE::LOGAN::MODE mode)
{
  if (m_parent_data.mode != mode)
  {
    if (time_per_division () >= LABC::LOGAN::MIN_TIME_PER_DIVISION_SCREEN)
    {
      m_parent_data.last_mode_before_repeated = mode;
    }

    switch (mode)
    {
      case (LABE::LOGAN::MODE::REPEATED):
      {
        dma_buffer_count (LABE::LOGAN::BUFFER_COUNT::DOUBLE);
        break;
      }

      case (LABE::LOGAN::MODE::SCREEN):
      {
        dma_buffer_count (LABE::LOGAN::BUFFER_COUNT::SINGLE);
        break;
      }
    }

    m_parent_data.mode = mode;
  }
}

void LAB_Logic_Analyzer:: 
dma_buffer_count  (LABE::LOGAN::BUFFER_COUNT buffer_count)
{
  bool is_running = false; 

  LAB_DMA_Data_Logic_Analyzer& dma_data = 
    *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(m_uncached_memory.virt ()));
  
  // 1. Pause PWM pacing if running
  if (m_LAB.rpi ().dma.is_running (LABC::DMA::CHAN::PWM_PACING))
  {
    is_running = true;
    m_LAB.rpi ().dma.pause (LABC::DMA::CHAN::PWM_PACING);
  }

  // 2. Assign next control block depending on buffer
  if (buffer_count == LABE::LOGAN::BUFFER_COUNT::SINGLE)
  { 
    m_LAB.rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX, m_uncached_memory.bus (&dma_data.cbs[4]));
  }
  else if (buffer_count == LABE::LOGAN::BUFFER_COUNT::DOUBLE)
  {
    m_LAB.rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX, m_uncached_memory.bus (&dma_data.cbs[0]));
  }

  // 3. Abort the current control block 
  m_LAB.rpi ().dma.abort (LABC::DMA::CHAN::OSC_RX);

  // 4. Clean buffer status
  dma_data.status[0] = dma_data.status[1] = 0;

  // 5. Run DMA channel if it was running
  if (is_running)
  {
    m_LAB.rpi ().dma.run (LABC::DMA::CHAN::PWM_PACING);
  }
}

void LAB_Logic_Analyzer:: 
set_samples (unsigned value)
{
  m_parent_data.samples = value;

  LAB_DMA_Data_Logic_Analyzer& uncached_dma_data = 
    *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(m_uncached_memory.virt ()));
  
  uncached_dma_data.cbs[0].txfr_len = static_cast<uint32_t>(sizeof (uint32_t) * value);
  uncached_dma_data.cbs[2].txfr_len = static_cast<uint32_t>(sizeof (uint32_t) * value);
  uncached_dma_data.cbs[4].txfr_len = static_cast<uint32_t>(sizeof (uint32_t) * value);
}

void LAB_Logic_Analyzer:: 
set_time_per_division (double value)
{
  m_parent_data.time_per_division = value;

  // set_mode (calc_mode (value));

  // reset_dma_process ();
}

void LAB_Logic_Analyzer:: 
set_time_per_division (unsigned samples, 
                       double   sampling_rate)
{
  set_time_per_division (calc_time_per_division (samples, sampling_rate));
}

double LAB_Logic_Analyzer:: 
calc_sampling_rate (unsigned  samples, 
                    double    time_per_division)
{
  double new_sampling_rate = samples / (time_per_division *
    LABC::LOGAN::DISPLAY_NUMBER_OF_COLUMNS);
  
  if (new_sampling_rate > LABC::LOGAN::MAX_SAMPLING_RATE)
  {
    return (LABC::OSC::MAX_SAMPLING_RATE);
  }
  else 
  {
    return (new_sampling_rate);
  }
}

double LAB_Logic_Analyzer:: 
calc_sample_count (double sampling_rate, 
                   double time_per_division)
{
  double new_sample_count = sampling_rate * 
    LABC::LOGAN::DISPLAY_NUMBER_OF_COLUMNS * time_per_division;

  if (new_sample_count > LABC::LOGAN::MAX_SAMPLES)
  {
    return (LABC::LOGAN::MAX_SAMPLES);
  }
  else 
  {
    return (new_sample_count);
  }
}

void LAB_Logic_Analyzer:: 
set_sampling_rate (double value)
{
  // 1. Change the source frequency of the PWM peripheral
  //m_LAB.rpi ().pwm.frequency (LABC::PWM::DMA_PACING_CHAN, value);

  // // while we are using the PWM pacing of the oscilloscope module
  // {
  //   // 2. Set the DMA PWM duty cycle to 50%
  //   LAB_DMA_Data_Logic_Analyzer& dma_data = 
  //     *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(m_uncached_memory.virt ()));

  //   // 2. Set the DMA PWM duty cycle to 50%
  //   LAB_DMA_Data_Logic_Analyzer& dma_data = 
  //     *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(m_uncached_memory.virt ()));
  //     
  //   dma_data.pwm_duty_cycle = (m_LAB.rpi ().pwm.range (LABC::PWM::DMA_PACING_CHAN)) / 2.0;
  // }

  // 3. Store the sampling rate
  //m_parent_data.sampling_rate = value;

  // ==========

  m_parent_data.sampling_rate   = value;
  m_parent_data.sampling_period = 1.0 / value;
}

void LAB_Logic_Analyzer:: 
set_trigger_condition (unsigned               gpio_pin, 
                       LABE::LOGAN::TRIG::CND condition)
{ 
  AP::GPIO::EVENT event;

  switch (condition)
  {
    case (LABE::LOGAN::TRIG::CND::IGNORE):
    {
      return;
    }

    case (LABE::LOGAN::TRIG::CND::LOW):
    {
      event = AP::GPIO::EVENT::LOW;
      break;
    }

    case (LABE::LOGAN::TRIG::CND::HIGH):
    {
      event = AP::GPIO::EVENT::HIGH;
      break;
    }

    case (LABE::LOGAN::TRIG::CND::RISING_EDGE):
    {
      event = AP::GPIO::EVENT::RISING_EDGE;
      break;
    }

    case (LABE::LOGAN::TRIG::CND::FALLING_EDGE):
    {
      event = AP::GPIO::EVENT::FALLING_EDGE;
      break;
    }

    case (LABE::LOGAN::TRIG::CND::EITHER_EDGE):
    {
      set_trigger_condition (gpio_pin, LABE::LOGAN::TRIG::CND::RISING_EDGE);
      set_trigger_condition (gpio_pin, LABE::LOGAN::TRIG::CND::FALLING_EDGE);

      return;
    }
  }

  m_LAB.rpi ().gpio.set_event_detect (gpio_pin, event, 1);
}

void LAB_Logic_Analyzer::
parse_trigger_mode ()
{
  switch (m_parent_data.trigger_mode)
  {
    case (LABE::LOGAN::TRIG::MODE::NONE):
    {
      if (m_thread_find_trigger.joinable ())
      {
        m_parent_data.trigger_enabled = false;
        m_thread_find_trigger.join ();
      }

      break;
    }

    case (LABE::LOGAN::TRIG::MODE::AUTO):
    {
      if (!m_thread_find_trigger.joinable ())
      {
        m_parent_data.trigger_enabled = true;
        m_thread_find_trigger = std::thread (&LAB_Logic_Analyzer::find_trigger_point_loop, this);
      }

      break;
    }

    case (LABE::LOGAN::TRIG::MODE::NORMAL):
    {
      if (!m_thread_find_trigger.joinable ())
      {
        m_parent_data.trigger_enabled = true;
        m_thread_find_trigger = std::thread (&LAB_Logic_Analyzer::find_trigger_point_loop, this);
      }

      break;
    }
  }
}

/**
 * This is the main find trigger loop. 
 */
void LAB_Logic_Analyzer:: 
find_trigger_point_loop ()
{
  LAB_DMA_Data_Oscilloscope& dma_data = *(static_cast<LAB_DMA_Data_Oscilloscope*>
    (m_uncached_memory.virt ()));

  LAB_Parent_Data_Logic_Analyzer& pdata = m_parent_data;

  // ===== Cached Variables =====

  uint32_t buff0_cbs_addr[2] = {m_uncached_memory.bus (&dma_data.cbs[0]),
                                m_uncached_memory.bus (&dma_data.cbs[1])};
  uint32_t buff1_cbs_addr[2] = {m_uncached_memory.bus (&dma_data.cbs[2]),
                                m_uncached_memory.bus (&dma_data.cbs[3])};
  
  // ====================

  m_LAB.rpi ().gpio.clear_event_detect_status ();

  while (pdata.trigger_enabled)
  {
    if (!pdata.trigger_found)
    {
      // 1. Check if there is a change in any of the bits in the 
      //    GPIO Event Detect Status Register (GPEDS0). If there is,
      //    this means that a trigger condition on a channel just happened.
      uint32_t temp_GPSED0 = m_LAB.rpi ().gpio.event_detect_status ();

      if (pdata.trigger_flags != temp_GPSED0)
      {
        // 2. Store the memory address where the DMA engine is currently
        //    writing data on. This to aid in finding the trigger index.
        unsigned curr_dma_write_mem_addr = *(m_LAB.rpi ().dma.reg 
          (LABC::DMA::CHAN::LOGAN_GPIO_STORE, AP::DMA::DEST_AD));

        // 3. Store the memory address of the current DMA control block 
        //    running in the Logic Analyzer DMA engine. 
        //    This is to know what buffer (0 or 1) is currently being filled.
        uint32_t curr_conblk_ad = *(m_LAB.rpi ().dma.reg 
          (LABC::DMA::CHAN::LOGAN_GPIO_STORE, AP::DMA::CONBLK_AD));
        
        // 4. Identify the current buffer index.
        if (curr_conblk_ad == buff0_cbs_addr[0] || curr_conblk_ad == buff0_cbs_addr[1])
        {
          m_parent_data.trigger_buffer_index = 1;
        }
        else if (curr_conblk_ad == buff1_cbs_addr[0] || curr_conblk_ad == buff1_cbs_addr[1])
        {
          m_parent_data.trigger_buffer_index = 0;
        }

        

        // 4. Check if the triggered channel resulted to an actual trigger event.
        //    This takes into account the trigger conditions of other channels.
        if (check_if_triggered (temp_GPSED0))
        {
          create_trigger_frame ();
        }
      }

      std::this_thread::sleep_for (
        std::chrono::duration<double, std::milli> (m_parent_data.check_trigger_sleep_period)
      );
    }
  }
}

/**
 * This function checks whether the triggered pin
 * results to an actual trigger event. 
 */
bool LAB_Logic_Analyzer:: 
check_if_triggered (uint32_t event_detect_status_register_value)
{
  // 1. Check if the changed bit in the Event Detect Status register
  //    corresponds to a channel that has a edge trigger condition.
  for (unsigned i = 0; i < m_parent_data.trigger_cache_edge.size (); i++)
  {
    unsigned chan     = m_parent_data.trigger_cache_edge[i];
    unsigned gpio_pin = LABC::PIN::LOGAN[chan];

    if (((event_detect_status_register_value >> gpio_pin) & 0x1) == 1)
    {
      // reset event detect status register 
      m_LAB.rpi ().gpio.clear_event_detect_status ();

      return (true);
    }
  }

  // 2. Check if the changed bit in the Event Detect Status register
  //    corresponds to a channel that has an edge trigger condition.
  {
    for (unsigned i = 0; i < m_parent_data.trigger_cache_level.size (); i++)
    {
      unsigned chan     = m_parent_data.trigger_cache_level[i];
      unsigned gpio_pin = LABC::PIN::LOGAN[chan];

      if (((event_detect_status_register_value >> gpio_pin) & 0x1) != 1)
      {
        return (false);
      }
    }

    // reset event detect status register 
    m_LAB.rpi ().gpio.clear_event_detect_status ();
    return (true);
  }
}

/**
 * This function creates a trigger frame. A trigger frame consists of  
 * the samples before the trigger point, the actual trigger point, and
 * all samples after the trigger point.
 */
void LAB_Logic_Analyzer:: 
create_trigger_frame ()
{
  // static constexpr unsigned samp_half           = LABC::LOGAN::MAX_NUMBER_OF_SAMPLES / 2;
  // static constexpr unsigned samp_half_index     = samp_half - 1;
  // static LAB_DMA_Data_Logic_Analyzer& dma_data  = *(static_cast<LAB_DMA_Data_Logic_Analyzer*>
  //                                                   (m_uncached_memory.virt ()));

  // if (m_parent_data.trig_index >= samp_half_index)
  // {
  //   unsigned  copy_count_0  = samp_half,
  //             copy_count_1  = LABC::LOGAN::MAX_NUMBER_OF_SAMPLES - 1 - m_parent_data.trig_index,
  //             copy_count_2  = samp_half - copy_count_1;
    
  //   std::memcpy (
  //     m_parent_data.raw_data_buffer.data (),
  //     m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index].data () 
  //       + (m_parent_data.trig_index - samp_half_index),
  //     sizeof (uint32_t) * copy_count_0
  //   );

  //   std::memcpy (
  //     m_parent_data.raw_data_buffer.data () + (copy_count_0),
  //     m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index].data () 
  //       + (m_parent_data.trig_index + 1),
  //     sizeof (uint32_t) * copy_count_1
  //   );

  //   while (!((*(m_LAB.rpi ().dma.Peripheral::reg (AP::DMA::INT_STATUS)) >> LABC::DMA::CHAN::OSC_RX) & 0x1));

  //   std::memcpy (
  //     m_parent_data.raw_data_buffer.data () + (copy_count_0 + copy_count_1),
  //     const_cast<const void*>(static_cast<const volatile void*>(dma_data.rxd[m_parent_data.trigger_buffer_index ^ 1])),
  //     sizeof (uint32_t) * copy_count_2
  //   );

  //   m_LAB.rpi ().dma.Peripheral::reg_wbits (AP::DMA::INT_STATUS, 0, LABC::DMA::CHAN::OSC_RX);
  // }
  // else if (m_parent_data.trig_index < samp_half_index)
  // {
  //   unsigned  copy_count_2 = samp_half,
  //             copy_count_1 = m_parent_data.trig_index + 1,
  //             copy_count_0 = samp_half - copy_count_1;
                  
  //   std::memcpy (
  //     m_parent_data.raw_data_buffer.data (),
  //     m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index ^ 1].data () 
  //       + (LABC::LOGAN::MAX_NUMBER_OF_SAMPLES - copy_count_0),
  //     sizeof (uint32_t) * copy_count_0
  //   );

  //   std::memcpy (
  //     m_parent_data.raw_data_buffer.data () + (copy_count_0),
  //     m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index].data (),
  //     sizeof (uint32_t) * copy_count_1
  //   );

  //   std::memcpy (
  //     m_parent_data.raw_data_buffer.data () + (copy_count_0 + copy_count_1),
  //     m_parent_data.trig_buffs.pre_trigger[m_parent_data.trigger_buffer_index].data () + 
  //       (m_parent_data.trig_index + 1),
  //     sizeof (uint32_t) * copy_count_2
  //   );
  // }

  m_parent_data.trigger_frame_ready = true;
}

void LAB_Logic_Analyzer:: 
cache_trigger_condition (unsigned               channel, 
                         LABE::LOGAN::TRIG::CND condition)
{
  LABE::LOGAN::TRIG::CND prev_cnd = m_parent_data.channel_data[channel].trigger_condition;

  if (condition != prev_cnd)
  {
    // 1. Delete old condition from trigger cache.
    if (prev_cnd != LABE::LOGAN::TRIG::CND::IGNORE)
    {
      std::vector<unsigned>* vec;

      switch (prev_cnd)
      {
        case (LABE::LOGAN::TRIG::CND::HIGH):
        case (LABE::LOGAN::TRIG::CND::LOW):
        {
          vec = &m_parent_data.trigger_cache_level; 
          break;
        }

        case (LABE::LOGAN::TRIG::CND::RISING_EDGE):
        case (LABE::LOGAN::TRIG::CND::FALLING_EDGE):
        case (LABE::LOGAN::TRIG::CND::EITHER_EDGE):
        {
          vec = &m_parent_data.trigger_cache_edge;
          break;
        }

        std::vector<unsigned>::iterator it = std::find (vec->begin (), vec->end (), channel);
      
        if (it != vec->end ())
        {
          vec->erase (it);
        }
      }
    }

    // 2. Add new condition to trigger cache.
    if (condition != LABE::LOGAN::TRIG::CND::IGNORE)
    {
      switch (prev_cnd)
      {
        case (LABE::LOGAN::TRIG::CND::HIGH):
        case (LABE::LOGAN::TRIG::CND::LOW):
        {
          m_parent_data.trigger_cache_level.emplace_back (channel);
          break;
        }

        case (LABE::LOGAN::TRIG::CND::RISING_EDGE):
        case (LABE::LOGAN::TRIG::CND::FALLING_EDGE):
        case (LABE::LOGAN::TRIG::CND::EITHER_EDGE):
        {
          m_parent_data.trigger_cache_level.emplace_back (channel);
          break;
        }
      }
    }
  }
}

void LAB_Logic_Analyzer:: 
reset_dma_process ()
{
  LAB_DMA_Data_Logic_Analyzer& dma_data = 
    *(static_cast<LAB_DMA_Data_Logic_Analyzer*>(m_uncached_memory.virt ()));

  bool is_running = m_LAB.rpi ().dma.is_running (LABC::DMA::CHAN::PWM_PACING);

  // 1. Check if DMA is running. It is, pause it
  if (is_running)
  {
    m_LAB.rpi ().dma.pause (LABC::DMA::CHAN::PWM_PACING);
  }

  // 2. Reset the DMA engine to the first control block, depending on the buffer
  switch (mode ())
  {
    case (LABE::LOGAN::MODE::REPEATED): // dual buffer
    {
      m_LAB.rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX, 
        m_uncached_memory.bus (&dma_data.cbs[0]));

      break;
    }

    case (LABE::LOGAN::MODE::SCREEN): // single buffer
    {
      m_LAB.rpi ().dma.next_cb (LABC::DMA::CHAN::OSC_RX, 
        m_uncached_memory.bus (&dma_data.cbs[4]));

      break;
    }
  }

  // 3. Abort the current control block
  m_LAB.rpi ().dma.abort (LABC::DMA::CHAN::OSC_RX);

  // 4. Reset the DMA status flags
  dma_data.status[0] = dma_data.status[1] = 0;

  // 5. Reset the 2D DMA OSC RX array
  std::memset (
    const_cast<void*>(static_cast<volatile void*>(dma_data.rxd)),
    0,
    sizeof (dma_data.rxd)
  );

  // 6. Run DMA if it was running
  if (is_running)
  {
    m_LAB.rpi ().dma.run (LABC::DMA::CHAN::PWM_PACING);
  }
}

void LAB_Logic_Analyzer:: 
update_data_samples ()
{
  if (is_running ())
  {
    switch (m_parent_data.trigger_mode)
    {
      case (LABE::LOGAN::TRIG::MODE::NONE):
      {
        fill_raw_sample_buffer_from_dma_buffer  ();
        parse_raw_sample_buffer                 ();  

        break;
      }

      case (LABE::LOGAN::TRIG::MODE::NORMAL):
      {
        if (m_parent_data.trigger_frame_ready)
        {
          parse_raw_sample_buffer ();
        }

        break;
      }

      case (LABE::LOGAN::TRIG::MODE::AUTO):
      {
        if (m_parent_data.trigger_frame_ready)
        {
          parse_raw_sample_buffer ();
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
}

double LAB_Logic_Analyzer:: 
calc_samp_count (double time_per_div, unsigned osc_disp_num_cols)
{
  if (time_per_div >= LABC::LOGAN::MIN_TIME_PER_DIV_NO_ZOOM)
  {
    return (LABC::LOGAN::MAX_NUMBER_OF_SAMPLES);
  }
  else 
  {
    return (LABC::LOGAN::MAX_SAMPLING_RATE * osc_disp_num_cols *
      time_per_div);
  }
}

double LAB_Logic_Analyzer:: 
calc_samp_rate (double time_per_div, unsigned osc_disp_num_cols)
{
  if (time_per_div <= LABC::LOGAN::MIN_TIME_PER_DIV_NO_ZOOM)
  {
    return (LABC::LOGAN::MAX_SAMPLING_RATE);
  }
  else 
  {
    return (
      LABC::LOGAN::MAX_NUMBER_OF_SAMPLES / (time_per_div * osc_disp_num_cols)
    );
  }
}

LABE::LOGAN::MODE LAB_Logic_Analyzer:: 
calc_mode (double time_per_division)
{
  LABE::LOGAN::MODE mode;

  if (time_per_division < LABC::LOGAN::MIN_TIME_PER_DIVISION_SCREEN)
  {
    mode = LABE::LOGAN::MODE::REPEATED;
  } 
  else if (m_parent_data.time_per_division < LABC::LOGAN::MIN_TIME_PER_DIVISION_SCREEN &&
    time_per_division >= LABC::LOGAN::MIN_TIME_PER_DIVISION_SCREEN)
  {
    mode = LABE::LOGAN::MODE::SCREEN;
  }

  return (mode);
}

LAB_Parent_Data_Logic_Analyzer& LAB_Logic_Analyzer:: 
parent_data ()
{
  return (m_parent_data);
}

void LAB_Logic_Analyzer:: 
mode (LABE::LOGAN::MODE mode)
{
  switch (mode)
  {
    case (LABE::LOGAN::MODE::REPEATED):
    {
      break;
    }

    case (LABE::LOGAN::MODE::SCREEN):
    {
      if (time_per_division () < LABC::LOGAN::MIN_TIME_PER_DIVISION_SCREEN)
      {
        time_per_division (LABC::LOGAN::MIN_TIME_PER_DIVISION_SCREEN);
      }

      break;
    }

    case (LABE::LOGAN::MODE::RECORD):
    {
      stop ();
      break;
    }
  }

  set_mode (mode);
}

LABE::LOGAN::MODE LAB_Logic_Analyzer:: 
mode ()
{
  return (m_parent_data.mode);
}

// --- Horizontal

void LAB_Logic_Analyzer:: 
horizontal_offset (double value)
{
  m_parent_data.horizontal_offset = value;
}

double LAB_Logic_Analyzer:: 
horizontal_offset () const
{
  return (m_parent_data.horizontal_offset);
}

void LAB_Logic_Analyzer:: 
time_per_division (double value)
{
  if (LABF::is_within_range (value, LABC::LOGAN::MIN_TIME_PER_DIVISION, 
    LABC::LOGAN::MAX_TIME_PER_DIVISION, LABC::LABSOFT::EPSILON))
  {
    double new_sampling_rate = calc_sampling_rate (m_parent_data.samples, value);

    if (value < LABC::LOGAN::MIN_TIME_PER_DIVISION_NO_ZOOM)
    {
      unsigned new_sample_count = calc_sample_count (m_parent_data.sampling_rate, value);

      set_samples (new_sample_count);
    }

    set_sampling_rate     (new_sampling_rate);
    set_time_per_division (value);
  }
}

double LAB_Logic_Analyzer:: 
time_per_division () const
{
  return (m_parent_data.time_per_division);
}

void LAB_Logic_Analyzer::
samples (unsigned value)
{

}

unsigned LAB_Logic_Analyzer::
samples () const
{
  return (m_parent_data.samples);
}

void LAB_Logic_Analyzer::
sampling_rate (double value)
{
  if (LABF::is_within_range (value, LABC::LOGAN::MIN_SAMPLING_RATE,
    LABC::LOGAN::MAX_SAMPLING_RATE, LABC::LABSOFT::EPSILON))
  {
    set_time_per_division (m_parent_data.samples, value);
    set_sampling_rate     (value);
  }
}

double LAB_Logic_Analyzer:: 
calc_time_per_division (unsigned  samples,
                        double    sampling_rate)
{
  return (samples / (sampling_rate * LABC::LOGAN::DISPLAY_NUMBER_OF_COLUMNS));
}

double LAB_Logic_Analyzer::
sampling_rate () const
{
  return (m_parent_data.sampling_rate);
}

void LAB_Logic_Analyzer:: 
trigger_mode (LABE::LOGAN::TRIG::MODE value)
{
  m_parent_data.trigger_mode = value;

  parse_trigger_mode ();
}

void LAB_Logic_Analyzer:: 
trigger_condition (unsigned channel, LABE::LOGAN::TRIG::CND condition)
{  
  // 1. Delete the old trigger condition of the channel and cache the new one.
  cache_trigger_condition (channel, condition);

  // 2. Store the channel's new trigger condition to channel_data.
  m_parent_data.channel_data[channel].trigger_condition = condition;

  // 3. Get the BCM GPIO pin of the channel.
  unsigned gpio_pin = LABC::PIN::LOGAN[channel];

  // 4. Clear all event detect conditions of the GPIO pin.
  //    In LAB, a pin/channel should only have one trigger condition.
  m_LAB.rpi ().gpio.clear_all_event_detect (gpio_pin);

  // 5. Set the trigger condition of the pin/channel.
  set_trigger_condition (gpio_pin, condition);
}

// EOF