#ifndef LAB_SOFTWARE_NAVIGATION_H
#define LAB_SOFTWARE_NAVIGATION_H

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <array>
#include <functional>
#include <memory>

class LAB_Software_Navigation : public LAB_Module
{
  private:
    static constexpr std::chrono::microseconds POLL_INTERVAL{500};
    static constexpr std::chrono::nanoseconds CS_SETUP_DELAY{100};
    static constexpr uint8_t TRANSFER_SIZE = 2;
    static constexpr uint16_t INVALID_PACKET = 0x0000;

    // Pre-allocated buffers
    alignas(16) std::array<uint8_t, TRANSFER_SIZE> m_rx_buffer{};
    alignas(16) std::array<uint8_t, TRANSFER_SIZE> m_tx_buffer{};

    // Thread management
    std::unique_ptr<std::thread> m_polling_thread;
    std::atomic<bool> m_should_stop{false};
    std::atomic<bool> m_is_running{false};

    LAB_Parent_Data_Software_Navigation m_parent_data;

    std::function<void(uint8_t, uint8_t, uint8_t)> m_packet_handler;

    std::atomic<uint64_t> m_packets_processed{0};
    std::atomic<uint64_t> m_invalid_packets{0};

  public:
    explicit LAB_Software_Navigation(LAB &lab);
    ~LAB_Software_Navigation();

    void start_navigation();
    void stop_navigation();
    bool is_running() const noexcept { return m_is_running.load(std::memory_order_relaxed); }

    uint64_t get_packets_processed() const noexcept { return m_packets_processed.load(); }
    uint64_t get_invalid_packets() const noexcept { return m_invalid_packets.load(); }

    // Event callback registration
    void set_packet_handler(std::function<void(uint8_t, uint8_t, uint8_t)> handler)
    {
      m_packet_handler = std::move(handler);
    }

  private:
    void initialize_spi();
    void cleanup_spi();
    void polling_loop();

    inline bool process_packet(uint16_t packet) noexcept;
    inline bool validate_packet_fast(uint16_t packet) noexcept;
    inline void handle_valid_packet(uint8_t type, uint8_t action, uint8_t value) noexcept;

    inline bool spi_transfer_fast() noexcept;
};

#endif

// EOF
