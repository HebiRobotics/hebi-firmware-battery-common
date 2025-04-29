/**
 * Flash_STM32L4.h
 * 
 * Flash storage for STM32L4 using ChibiOS MFS driver
*/

#include "Flash_Database.h"
#include "Driver.h"

#pragma once

namespace hebi::firmware::hardware {

class Flash_STM32L4 : public Flash_Database, public Driver {
public:

    enum class Partition {
        ALL = 0,
        APPLICATION = 1,
        DATABASE = 2,
        BOOTLOADER = 3,
    };

    enum class Status {
        COMPLETE,
        ERROR_INVALID_ADDRESS,
        ERROR_PROGRAM,
        ERROR_PROGRAM_VERIFY,
        ERROR_READ
    };

    Flash_STM32L4();

    void startDriver();
    void stopDriver();

    uint32_t getSize(Partition partition);
    Status program(Partition id, uint32_t offset, const void *data, uint32_t length);
    Status read(Partition id, uint32_t offset, void *data, uint32_t length);
    Status erase(Partition id);

protected:
    bool getArray(FlashDatabaseKey key, uint8_t *data, uint32_t size) const override;
    bool putArray(FlashDatabaseKey key, const uint8_t *data, uint32_t size) override;

    uint32_t resolveOffset(Partition id, uint32_t offset);
    static const uint32_t OFFSET_OUT_OF_RANGE = -1;

    static const uint32_t SECTOR_SIZE = 0x800;
    static const uint8_t NUM_SECTORS = 3;
    static const uint32_t PAGE_BASES[NUM_SECTORS];
    static const uint32_t PAGE_SIZES[NUM_SECTORS];
    static const Partition PARTITION_TABLE[NUM_SECTORS];
};

} //namespace hebi::firmware::hardware
 