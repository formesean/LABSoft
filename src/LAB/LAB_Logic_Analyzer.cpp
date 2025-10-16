#include "LAB_Logic_Analyzer.h"

#include "LAB.h"
#include "../Utility/LAB_Utility_Functions.h"

LAB_Logic_Analyzer::
LAB_Logic_Analyzer (LAB& _LAB)
  : LAB_Module (_LAB)
{
  // Sampling is handled on the slave device; no local DMA/GPIO init on master
}

LAB_Logic_Analyzer::
~LAB_Logic_Analyzer ()
{

}

// init_gpio_pins no longer needed on master; slave performs sampling

// init_dma removed; no DMA on master for LOGAN

// init_interrupts removed; hardware trigger detection not used on master

// config_dma_cb removed

void LAB_Logic_Analyzer::
run ()
{
  // m_LAB.rpi ().pwm.start (LABC::PWM::DMA_PACING_CHAN);

  // ==========

  m_parent_data.is_backend_running  = true;
  m_parent_data.is_frontend_running = true;

  m_parent_data.status = LABE::LOGAN::STATUS::AUTO;
}

void LAB_Logic_Analyzer::
stop ()
{
  // m_LAB.rpi ().pwm.stop (LABC::PWM::DMA_PACING_CHAN);

  // ==========

  m_parent_data.is_backend_running  = false;
  m_parent_data.is_frontend_running = false;

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

// fill_raw_sample_buffer_from_dma_buffer removed; data is streamed from slave

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

// get_samples_loop removed; sampling occurs on the slave

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
        break;
      }

      case (LABE::LOGAN::MODE::SCREEN):
      {
        break;
      }

      case (LABE::LOGAN::MODE::RECORD):
      {
        stop ();
        break;
      }
    }

    m_parent_data.mode = mode;
  }
}

// dma_buffer_count removed; no DMA on master

void LAB_Logic_Analyzer::
set_samples (unsigned value)
{
  m_parent_data.samples = value;

  // DMA lengths no longer updated on master; sampling handled on slave
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
  m_parent_data.sampling_rate   = value;
  m_parent_data.sampling_period = 1.0 / value;
}

// set_trigger_condition not needed on master; triggers handled on slave

void LAB_Logic_Analyzer::
parse_trigger_mode ()
{
  // No-op: slave device handles trigger detection and frames
}

/**
 * This is the main find trigger loop.
 */
// find_trigger_point_loop removed; trigger detection is slave-side

/**
 * This function checks whether the triggered pin
 * results to an actual trigger event.
 */
// check_if_triggered removed; slave evaluates triggers

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

  if (condition == prev_cnd)
  {
    return;
  }

  // 1) Remove channel from both caches to avoid duplicates
  {
    std::vector<unsigned>::iterator it = std::find (
      m_parent_data.trigger_cache_level.begin (),
      m_parent_data.trigger_cache_level.end (),
      channel
    );
    if (it != m_parent_data.trigger_cache_level.end ())
    {
      m_parent_data.trigger_cache_level.erase (it);
    }
  }
  {
    std::vector<unsigned>::iterator it = std::find (
      m_parent_data.trigger_cache_edge.begin (),
      m_parent_data.trigger_cache_edge.end (),
      channel
    );
    if (it != m_parent_data.trigger_cache_edge.end ())
    {
      m_parent_data.trigger_cache_edge.erase (it);
    }
  }

  // 2) Add channel to the appropriate cache for the NEW condition
  switch (condition)
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
      m_parent_data.trigger_cache_edge.emplace_back (channel);
      break;
    }

    case (LABE::LOGAN::TRIG::CND::IGNORE):
    {
      break;
    }
  }
}

// reset_dma_process removed; no DMA on master

void LAB_Logic_Analyzer::
update_data_samples ()
{
  // Always parse a completed frame, even if not running (single-shot)
  if (m_parent_data.trigger_frame_ready)
  {
    parse_raw_sample_buffer ();
    m_parent_data.trigger_frame_ready = false;

    if (m_parent_data.single)
    {
      m_parent_data.single = false;
      stop ();
    }
    return;
  }

  // Below here is only for continuous mode
  if (!is_running())
  {
    return;
  }

  // Data arrives from the slave; when ready, parse in the block above
  if (m_parent_data.single)
  {
    m_parent_data.single = false;
    stop ();
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
  // Guard against out-of-range values
  if (!LABF::is_within_range(
        static_cast<double>(value),
        static_cast<double>(LABC::LOGAN::MIN_SAMPLES),
        static_cast<double>(LABC::LOGAN::MAX_SAMPLES),
        LABC::LABSOFT::EPSILON))
  {
    return;
  }

  // Update cached sample count and DMA transfer lengths
  set_samples (value);

  // Keep horizontal timing consistent with the new sample count
  set_time_per_division (
    calc_time_per_division (value, m_parent_data.sampling_rate)
  );
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
    const unsigned min_samples = LABC::LOGAN::MIN_SAMPLES;
    const unsigned max_samples = LABC::LOGAN::MAX_SAMPLES;
    const double   target_window_seconds = 5.0;

    unsigned recommended_samples = static_cast<unsigned>(value * target_window_seconds);

    if (recommended_samples < min_samples)
    {
      recommended_samples = min_samples;
    }
    else if (recommended_samples > max_samples)
    {
      recommended_samples = max_samples;
    }

    if (recommended_samples != m_parent_data.samples)
    {set_samples (recommended_samples);
    }

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
  const bool trigger_thread_active = false; // no trigger thread on master

  // Enforce single active trigger channel: if setting a non-IGNORE condition
  // on this channel, clear all other channels to IGNORE first.
  if (condition != LABE::LOGAN::TRIG::CND::IGNORE)
  {
    for (unsigned c = 0; c < m_parent_data.channel_data.size (); c++)
    {
      if (c == channel)
      {
        continue;
      }

      if (m_parent_data.channel_data[c].trigger_condition != LABE::LOGAN::TRIG::CND::IGNORE)
      {
        // Update caches and stored state
        cache_trigger_condition (c, LABE::LOGAN::TRIG::CND::IGNORE);
        m_parent_data.channel_data[c].trigger_condition = LABE::LOGAN::TRIG::CND::IGNORE;

        // GPIO event detects are not managed on master
      }
    }
  }

  // 1. Update caches for this channel
  cache_trigger_condition (channel, condition);

  // 2. Store the channel's new trigger condition
  m_parent_data.channel_data[channel].trigger_condition = condition;

  // 3. Reapply GPIO event-detect configuration and (re)start thread if needed
  if (trigger_thread_active)
  {
    parse_trigger_mode (); // will re-init interrupts and restart thread
  }
}

// EOF
