#include "Flash_STM32L4.h"

extern "C" {
#include <ch.h>
#include <hal.h>
#include <hal_mfs.h>
}

namespace hebi::firmware::hardware {


mfs_nocache_buffer_t __nocache_mfsbuf1;
MFSDriver mfs1;
uint8_t __nocache_mfs_buffer[512];

const MFSConfig mfscfg1 = {
    .flashp           = (BaseFlash *)&EFLD1,
    .erased           = 0xFFFFFFFFU,
    .bank_size        = 3*2048U,
    .bank0_start      = 121U,
    .bank0_sectors    = 3U,
    .bank1_start      = 124U,
    .bank1_sectors    = 3U
};

const uint32_t Flash_STM32L4::PAGE_SIZES[NUM_SECTORS] = {
    0x30000, // 96k (Bootloader)
    0x4A000, // 148k (Application)
    0x03000, // 6 * 2k (Database)
};

const uint32_t Flash_STM32L4::PAGE_BASES[NUM_SECTORS] = {
    0x08000000,
    0x08000000 + PAGE_SIZES[0],
    0x08000000 + PAGE_SIZES[0] + PAGE_SIZES[1],
};


const Flash_STM32L4::Partition Flash_STM32L4::PARTITION_TABLE[NUM_SECTORS] = {
    Flash_STM32L4::Partition::BOOTLOADER,
    Flash_STM32L4::Partition::APPLICATION,
    Flash_STM32L4::Partition::DATABASE,
};



Flash_STM32L4::Flash_STM32L4(){

    mfsObjectInit(&mfs1, &__nocache_mfsbuf1);

    startDriver();

}

void Flash_STM32L4::startDriver(){
    /* 
    Starting EFL driver.
    No config needed.
    */
    eflStart(&EFLD1, NULL);

    mfsStart(&mfs1, &mfscfg1);
}

void Flash_STM32L4::stopDriver(){
    eflStop(&EFLD1);

    mfsStop(&mfs1);
}

bool Flash_STM32L4::getArray(FlashDatabaseKey key, uint8_t *data, uint32_t size) const {
    mfs_id_t id = static_cast<mfs_id_t>(key);
    size_t size_ = size;
    mfs_error_t err = mfsReadRecord(&mfs1, id, &size_, data);

    return size_ == size && err == MFS_NO_ERROR;
}

bool Flash_STM32L4::putArray(FlashDatabaseKey key, const uint8_t *data, uint32_t size) {
    mfs_id_t id = static_cast<mfs_id_t>(key);
    mfs_error_t err = mfsWriteRecord(&mfs1, id, size, data);

    return err == MFS_NO_ERROR;
}

uint32_t Flash_STM32L4::getSize(Flash_STM32L4::Partition partition){
    // Iterate through partition table and sum them up
    int sum = 0;
    for (int i = 0; i < NUM_SECTORS; i++) {
        if (PARTITION_TABLE[i] == partition || partition == Partition::ALL)
            sum += PAGE_SIZES[i];
    }
    return sum;
}

uint32_t Flash_STM32L4::resolveOffset(Flash_STM32L4::Partition id, uint32_t offset) {
    //TODO: More error checking here?
    if(id == Partition::ALL) return offset + PAGE_BASES[0];

    for (int i = 0; i < NUM_SECTORS; i++) {
        if (PARTITION_TABLE[i] == id && offset < PAGE_SIZES[i]) {
            return offset + PAGE_BASES[i];
        }
    }

    // if getSize() gets called first, this function will
    // never get here.
    return OFFSET_OUT_OF_RANGE;
}

Flash_STM32L4::Status Flash_STM32L4::program(Flash_STM32L4::Partition id, uint32_t offset, const void *data, uint32_t length){
    // Make sure partition is large enough
    // if (resolveOffset(id, offset + length - 1) == OFFSET_OUT_OF_RANGE)
    //     return Status::ERROR_INVALID_ADDRESS;

    // Resolve address
    uint32_t address = resolveOffset(id, offset);
    if (address == OFFSET_OUT_OF_RANGE)
        return Status::ERROR_INVALID_ADDRESS;

    auto result = flashProgram(&EFLD1, address, length, (const uint8_t*) data);

    if(result != FLASH_NO_ERROR)
        return Status::ERROR_PROGRAM;
    
    //TODO: Verify?
    return Status::COMPLETE;
}

Flash_STM32L4::Status Flash_STM32L4::read(Flash_STM32L4::Partition id, uint32_t offset, void *data, uint32_t length){
    // Resolve address
    uint32_t address = resolveOffset(id, offset);
    if (address == OFFSET_OUT_OF_RANGE)
        return Status::ERROR_INVALID_ADDRESS;

    auto result = flashRead(&EFLD1, address, length, (uint8_t*) data);

    if(result != FLASH_NO_ERROR)
        return Status::ERROR_READ;
    
    return Status::COMPLETE;
}

Flash_STM32L4::Status Flash_STM32L4::erase(Flash_STM32L4::Partition id){
    // Sequentially delete all sectors belongig
    // to the partition
    for (int i = 0; i < NUM_SECTORS; i++) {
        if (PARTITION_TABLE[i] == id || id == Partition::ALL) {
            uint32_t n_2k_sectors = PAGE_SIZES[i] / SECTOR_SIZE;
            uint32_t sector_base = PAGE_BASES[i] / SECTOR_SIZE;
            for(uint16_t ind = 0; ind < n_2k_sectors; ind++){
                // Try to erase
                auto result = flashStartEraseSector(&EFLD1, sector_base + ind);

                if(result != FLASH_NO_ERROR) return Status::ERROR_PROGRAM;

                //Wait for erase to finish
                uint32_t sleep_time_ms = 1;
                result = flashQueryErase(&EFLD1, &sleep_time_ms);
                while(result == FLASH_BUSY_ERASING){
                    chThdSleepMilliseconds(sleep_time_ms);
                    result = flashQueryErase(&EFLD1, &sleep_time_ms);
                }

                if(result != FLASH_NO_ERROR) return Status::ERROR_PROGRAM;
            }
        }
    }

    // Return
    return Status::COMPLETE;
}

} //namespace hebi::firmware::hardware