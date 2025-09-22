#ifndef LAB_SOFTWARE_NAVIGATION_H
#define LAB_SOFTWARE_NAVIGATION_H

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"

#include <cstdint>
#include <array>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>

class LAB_Software_Navigation : public LAB_Module
{
  private:
    LAB_Parent_Data_Software_Navigation m_parent_data;

    std::array<uint8_t, LABC::PIN::SNM::TRANSFER_SIZE> m_rx_buffer = {0};
    std::array<uint8_t, LABC::PIN::SNM::TRANSFER_SIZE> m_tx_buffer = {0};

    // Worker thread infrastructure
    std::thread                         m_worker;
    std::atomic<bool>                   m_running {false};
    std::mutex                          m_tx_mutex;
    std::queue<std::array<uint8_t, 3> >  m_queue;
    std::mutex                          m_queue_mutex;
    std::condition_variable             m_queue_cv;

  public:
    LAB_Software_Navigation                (LAB &lab);
    ~LAB_Software_Navigation               ();

    std::array<uint8_t, 3> update_spi_data ();
    bool try_dequeue                       (std::array<uint8_t, 3>& out);

    void set_worker_enabled                (bool enable);

    void set_tx_logan_message              (uint8_t type, uint8_t samples, uint8_t sampling_rate);
    void set_tx_logan_config               (unsigned samples, double sampling_rate);
    void set_tx_logan_triggers             ();

  private:
    void init_spi           ();
    void spi_transfer       (uint8_t* rx, const uint8_t* tx, unsigned n);
    void worker_loop        ();

    bool validate_spi_data (uint16_t spi_data) noexcept;

    std::atomic<bool> m_read_enabled {true};

    static constexpr std::size_t MAX_QUEUE = 64;
};

#endif

// EOF
