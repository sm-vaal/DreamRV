//
// Created by sergi on 18/12/2025.
//

#ifndef DREAMRV_DISASSEMBLER_H
#define DREAMRV_DISASSEMBLER_H

#include "RV32_core.h"

void getDisassembledInstruction(RV32_instruction_t binaryInstruction, char* buf, int bufSize);

void disassembleOpReg(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpLoad(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpImm(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpJalr(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpEcall(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpStore(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpBranch(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpAuipc(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpLui(RV32_decodedInstruction_t decoded, char* buf, int bufSize);

void disassembleOpJal(RV32_decodedInstruction_t decoded, char* buf, int bufSize);


#endif //DREAMRV_DISASSEMBLER_H