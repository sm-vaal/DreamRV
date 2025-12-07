#include <stdio.h>
#include "RV32_emu/RV32_core.h"
#include "RV32_emu/memory.h"
#include "user_interfaces/ui.h"
#include "user_interfaces/input.h"
#include "command_parser.h"
#include "user_interfaces/screen_only.h"

void tickCPU();

int main(int argc, char *argv[]) {
    const int targetFPS       = 60;

    SetTraceLogLevel(LOG_NONE);
    initMemory(0x100000); // 1 MiB
    initCPU(0x00);

    parsedArguments_t parsedArguments = parseArguments(argc, argv);
    uint32_t targetFrequency = parsedArguments.clockFreq;

    bool wasNewCharBufCreated = false; // to free later if needed
    char* romName = parsedArguments.executablePath;

    printf("PARSED ARGUMENTS\n"
           "\tskipLoad: %d\n"
           "\tisScreenOnlyMode: %d\n"
           "\tclockFreq: %d\n"
           "\tmemPage: %d\n"
           "\texecutablePath: %s\n",
           parsedArguments.skipProgramLoad,
           parsedArguments.isScreenOnlyMode,
           parsedArguments.clockFreq,
           parsedArguments.memPage,
           parsedArguments.executablePath
           );

    if (!parsedArguments.skipProgramLoad) {
        if (parsedArguments.executablePath == NULL) {
            wasNewCharBufCreated = true;
            romName = malloc(512 * sizeof(char));
            printf("Enter executable path: program.bin, /folder/program.bin ...\n");
            scanf("%s", romName);
        }

        if (!loadBinary(romName) && errno == EINVAL) {
            printf("FILE DOES NOT EXIST OR ERROR LOADING\n");
            exit(1);
        }

        setMemViewStart(parsedArguments.memPage);
    }



    uint32_t instructionsPerFrame = targetFrequency / targetFPS;
    if (instructionsPerFrame < 1) instructionsPerFrame = 1;

    if (!parsedArguments.isScreenOnlyMode) {
        initWindowUI();
        SetTargetFPS(targetFPS);

        while (!WindowShouldClose()) {
            if (isClockUnlocked) {
                isUiDirty = true;
                for (int i = 0; i < instructionsPerFrame; i++) {
                    tickCPU();
                    updateChangedRegisters();
                }
            }

            else if (nextInstruction) {
                tickCPU();
                updateChangedRegisters();
                nextInstruction = false;
            }

            checkMousePresses();

            BeginDrawing();
            BeginMode2D(uiCam);

            drawElements();

            EndMode2D();
            EndDrawing();
        }

        freeUi();
    } else {
        // TODO: IMPLEMENT SCREEN ONLY MODE
        initWindowScreenOnly();
        SetTargetFPS(targetFPS);
        while (!WindowShouldClose()) {
            for (int i = 0; i < instructionsPerFrame; i++) {
                tickCPU();
            }
            BeginDrawing();
            drawFrameBufferToScreenOnly();
            EndDrawing();
        }
    }

    if (wasNewCharBufCreated) {
        free(romName);
        romName = NULL;
    }

    return 0;
}

void tickCPU() {
    pollInputs();
    cpuState.addToPc = 4; // by default, 4
    RV32_instruction_t instruction = getWord(cpuState.PC);
    cpuState.currentInstruction = instruction;
    if (!decodeAndRun(instruction).isValid) {
        // todo: throw invalid or something, will do later
    }
    cpuState.PC += cpuState.addToPc;
}