#include "LAB_Software_Navigation.h"
#include "LAB.h"
#include <cstdio>

// Shared mapping from samples nibble to count
static inline unsigned map_samples_nibble_host(uint8_t n)
{
  switch (n & 0x0F)
  {
    case 0xA: return 2000;
    case 0x9: return 1000;
    case 0x8: return 500;
    case 0x7: return 200;
    case 0x6: return 100;
    case 0x5: return 50;
    case 0x4: return 20;
    case 0x3: return 10;
    case 0x2: return 5;
    case 0x1: return 2;
    default:  return 500;
  }
}

LAB_Software_Navigation::
LAB_Software_Navigation(LAB &lab)
  : LAB_Module(lab)
{
  init_spi();
}

LAB_Software_Navigation::
~LAB_Software_Navigation()
{
  announce_program_stopping();
  rpi().aux.spi(0).disable();
}

void
LAB_Software_Navigation::
init_spi()
{
  rpi().gpio.set(m_parent_data.SCLK_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi().gpio.set(m_parent_data.MISO_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi().gpio.set(m_parent_data.MOSI_PIN, AP::GPIO::FUNC::ALT4, AP::GPIO::PULL::OFF);
  rpi().gpio.set(m_parent_data.CS_PIN, AP::GPIO::FUNC::OUTPUT, AP::GPIO::PULL::UP);
  rpi().gpio.write(m_parent_data.CS_PIN, true);

  rpi().aux.master_enable_spi(0);
  rpi().aux.spi(0).clear_fifos();
  rpi().aux.spi(0).frequency(m_parent_data.BAUD_RATE);
  rpi().aux.spi(0).enable();
}

std::array<uint8_t, 3>
LAB_Software_Navigation::
update_spi_data()
{
  std::array<uint8_t, 3> result = {0, 0, 0};
  try_dequeue(result);
  return result;
}

void
LAB_Software_Navigation::
tick()
{
  service_once();
}

void
LAB_Software_Navigation::
set_tx_logan_config(unsigned samples, double sampling_rate, bool continuous)
{
  uint8_t samples_nibble = 0;
  if (samples >= 2000) samples_nibble = 0xA;
  else if (samples >= 1000) samples_nibble = 0x9;
  else if (samples >= 500) samples_nibble = 0x8;
  else if (samples >= 200) samples_nibble = 0x7;
  else if (samples >= 100) samples_nibble = 0x6;
  else if (samples >= 50) samples_nibble = 0x5;
  else if (samples >= 20) samples_nibble = 0x4;
  else if (samples >= 10) samples_nibble = 0x3;
  else if (samples >= 5)  samples_nibble = 0x2;
  else if (samples >= 2)  samples_nibble = 0x1;
  else                    samples_nibble = 0x8;

  uint8_t rate_nibble = 0;
  if (sampling_rate >= 1000) rate_nibble = 0xA;
  else if (sampling_rate >= 500)  rate_nibble = 0x9;
  else if (sampling_rate >= 200)  rate_nibble = 0x8;
  else if (sampling_rate >= 100)  rate_nibble = 0x7;
  else if (sampling_rate >= 50)  rate_nibble = 0x6;
  else if (sampling_rate >= 20)  rate_nibble = 0x5;
  else if (sampling_rate >= 10)  rate_nibble = 0x4;
  else if (sampling_rate >= 5)   rate_nibble = 0x3;
  else if (sampling_rate >= 2)   rate_nibble = 0x2;
  else if (sampling_rate >= 1)   rate_nibble = 0x1;
  else                           rate_nibble = 0x6;

  // Determine single active trigger channel (1..4) and trigger mode nibble
  auto enc_trig_cnd = [](LABE::LOGAN::TRIG::CND c) -> uint8_t {
    switch (c)
    {
      case LABE::LOGAN::TRIG::CND::IGNORE:       return 0x0;
      case LABE::LOGAN::TRIG::CND::LOW:          return 0x1;
      case LABE::LOGAN::TRIG::CND::HIGH:         return 0x2;
      case LABE::LOGAN::TRIG::CND::RISING_EDGE:  return 0x3;
      case LABE::LOGAN::TRIG::CND::FALLING_EDGE: return 0x4;
      case LABE::LOGAN::TRIG::CND::EITHER_EDGE:  return 0x5;
      default:                                   return 0x0;
    }
  };

  uint8_t trig_chan_nibble = 0x1; // Default to channel 1 for immediate mode
  uint8_t trig_mode_nibble = 0x0; // Default to immediate mode (no trigger)
  {
    const auto &pdata = lab().m_Logic_Analyzer.parent_data();
    for (unsigned i = 0; i < LABC::LOGAN::NUMBER_OF_CHANNELS; ++i)
    {
      const auto cnd = pdata.channel_data[i].trigger_condition;
      if (cnd != LABE::LOGAN::TRIG::CND::IGNORE)
      {
        trig_chan_nibble = static_cast<uint8_t>((i + 1) & 0x07); // 1..4
        trig_mode_nibble = enc_trig_cnd(cnd) & 0x0F;
        break; // only one channel can have a trigger
      }
    }
  }

  // [15:12] type nibble: MSB indicates run mode. 1=continuous (cb_run_stop), 0=single (cb_single).
  //           low 3 bits = trigger channel (0..4)
  // [11:8]  trigger mode nibble
  // [7:4]   samples nibble
  // [3:0]   rate nibble
  const uint8_t type_nibble = static_cast<uint8_t>(((continuous ? 0x8 : 0x0) | (trig_chan_nibble & 0x07)) & 0x0F);
  const uint8_t b0 = static_cast<uint8_t>((type_nibble << 4) | (trig_mode_nibble & 0x0F));
  const uint8_t b1 = static_cast<uint8_t>(((samples_nibble & 0x0F) << 4) | (rate_nibble & 0x0F));

  send_immediate_tx_blocking(b0, b1);

  m_logan_stop_sent.store(false);
  std::lock_guard<std::mutex> lock(m_tx_mutex);
  m_tx_buffer[0] = 0x00;
  m_tx_buffer[1] = 0x00;
}

void
LAB_Software_Navigation::
publish_completed_logan_block()
{
  if (m_logan_frame_expected_samples > 0)
  {
    auto &pdata = lab().m_Logic_Analyzer.parent_data();
    // Do not overwrite frontend-selected samples; only mark frame ready
    pdata.trigger_frame_ready = true;

    m_logan_expected_samples = 0;
    m_logan_samples_written  = 0;
    m_logan_words_remaining  = 0;
    m_logan_checksum_accum   = 0;
    m_logan_frame_expected_samples = 0;
  }
}

void
LAB_Software_Navigation::
set_tx_header(uint8_t type, uint8_t action, uint8_t value)
{
  const uint8_t checksum = (type ^ action ^ value) & 0x0F;
  const uint8_t b0 = static_cast<uint8_t>((type << 4) | action);
  const uint8_t b1 = static_cast<uint8_t>((value << 4) | checksum);
  std::lock_guard<std::mutex> lock(m_tx_mutex);
  m_tx_buffer[0] = b0;
  m_tx_buffer[1] = b1;
}

void
LAB_Software_Navigation::
send_immediate_tx_blocking(uint8_t b0, uint8_t b1)
{
  uint8_t rx[2] = {0};
  const uint8_t tx[2] = {b0, b1};
  spi_transfer(rx, tx, 2);
}

void
LAB_Software_Navigation::
set_snm_attached(bool attached)
{
  m_snm_attached = attached;
  if (attached)
  {
    announce_snm_attached();
  }
}

void
LAB_Software_Navigation::
announce_snm_attached()
{
  // type=0xA, action=0x1 means "ANNOUNCE", value=0x1 means "SNM attached"
  constexpr uint8_t type = 0xA;
  constexpr uint8_t action = 0x1;
  constexpr uint8_t value = 0x1;
  const uint8_t checksum = (type ^ action ^ value) & 0x0F;
  const uint8_t b0 = static_cast<uint8_t>((type << 4) | action);
  const uint8_t b1 = static_cast<uint8_t>((value << 4) | checksum);

  {
    std::lock_guard<std::mutex> lock(m_tx_mutex);
    m_tx_buffer[0] = b0;
    m_tx_buffer[1] = b1;
  }

  send_immediate_tx_blocking(b0, b1);
}

void
LAB_Software_Navigation::
announce_program_stopping()
{
  // type=0xA, action=0x1 means "ANNOUNCE", value=0x0 means "program stopping"
  constexpr uint8_t type = 0xA;
  constexpr uint8_t action = 0x1;
  constexpr uint8_t value = 0x0;
  const uint8_t checksum = (type ^ action ^ value) & 0x0F;
  const uint8_t b0 = static_cast<uint8_t>((type << 4) | action);
  const uint8_t b1 = static_cast<uint8_t>((value << 4) | checksum);

  bool expected = false;
  if (m_stop_sent.compare_exchange_strong(expected, true))
  {
    m_read_enabled = false;

    {
      std::lock_guard<std::mutex> lock(m_tx_mutex);
      m_tx_buffer[0] = b0;
      m_tx_buffer[1] = b1;
    }

    send_immediate_tx_blocking(b0, b1);
  }
}

void
LAB_Software_Navigation::
set_tx_logan_stop()
{
  constexpr uint8_t b0 = static_cast<uint8_t>((0x8 << 4) | 0x0);
  constexpr uint8_t b1 = static_cast<uint8_t>((0x0 << 4) | 0x0);

  bool expected = false;
  if (m_logan_stop_sent.compare_exchange_strong(expected, true))
  {
    send_immediate_tx_blocking(b0, b1);

    m_logan_start_sent.store(false);
    std::lock_guard<std::mutex> lock(m_tx_mutex);
    m_tx_buffer[0] = 0x00;
    m_tx_buffer[1] = 0x00;
  }

  m_logan_rx_state = LOGAN_RX_STATE::NONE;
  m_logan_expected_samples = 0;
  m_logan_samples_written  = 0;
  m_logan_words_remaining  = 0;
  m_logan_checksum_accum   = 0;
  m_logan_frame_expected_samples = 0;
  m_logan_current_channel = 0;
  m_logan_channels_received_mask = 0;
}

bool
LAB_Software_Navigation::
is_snm_config_enabled() const
{
  return m_parent_data.SNM_ATTACHED;
}

void
LAB_Software_Navigation::
spi_transfer(uint8_t* rx, const uint8_t* tx, unsigned n)
{
  if (n == 0) return;
  std::lock_guard<std::mutex> spi_lock(m_spi_mutex);
  rpi().aux.spi(0).clear_fifos();
  rpi().gpio.write(m_parent_data.CS_PIN, false);
  rpi().aux.spi(0).xfer(
    reinterpret_cast<char *>(rx),
    reinterpret_cast<char *>(const_cast<uint8_t*>(tx)),
    n
  );
  rpi().gpio.write(m_parent_data.CS_PIN, true);
}

bool
LAB_Software_Navigation::
validate_spi_data(uint16_t spi_data) noexcept
{
  const uint8_t type     = (spi_data >> 12) & 0x0F;
  const uint8_t action   = (spi_data >> 8)  & 0x0F;
  const uint8_t value    = (spi_data >> 4)  & 0x0F;
  const uint8_t checksum =  spi_data        & 0x0F;

  return checksum == ((type ^ action ^ value) & 0x0F);
}

static inline bool is_snm_event(uint16_t word) noexcept
{
  const uint8_t type = static_cast<uint8_t>((word >> 12) & 0x0F);
  if ((type & 0x8) != 0) return false;

  const uint8_t action   = static_cast<uint8_t>((word >> 8)  & 0x0F);
  const uint8_t value    = static_cast<uint8_t>((word >> 4)  & 0x0F);
  const uint8_t checksum = static_cast<uint8_t>( word        & 0x0F);
  const bool checksum_ok = (checksum == ((type ^ action ^ value) & 0x0F));
  return checksum_ok && ((type >= 0x1 && type <= 0x3) || type == 0xA);
}

bool
LAB_Software_Navigation::
is_logan_header(uint16_t word) noexcept
{
  const uint8_t type = static_cast<uint8_t>((word >> 12) & 0x0F);

  if ((type & 0x8) != 0) return true;

  if (!is_snm_event(word))
  {
    const uint8_t chan = static_cast<uint8_t>(type & 0x07);
    if (chan >= 1 && chan <= 4) return true;
  }
  return false;
}

void
LAB_Software_Navigation::
service_once()
{
  const unsigned transfer_size = m_parent_data.TRANSFER_SIZE;
  if (!m_read_enabled) return;

  uint8_t rx_buffer_static[LABC::PIN::SNM::TRANSFER_SIZE] = {0};
  uint8_t tx_buffer_static[LABC::PIN::SNM::TRANSFER_SIZE] = {0};

  // Track whether the current LOGAN frame was initiated as single-shot (MSB=0)
  static bool s_logan_single_frame = false;

  // Helper: consume one payload word with correct nibble ordering and first-word remainder handling
  auto consume_payload_word = [&](uint16_t word)
  {
    auto &pdata = lab().m_Logic_Analyzer.parent_data();
    unsigned end_i = 3u;
    if (m_logan_is_first_payload_word)
    {
      const unsigned rem = (m_logan_expected_samples % 4);
      if (rem != 0) end_i = rem - 1u;
    }
    for (unsigned i = 0; i <= end_i && m_logan_samples_written < m_logan_expected_samples; ++i)
    {
      const uint8_t nibble = static_cast<uint8_t>((word >> ((3 - i) * 4)) & 0x0F);
      const unsigned sample_index = m_logan_samples_written++;
      if (sample_index < pdata.raw_data_buffer.size())
      {
        unsigned chan_index = (m_logan_current_channel > 0 && m_logan_current_channel <= 4)
                                ? (m_logan_current_channel - 1)
                                : 0u;
        const unsigned bit = (LABC::PIN::LOGAN[chan_index]);
        pdata.raw_data_buffer[sample_index] &= ~(static_cast<uint32_t>(1u) << bit);
        pdata.raw_data_buffer[sample_index] |= (static_cast<uint32_t>(nibble & 0x1u) << bit);
      }
    }
    m_logan_checksum_accum ^= word;
    if (m_logan_words_remaining > 0) --m_logan_words_remaining;
    if (m_logan_is_first_payload_word)
    {
      m_logan_is_first_payload_word = false;
      m_logan_skip_nibbles_first_word = 0;
    }
  };

  {
    std::lock_guard<std::mutex> lock(m_tx_mutex);
    tx_buffer_static[0] = m_tx_buffer[0];
    tx_buffer_static[1] = m_tx_buffer[1];
  }

  spi_transfer(rx_buffer_static, tx_buffer_static, transfer_size);
  if (!m_read_enabled) return;

  const uint16_t word0 = (static_cast<uint16_t>(rx_buffer_static[0]) << 8) | rx_buffer_static[1];
  if (m_logan_rx_state == LOGAN_RX_STATE::EXPECT_PAYLOAD)
  {
    const bool is_logan_hdr = is_logan_header(word0);
    const uint8_t type_nibble = static_cast<uint8_t>((word0 >> 12) & 0x0F);

    // Resynchronize if a new LOGAN header arrives mid-payload
    if (is_logan_hdr)
    {
      m_logan_rx_state = LOGAN_RX_STATE::EXPECT_PAYLOAD;

      const uint8_t type_nibble_hdr = static_cast<uint8_t>((word0 >> 12) & 0x0F);
      // MSB==0 indicates single-shot header from master
      s_logan_single_frame = ((type_nibble_hdr & 0x8) == 0);
      const uint8_t samples_nibble = static_cast<uint8_t>((word0 >> 4) & 0x0F);
      const uint8_t chan_num = static_cast<uint8_t>(type_nibble_hdr & 0x07);
      if (chan_num >= 1 && chan_num <= 4)
      {
        m_logan_current_channel = chan_num;
      }
      else
      {
        m_logan_current_channel = 1;
      }
      const unsigned samples_for_channel = map_samples_nibble_host(samples_nibble);

      if (m_logan_channels_received_mask == 0)
      {
        m_logan_frame_expected_samples = samples_for_channel;

        auto &pdata = lab().m_Logic_Analyzer.parent_data();
        const size_t to_clear = std::min(static_cast<size_t>(m_logan_frame_expected_samples), pdata.raw_data_buffer.size());
        for (size_t i = 0; i < to_clear; ++i) pdata.raw_data_buffer[i] = 0u;

        // For single-shot triggered captures, set trigger index to center (zero-second reference)
        if (s_logan_single_frame)
        {
          pdata.trigger_index = samples_for_channel / 2;
        }
      }
      m_logan_expected_samples = samples_for_channel;
      m_logan_samples_written  = 0;
      m_logan_words_remaining  = (m_logan_expected_samples + 3) / 4;
      m_logan_checksum_accum   = 0;
      // Prepare partial-first-word handling
      {
        const unsigned remainder = (m_logan_expected_samples % 4);
        if (remainder == 0)
        {
          m_logan_skip_nibbles_first_word = 0;
          m_logan_is_first_payload_word = false;
        }
        else
        {
          m_logan_skip_nibbles_first_word = 4 - remainder; // skip low nibbles
          m_logan_is_first_payload_word = true;
        }
      }
      // Consume the first payload word that arrived in the same transaction, if present
      if (transfer_size >= 4 && m_logan_words_remaining > 0)
      {
        const uint16_t payload_inline = static_cast<uint16_t>((static_cast<uint16_t>(rx_buffer_static[2]) << 8) | rx_buffer_static[3]);
        consume_payload_word(payload_inline);
      }
    }
    else if (!is_logan_hdr && is_snm_event(word0))
    {
      const uint8_t type   = (word0 >> 12) & 0x0F;
      const uint8_t action = (word0 >> 8)  & 0x0F;
      const uint8_t value  = (word0 >> 4)  & 0x0F;
      {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        if (m_queue.size() < MAX_QUEUE)
        {
          m_queue.push({type, action, value});
        }
      }
    }
    else
    {
      if (m_logan_words_remaining > 0)
      {
        consume_payload_word(word0);

        // Opportunistically drain a few more words this tick to avoid backlog/flicker
        if (m_logan_words_remaining > 0)
        {
          const unsigned max_extra_reads = 128; // larger burst to drain payload faster
          for (unsigned r = 0; r < max_extra_reads && m_logan_words_remaining > 0; ++r)
          {
            uint8_t rx_extra[LABC::PIN::SNM::TRANSFER_SIZE] = {0};
            uint8_t tx_extra[LABC::PIN::SNM::TRANSFER_SIZE] = {0};
            spi_transfer(rx_extra, tx_extra, LABC::PIN::SNM::TRANSFER_SIZE);
            const uint16_t w = (static_cast<uint16_t>(rx_extra[0]) << 8) | rx_extra[1];

            const bool hdr = is_logan_header(w);
            if (hdr)
            {
              // Resync to next channel as above
              const uint8_t type_nib_hdr = static_cast<uint8_t>((w >> 12) & 0x0F);
              const uint8_t samples_nib  = static_cast<uint8_t>((w >> 4) & 0x0F);
              const uint8_t chnum        = static_cast<uint8_t>(type_nib_hdr & 0x07);
              m_logan_current_channel = (chnum >= 1 && chnum <= 4) ? chnum : 1;
              const unsigned samp_ch = map_samples_nibble_host(samples_nib);
              if (m_logan_channels_received_mask == 0)
              {
                m_logan_frame_expected_samples = samp_ch;
                auto &pd2 = lab().m_Logic_Analyzer.parent_data();
                const size_t to_clear2 = std::min(static_cast<size_t>(m_logan_frame_expected_samples), pd2.raw_data_buffer.size());
                for (size_t i = 0; i < to_clear2; ++i) pd2.raw_data_buffer[i] = 0u;

                // For single-shot triggered captures, set trigger index to center (zero-second reference)
                if (s_logan_single_frame)
                {
                  pd2.trigger_index = samp_ch / 2;
                }
              }
              m_logan_expected_samples = samp_ch;
              m_logan_samples_written  = 0;
              m_logan_words_remaining  = (m_logan_expected_samples + 3) / 4;
              m_logan_checksum_accum   = 0;
              // Prepare partial-first-word handling for the new channel
              {
                const unsigned rem = (m_logan_expected_samples % 4);
                if (rem == 0)
                {
                  m_logan_skip_nibbles_first_word = 0;
                  m_logan_is_first_payload_word = false;
                }
                else
                {
                  m_logan_skip_nibbles_first_word = 4 - rem;
                  m_logan_is_first_payload_word = true;
                }
              }
              // Consume the first payload word from this same transaction, if present
              if (LABC::PIN::SNM::TRANSFER_SIZE >= 4 && m_logan_words_remaining > 0)
              {
                const uint16_t payload_inline2 = static_cast<uint16_t>((static_cast<uint16_t>(rx_extra[2]) << 8) | rx_extra[3]);
                consume_payload_word(payload_inline2);
              }
              continue;
            }

            if (!hdr && is_snm_event(w))
            {
              const uint8_t t = (w >> 12) & 0x0F;
              const uint8_t a = (w >> 8)  & 0x0F;
              const uint8_t v = (w >> 4)  & 0x0F;
              {
                std::lock_guard<std::mutex> ql(m_queue_mutex);
                if (m_queue.size() < MAX_QUEUE) m_queue.push({t, a, v});
              }
              continue;
            }

            // payload word
            consume_payload_word(w);
          }
        }
      }
      else
      {
        if (m_logan_current_channel >= 1 && m_logan_current_channel <= 4)
        {
          m_logan_channels_received_mask |= static_cast<uint8_t>(1u << (m_logan_current_channel - 1));
        }

        if ((m_logan_channels_received_mask & 0x0F) == 0x0F)
        {
          m_logan_rx_state = LOGAN_RX_STATE::NONE;
          if (s_logan_single_frame)
          {
            s_logan_single_frame = false;
          }

          publish_completed_logan_block();
          m_logan_channels_received_mask = 0;
          m_logan_current_channel = 0;
          m_logan_frame_expected_samples = 0;
        }
        else
        {
          m_logan_rx_state = LOGAN_RX_STATE::NONE;
        }
      }
    }
  }
  else
  {
    if (m_read_enabled && is_snm_event(word0))
    {
      const uint8_t type   = (word0 >> 12) & 0x0F;
      const uint8_t action = (word0 >> 8)  & 0x0F;
      const uint8_t value  = (word0 >> 4)  & 0x0F;

      {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        if (m_queue.size() < MAX_QUEUE)
        {
          m_queue.push({type, action, value});
        }
      }
    }
    else if (is_logan_header(word0))
    {
      m_logan_rx_state = LOGAN_RX_STATE::EXPECT_PAYLOAD;

      const uint8_t type_nibble_hdr = static_cast<uint8_t>((word0 >> 12) & 0x0F);
      // MSB==0 indicates single-shot header from master
      s_logan_single_frame = ((type_nibble_hdr & 0x8) == 0);
      // Parse header fields; do not push samples/rate to frontend
      const uint8_t trig_mode_nibble = static_cast<uint8_t>((word0 >> 8) & 0x0F);
      const uint8_t samples_nibble = static_cast<uint8_t>((word0 >> 4) & 0x0F);
      const uint8_t rate_nibble    = static_cast<uint8_t>((word0 >> 0) & 0x0F);
      (void)trig_mode_nibble; (void)rate_nibble;

      const uint8_t chan_num = static_cast<uint8_t>(type_nibble_hdr & 0x07);
      if (chan_num >= 1 && chan_num <= 4)
      {
        m_logan_current_channel = chan_num;
      }
      else
      {
        m_logan_current_channel = 1;
      }
      const unsigned samples_for_channel = map_samples_nibble_host(samples_nibble);

      if (m_logan_channels_received_mask == 0)
      {
        m_logan_frame_expected_samples = samples_for_channel;

        auto &pdata = lab().m_Logic_Analyzer.parent_data();
        const size_t to_clear = std::min(static_cast<size_t>(m_logan_frame_expected_samples), pdata.raw_data_buffer.size());
        for (size_t i = 0; i < to_clear; ++i) pdata.raw_data_buffer[i] = 0u;

        // For single-shot triggered captures, set trigger index to center (zero-second reference)
        if (s_logan_single_frame)
        {
          pdata.trigger_index = samples_for_channel / 2;
        }
      }
      m_logan_expected_samples = samples_for_channel;
      m_logan_samples_written  = 0;
      m_logan_words_remaining  = (m_logan_expected_samples + 3) / 4;
      m_logan_checksum_accum   = 0;
      // Partial-first-word setup
      {
        const unsigned remainder = (m_logan_expected_samples % 4);
        if (remainder == 0)
        {
          m_logan_skip_nibbles_first_word = 0;
          m_logan_is_first_payload_word = false;
        }
        else
        {
          m_logan_skip_nibbles_first_word = 4 - remainder;
          m_logan_is_first_payload_word = true;
        }
      }

      if (transfer_size >= 4)
      {
        const uint16_t payload = (static_cast<uint16_t>(rx_buffer_static[2]) << 8) | rx_buffer_static[3];

        if (m_logan_words_remaining > 0)
        {
          consume_payload_word(payload);
        }
      }
      if (transfer_size >= 6)
      {
        const uint16_t csum = (static_cast<uint16_t>(rx_buffer_static[4]) << 8) | rx_buffer_static[5];

        if (m_logan_current_channel >= 1 && m_logan_current_channel <= 4)
        {
          m_logan_channels_received_mask |= static_cast<uint8_t>(1u << (m_logan_current_channel - 1));
        }
        if ((m_logan_channels_received_mask & 0x0F) == 0x0F)
        {
          m_logan_rx_state = LOGAN_RX_STATE::NONE;
          if (s_logan_single_frame)
          {
            s_logan_single_frame = false;
          }

          publish_completed_logan_block();
          m_logan_channels_received_mask = 0;
          m_logan_current_channel = 0;
          m_logan_frame_expected_samples = 0;
        }
        else
        {
          m_logan_rx_state = LOGAN_RX_STATE::NONE;
        }
      }
    }
  }

  {
    std::lock_guard<std::mutex> lock(m_tx_mutex);

    if (tx_buffer_static[0] != 0x00 || tx_buffer_static[1] != 0x00)
    {
      m_tx_buffer[0] = 0x00;
      m_tx_buffer[1] = 0x00;
    }
  }
}

bool
LAB_Software_Navigation::
try_dequeue(std::array<uint8_t, 3>& out)
{
  std::lock_guard<std::mutex> lock(m_queue_mutex);
  if (!m_queue.empty())
  {
    out = m_queue.front();
    m_queue.pop();
    return true;
  }
  return false;
}

// EOF
