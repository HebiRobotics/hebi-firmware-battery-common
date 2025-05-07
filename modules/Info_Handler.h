
#include "nodes/base_node.h"
#include "nodes/can_driver.h"
#include "nodes/StringMsgHelper.h"
#include "Flash_Database.h"

#pragma once

namespace hebi::firmware::modules {

class Info_Handler : public protocol::Base_Node {
public:
    Info_Handler(
        hardware::Flash_Database& database,
        protocol::CAN_driver& can_driver);

    uint8_t nodeID(){
        return node_id_;
    }

protected:
    void initNodeID();

    void recvd_ctrl_read_info(protocol::info_read_msg& msg) override;

    #ifdef _FIRMWARE_MODE_BOOTLOADER
    void recvd_hw_type(info_hw_type_msg& msg) override;
    void recvd_serial_num(info_serial_num_msg& msg) override;
    void recvd_hw_rev(info_hw_rev_msg& msg) override;
    void recvd_elec_rev(info_elec_rev_msg& msg) override;
    #endif

    bool node_id_valid_ {false};
    uint8_t node_id_ { protocol::DEFAULT_NODE_ID };
    uint64_t uid_48_ {0};

    static const uint8_t SERIAL_NUM_LEN = 16;
    static const uint8_t FW_HASH_LEN = 32;
    static const uint8_t HW_TYPE_LEN = 16;
    static const uint8_t HW_REV_LEN = 7;

    util::StringMsgHelper<SERIAL_NUM_LEN> serial_number_handler_;
    util::StringMsgHelper<FW_HASH_LEN> app_fw_hash_handler_;
    util::StringMsgHelper<HW_TYPE_LEN> hw_type_handler_;
    util::StringMsgHelper<HW_REV_LEN> hw_rev_handler_;
    util::StringMsgHelper<HW_REV_LEN> elec_rev_handler_;

    hardware::Flash_Database& database_;
    protocol::CAN_driver& can_;
};

}