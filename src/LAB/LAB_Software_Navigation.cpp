#include "LAB_Software_Navigation.h"
#include "LAB.h"
#include <cstdio>
#include <chrono>
#include <vector>

#ifndef SNM_DEBUG_SPI_DUMP
#define SNM_DEBUG_SPI_DUMP 1
#endif

LAB_Software_Navigation::
LAB_Software_Navigation(LAB &lab)
  : LAB_Module(lab)
{
  init_spi();

  // Start worker thread
  m_running = true;
  m_worker = std::thread(&LAB_Software_Navigation::worker_loop, this);
}

LAB_Software_Navigation::
~LAB_Software_Navigation()
{
  // Stop worker thread
  m_running = false;
  m_queue_cv.notify_one();
  if (m_worker.joinable())
    m_worker.join();

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
  // This is now called from the GUI thread to get queued data
  std::array<uint8_t, 3> result = {0, 0, 0};
  try_dequeue(result);
  return result;
}

void
LAB_Software_Navigation::
set_tx_message(uint8_t type, uint8_t action, uint8_t value)
{
  type   &= 0x0F;
  action &= 0x0F;
  value  &= 0x0F;

  const uint8_t checksum = (type ^ action ^ value) & 0x0F;

  std::lock_guard<std::mutex> lock(m_tx_mutex);
  m_tx_buffer[0] = static_cast<uint8_t>((type << 4) | action);
  m_tx_buffer[1] = static_cast<uint8_t>((value << 4) | checksum);
}

void
LAB_Software_Navigation::
set_tx_logan_config(unsigned samples, double sampling_rate)
{
  uint8_t samples_nibble = 0;
  if (samples == 2000) samples_nibble = 0xA;
  else if (samples == 1000) samples_nibble = 0x9;
  else if (samples == 500) samples_nibble = 0x8;
  else if (samples == 200) samples_nibble = 0x7;
  else if (samples == 100) samples_nibble = 0x6;
  else if (samples == 50) samples_nibble = 0x5;
  else if (samples == 20) samples_nibble = 0x4;
  else if (samples == 10) samples_nibble = 0x3;
  else if (samples == 5)  samples_nibble = 0x2;
  else if (samples == 2)  samples_nibble = 0x1;

  uint8_t rate_nibble = 0;
  if (sampling_rate >= 100) rate_nibble = 0x7;
  else if (sampling_rate >= 50)  rate_nibble = 0x6;
  else if (sampling_rate >= 20)  rate_nibble = 0x5;
  else if (sampling_rate >= 10)  rate_nibble = 0x4;
  else if (sampling_rate >= 5)   rate_nibble = 0x3;
  else if (sampling_rate >= 2)   rate_nibble = 0x2;
  else if (sampling_rate >= 1)   rate_nibble = 0x1;

  set_tx_message(0x6, samples_nibble, rate_nibble);
}

void
LAB_Software_Navigation::
spi_transfer(uint8_t* rx, const uint8_t* tx, unsigned n)
{
  if (n == 0) return;
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

void
LAB_Software_Navigation::
set_worker_enabled(bool enable)
{
  m_read_enabled = enable;
  if (enable)
  {
    m_queue_cv.notify_one();
  }
  else
  {
    // Flush any queued messages so GUI stops receiving immediately
    std::lock_guard<std::mutex> ql(m_queue_mutex);
    while (!m_queue.empty()) m_queue.pop();
  }
}

void
LAB_Software_Navigation::
worker_loop()
{
  const unsigned transfer_size = m_parent_data.TRANSFER_SIZE;
  uint8_t rx_buffer_static[LABC::PIN::SNM::TRANSFER_SIZE] = {0};
  uint8_t tx_buffer_static[LABC::PIN::SNM::TRANSFER_SIZE] = {0};

  while (m_running)
  {
    // If reads are disabled, idle briefly and skip any SPI interaction
    if (!m_read_enabled)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      continue;
    }

    // Check if there's data to send
    {
      std::lock_guard<std::mutex> lock(m_tx_mutex);
      tx_buffer_static[0] = m_tx_buffer[0];
      tx_buffer_static[1] = m_tx_buffer[1];
    }

    // Clear RX buffer
    rx_buffer_static[0] = 0;
    if (transfer_size > 1) rx_buffer_static[1] = 0;

    // Single transfer for fixed small transfer_size
    spi_transfer(rx_buffer_static, tx_buffer_static, transfer_size);

    // If we aborted mid-burst, skip processing
    if (!m_read_enabled)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }

    // Optional debug dump of RX data (always tied to SPI, not LA state)
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

    // Parse and validate header (first 2 bytes)
    const uint16_t header = (static_cast<uint16_t>(rx_buffer_static[0]) << 8) | rx_buffer_static[1];
    if (header != 0x0000 && header != 0xFFFF && validate_spi_data(header) && m_read_enabled)
    {
      const uint8_t type   = (header >> 12) & 0x0F;
      const uint8_t action = (header >> 8)  & 0x0F;
      const uint8_t value  = (header >> 4)  & 0x0F;

      // Queue the parsed data
      {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        if (m_queue.size() < MAX_QUEUE)
        {
          m_queue.push({type, action, value});
        }
      }
      if (!m_queue.empty()) m_queue_cv.notify_one();
    }

    // Clear TX buffer only when LA is not running, so stream command stays active
    {
      std::lock_guard<std::mutex> lock(m_tx_mutex);
      if (!lab().m_Logic_Analyzer.is_running())
      {
        m_tx_buffer[0] = 0x00;
        m_tx_buffer[1] = 0x00;
      }
    }

    // Small delay between reads (adjust as needed)
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
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
