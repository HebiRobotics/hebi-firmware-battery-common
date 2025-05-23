/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/* Memory map for STM32L432xC 
    256k ROM, 64k RAM
*/
MEMORY
{
    flash0 (rx) : org = 0x08000000, len = 64k   /* Bootloader */
    flash1 (rx) : org = 0x08010000, len = 180k  /* Application */
    flash2 (rx) : org = 0x0803D000, len = 12k   /* Database */
    flash3 (rx) : org = 0x00000000, len = 0
    flash4 (rx) : org = 0x00000000, len = 0
    flash5 (rx) : org = 0x00000000, len = 0
    flash6 (rx) : org = 0x00000000, len = 0
    flash7 (rx) : org = 0x00000000, len = 0
    shared (wx) : org = 0x20000000, len = 8     /* Shared RAM */
    ram0   (wx) : org = 0x20000008, len = 65528   /* 64k - 8 bytes RAM */
    ram1   (wx) : org = 0x00000000, len = 0
    ram2   (wx) : org = 0x00000000, len = 0
    ram3   (wx) : org = 0x00000000, len = 0
    ram4   (wx) : org = 0x00000000, len = 0
    ram5   (wx) : org = 0x00000000, len = 0
    ram6   (wx) : org = 0x00000000, len = 0
    ram7   (wx) : org = 0x00000000, len = 0
}

/* shared data starts point at the origin of the shared region */
_boot_ctrl_data_start = ORIGIN(shared);

/* For each data/text section two region are defined, a virtual region
   and a load region (_LMA suffix).*/

/* Flash region to be used for exception vectors.*/
REGION_ALIAS("VECTORS_FLASH", flash1);
REGION_ALIAS("VECTORS_FLASH_LMA", flash1);

/* Flash region to be used for constructors and destructors.*/
REGION_ALIAS("XTORS_FLASH", flash1);
REGION_ALIAS("XTORS_FLASH_LMA", flash1);

/* Flash region to be used for code text.*/
REGION_ALIAS("TEXT_FLASH", flash1);
REGION_ALIAS("TEXT_FLASH_LMA", flash1);

/* Flash region to be used for read only data.*/
REGION_ALIAS("RODATA_FLASH", flash1);
REGION_ALIAS("RODATA_FLASH_LMA", flash1);

/* Flash region to be used for various.*/
REGION_ALIAS("VARIOUS_FLASH", flash1);
REGION_ALIAS("VARIOUS_FLASH_LMA", flash1);

/* Flash region to be used for RAM(n) initialization data.*/
REGION_ALIAS("RAM_INIT_FLASH_LMA", flash1);

/* RAM region to be used for Main stack. This stack accommodates the processing
   of all exceptions and interrupts.*/
REGION_ALIAS("MAIN_STACK_RAM", ram0);

/* RAM region to be used for the process stack. This is the stack used by
   the main() function.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram0);

/* RAM region to be used for data segment.*/
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("DATA_RAM_LMA", flash1);

/* RAM region to be used for BSS segment.*/
REGION_ALIAS("BSS_RAM", ram0);

/* RAM region to be used for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* Generic rules inclusion.*/
INCLUDE rules.ld
