//
// Created by sergi on 04/12/2025.
//

#ifndef RV32I_PICO8_UI_H
#define RV32I_PICO8_UI_H

#include "raylib.h"
#include "stdint.h"

#define TOSTRING2(x) #x
#define TOSTRING(x) TOSTRING2(x)



#define NBUTTONS 16
#define NTEXTBOXES 16
#define NRECTANGLES 64

#define MAX_TEXT_LEN 1024


typedef void* (*uiButtonHandler)(void* arg);

typedef struct {
    Color defaultColor;
    Color hoverColor;
    Color pressColor;
} actionColors_t;

typedef struct {
    bool isEnabled;
    int xPos;
    int yPos;
    int width;
    int height;
    int borderWidth;
    Color borderColor;
    actionColors_t innerColors;
    uiButtonHandler pressHandler;
    uiButtonHandler hoverHandler;
    uiButtonHandler releaseHandler;
    bool isHovered;
    bool isPressed;
    char* text;
    int textHorizPadding;
    int textVertPadding;
    int fontSize;
    actionColors_t textColors;
} uiButton_t;

typedef struct {
    bool isEnabled;
    bool skipBackground; // draw text only
    int xPos;
    int yPos;
    int width;
    int height;
    int borderWidth;
    Color borderColor;
    Color innerColor;
    char* text;
    int textHorizPadding;
    int textVertPadding;
    int fontSize;
    Color textColor;

} uiTextBox_t;

typedef struct {
    bool isEnabled;
    int borderWidth;
    Color borderColor;
    int xPos;
    int yPos;
    int scale;
    Color* frameBuffer;
    Image* frameBufferImage;
    Texture2D* frameBufferTexture;
} uiDisplayBox_t;

typedef struct {
    bool isEnabled;
    int xPos;
    int yPos;
    int width;
    int height;
    Color color;
}uiRectangle_t;

extern Camera2D uiCam;
extern RenderTexture2D uiTexture;
extern bool isUiDirty;
extern bool nextInstruction;
extern bool isClockUnlocked;

void initWindowUI();
void setMemViewStart(uint32_t addr);

void drawButtons();
void drawTextBoxes();
void drawElements();
void drawDisplayBox();
void drawRectangles();

void updateChangedRegisters();

void checkMousePresses();
void* defaultPressHandler(void* arg);
void* defaultHoverHandler(void* arg);
void* defaultReleaseHandler(void* arg);

void* nextInstructionHandler(void* arg);
void* resetHandler(void* arg);
void* unlockClockHandler(void* arg);
void* lockClockHandler(void* arg);
void* prevMemPageHandler(void* arg);
void* nextMemPageHandler(void* arg);

void initButtons();
void initTextBoxes();
void initDisplayBox();
void initRectangles();

void updateBoxesToCpuState(uiTextBox_t* aliasBox, uiTextBox_t* regIndexBox,uiTextBox_t* regValBox,
                           uiTextBox_t* pcBox, uiTextBox_t* currentInstructionBox,
                           uiTextBox_t* memAddrBox, uiTextBox_t* memValBox);
void freeUi();


#endif //RV32I_PICO8_UI_H