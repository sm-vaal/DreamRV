//
// Created by sergi on 05/12/2025.
//

#ifndef RV32I_PICO8_GFX_H
#define RV32I_PICO8_GFX_H

#include "raylib.h"

#define VRAM_MMIO_START 0x80000

#define FB_V_RES 128
#define FB_H_RES 128

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 860

#define WINDOW_SCALE 1

extern Color* frameBuffer;
extern Texture2D* fbTexture;
extern Image* fbImage;

void initFrameBuffer();
void updateFrameBufferFromMemory();

#endif //RV32I_PICO8_GFX_H