/**
 * Battery_CAN.h
 * 
 * 
*/

#pragma once

#include "all_msg.h"
#include "Driver.h"
#include "nodes/can_driver.h"

extern "C" {
#include <ch.h>
#include <hal.h>
}
    

namespace hebi::firmware::hardware {

class Battery_CAN : public Driver, public protocol::CAN_driver {
public:
    Battery_CAN();

    void sendMessage(protocol::base_msg msg) override;
    std::optional<protocol::base_msg> getMessage() override;

    void startDriver() override;
    void stopDriver() override;
protected:
    static void can_rx_callback(CANDriver *canp, uint32_t flags);

    static THD_FUNCTION(can_tx_thd_fn, arg);
    static THD_WORKING_AREA(can_tx_wa, 1024);
    static thread_t *can_tx_thread_;

    static THD_FUNCTION(can_rx_thd_fn, arg);
    static THD_WORKING_AREA(can_rx1_wa, 1024);
    static thread_t *can_rx_thread_;
    
    static util::LF_RingBuffer<protocol::base_msg, 30> rx_buffer_;
    static util::LF_RingBuffer<protocol::base_msg, 30> tx_buffer_;

    static CANConfig cancfg_;
};

} //namespace hebi::firmware::hardware
 