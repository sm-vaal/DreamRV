//
// Created by sergi on 04/12/2025.
//

#include "ui.h"
#include "../RV32_emu/RV32_core.h"
#include "gfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../RV32_emu/memory.h"

uiButton_t* buttons;
uiTextBox_t*  textBoxes;
uiDisplayBox_t* displayBox;
uiRectangle_t* rectangles;

uint32_t currentMemPage = 0;


const char* registerAliases[32] = {
    "zero", "ra",  "sp",  "gp", "tp", "t0", "t1", "t2",
    "s0",   "s1",  "a0",  "a1", "a2", "a3", "a4", "a5",
    "a6",   "a7",  "s2",  "s3", "s4", "s5", "s6", "s7",
    "s8",   "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

// for detecting changes
uint32_t prevRegFile[32];
bool changedRegister[32] = {false};

Camera2D uiCam = {0};
bool isUiDirty;
RenderTexture2D uiTexture = {0};
bool isClockUnlocked;
bool nextInstruction = false;


void initWindowUI() {
    InitWindow(WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE, "DreamRV");
    // uiTexture = LoadRenderTexture(WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE);
    uiCam.target = (Vector2){0, 0};      // Top-left corner as origin
    uiCam.offset = (Vector2){0, 0};      // No offset
    uiCam.rotation = 0.0f;
    uiCam.zoom = WINDOW_SCALE;

    buttons = (uiButton_t*) calloc(NBUTTONS, sizeof(uiButton_t));
    textBoxes = (uiTextBox_t*) calloc(NTEXTBOXES, sizeof(uiTextBox_t));
    displayBox = (uiDisplayBox_t*) calloc(1, sizeof(uiDisplayBox_t));
    rectangles = (uiRectangle_t*) calloc(NRECTANGLES, sizeof(uiRectangle_t));

    initFrameBuffer();

    if (!buttons || !textBoxes || !displayBox || !fbTexture || !fbImage) {
        perror("COULD NOT ALLOCATE MEMORY FOR UI ELEMENTS\n");
        exit(1);
    }

    memcpy(prevRegFile, cpuState.regFile, sizeof(prevRegFile));

    initButtons();
    initTextBoxes();
    initDisplayBox();
    initRectangles();

    isUiDirty = true;
    isClockUnlocked = false;
}

void setMemViewStart(uint32_t addr) {
    currentMemPage = addr / (34 * 4);
}

void drawElements() {
    ClearBackground(WHITE);
    drawButtons();
    updateBoxesToCpuState(&textBoxes[0], &textBoxes[1],
        &textBoxes[2], &textBoxes[3], &textBoxes[4],
        &textBoxes[6], &textBoxes[7]);
    drawTextBoxes();
    drawDisplayBox();
    drawRectangles();
}

void drawButtons() {
    for (int i = 0; i < NBUTTONS; i++) {
        uiButton_t* button = &buttons[i];
        if (!button->isEnabled) continue;

        Color buttonColor = button->innerColors.defaultColor;
        Color textColor = button->textColors.defaultColor;
        if (button->isHovered) {
            buttonColor = button->innerColors.hoverColor;
            textColor = button->textColors.hoverColor;
        }
        if (button->isPressed) {
            buttonColor = button->innerColors.pressColor;
            textColor = button->textColors.pressColor;
        }

        DrawRectangle(button->xPos,   button->yPos, button->width, button->height, button->borderColor);
        DrawRectangle(button->xPos + button->borderWidth, button->yPos + button->borderWidth,
                     button->width - 2 * button->borderWidth , button->height - 2 * button->borderWidth,
                            buttonColor);
        DrawText(button->text, button->xPos + button->textHorizPadding,
            button->yPos + button->textVertPadding, button->fontSize, textColor);
    }
}

void drawTextBoxes() {
    for (int i = 0; i < NTEXTBOXES; i++) {
        uiTextBox_t* textBox = &textBoxes[i];
        if (!textBox->isEnabled) continue;

        if (!textBox->skipBackground) {
            DrawRectangle(textBox->xPos, textBox->yPos, textBox->width,
                      textBox->height, textBox->borderColor);
            DrawRectangle(textBox->xPos + textBox->borderWidth, textBox->yPos + textBox->borderWidth,
                          textBox->width - 2 * textBox->borderWidth, textBox->height - 2 * textBox->borderWidth,
                          textBox->innerColor);
        }
        DrawText(textBox->text, textBox->xPos + textBox->textHorizPadding,
                 textBox->yPos + textBox->textVertPadding, textBox->fontSize, textBox->textColor);
    }
}

void drawDisplayBox() {
    uiDisplayBox_t* box = &displayBox[0];

    if (!box->isEnabled) return;

    updateFrameBufferFromMemory();
    UpdateTexture(*box->frameBufferTexture, box->frameBuffer);

    int contentWidth  = box->frameBufferTexture->width * box->scale;
    int contentHeight = box->frameBufferTexture->height * box->scale;
    int border        = box->borderWidth;

    DrawRectangle(box->xPos,
                  box->yPos,
                  contentWidth + (2 * border),
                  contentHeight + (2 * border),
                  box->borderColor);

    Rectangle source = { 0, 0, (float)box->frameBufferTexture->width, (float)box->frameBufferTexture->height };

    Rectangle dest = {
        (float)box->xPos + border,
        (float)box->yPos + border,
        (float)contentWidth,
        (float)contentHeight
    };

    DrawTexturePro(*box->frameBufferTexture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void drawRectangles() {
    for (int i = 0; i < NRECTANGLES; i++) {
        if (!rectangles[i].isEnabled) continue;
        DrawRectangle(rectangles[i].xPos, rectangles[i].yPos, rectangles[i].width,
            rectangles[i].height, rectangles[i].color);
    }
}

void checkMousePresses() {

    Vector2 mouseDelta = GetMouseDelta();
    bool moved = mouseDelta.x != 0 && mouseDelta.y != 0;
    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    if (!moved && !clicked && !released) return;

    isUiDirty = true;

    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), uiCam);
    for (int i = 0; i < NBUTTONS; i++) {
        uiButton_t* button = &buttons[i];

        bool inside = (button->isEnabled && button->xPos <= (int) mousePos.x &&
                       button->yPos <= (int) mousePos.y &&
                       (button->xPos + button->width) >= (int) mousePos.x &&
                       button->yPos + button->height >= (int) mousePos.y);

        button->isHovered = inside;
        button->isPressed = inside && clicked && !released;

        if (inside && clicked && button->pressHandler) {
            button->pressHandler(button);
        } else if (inside && released && button->releaseHandler) {
            button->releaseHandler(button);
        } else if (inside && button->hoverHandler) {
            button->hoverHandler(button);
        }
    }
}

void* defaultPressHandler(void* arg) {
    ((uiButton_t*) arg)->isPressed = true;
}

void* defaultHoverHandler(void* arg) {
    ((uiButton_t*) arg)->isHovered = true;
}

void* defaultReleaseHandler(void* arg) {
    ((uiButton_t*) arg)->isPressed = false;
}

void freeUi() {
    free(buttons);
    buttons = NULL;
    for (int i = 0; i < NTEXTBOXES; i++) {
        if (textBoxes[i].text) free(textBoxes[i].text);
    }
    free(textBoxes);
    textBoxes = NULL;
    UnloadImage(*fbImage);
    fbImage = NULL;
}

void updateBoxesToCpuState(uiTextBox_t* aliasBox, uiTextBox_t* regIndexBox,uiTextBox_t* regValBox,
                           uiTextBox_t* pcBox, uiTextBox_t* currentInstructionBox,
                           uiTextBox_t* memAddrBox, uiTextBox_t* memValBox) {

    // registers
    char aliasBuf[8];
    char regIndexBuf[6];
    char regValBuf[16];

    aliasBox->text[0] = '\0';
    regIndexBox->text[0] = '\0';
    regValBox->text[0] = '\0';

    for (int i = 0; i < 32; i++) { // 32 registers

        snprintf(aliasBuf, sizeof(aliasBuf), "%s\n", registerAliases[i]);
        strcat(aliasBox->text, aliasBuf);

        snprintf(regIndexBuf, 6, "x%d\n", i);
        strcat(regIndexBox->text, regIndexBuf);

        uint32_t regVal = cpuState.regFile[i];

        snprintf(regValBuf, sizeof(regValBuf), "0x%08X\n", regVal);
        strcat(regValBox->text, regValBuf);
    }

    snprintf(currentInstructionBox->text, 50, "Last instruction: 0x%08X", cpuState.currentInstruction);
    snprintf(pcBox->text, 32, "PC          0x%08X", cpuState.PC);

    // memory
    memAddrBox->text[0] = '\0';
    memValBox->text[0] = '\0';

    for (int i = 0; i < 34; i++) {
        uint32_t addr = (currentMemPage * 34 + i) * 4;

        // reusing regValBuf as memAddrBuf
        snprintf(regValBuf, sizeof(regValBuf), "0x%08X\n", addr);
        strcat(memAddrBox->text, regValBuf);

        snprintf(regValBuf, sizeof(regValBuf), "%02X %02X %02X %02X\n",
                 getByte(addr + 3), getByte(addr + 2), getByte(addr + 1), getByte(addr));
        strcat(memValBox->text, regValBuf);
    }

}

void updateChangedRegisters() {
    for (int i = 0; i < 32; i++) {
        changedRegister[i] = false;
        if (cpuState.regFile[i] != prevRegFile[i]) changedRegister[i] = true;
        rectangles[i].isEnabled = changedRegister[i];
    }
    memcpy(prevRegFile, cpuState.regFile, sizeof(cpuState.regFile));
}

void initButtons() {
    // clock button, top corner
    buttons[0] = (uiButton_t) {
        .isEnabled = true,
        .height = 50,
        .width = 150,
        .xPos = 10,
        .yPos = 10,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,

        .innerColors = {
            .defaultColor = WHITE,
            .hoverColor = GREEN,
            .pressColor = RED,
        },

        .pressHandler = nextInstructionHandler,
        .hoverHandler = defaultHoverHandler,
        .releaseHandler = defaultReleaseHandler,
        .isHovered = false,
        .isPressed = false,
        .text = "next cycle",
        .textHorizPadding = 16,
        .textVertPadding = 14,
        .fontSize = 20,

        .textColors = {
            .defaultColor = BLACK,
            .hoverColor = BLUE,
            .pressColor = WHITE,
        }
    };

    buttons[1] = (uiButton_t) {
        .isEnabled = true,
        .height = 50,
        .width = 100,
        .xPos = 170,
        .yPos = 10,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,

        .innerColors = {
            .defaultColor = WHITE,
            .hoverColor = GREEN,
            .pressColor = RED,
        },

        .pressHandler = resetHandler,
        .hoverHandler = defaultHoverHandler,
        .releaseHandler = defaultReleaseHandler,
        .isHovered = false,
        .isPressed = false,
        .text = "reset",
        .textHorizPadding = 20,
        .textVertPadding = 14,
        .fontSize = 20,

        .textColors = {
            .defaultColor = BLACK,
            .hoverColor = BLUE,
            .pressColor = WHITE,
        }
    };

    buttons[2] = (uiButton_t) {
        .isEnabled = true,
        .height = 50,
        .width = 180,
        .xPos = 280,
        .yPos = 10,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,

        .innerColors = {
            .defaultColor = WHITE,
            .hoverColor = GREEN,
            .pressColor = RED,
        },

        .pressHandler = unlockClockHandler,
        .hoverHandler = defaultHoverHandler,
        .releaseHandler = defaultReleaseHandler,
        .isHovered = false,
        .isPressed = false,
        .text = "Run fullspeed",
        .textHorizPadding = 20,
        .textVertPadding = 14,
        .fontSize = 20,

        .textColors = {
            .defaultColor = BLACK,
            .hoverColor = BLUE,
            .pressColor = WHITE,
        }
    };

    buttons[3] = (uiButton_t) {
        .isEnabled = true,
        .height = 50,
        .width = 150,
        .xPos = 470,
        .yPos = 10,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,

        .innerColors = {
            .defaultColor = WHITE,
            .hoverColor = GREEN,
            .pressColor = RED,
        },

        .pressHandler = lockClockHandler,
        .hoverHandler = defaultHoverHandler,
        .releaseHandler = defaultReleaseHandler,
        .isHovered = false,
        .isPressed = false,
        .text = "Stop clock",
        .textHorizPadding = 20,
        .textVertPadding = 14,
        .fontSize = 20,

        .textColors = {
            .defaultColor = BLACK,
            .hoverColor = BLUE,
            .pressColor = WHITE,
        }
    };

    buttons[4] = (uiButton_t) {
        .isEnabled = true,
           .height = 50,
           .width = 80,
           .xPos = 1110,
           .yPos = 10,
           .borderWidth = 4,
           .borderColor =  DARKGRAY,

           .innerColors = {
            .defaultColor = WHITE,
            .hoverColor = GREEN,
            .pressColor = RED,
        },

        .pressHandler = prevMemPageHandler,
        .hoverHandler = defaultHoverHandler,
        .releaseHandler = defaultReleaseHandler,
        .isHovered = false,
        .isPressed = false,
        .text = "<--",
        .textHorizPadding = 25,
        .textVertPadding = 14,
        .fontSize = 20,

        .textColors = {
            .defaultColor = BLACK,
            .hoverColor = BLUE,
            .pressColor = WHITE,
        }
    };

    buttons[5] = (uiButton_t) {
        .isEnabled = true,
           .height = 50,
           .width = 80,
           .xPos = 1190,
           .yPos = 10,
           .borderWidth = 4,
           .borderColor =  DARKGRAY,

           .innerColors = {
            .defaultColor = WHITE,
            .hoverColor = GREEN,
            .pressColor = RED,
        },

        .pressHandler = nextMemPageHandler,
        .hoverHandler = defaultHoverHandler,
        .releaseHandler = defaultReleaseHandler,
        .isHovered = false,
        .isPressed = false,
        .text = "-->",
        .textHorizPadding = 25,
        .textVertPadding = 14,
        .fontSize = 20,

        .textColors = {
            .defaultColor = BLACK,
            .hoverColor = BLUE,
            .pressColor = WHITE,
        }
    };
}

void initTextBoxes() {
    if (textBoxes[0].text != NULL) return; // already initialized

    // REGISTER ALIAS TEXT BOX. ACTS AS THE MASTER ONE, AS IT HAS SHAPES
    char* box0TextBuffer = calloc(sizeof(char), MAX_TEXT_LEN);
    box0TextBuffer[0] = '\0';
    textBoxes[0] = (uiTextBox_t) {
        .isEnabled = true,
        .skipBackground = false,
        .xPos = 10,
        .yPos = 120, // 90
        .width = 260,
        .height = 730,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .innerColor  =  WHITE,
        .text = box0TextBuffer,
        .textHorizPadding = 18,
        .textVertPadding = 14,
        .fontSize = 20,
        .textColor = BLACK
    };

    // REGISTER NAMES TEXT BOX. BORDERS NOT VISIBLE
    char* box1TextBuffer = calloc(sizeof(char), MAX_TEXT_LEN);
    box1TextBuffer[0] = '\0';
    textBoxes[1] = (uiTextBox_t) {
        .isEnabled = true,
        .skipBackground = true,
        .xPos = 70,
        .yPos = textBoxes[0].yPos,
        .width = 200,
        .height = 640,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .innerColor  =  WHITE,
        .text = box1TextBuffer,
        .textHorizPadding = 18,
        .textVertPadding = 14,
        .fontSize = textBoxes[0].fontSize,
        .textColor = BLACK
    };

    // REGISTER VALUE BOX
    char* box2TextBuffer = calloc(sizeof(char), MAX_TEXT_LEN);
    box2TextBuffer[0] = '\0';
    textBoxes[2] = (uiTextBox_t) {
        .isEnabled = true,
        .skipBackground = true,
        .xPos = 120,
        .yPos = textBoxes[0].yPos,
        .width = 200,
        .height = 640,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .innerColor  =  WHITE,
        .text = box2TextBuffer,
        .textHorizPadding = 18,
        .textVertPadding = 14,
        .fontSize = textBoxes[0].fontSize,
        .textColor = BLACK
    };


    // PC TEXT BOX
    char* box3TextBuffer = calloc(sizeof(char), MAX_TEXT_LEN);
    box3TextBuffer[0] = '\0';
    textBoxes[3] = (uiTextBox_t) {
        .isEnabled = true,
        .skipBackground = false,
        .xPos = 10,
        .yPos = 70,
        .width = 260,
        .height = 50,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .innerColor  =  WHITE,
        .text = box3TextBuffer,
        .textHorizPadding = 18,
        .textVertPadding = 16,
        .fontSize = 20,
        .textColor = BLACK
    };

    // CURRENT INSTRUCTION TEXT BOX
    char* box4TextBuffer = calloc(sizeof(char), MAX_TEXT_LEN);
    box4TextBuffer[0] = '\0';
    textBoxes[4] = (uiTextBox_t) {
        .isEnabled = true,
        .skipBackground = false,
        .xPos = 280,
        .yPos = 70,
        .width = 340,
        .height = 50,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .innerColor  =  WHITE,
        .text = box4TextBuffer,
        .textHorizPadding = 18,
        .textVertPadding = 16,
        .fontSize = 20,
        .textColor = BLACK
    };

    // MEMORY TAB TITLE TEXT BOX
    char* box5TextBuffer = calloc(sizeof(char), MAX_TEXT_LEN);
    box5TextBuffer[0] = '\0';
    strcpy(box5TextBuffer, "Memory view");
    textBoxes[5] = (uiTextBox_t) {
        .isEnabled = true,
        .skipBackground = false,
        .xPos = 940,
        .yPos = 10,
        .width = 160,
        .height = 50,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .innerColor  =  WHITE,
        .text = box5TextBuffer,
        .textHorizPadding = 18,
        .textVertPadding = 16,
        .fontSize = 20,
        .textColor = BLACK
    };

    // MEMORY VIEW SQUARE AND ADDRESSES
    char* box6TextBuffer = calloc(sizeof(char), MAX_TEXT_LEN);
    box6TextBuffer[0] = '\0';

    textBoxes[6] = (uiTextBox_t) (uiTextBox_t) {
        .isEnabled = true,
        .skipBackground = false,
        .xPos = 940,
        .yPos = 70,
        .width = 330,
        .height = 776,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .innerColor  =  WHITE,
        .text = box6TextBuffer,
        .textHorizPadding = 18,
        .textVertPadding = 16,
        .fontSize = 20,
        .textColor = BLACK
    };

    // MEMORY VALUES
    char* box7TextBuffer = calloc(sizeof(char), MAX_TEXT_LEN);
    box7TextBuffer[0] = '\0';

    textBoxes[7] = (uiTextBox_t) (uiTextBox_t) {
        .isEnabled = true,
        .skipBackground = true,
        .xPos = 1100,
        .yPos = 70,
        .width = 330,
        .height = 776,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .innerColor  =  WHITE,
        .text = box7TextBuffer,
        .textHorizPadding = 18,
        .textVertPadding = 16,
        .fontSize = 20,
        .textColor = BLACK
    };


}

void initDisplayBox() {

    *fbImage = (Image) {
        .data = frameBuffer,
        .width = FB_H_RES,
        .height = FB_V_RES,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    *fbTexture = LoadTextureFromImage(*fbImage);

    displayBox[0] = (uiDisplayBox_t) {
        .xPos = 280,
        .yPos = 200,
        .scale = 5,
        .isEnabled = true,
        .borderWidth = 4,
        .borderColor =  DARKGRAY,
        .frameBuffer = frameBuffer,
        .frameBufferImage = fbImage,
        .frameBufferTexture = fbTexture,
    };


}

void initRectangles() {
    if (NRECTANGLES < 32) return; // just in case?

    int baseY = textBoxes[0].yPos + textBoxes[0].textVertPadding;
    int lineHeight = textBoxes[0].fontSize + 2;

    for (int i = 0; i < 32; i++) {
        rectangles[i] = (uiRectangle_t) {
            .isEnabled = false,
            .xPos = 18,
            .yPos = baseY + i * lineHeight,
            .width = 6,
            .height = 14,
            .color = GREEN
        };
    }
}

void* nextInstructionHandler(void* arg) {
    nextInstruction = true;
    return NULL;
}

void* resetHandler(void* arg) {
    initCPU(0x00);
    cpuState.currentInstruction = 0;
    isClockUnlocked = false;
    clearMemory();
    return NULL;
}

void* unlockClockHandler(void* arg) {
    isClockUnlocked = true;
    return NULL;
}

void* lockClockHandler(void* arg) {
    isClockUnlocked = false;
    return NULL;
}

void* prevMemPageHandler(void* arg) {
    if (currentMemPage > 0) currentMemPage --;
    return NULL;
}

void* nextMemPageHandler(void* arg) {
    currentMemPage ++;
    return NULL;
}