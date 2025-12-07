//
// Created by sergi on 26/11/2025.
//

#include "RV32M.h"

RV32_executionReturn_t fun_mul(RV32_decodedInstruction_t inst) {
    int32_t a = (int32_t) getRegister(cpuState, inst.rs1);
    int32_t b = (int32_t) getRegister(cpuState, inst.rs2);
    int32_t res = a * b;

    writeRegister(&cpuState, inst.rd, (uint32_t)res);
    RV32RET(1);
}

RV32_executionReturn_t fun_mulh(RV32_decodedInstruction_t inst) {
    int64_t a = (int64_t)(int32_t)getRegister(cpuState, inst.rs1);
    int64_t b = (int64_t)(int32_t)getRegister(cpuState, inst.rs2);

    int64_t res = a * b;
    uint32_t high = (uint32_t)(res >> 32);

    writeRegister(&cpuState, inst.rd, high);
    RV32RET(1);
}

RV32_executionReturn_t fun_mulhsu(RV32_decodedInstruction_t inst) {
    int64_t  a = (int64_t)(int32_t)getRegister(cpuState, inst.rs1);
    uint64_t b = (uint64_t)getRegister(cpuState, inst.rs2);

    // DANGEROUS! COMPILER DEPENDENT
    __uint128_t res = (__uint128_t)a * (__uint128_t)b;
    uint32_t high = (uint32_t)(res >> 32);

    writeRegister(&cpuState, inst.rd, high);
    RV32RET(1);
}

RV32_executionReturn_t fun_mulhu(RV32_decodedInstruction_t inst) {
    uint64_t a = (uint64_t)getRegister(cpuState, inst.rs1);
    uint64_t b = (uint64_t)getRegister(cpuState, inst.rs2);

    uint64_t res = a * b;
    uint32_t high = (uint32_t)(res >> 32);

    writeRegister(&cpuState, inst.rd, high);
    RV32RET(1);
}

RV32_executionReturn_t fun_div(RV32_decodedInstruction_t inst) {
    int32_t a = (int32_t) getRegister(cpuState, inst.rs1);
    int32_t b = (int32_t) getRegister(cpuState, inst.rs2);
    int32_t res;

    // corner cases
    if (b == 0)   res = -1;
    else if (a == INT32_MIN && b == -1) res = INT32_MIN;
    else res = a / b;

    writeRegister(&cpuState, inst.rd, (uint32_t)res);
    RV32RET(1);
}

RV32_executionReturn_t fun_divu(RV32_decodedInstruction_t inst) {
    uint32_t a = getRegister(cpuState, inst.rs1);
    uint32_t b = getRegister(cpuState, inst.rs2);
    uint32_t res;

    // corner cases
    if (b == 0)   res = 0xFFFFFFFF;
    else if (a == INT32_MIN && b == -1) res = INT32_MIN;
    else res = a / b;

    writeRegister(&cpuState, inst.rd, res);
    RV32RET(1);
}

RV32_executionReturn_t fun_rem(RV32_decodedInstruction_t inst) {
    int32_t a = (int32_t) getRegister(cpuState, inst.rs1);
    int32_t b = (int32_t) getRegister(cpuState, inst.rs2);
    int32_t res;

    // corner cases
    if (b == 0) res = a;
    else if (a == INT32_MIN && b == -1) res = 0;
    else res = a % b;

    writeRegister(&cpuState, inst.rd, (uint32_t)res);
    RV32RET(1);
}

RV32_executionReturn_t fun_remu(RV32_decodedInstruction_t inst) {
    uint32_t a = getRegister(cpuState, inst.rs1);
    uint32_t b = getRegister(cpuState, inst.rs2);
    uint32_t res;

    // corner cases
    if (b == 0) res = a;
    else res = a % b;

    writeRegister(&cpuState, inst.rd, res);
    RV32RET(1);
}