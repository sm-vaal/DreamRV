//
// Created by sergi on 29/10/2025.
//

#include "debug.h"

static FILE *debugFile = NULL;
int debugEnabled = 0;

void printError(errorLevel_t level, const char *format, ...) {
    va_list args;
    va_start(args, format);

    char strBuf[20] = "[DEBUG ";

    switch (level) {
        case INFO: {
            strcat(strBuf, "INFO] ");
            break;
        }

        case WARNING: {
            strcat(strBuf, "WARNING] ");
            break;
        }

        case FATAL: {
            strcat(strBuf, "ERROR] ");
            break;
        }

        default: {
            strcat(strBuf, "] ");
            break;
        }
    }


    printf("%s", strBuf);
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

void initDebugFile() {
    // Open the file in write mode (overwrites old logs)
    debugFile = fopen("debug.log", "w");
    if (!debugFile) {
        perror("Failed to open debug.log");
    }
}

void printToDebugFile(const char *format, ...) {
    if (!debugFile) return; // Avoid crashes if file not initialized
    if (!debugEnabled) return;

    va_list args;
    va_start(args, format);
    vfprintf(debugFile, format, args);
    va_end(args);

    fflush(debugFile); // Ensure logs are written immediately
}