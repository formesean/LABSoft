#ifndef LAB_SOFTWARE_NAVIGATION_H
#define LAB_SOFTWARE_NAVIGATION_H

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"

#include <cstdint>
#include <array>
#include <mutex>
#include <queue>
#include <atomic>

class LAB_Software_Navigation : public LAB_Module
{
  private:
    LAB_Parent_Data_Software_Navigation m_parent_data;

    std::array<uint8_t, LABC::PIN::SNM::TRANSFER_SIZE> m_rx_buffer = {0};
    std::array<uint8_t, LABC::PIN::SNM::TRANSFER_SIZE> m_tx_buffer = {0};

    // Concurrency and queuing (single-threaded service, guarded transfers)
    std::mutex                          m_spi_mutex;
    std::mutex                          m_tx_mutex;
    std::queue<std::array<uint8_t, 3> >  m_queue;
    std::mutex                          m_queue_mutex;
    std::atomic<bool>                   m_stop_sent {false};
    std::atomic<bool>                   m_logan_start_sent {false};
    std::atomic<bool>                   m_logan_stop_sent  {false};

  public:
    LAB_Software_Navigation                (LAB &lab);
    ~LAB_Software_Navigation               ();

    std::array<uint8_t, 3> update_spi_data ();
    void tick                              ();
    bool try_dequeue                       (std::array<uint8_t, 3>& out);

    void set_tx_logan_config               (unsigned samples, double sampling_rate);
    void set_tx_logan_triggers             ();
    void set_tx_logan_stop                 ();
    void reset_logan_rx_state              ();

    // SNM attach handshake
    void set_snm_attached                  (bool attached);
    void announce_snm_attached             ();
    void announce_program_stopping         ();
    bool is_snm_config_enabled             () const;

  private:
    void init_spi           ();
    void spi_transfer       (uint8_t* rx, const uint8_t* tx, unsigned n);
    void service_once       ();

    bool validate_spi_data (uint16_t spi_data) noexcept;
    bool is_logan_header   (uint16_t word) noexcept;

    enum class LOGAN_RX_STATE : uint8_t { NONE = 0, EXPECT_PAYLOAD, EXPECT_CHECKSUM };
    LOGAN_RX_STATE m_logan_rx_state { LOGAN_RX_STATE::NONE };

    std::atomic<bool> m_read_enabled {true};

    // SNM state and helpers
    std::atomic<bool> m_snm_attached {false};
    void set_tx_header(uint8_t type, uint8_t action, uint8_t value);
    void send_immediate_tx_blocking(uint8_t b0, uint8_t b1);

    static constexpr std::size_t MAX_QUEUE = 127;
};

#endif

// EOF
