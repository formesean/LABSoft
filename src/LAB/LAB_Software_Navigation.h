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

class LAB_Software_Navigation : public LAB_Module
{
  private:
    LAB_Parent_Data_Software_Navigation m_parent_data;
    std::thread m_polling_thread;
    std::atomic<bool> m_should_stop{false};
    std::atomic<bool> m_is_running{false};

    // Pre-allocated buffers
    uint8_t m_rx_buffer[LAB_Parent_Data_Software_Navigation::BUFFER_SIZE];
    uint8_t m_tx_buffer[LAB_Parent_Data_Software_Navigation::BUFFER_SIZE];

    // Timing analysis
    struct TimingStats {
        std::chrono::high_resolution_clock::time_point last_poll_time;
        std::array<double, 1000> poll_times;
        std::array<double, 1000> spi_times;
        std::array<double, 1000> gpio_times;
        size_t sample_count = 0;
        double avg_poll_time = 0.0;
        double avg_spi_time = 0.0;
        double avg_gpio_time = 0.0;
    } m_timing_stats;

    // Performance settings
    std::chrono::microseconds m_poll_interval{50};
    bool m_enable_timing_analysis{true};

  private:
    void parse_and_handle_packet(uint16_t* packet);
    void polling_loop();
    void update_timing_stats();
    void print_timing_analysis();

  public:
    LAB_Software_Navigation(LAB& LAB);
    ~LAB_Software_Navigation();

    void run();
    void poll_spi();
    void start_navigation();
    void stop_navigation();
    bool is_running() const { return m_is_running; }

    // Performance tuning
    void set_poll_interval(std::chrono::microseconds interval) { m_poll_interval = interval; }
    void enable_timing_analysis(bool enable) { m_enable_timing_analysis = enable; }
    void print_performance_stats();
};

#endif

// EOF
