#include <hardware/flash.h>
#include <hardware/regs/addressmap.h>

#include <sys/stat.h>
#include <iostream>
#include <fstream>

uint8_t* flashMemory = NULL;

/**
 * The flash memory can be used to store configuration data which is persistent
 * over software updates. To emulate this, we use a file as large as the flash
 * memory of the PICO and store the data in that file. The data is not
 * automatically saved to disk.
 * This file does not contain the code as it would in the PICO flash memory,
 * so any problems arising from overwriting the code will not be detected.
 * 
 * To store the 
 */
void* getPointerInFlashMemory(uint32_t offset) {
    if (!flashMemory) {
        flashMemory = new uint8_t[PICO_FLASH_SIZE_BYTES];
        memset(flashMemory, -1, PICO_FLASH_SIZE_BYTES);
        struct stat buffer;
        if (stat("pico-flash.bin", &buffer) == 0) {
            std::ifstream file;
            file.open("pico-flash.bin", std::ios::in | std::ios::binary);
            file.read((char*)flashMemory, PICO_FLASH_SIZE_BYTES);
            file.close();
        }
    }

    return flashMemory + offset;
}

void storeFlashMemory() {
    std::ofstream file;
    file.open("pico-flash.bin", std::ios::out | std::ios::binary);
    file.write((char*)flashMemory, PICO_FLASH_SIZE_BYTES);
    file.close();
}

void flash_range_erase(uint32_t offset, size_t count) {
    if (!flashMemory) {
        getPointerInFlashMemory(0);
    }

    memset(flashMemory + offset, -1, count);
}

void flash_range_program(uint32_t offset, const uint8_t* data, size_t count) {
    if (!flashMemory) {
        getPointerInFlashMemory(0);
    }

    memcpy(flashMemory + offset, data, count);
}