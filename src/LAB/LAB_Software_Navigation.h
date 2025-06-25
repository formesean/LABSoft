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

class LAB_Software_Navigation : public LAB_Module
{
  private:
    LAB_Parent_Data_Software_Navigation m_parent_data;
    std::thread m_polling_thread;
    std::atomic<bool> m_should_stop{false};
    std::atomic<bool> m_is_running{false};

    uint8_t m_rx_buffer[LAB_Parent_Data_Software_Navigation::BUFFER_SIZE];
    uint8_t m_tx_buffer[LAB_Parent_Data_Software_Navigation::BUFFER_SIZE];

    static constexpr std::chrono::microseconds DEFAULT_POLL_INTERVAL{100};
    std::chrono::microseconds m_poll_interval{DEFAULT_POLL_INTERVAL};

  private:
    void parse_and_handle_packet (uint16_t* packet);
    void polling_loop();

  public:
    LAB_Software_Navigation (LAB& LAB);
    ~LAB_Software_Navigation ();

    void run              ();
    void poll_spi         ();
    void start_navigation ();
    void stop_navigation  ();
    bool is_running       () const { return m_is_running; }

    void set_poll_interval(std::chrono::microseconds interval) { m_poll_interval = interval; }
    std::chrono::microseconds get_poll_interval() const { return m_poll_interval; }
};

#endif

// EOF
