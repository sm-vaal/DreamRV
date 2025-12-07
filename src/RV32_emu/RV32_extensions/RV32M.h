//
// Created by sergi on 26/11/2025.
//

#ifndef RV32I_PICO8_RV32M_H
#define RV32I_PICO8_RV32M_H

#include "../RV32_core.h"

// all R type
RV32_executionReturn_t fun_mul(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_mulh(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_mulhsu(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_mulhu(RV32_decodedInstruction_t inst);

RV32_executionReturn_t fun_div(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_divu(RV32_decodedInstruction_t inst);

RV32_executionReturn_t fun_rem(RV32_decodedInstruction_t inst);
RV32_executionReturn_t fun_remu(RV32_decodedInstruction_t inst);

#endif //RV32I_PICO8_RV32M_H