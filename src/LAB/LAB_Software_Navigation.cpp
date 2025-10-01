#include "LAB_Software_Navigation.h"
#include "LAB.h"
#include <cstdio>
#ifndef SNM_DEBUG_SPI_DUMP
#define SNM_DEBUG_SPI_DUMP 1
#endif

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
set_tx_logan_config(unsigned samples, double sampling_rate)
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
  else                    samples_nibble = 0x0;

  uint8_t rate_nibble = 0;
  if (sampling_rate >= 100) rate_nibble = 0x7;
  else if (sampling_rate >= 50)  rate_nibble = 0x6;
  else if (sampling_rate >= 20)  rate_nibble = 0x5;
  else if (sampling_rate >= 10)  rate_nibble = 0x4;
  else if (sampling_rate >= 5)   rate_nibble = 0x3;
  else if (sampling_rate >= 2)   rate_nibble = 0x2;
  else if (sampling_rate >= 1)   rate_nibble = 0x1;
  else                           rate_nibble = 0x0;

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

  uint8_t trig_chan_nibble = 0x0; // 0 means no channel has a trigger
  uint8_t trig_mode_nibble = 0x0;
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

  // New header layout (no checksum):
  // [15:12] type nibble with MSB=1 and low 3 bits = trigger channel (0..4)
  // [11:8]  trigger mode nibble
  // [7:4]   samples nibble
  // [3:0]   rate nibble
  const uint8_t type_nibble = static_cast<uint8_t>(0x8 | (trig_chan_nibble & 0x07));
  const uint8_t b0 = static_cast<uint8_t>((type_nibble << 4) | (trig_mode_nibble & 0x0F));
  const uint8_t b1 = static_cast<uint8_t>(((samples_nibble & 0x0F) << 4) | (rate_nibble & 0x0F));

  bool expected = false;
  if (m_logan_start_sent.compare_exchange_strong(expected, true))
  {
    // One-shot immediate send for LOGAN START
    send_immediate_tx_blocking(b0, b1);

    // Allow STOP to be sent later, and avoid repeating START in service loop
    m_logan_stop_sent.store(false);
    std::lock_guard<std::mutex> lock(m_tx_mutex);
    m_tx_buffer[0] = 0x00;
    m_tx_buffer[1] = 0x00;
  }
}

void
LAB_Software_Navigation::
publish_completed_logan_block()
{
  // When expected samples have been written, mark trigger frame ready
  if (m_logan_expected_samples > 0 && m_logan_samples_written >= m_logan_expected_samples)
  {
    auto &pdata = lab().m_Logic_Analyzer.parent_data();
    pdata.samples = m_logan_expected_samples;
    pdata.trigger_frame_ready = true;
    // Reset counters to allow next block
    m_logan_expected_samples = 0;
    m_logan_samples_written  = 0;
    m_logan_words_remaining  = 0;
    m_logan_checksum_accum   = 0;
  }
}

void
LAB_Software_Navigation::
set_tx_logan_triggers()
{
  auto enc = [](LABE::LOGAN::TRIG::CND c) -> uint8_t {
    switch (c)
    {
      case LABE::LOGAN::TRIG::CND::IGNORE:          return 0x0;
      case LABE::LOGAN::TRIG::CND::LOW:             return 0x1;
      case LABE::LOGAN::TRIG::CND::HIGH:            return 0x2;
      case LABE::LOGAN::TRIG::CND::RISING_EDGE:     return 0x3;
      case LABE::LOGAN::TRIG::CND::FALLING_EDGE:    return 0x4;
      case LABE::LOGAN::TRIG::CND::EITHER_EDGE:     return 0x5;
      default:                                      return 0x0;
    }
  };

  const auto &pdata = lab().m_Logic_Analyzer.parent_data();

  // first nibble     = channel 1 and its trigger mode
  // second nibble    = channel 2 and its trigger mode
  // third nibble     = channel 3 and its trigger mode
  // fourth nibble    = channel 4 and its trigger mode

  uint8_t m1 = 0, m2 = 0, m3 = 0, m4 = 0;
  if (LABC::LOGAN::NUMBER_OF_CHANNELS > 0) m1 = enc(pdata.channel_data[0].trigger_condition);
  if (LABC::LOGAN::NUMBER_OF_CHANNELS > 1) m2 = enc(pdata.channel_data[1].trigger_condition);
  if (LABC::LOGAN::NUMBER_OF_CHANNELS > 2) m3 = enc(pdata.channel_data[2].trigger_condition);
  if (LABC::LOGAN::NUMBER_OF_CHANNELS > 3) m4 = enc(pdata.channel_data[3].trigger_condition);

  const uint16_t payload =
      (static_cast<uint16_t>(m4) << 12) |
      (static_cast<uint16_t>(m3) <<  8) |
      (static_cast<uint16_t>(m2) <<  4) |
      (static_cast<uint16_t>(m1) <<  0);

  std::lock_guard<std::mutex> lock(m_tx_mutex);
  m_tx_buffer[0] = static_cast<uint8_t>((payload >> 8) & 0xFF);
  m_tx_buffer[1] = static_cast<uint8_t>(payload & 0xFF);
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
  // New STOP: MSB=1 header with all fields zero
  constexpr uint8_t b0 = static_cast<uint8_t>((0x8 << 4) | 0x0);
  constexpr uint8_t b1 = static_cast<uint8_t>((0x0 << 4) | 0x0);

  bool expected = false;
  if (m_logan_stop_sent.compare_exchange_strong(expected, true))
  {
    send_immediate_tx_blocking(b0, b1);
    // Allow future START after a STOP
    m_logan_start_sent.store(false);
    std::lock_guard<std::mutex> lock(m_tx_mutex);
    m_tx_buffer[0] = 0x00;
    m_tx_buffer[1] = 0x00;
  }
}

void
LAB_Software_Navigation::
reset_logan_rx_state()
{
  m_logan_rx_state = LOGAN_RX_STATE::NONE;
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

bool
LAB_Software_Navigation::
is_logan_header(uint16_t word) noexcept
{
  // New format: treat any word whose top nibble has MSB=1 as a LOGAN header.
  // Top nibble encodes: 1xxx where low 3 bits carry trigger channel (0..4)
  const uint8_t type = (word >> 12) & 0x0F;
  return (type & 0x8) != 0;
}

void
LAB_Software_Navigation::
service_once()
{
  const unsigned transfer_size = m_parent_data.TRANSFER_SIZE;
  if (!m_read_enabled) return;

  uint8_t rx_buffer_static[LABC::PIN::SNM::TRANSFER_SIZE] = {0};
  uint8_t tx_buffer_static[LABC::PIN::SNM::TRANSFER_SIZE] = {0};

  // Prepare current TX bytes
  {
    std::lock_guard<std::mutex> lock(m_tx_mutex);
    tx_buffer_static[0] = m_tx_buffer[0];
    tx_buffer_static[1] = m_tx_buffer[1];
  }

  // Single transfer for fixed small transfer_size
  spi_transfer(rx_buffer_static, tx_buffer_static, transfer_size);
  if (!m_read_enabled) return;

#if SNM_DEBUG_SPI_DUMP
  if (rx_buffer_static[0] != 0x00 && rx_buffer_static[1] != 0x00)
  {
    std::printf("SPI RX (%u):", transfer_size);
    for (unsigned i = 0; i < transfer_size; ++i)
    {
      std::printf(" %02X", static_cast<unsigned>(rx_buffer_static[i]));
    }
    std::printf("\n");
  }
#endif

  // Parse incoming words with LOGAN state machine precedence
  const uint16_t word0 = (static_cast<uint16_t>(rx_buffer_static[0]) << 8) | rx_buffer_static[1];
  if (m_logan_rx_state == LOGAN_RX_STATE::EXPECT_PAYLOAD)
  {
    const bool is_logan_hdr = is_logan_header(word0);
    const uint8_t type_nibble = (word0 >> 12) & 0x0F;
    const bool is_software_nav_type = (type_nibble >= 0x1 && type_nibble <= 0x3);

    // While receiving payload, still allow SNM control/event words to be queued
    if (!is_logan_hdr && is_software_nav_type)
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
      // Treat word0 as part of LOGAN stream when LA is running.
      if (lab().m_Logic_Analyzer.is_running())
      {
        // If we still expect payload words, unpack and store into channel 1 bit
        if (m_logan_words_remaining > 0)
        {
#if SNM_DEBUG_SPI_DUMP
          std::printf("LOGAN PAYLOAD: 0x%04X\n", static_cast<unsigned>(word0));
#endif
          auto &pdata = lab().m_Logic_Analyzer.parent_data();
          for (unsigned i = 0; i < 4 && m_logan_samples_written < m_logan_expected_samples; ++i)
          {
            const uint8_t nibble = static_cast<uint8_t>((word0 >> (i * 4)) & 0x0F);
            const unsigned sample_index = m_logan_samples_written++;
            if (sample_index < pdata.raw_data_buffer.size())
            {
              const unsigned bit = (LABC::PIN::LOGAN[0]);
              // Build fresh 32-bit snapshot for this sample index
              pdata.raw_data_buffer[sample_index] = 0;
              pdata.raw_data_buffer[sample_index] &= ~(static_cast<uint32_t>(1u) << bit);
              pdata.raw_data_buffer[sample_index] |= (static_cast<uint32_t>(nibble & 0x1u) << bit);
            }
          }
          m_logan_checksum_accum ^= word0;
          if (m_logan_words_remaining > 0) --m_logan_words_remaining;

          if (m_logan_words_remaining == 0)
          {
            // Next word will be checksum; nothing else here
          }
        }
        else
        {
          // Expecting checksum word now; verify and finish block
#if SNM_DEBUG_SPI_DUMP
          std::printf("LOGAN CHECKSUM: 0x%04X\n", static_cast<unsigned>(word0));
#endif
          // Optional: verify checksum
          // if (m_logan_checksum_accum != word0) { /* handle mismatch */ }
          m_logan_rx_state = LOGAN_RX_STATE::NONE;
          publish_completed_logan_block();
        }
      }
      else
      {
        // Reset state if LA is not running to avoid stuck state
        m_logan_rx_state = LOGAN_RX_STATE::NONE;
      }
    }
  }
  else
  {
    // Treat only specific type ranges as Software Navigation control/events (avoid LOGAN payload confusion)
    const uint8_t type_nibble = (word0 >> 12) & 0x0F;
    const bool is_software_nav_type = (type_nibble >= 0x1 && type_nibble <= 0x3);

    if (m_read_enabled && !is_logan_header(word0) && is_software_nav_type)
    {
      const uint8_t type   = (word0 >> 12) & 0x0F;
      const uint8_t action = (word0 >> 8)  & 0x0F;
      const uint8_t value  = (word0 >> 4)  & 0x0F;

      // Queue SNM message for presenter
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
      // For visibility while debugging, print LOGAN header when detected.
#if SNM_DEBUG_SPI_DUMP
      std::printf("LOGAN HEADER: 0x%04X\n", static_cast<unsigned>(word0));
#endif
      m_logan_rx_state = LOGAN_RX_STATE::EXPECT_PAYLOAD;

      // Decode samples/rate nibbles to determine expected payload words (new format)
      const uint8_t samples_nibble = (word0 >> 4) & 0x0F;
      const uint8_t rate_nibble    = (word0 >> 0) & 0x0F;
      // Map to sample count as per slave mapping
      auto map_samples = [](uint8_t n) -> unsigned {
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
      };
      m_logan_expected_samples = map_samples(samples_nibble);
      m_logan_samples_written  = 0;
      m_logan_words_remaining  = (m_logan_expected_samples + 3) / 4;
      m_logan_checksum_accum   = 0;

      // If more bytes are available in this same transfer, consume them now
      if (transfer_size >= 4)
      {
        const uint16_t payload = (static_cast<uint16_t>(rx_buffer_static[2]) << 8) | rx_buffer_static[3];
#if SNM_DEBUG_SPI_DUMP
        std::printf("LOGAN PAYLOAD: 0x%04X\n", static_cast<unsigned>(payload));
#endif
        // Consume first payload word immediately
        if (m_logan_words_remaining > 0)
        {
          // Unpack up to 4 nibbles -> bits into raw_data_buffer
          auto &pdata = lab().m_Logic_Analyzer.parent_data();
          for (unsigned i = 0; i < 4 && m_logan_samples_written < m_logan_expected_samples; ++i)
          {
            const uint8_t nibble = static_cast<uint8_t>((payload >> (i * 4)) & 0x0F);
            // Store nibble LSB as 1-bit sample into raw_data_buffer as packed bits of channel 0
            // Here we store into bit position of first channel GPIO to match parse_raw_sample_buffer
            const unsigned sample_index = m_logan_samples_written++;
            if (sample_index < pdata.raw_data_buffer.size())
            {
              const unsigned bit = (LABC::PIN::LOGAN[0]);
              // Build fresh 32-bit snapshot for this sample index
              pdata.raw_data_buffer[sample_index] = 0;
              // Clear then set bit according to nibble LSB
              pdata.raw_data_buffer[sample_index] &= ~(static_cast<uint32_t>(1u) << bit);
              pdata.raw_data_buffer[sample_index] |= (static_cast<uint32_t>(nibble & 0x1u) << bit);
            }
          }
          m_logan_checksum_accum ^= payload;
          if (m_logan_words_remaining > 0) --m_logan_words_remaining;
        }
        // Keep state as EXPECT_PAYLOAD; checksum handling is done in the payload branch
      }
      if (transfer_size >= 6)
      {
        const uint16_t csum = (static_cast<uint16_t>(rx_buffer_static[4]) << 8) | rx_buffer_static[5];
#if SNM_DEBUG_SPI_DUMP
        std::printf("LOGAN CHECKSUM: 0x%04X\n", static_cast<unsigned>(csum));
#endif
        // Optionally verify checksum (slave uses XOR of payload words)
        // m_logan_checksum_accum should match csum
        m_logan_rx_state = LOGAN_RX_STATE::NONE;
        publish_completed_logan_block();
      }
    }
    // else: unknown/non-SNM 16-bit word; ignore
  }

  {
    std::lock_guard<std::mutex> lock(m_tx_mutex);
    // Clear one-shot header after a send to avoid repeating it in subsequent cycles
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
