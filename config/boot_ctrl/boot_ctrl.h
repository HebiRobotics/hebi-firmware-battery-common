#include "stdint.h"

#pragma once

void setJumpToApplication(uint8_t jump);
void setStayInBootloader(uint8_t stay);

uint8_t shouldJumpToApplication(void);
uint8_t shouldStayInBootloader(void);

uint32_t appDataStartAddress(void);