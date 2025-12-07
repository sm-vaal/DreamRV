//
// Created by sergi on 07/12/2025.
//

#ifndef DREAMRV_COMMAND_PARSER_H
#define DREAMRV_COMMAND_PARSER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool skipProgramLoad;       // -f
    bool isScreenOnlyMode;      // -s
    char* executablePath;       // just filename
    uint32_t clockFreq;         // -c
    uint32_t memPage;           // -m
} parsedArguments_t;

parsedArguments_t parseArguments(int argc, char* argv[]);

#endif //DREAMRV_COMMAND_PARSER_H

