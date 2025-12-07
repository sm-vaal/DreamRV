//
// Created by sergi on 05/12/2025.
//

#include "gfx.h"

#include <stdio.h>

#include "../RV32_emu/memory.h"

#include <stdlib.h>

Color* frameBuffer;
Texture2D* fbTexture;
Image* fbImage;

void initFrameBuffer() {
    frameBuffer = calloc(FB_H_RES * FB_V_RES, sizeof(Color));
    fbTexture = (Texture2D*) calloc(1, sizeof(Texture2D));
    fbImage = (Image*) calloc(1, sizeof(Image));

    if (!frameBuffer || !fbTexture || !fbImage) {
        perror("COULD NOT ALLOCATE MEMORY FOR FRAMEBUFFER\n");
        exit(1);
    }

    *fbImage = (Image) {
        .data = frameBuffer,
        .width = FB_H_RES,
        .height = FB_V_RES,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    *fbTexture = LoadTextureFromImage(*fbImage);
}

void updateFrameBufferFromMemory() {
    for (int i = 0; i < (FB_H_RES * FB_V_RES); i++) {
        uint32_t word = getWord(VRAM_MMIO_START + (i * 4));
        // Color    color = *((Color*) &word); // cursed optimization in little endian
        Color color = {
            word & 0xFF,
            (word >> 8) & 0xFF,
            (word >> 16) & 0xFF,
            (word >> 24) & 0xFF
        };
        color.a = 255;
        frameBuffer[i] = color;
    }
}