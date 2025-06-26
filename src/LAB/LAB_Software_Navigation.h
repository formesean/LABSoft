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

class LAB_Software_Navigation : public LAB_Module
{
  private:
    LAB_Parent_Data_Software_Navigation m_parent_data;
    std::thread m_polling_thread;
    std::atomic<bool> m_should_stop{false};
    std::atomic<bool> m_is_running{false};

    uint8_t m_rx_buffer[2];
    uint8_t m_tx_buffer[2];

    std::atomic<bool> m_buffer_dirty{false};
    const uint8_t m_transfer_size;

    std::chrono::microseconds m_poll_interval{1};

    static constexpr uint16_t INVALID_PACKET_1 = 0x0000;
    static constexpr uint16_t INVALID_PACKET_2 = 0xFFFF;

  private:
    void parse_and_handle_packet(uint16_t packet);
    void polling_loop();
    bool is_valid_packet(uint16_t packet) const;

  public:
    LAB_Software_Navigation(LAB& LAB);
    ~LAB_Software_Navigation();

    void run();
    void poll_spi();
    void start_navigation();
    void stop_navigation();
    bool is_running() const { return m_is_running; }

    void set_poll_interval(std::chrono::microseconds interval) { m_poll_interval = interval; }

    static uint8_t get_packet_type(uint16_t packet) { return (packet >> 12) & 0x0F; }
    static uint8_t get_packet_action(uint16_t packet) { return (packet >> 8) & 0x0F; }
    static uint8_t get_packet_value(uint16_t packet) { return (packet >> 4) & 0x0F; }
    static uint8_t get_packet_checksum(uint16_t packet) { return packet & 0x0F; }
    static bool validate_checksum(uint16_t packet);
};

#endif

// EOF
