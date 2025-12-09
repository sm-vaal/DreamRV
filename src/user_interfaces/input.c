//
// Created by sergi on 07/12/2025.
//

#include "input.h"
#include "../RV32_emu/memory.h"

typedef struct {
    KeyboardKey keyCode;
    uint32_t mmioAddress;
}buttonMap_t;

const buttonMap_t buttonMap[8] = {
    { BUTTON_UP_KEY,    BUTTON_UP_MMIO_ADDR },
    { BUTTON_DOWN_KEY,  BUTTON_DOWN_MMIO_ADDR },
    { BUTTON_LEFT_KEY,  BUTTON_LEFT_MMIO_ADDR },
    { BUTTON_RIGHT_KEY, BUTTON_RIGHT_MMIO_ADDR },
    { BUTTON_A_KEY,     BUTTON_A_MMIO_ADDR },
    { BUTTON_B_KEY,     BUTTON_B_MMIO_ADDR },
    { BUTTON_C_KEY,     BUTTON_C_MMIO_ADDR },
    { BUTTON_D_KEY,     BUTTON_D_MMIO_ADDR },
};

void pollInputs() {
    // set all buttons down by default
    writeWord(0x90000, 0x00000000);
    writeWord(0x90004, 0x00000000);

    for (int i = 0; i < (sizeof(buttonMap) / sizeof(buttonMap_t)); i++) {
        if (IsKeyDown(buttonMap[i].keyCode)) {
            writeByte(buttonMap[i].mmioAddress, 0x01);
        }
    }
}
