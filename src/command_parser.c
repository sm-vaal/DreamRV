//
// Created by sergi on 07/12/2025.
//

#include "command_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

parsedArguments_t parseArguments(int argc, char* argv[]) {

    parsedArguments_t parsedArguments = {
        .isScreenOnlyMode = false,
        .skipProgramLoad = false,
        .clockFreq = 1000000,
        .memPage = 0,
        .executablePath = NULL,
    };

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') { // -x option
            char* opt = argv[i] + 1; // remove '-'
            while (*opt != '\0') {
                switch (*opt) {
                    case 'f': case 'F':
                        parsedArguments.skipProgramLoad = true;
                        break;

                    case 's': case 'S':
                        parsedArguments.isScreenOnlyMode = true;
                        break;

                    case 'c': case 'C': {
                        char* arg = NULL;
                        if (*(opt + 1) != '\0') { // -c100
                            arg = opt + 1;
                            opt += strlen(opt + 1); // skip rest
                        } else if (i + 1 < argc) { // -c 100
                            arg = argv[++i];
                        } else {
                            fprintf(stderr, "-c [Clock] requires a value\n");
                            exit(1);
                        }

                        errno = 0;
                        long val = strtol(arg, NULL, 0);
                        if (errno != 0 || val < 0) {
                            fprintf(stderr, "Invalid value for -c: %s\n", arg);
                            exit(1);
                        }
                        parsedArguments.clockFreq = (uint32_t)val;
                        break;
                    }

                    case 'm': case 'M': {
                        char* arg = NULL;
                        if (*(opt + 1) != '\0') { // -p10
                            arg = opt + 1;
                            opt += strlen(opt + 1);
                        } else if (i + 1 < argc) { // -p 10
                            arg = argv[++i];
                        } else {
                            fprintf(stderr, "-m [memory page] requires a value\n");
                            exit(1);
                        }

                        errno = 0;
                        long val = strtol(arg, NULL, 0);
                        if (errno != 0 || val < 0) {
                            fprintf(stderr, "Invalid value for -m: %s\n", arg);
                            exit(1);
                        }
                        parsedArguments.memPage = (uint32_t)val;
                        break;
                    }

                    default:
                        fprintf(stderr, "Unknown option -%c\n", *opt);
                        exit(1);
                }
                opt++; // move to next char for combined flags -sfm
            }
        } else {
            parsedArguments.executablePath = argv[i];
        }
    }
    return parsedArguments;
}