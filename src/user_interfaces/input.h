//
// Created by sergi on 07/12/2025.
//

#ifndef DREAMRV_INPUT_H
#define DREAMRV_INPUT_H

#include "raylib.h"


#define BUTTON_UP_MMIO_ADDR    0x90000
#define BUTTON_DOWN_MMIO_ADDR  0x90001
#define BUTTON_LEFT_MMIO_ADDR  0x90002
#define BUTTON_RIGHT_MMIO_ADDR 0x90003
#define BUTTON_A_MMIO_ADDR     0x90004
#define BUTTON_B_MMIO_ADDR     0x90005
#define BUTTON_C_MMIO_ADDR     0x90006
#define BUTTON_D_MMIO_ADDR     0x90007

#define BUTTON_UP_KEY          KEY_UP
#define BUTTON_DOWN_KEY        KEY_DOWN
#define BUTTON_LEFT_KEY        KEY_LEFT
#define BUTTON_RIGHT_KEY       KEY_RIGHT
#define BUTTON_A_KEY           KEY_Q
#define BUTTON_B_KEY           KEY_W
#define BUTTON_C_KEY           KEY_E
#define BUTTON_D_KEY           KEY_R


void pollInputs();





#endif //DREAMRV_INPUT_H