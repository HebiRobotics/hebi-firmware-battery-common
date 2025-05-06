#include "Info_Handler.h"
#include "project_info.h"

extern "C" {
#include <hal.h>
}

namespace hebi::firmware::modules {

using namespace protocol;

Info_Handler::Info_Handler(
    hardware::Flash_Database& database,
    protocol::CAN_driver& can_driver) : 
    database_(database), can_(can_driver){

    initNodeID();

    //Generate 48 bit UID from 96 bit GUID
    uint64_t MAC_MASK           = 0x0000FCFFFFFFFFFF; //Mask off local / unicast bits
    uint64_t MAC_LOCAL_ADMIN    = 0x0000020000000000; //Mark as local admin
    uint16_t* uid = (uint16_t*)UID_BASE;
    uid_48_ = (uint64_t)((uint64_t)uid[0] | (uint64_t)uid[1] << 16 | (uint64_t)uid[2] << 32) ^ (uint64_t)((uint64_t)uid[3] | (uint64_t)uid[4] << 16 | (uint64_t)uid[5] << 32);
    uid_48_ = ((uid_48_ & MAC_MASK) | MAC_LOCAL_ADMIN);

    serial_number_handler_.load(database_.get(
        hardware::FlashDatabaseKey::SERIAL_NUMBER, 
        serial_number_handler_.buffer_));
    app_fw_hash_handler_.load(database_.get(
        hardware::FlashDatabaseKey::APPLICATION_HASH, 
        app_fw_hash_handler_.buffer_));
    hw_type_handler_.load(database_.get(
        hardware::FlashDatabaseKey::HW_TYPE, 
        hw_type_handler_.buffer_));
    hw_rev_handler_.load(database_.get(
        hardware::FlashDatabaseKey::HW_REV, 
        hw_rev_handler_.buffer_));
    elec_rev_handler_.load(database_.get(
        hardware::FlashDatabaseKey::ELEC_REV, 
        elec_rev_handler_.buffer_));
}

void Info_Handler::initNodeID(){
    node_id_valid_ = database_.get(hardware::FlashDatabaseKey::NODE_ID, node_id_);

    #ifdef _FIRMWARE_MODE_BOOTLOADER
    //temporary...
    if(!node_id_valid_){
        node_id_ = 0x01;
        database_.put(hardware::FlashDatabaseKey::NODE_ID, node_id_);
    }
    #endif
}

void Info_Handler::recvd_ctrl_read_info(protocol::ctrl_read_info_msg& msg) {
    if(msg.EID.node_id != node_id_) return;

    if(msg.read_GUID()) can_.sendMessage(protocol::ctrl_guid_msg(node_id_, uid_48_));
    if(msg.read_elec_type()){
        //Send string as a series of smaller messages
        size_t str_len = strlen(ELECTRICAL_TYPE);
        can_.sendString(node_id_, MessageType::INFO_ELEC_TYPE, 
            ELECTRICAL_TYPE, str_len);
    }
    if(msg.read_HW_type()){
        //Send string as a series of smaller messages if we have it
        if(hw_type_handler_.hasString()){
            can_.sendString(node_id_, MessageType::INFO_HW_TYPE, 
                hw_type_handler_.string(), hw_type_handler_.stringLength());
        }
    }
    if(msg.read_FW_version()){
        //Send string as a series of smaller messages
        size_t str_len = strlen(FIRMWARE_REVISION);
        can_.sendString(node_id_, MessageType::INFO_FW_VERSION, 
            FIRMWARE_REVISION, str_len);
    }
    if(msg.read_FW_mode()) can_.sendMessage(protocol::ctrl_fw_mode_msg(node_id_, protocol::ctrl_fw_mode_msg::FW_MODE_BOOT));
    if(msg.read_APP_FW_hash()){
        //Send string as a series of smaller messages if we have it
        if(app_fw_hash_handler_.hasString()){
            can_.sendString(node_id_, MessageType::INFO_APP_FW_HASH, 
                app_fw_hash_handler_.string(), app_fw_hash_handler_.stringLength());
        }
    }
    if(msg.read_serial_number()){
        //Send string as a series of smaller messages if we have it
        if(serial_number_handler_.hasString()){
            can_.sendString(node_id_, MessageType::INFO_SERIAL_NUM, 
                serial_number_handler_.string(), serial_number_handler_.stringLength());
        }
    }
    if(msg.read_HW_revision()){
        //Send string as a series of smaller messages if we have it
        if(hw_rev_handler_.hasString()){
            can_.sendString(node_id_, MessageType::INFO_HW_REV, 
                hw_rev_handler_.string(), hw_rev_handler_.stringLength());
        }
    }
    if(msg.read_ELEC_revision()){
        //Send string as a series of smaller messages if we have it
        if(elec_rev_handler_.hasString()){
            can_.sendString(node_id_, MessageType::INFO_ELEC_REV, 
                elec_rev_handler_.string(), elec_rev_handler_.stringLength());
        }
    }
}

#ifdef _FIRMWARE_MODE_BOOTLOADER
void Info_Handler::recvd_boot_set_serial_num(protocol::boot_set_serial_num_msg& msg) {
    if(msg.EID.node_id != node_id_) return;

    serial_number_handler_.handleMessage(msg);

    if(serial_number_handler_.hasString()){
        database_.put(hardware::FlashDatabaseKey::SERIAL_NUMBER, serial_number_handler_.buffer_);
    }
}

void Info_Handler::recvd_boot_set_hw_type(protocol::boot_set_hw_type_msg& msg) {
    if(msg.EID.node_id != node_id_) return;

    hw_type_handler_.handleMessage(msg);

    if(hw_type_handler_.hasString()){
        database_.put(hardware::FlashDatabaseKey::HW_TYPE, hw_type_handler_.buffer_);
    }
}

void Info_Handler::recvd_boot_set_hw_rev(protocol::boot_set_hw_rev_msg& msg) {
    if(msg.EID.node_id != node_id_) return;

    hw_rev_handler_.handleMessage(msg);

    if(hw_rev_handler_.hasString()){
        database_.put(hardware::FlashDatabaseKey::HW_REV, hw_rev_handler_.buffer_);
    }
}

void Info_Handler::recvd_boot_set_elec_rev(protocol::boot_set_elec_rev_msg& msg) {
    if(msg.EID.node_id != node_id_) return;

    elec_rev_handler_.handleMessage(msg);

    if(elec_rev_handler_.hasString()){
        database_.put(hardware::FlashDatabaseKey::ELEC_REV, elec_rev_handler_.buffer_);
    }
}
#endif
    
}