#ifndef _FLASH_H
#define _FLASH_H

#include <types.h>

#define FLASH_SECTOR_SIZE 4096
#define FLASH_PAGE_SIZE 256

void* getPointerInFlashMemory(uint32_t offset);

void flash_range_erase(uint32_t offset, size_t count);
void flash_range_program(uint32_t offset, const uint8_t* data, size_t count);

#endif