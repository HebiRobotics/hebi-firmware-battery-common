#include "boot_ctrl.h"

/*  boot_ctrl.c

    This file uses a pre-defined shared area in memory to allow the application
    and bootloader to communicate without the use of the flash. It makes use of
    magic numbers in an unitialized area of shared memory. This allows the
    bootloader to make decisions about whether to boot or not in a lightweight 
    manner.

    "setJumpToApplication" should be used only by the bootloader to signal to
    itself that it wants to jump to the application code. This should only be 
    set once the application is verified. This value is checked in the reset 
    handler.

    "setStayInBootloader" should only be used by the application to signal to 
    the bootloader to not immediately jump to the application.
    
    References: 
    https://community.memfault.com/t/zero-to-main-how-to-write-a-bootloader-from-scratch-interrupt/66?page=2
    https://stackoverflow.com/questions/39998485/how-to-access-variable-define-in-linker-script-in-c
*/

#define MAGIC_NUMBER (0x4EB14EB1)

//Address, set in the linker script
extern uint32_t _boot_ctrl_data_start;
extern uint32_t _app_data_start;

__attribute__((packed))
struct boot_ctrl_data {
    uint32_t jump_to_app;
    uint32_t stay_in_boot;
};

static struct boot_ctrl_data *data = (struct boot_ctrl_data *)&_boot_ctrl_data_start;

void setJumpToApplication(uint8_t jump) {
    if(jump){
        data->jump_to_app = MAGIC_NUMBER;
    } else {
        data->jump_to_app = 0;
    }
}

void setStayInBootloader(uint8_t stay) {
    if(stay){
        data->stay_in_boot = MAGIC_NUMBER;
    } else {
        data->stay_in_boot = 0;
    }
}

uint8_t shouldJumpToApplication(void) {
    return data->jump_to_app == MAGIC_NUMBER;
}

uint8_t shouldStayInBootloader(void) {
    return data->stay_in_boot == MAGIC_NUMBER;
}

uint32_t appDataStartAddress(void) {
    return &_app_data_start;
}