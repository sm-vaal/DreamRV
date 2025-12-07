//
// Created by sergi on 25/11/2025.
//

#include "memory.h"
#include "../debug.h"

uint8_t* memory  = NULL;
uint32_t memSize = 0;
uint32_t romEnd = 0;

void initMemory(uint32_t memorySize) {
    memSize = memorySize;

    memory = (uint8_t*) malloc(memSize);
    if (!memory) {
        printError(FATAL,"Error allocating memory for RAM\n");
        exit(-1);
    }

    memset(memory,0,memSize);
}

void freeMemory() {
    free(memory);
    memory = NULL;
    memSize = 0;
}

uint8_t getByte(uint32_t address) {
    if (address >= memSize) {
        printError(WARNING,"Out of bounds memory read at address %08x\n", address);
        return 0x00; // return 0, could throw an exception to the CPU?
    } else {
        return memory[address];
    }
}

uint16_t getHalf(uint32_t address) {
    if ((address + 1) >= memSize) {
        printError(WARNING,"Out of bounds memory read at address %08x\n", address + 1);
        return 0x0000;
    } else {
        return ((uint16_t) memory[address] | ((uint16_t) memory[address+1] << 8));
    }
}

uint32_t getWord(uint32_t address) {
    if ((address + 3) >= memSize) {
        printError(WARNING,"Out of bounds memory read at address %08x\n", address + 3);
        return 0x0000;
    }
    else {
        return ((uint32_t) memory[address] | ((uint32_t) memory[address+1] << 8) |
            ((uint32_t) memory[address+2] << 16) | ((uint32_t) memory[address + 3] << 24));
    }
}

void writeByte(uint32_t address, uint8_t value) {
    if (address >= memSize) {
        printError(WARNING,"Out of bounds memory write at address %08x\n", address);
    } else {
        memory[address] = value;
    }
}

void writeHalf(uint32_t address, uint16_t value) {
    if (address + 1 >= memSize) {
        printError(WARNING,"Out of bounds memory write at address %08x\n", address + 1);
    } else {
        memory[address]   = (value & 0xFF);
        memory[address+1] = (value >> 8) & 0xFF;
    }
}

void writeWord(uint32_t address, uint32_t value) {
    if (address + 3 >= memSize) {
        printError(WARNING,"Out of bounds memory write at address %08x\n", address + 3);
    } else {
        memory[address]   = (value & 0xFF);
        memory[address+1] = (value >> 8)  & 0xFF;
        memory[address+2] = (value >> 16) & 0xFF;
        memory[address+3] = (value >> 24) & 0xFF;
    }
}

uint32_t loadBinary(const char* filename) {
    if (!memory) {
        printError(FATAL, "Memory not initialized before loading binary\n");
        return 0;
    }

    FILE* f = fopen(filename, "rb");
    if (!f) {
        printError(FATAL, "Could not open binary file '%s'\n", filename);
        return 0;
    }

    // Determine file size
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fileSize < 0) {
        printError(FATAL, "Error determining file size for '%s'\n", filename);
        fclose(f);
        return 0;
    }

    // Check memory bounds
    if ((uint32_t)fileSize > memSize) {
        printError(FATAL,
                   "Binary '%s' size (%ld bytes) exceeds allocated memory (%u bytes)\n",
                   filename, fileSize, memSize);
        fclose(f);
        return 0;
    }

    // Read binary into memory buffer
    size_t readBytes = fread(memory, 1, fileSize, f);
    fclose(f);

    if (readBytes != (size_t)fileSize) {
        printError(WARNING,
                   "File read mismatch: expected %ld bytes, got %zu bytes\n",
                   fileSize, readBytes);
    }

    romEnd = readBytes;

    return (uint32_t) readBytes;
}

void clearMemory() {
    memset(memory + romEnd, 0, memSize - romEnd);
}