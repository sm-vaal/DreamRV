//
// Created by sergi on 07/12/2025.
//

#include "screen_only.h"
#include "gfx.h"
#include "../RV32_emu/memory.h"

void initWindowScreenOnly() {
    InitWindow(SCREEN_ONLY_H_RES * SCREEN_ONLY_SCALE, SCREEN_ONLY_V_RES * SCREEN_ONLY_SCALE, "DreamRV");
    initFrameBuffer();

}

void drawScreenOnly() {
    ClearBackground(WHITE);
    drawFrameBufferToScreenOnly();
}

void drawFrameBufferToScreenOnly() {
    updateFrameBufferFromMemory();
    UpdateTexture(*fbTexture, frameBuffer);

    Rectangle src = {
        0,
        0,
        (float) fbTexture->width,
        (float) fbTexture->height,
    };

    Rectangle dst = {
        0, 0,
        (float) SCREEN_ONLY_H_RES * SCREEN_ONLY_SCALE,
        (float) SCREEN_ONLY_V_RES * SCREEN_ONLY_SCALE
    };

    DrawTexturePro(*fbTexture, src, dst, (Vector2) {0, 0}, 0, WHITE);
}