#ifndef LAB_SOFTWARE_NAVIGATION_H
#define LAB_SOFTWARE_NAVIGATION_H

#include "LAB_Module.h"
#include "../Utility/LAB_Definitions.h"

#include <iostream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <array>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

class LAB_Software_Navigation : public LAB_Module
{
  private:
    LAB_Parent_Data_Software_Navigation m_parent_data;
    std::thread m_polling_thread;
    std::atomic<bool> m_should_stop{false};
    std::atomic<bool> m_is_running{false};

    std::array<uint8_t, 2> m_rx_buffer{};
    std::array<uint8_t, 2> m_tx_buffer{};

    std::atomic<bool> m_buffer_dirty{false};
    static constexpr uint8_t TRANSFER_SIZE = 2;

    std::chrono::microseconds m_poll_interval{10000};

    static constexpr uint16_t INVALID_PACKET_1 = 0x0000;
    static constexpr uint16_t INVALID_PACKET_2 = 0xFFFF;
    static constexpr std::chrono::microseconds CS_SETUP_DELAY{5};

    struct PacketData {
      uint8_t type;
      uint8_t action;
      uint8_t value;
      uint8_t checksum;
      uint8_t expected_checksum;
      bool is_valid;
    };

  private:
    void initialize_spi();
    void cleanup_spi();
    PacketData parse_packet(uint16_t packet) const;
    void handle_packet(const PacketData& packet_data);
    void polling_loop();
    bool is_valid_packet(uint16_t packet) const;
    void log_packet_info(const PacketData& packet_data) const;

  public:
    explicit LAB_Software_Navigation(LAB& lab);
    ~LAB_Software_Navigation();

    void run();
    void poll_spi();
    void start_navigation();
    void stop_navigation();
    bool is_running() const noexcept { return m_is_running; }

    void set_poll_interval(std::chrono::microseconds interval) noexcept {
      m_poll_interval = interval;
    }

    // Static packet parsing utilities
    static uint8_t get_packet_type(uint16_t packet) noexcept {
      return (packet >> 12) & 0x0F;
    }
    static uint8_t get_packet_action(uint16_t packet) noexcept {
      return (packet >> 8) & 0x0F;
    }
    static uint8_t get_packet_value(uint16_t packet) noexcept {
      return (packet >> 4) & 0x0F;
    }
    static uint8_t get_packet_checksum(uint16_t packet) noexcept {
      return packet & 0x0F;
    }
    static uint8_t calculate_checksum(uint8_t type, uint8_t action, uint8_t value) noexcept {
      return (type ^ action ^ value) & 0x0F;
    }
    static bool validate_checksum(uint16_t packet) noexcept;
};

#endif

// EOF
