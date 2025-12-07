//
// Created by sergi on 25/11/2025.
//

#ifndef RV32I_PICO8_MEMORY_H
#define RV32I_PICO8_MEMORY_H

#include <stdint.h>
#include <stdlib.h>

void freeMemory();
void initMemory(uint32_t memorySize);

uint8_t getByte(uint32_t address);
uint16_t getHalf(uint32_t address);
uint32_t getWord(uint32_t address);

void writeByte(uint32_t address, uint8_t value);
void writeHalf(uint32_t address, uint16_t value);
void writeWord(uint32_t address, uint32_t value);

uint32_t loadBinary(const char* filename);
void clearMemory();

extern uint8_t* memory;


#endif //RV32I_PICO8_MEMORY_H