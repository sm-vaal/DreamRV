//
// Created by sergi on 25/11/2025.
//

#ifndef RV32I_PICO8_RV32I_CORE_H
#define RV32I_PICO8_RV32I_CORE_H

#include <stdint.h>
#include "../debug.h"

#define FUNCT7_MAX 0x80
#define FUNCT3_MAX 0x08
#define OPCODE_MAX 0x80

#define RV32RET(x) return (RV32_executionReturn_t) {x}

typedef uint32_t reg32_t;

typedef struct {
    reg32_t  regFile[32];
    uint32_t PC;
    int32_t  addToPc; // 4 by default
    uint32_t currentInstruction;
} RV32_cpuState_t;

typedef uint32_t RV32_instruction_t;

typedef enum {
    R_inst,
    I_inst,
    S_inst,
    B_inst,
    U_inst,
    J_inst,
    INVALID_inst
} RV32_instructionFormat_t;

typedef enum {
    // R TYPE
    OP_REG    = 0b0110011, // 51
    // I TYPE
    OP_LOAD   = 0b0000011, // 3
    OP_IMM    = 0b0010011, // 19
    OP_JALR   = 0b1100111, // 103
    OP_ECALL  = 0b1110011, // 115
    // S TYPE
    OP_STORE  = 0b0100011, // 35
    // B TYPE
    OP_BRANCH = 0b1100011, // 99
    // U TYPE
    OP_AUIPC  = 0b0010111, // 23
    OP_LUI    = 0b0110111, // 55
    // J TYPE
    OP_JAL    = 0b1101111  // 111
} RV32_operationType_t;

typedef struct {
    uint8_t isValid;
} RV32_executionReturn_t;

// generic instruction decoding.
typedef struct {
    uint8_t opcode;
    uint8_t rd;
    uint8_t funct3;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct7;
    uint32_t I_imm;
    uint32_t S_imm;
    uint32_t B_imm;
    uint32_t U_imm;
    uint32_t J_imm;
} RV32_decodedInstruction_t;

typedef RV32_executionReturn_t (*RV32_instHandler_t)(RV32_decodedInstruction_t);

typedef struct {
    // For opcodes with no funct3/funct7 needed
    RV32_instHandler_t noSubfunc;
    // For opcodes needing funct3
    RV32_instHandler_t funct3Table[FUNCT3_MAX];
    // Only for OP_REG (funct3 + funct7)
    RV32_instHandler_t funct7Table[FUNCT3_MAX][FUNCT7_MAX];
} opcodeDispatch_t;

extern RV32_cpuState_t cpuState;

void initCPU(uint32_t PCval);
void setAddToPC(int32_t add);
void fillDispatchTable();

uint32_t getRegister(RV32_cpuState_t cpu, uint8_t index);
void writeRegister(RV32_cpuState_t *cpu, uint8_t index, uint32_t value);

RV32_executionReturn_t decodeAndRun(RV32_instruction_t instruction);
RV32_instructionFormat_t getInstructionFormat(RV32_instruction_t instruction);
RV32_operationType_t getOperationType(RV32_instruction_t instruction);
RV32_decodedInstruction_t decodeInstruction(RV32_instruction_t instruction);

void printCPU();

#endif //RV32I_PICO8_RV32I_CORE_H