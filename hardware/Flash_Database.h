/**
 * Flash_Database.h
 * 
 * Simple interface class for a flash database
*/

#pragma once

#include <stdint.h>

namespace hebi::firmware::hardware {

enum class FlashDatabaseKey {
    INVALID = 0,
    NODE_ID = 1,
    AES_KEY = 2,
    APPLICATION_HASH = 3,
    BOOTLOADER_HASH = 4, //Unused
    APPLICATION_VALID = 5,
    STAY_IN_BOOTLOADER = 6, //Deprecated
    SERIAL_NUMBER = 7,
    HW_TYPE = 8,
    HW_REV = 9,
    ELEC_REV = 10,
};

class Flash_Database {
public:

    template<typename T> bool get(FlashDatabaseKey key, T &data) const {
        if(key == FlashDatabaseKey::INVALID) return false;

        // Attempt to retrieve the specified data structure
        // and report success status.
        return getArray(key, (uint8_t*) &data, sizeof (T));
    }

    template<typename T> bool put(FlashDatabaseKey key, const T &data) {
        if(key == FlashDatabaseKey::INVALID) return false;

        return putArray(key, (uint8_t*) &data, sizeof (T));
    }

protected:
    /**
     * Retrieves the data at a specified address and copies
     * 'size' bytes into the buffer specified by 'data'.
     * Returns false if the parameter was not found or if
     * the stored size did not match the expected size.
     */
    virtual bool getArray(FlashDatabaseKey key, uint8_t *data, uint32_t size) const = 0;
    
    /**
     * Copies 'size' bytes from the 'data' buffer to the address
     * specified by 'addr'.  Returns whether the write succeeded.
     * 
     * `no_swap` ensures that the write will not cause a page swap, using the reserved
     * safety buffer at the end of the partition if necessary.
     */
    virtual bool putArray(FlashDatabaseKey key, const uint8_t *data, uint32_t size) = 0;
};
 
 } //namespace hebi::firmware::hardware
 