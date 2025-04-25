/**
 * Battery_CAN.cpp
 * 
 * 
*/

#include "Battery_CAN.h"
#include "board.h"
#include "all_msg.h"

namespace hebi::firmware::hardware {

// static Battery_CAN *driver = nullptr;

/*
 * Internal loopback mode, 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 42.7.7 on the STM32 reference manual.
 */
CANConfig Battery_CAN::cancfg_ = {
    .mcr =  //CAN_MCR_NART | //No Auto retry
            CAN_MCR_ABOM | //Auto bus-off management
            CAN_MCR_AWUM | //Auto wake-up mode
            CAN_MCR_TXFP,  //Chronological TX FIFO priority
    .btr =  //CAN_BTR_LBKM | //Loopback mode
            CAN_BTR_SJW(3) | //Resync jump width
            CAN_BTR_TS2(1) |  //Time in TS2
            CAN_BTR_TS1(12) | //Time in TS1
            CAN_BTR_BRP(10)    //Baud rate = APB1 / BRP
};

util::LF_RingBuffer<protocol::base_msg, 5> Battery_CAN::rx_buffer_;
util::LF_RingBuffer<protocol::base_msg, 5> Battery_CAN::tx_buffer_;

/*
* Receiver thread.
*/
thread_t *Battery_CAN::can_rx_thread_ {nullptr};
THD_WORKING_AREA(Battery_CAN::can_rx1_wa, 256);
THD_FUNCTION(Battery_CAN::can_rx_thd_fn, p) {
    CANDriver *canp = (CANDriver *)p;
    event_listener_t el;
    CANRxFrame rxmsg;

    (void)p;
    chRegSetThreadName("receiver");
    chEvtRegister(&canp->rxfull_event, &el, 0);
    while(!chThdShouldTerminateX()) {
        if (chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100)) == 0)
            continue;
        while (canReceive(canp, CAN_ANY_MAILBOX,
                            &rxmsg, TIME_IMMEDIATE) == MSG_OK) {
            /* Process message.*/
            rx_buffer_.add(protocol::base_msg(rxmsg.EID, rxmsg.DLC, rxmsg.data8));
        }
    }
    chEvtUnregister(&CAND1.rxfull_event, &el);
}

/*
* Transmitter thread.
*/

#define CAN_TX_MSG_EVENT_MASK EVENT_MASK(0)

thread_t *Battery_CAN::can_tx_thread_ {nullptr};
THD_WORKING_AREA(Battery_CAN::can_tx_wa, 256);
THD_FUNCTION(Battery_CAN::can_tx_thd_fn, p) {
    
    // event_listener_t tx_msg_listener;
    CANTxFrame txmsg;
    // protocol::battery_state_msg test_msg(0x01, 1, 2, 3, 4); //TEST

    (void)p;
    chRegSetThreadName("transmitter");
    //chEvtRegisterMask(&tx_msg_event_source, &tx_msg_listener, CAN_TX_MSG_EVENT_MASK);

    while (!chThdShouldTerminateX()) {
        /* Waiting for any of the events we're registered on.*/
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);

        if(evt & CAN_TX_MSG_EVENT_MASK){
            bool has_data = true;
            while(has_data){
                auto msg_opt = tx_buffer_.take();
                if(msg_opt.has_value()){
                    auto msg = msg_opt.value();

                    txmsg.IDE = CAN_IDE_EXT;
                    txmsg.EID = msg.EID.raw;
                    txmsg.RTR = CAN_RTR_DATA;
                    txmsg.DLC = msg.len;
                    txmsg.data32[0] = msg.data32[0];
                    txmsg.data32[1] = msg.data32[1];
            
                    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
                } else {
                    has_data = false;
                }

            }
        }
    }
}


Battery_CAN::Battery_CAN() {
    // driver = this;

    startDriver();

    /*
    * Starting the transmitter and receiver threads.
    */
    Battery_CAN::can_rx_thread_ = 
        chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), 
        NORMALPRIO + 7, can_rx_thd_fn, &CAND1);
    Battery_CAN::can_tx_thread_ = 
        chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), 
        NORMALPRIO + 7, can_tx_thd_fn, NULL);
}

void Battery_CAN::startDriver() {
    palWriteLine(LINE_CAN1_STB, PAL_LOW);
    palWriteLine(LINE_CAN1_SHDN, PAL_LOW);
    
    canStart(&CAND1, &cancfg_);
}

void Battery_CAN::stopDriver() {
    palWriteLine(LINE_CAN1_STB, PAL_HIGH);
    palWriteLine(LINE_CAN1_SHDN, PAL_HIGH);
    
    canStop(&CAND1);
}
   
void Battery_CAN::sendMessage(protocol::base_msg msg){
    tx_buffer_.add(msg);

    if(can_tx_thread_ != NULL){    
        chEvtSignal(can_tx_thread_, CAN_TX_MSG_EVENT_MASK);
    }
}

std::optional<protocol::base_msg> Battery_CAN::getMessage(){
    return rx_buffer_.take();
}
    
} //namespace hebi::firmware::hardware