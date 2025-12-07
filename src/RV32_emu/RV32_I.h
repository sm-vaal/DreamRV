//
// Created by sergi on 25/11/2025.
//

#ifndef RV32I_PICO8_INSTRUCTIONFUNCTIONS_H
#define RV32I_PICO8_INSTRUCTIONFUNCTIONS_H

#include "RV32_core.h"
#include "memory.h"
#include "../debug.h"


RV32_executionReturn_t fun_default(RV32_decodedInstruction_t inst);

// I type
RV32_executionReturn_t fun_lb(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_lh(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_lw(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_lbu(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_lhu(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_addi(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_slli(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_slti(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_sltiu(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_xori(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_srli(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_srai(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_ori(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_andi(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_jalr(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_ecall(RV32_decodedInstruction_t inst);

// probably unused later, but part of the ISA and still I type
RV32_executionReturn_t fun_ebreak(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_fence(RV32_decodedInstruction_t inst);


// R type
RV32_executionReturn_t fun_add(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_sub(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_sll(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_slt(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_sltu(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_xor(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_srl(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_sra(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_or(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_and(RV32_decodedInstruction_t inst);

// S type
RV32_executionReturn_t fun_sb(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_sh(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_sw(RV32_decodedInstruction_t inst);

// B type
RV32_executionReturn_t fun_beq(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_bne(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_blt(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_bge(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_bltu(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_bgeu(RV32_decodedInstruction_t inst);

// U type
RV32_executionReturn_t fun_auipc(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_lui(RV32_decodedInstruction_t inst);

// J type
RV32_executionReturn_t fun_jal(RV32_decodedInstruction_t inst);




#endif //RV32I_PICO8_INSTRUCTIONFUNCTIONS_H