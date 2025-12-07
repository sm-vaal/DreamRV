//
// Created by sergi on 29/10/2025.
//

#ifndef SPACEGAMEPROJECT_DEBUG_H
#define SPACEGAMEPROJECT_DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef enum {
    INFO = 0,
    WARNING = 1,
    FATAL = 2,
} errorLevel_t;

void printError(errorLevel_t level, const char *format, ...);
void initDebugFile();
void printToDebugFile(const char *format, ...);

#endif //SPACEGAMEPROJECT_DEBUG_H