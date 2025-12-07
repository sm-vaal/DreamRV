//
// Created by sergi on 26/11/2025.
//

#include "RV32_I.h"


RV32_executionReturn_t fun_default(RV32_decodedInstruction_t inst) {
    printError(WARNING, "tried to run an invalid instruction");
    RV32RET(0);
}

// I type
RV32_executionReturn_t fun_lb(RV32_decodedInstruction_t inst) {
    int32_t  imm  = (int32_t) inst.I_imm;
    uint32_t base = getRegister(cpuState, inst.rs1);
    uint32_t addr = (uint32_t) imm + base;
    uint32_t mem  = (uint32_t) getByte(addr);
    mem = (uint32_t) (((int32_t) mem << 24) >> 24); // sign ext
    writeRegister(&cpuState, inst.rd, mem);
    RV32RET(1);
}

RV32_executionReturn_t fun_lh(RV32_decodedInstruction_t inst) {
    int32_t  imm  = (int32_t) inst.I_imm;
    uint32_t base = getRegister(cpuState, inst.rs1);
    uint32_t addr = (uint32_t) imm + base;
    uint32_t mem  = (uint32_t) getHalf(addr);
    mem = (uint32_t) (((int32_t) mem << 16) >> 16); // sign ext
    writeRegister(&cpuState, inst.rd, mem);
    RV32RET(1);
}

RV32_executionReturn_t fun_lw(RV32_decodedInstruction_t inst) {
    int32_t  imm  = (int32_t) inst.I_imm;
    uint32_t base = getRegister(cpuState, inst.rs1);
    uint32_t addr = (uint32_t) imm + base;
    uint32_t mem  = (uint32_t) getWord(addr);
    writeRegister(&cpuState, inst.rd, mem);
    RV32RET(1);
}

RV32_executionReturn_t fun_lbu(RV32_decodedInstruction_t inst) {
    int32_t  imm  = (int32_t) inst.I_imm;
    uint32_t base = getRegister(cpuState, inst.rs1);
    uint32_t addr = (uint32_t) imm + base;
    uint32_t mem  = (uint32_t) getByte(addr);
    writeRegister(&cpuState, inst.rd, mem);
    RV32RET(1);
}

RV32_executionReturn_t fun_lhu(RV32_decodedInstruction_t inst) {
    int32_t  imm  = (int32_t) inst.I_imm;
    uint32_t base = getRegister(cpuState, inst.rs1);
    uint32_t addr = (uint32_t) imm + base;
    uint32_t mem  = (uint32_t) getHalf(addr);
    writeRegister(&cpuState, inst.rd, mem);
    RV32RET(1);
}

RV32_executionReturn_t fun_addi(RV32_decodedInstruction_t inst) {
    int32_t  imm = (int32_t) inst.I_imm; // already sign extended in decode
    uint32_t readVal = getRegister(cpuState, inst.rs1); // unsigned
    int32_t  writeVal = imm + (int32_t) readVal;
    writeRegister(&cpuState, inst.rd, (uint32_t) writeVal);
    RV32RET(1);
}

RV32_executionReturn_t fun_slli(RV32_decodedInstruction_t inst) {
    uint32_t uimm = ((uint32_t) inst.I_imm << 20) >> 20; // remove sign bits
    uimm &= 0x1F; // lower 5  bits
    uint32_t readVal = getRegister(cpuState, inst.rs1);
    uint32_t writeVal = readVal << uimm;
    writeRegister(&cpuState, inst.rd, writeVal);
    RV32RET(1);
}

RV32_executionReturn_t fun_slti(RV32_decodedInstruction_t inst) {
    int32_t  imm = inst.I_imm;
    int32_t  readVal = (int32_t) getRegister(cpuState, inst.rs1);
    uint32_t writeVal = readVal < imm ? 1 : 0;
    writeRegister(&cpuState, inst.rd, writeVal);
    RV32RET(1);

}

RV32_executionReturn_t fun_sltiu(RV32_decodedInstruction_t inst) {
    uint32_t uimm = ((uint32_t) inst.I_imm << 20) >> 20; // rm sign bits
    uint32_t readVal = getRegister(cpuState, inst.rs1);
    uint32_t writeVal = readVal < uimm ? 1 : 0;
    writeRegister(&cpuState, inst.rd, writeVal);
    RV32RET(1);
}

RV32_executionReturn_t fun_xori(RV32_decodedInstruction_t inst) {
    int32_t  imm = inst.I_imm;
    int32_t  readVal = (int32_t) getRegister(cpuState, inst.rs1);
    uint32_t writeVal = readVal ^ imm;
    writeRegister(&cpuState, inst.rd, writeVal);
    RV32RET(1);
}

RV32_executionReturn_t fun_srli(RV32_decodedInstruction_t inst) {
    uint32_t uimm = ((uint32_t) inst.I_imm << 20) >> 20; // remove sign bits
    uimm &= 0x1F; // lower 5  bits
    uint32_t readVal = getRegister(cpuState, inst.rs1);
    uint32_t writeVal = readVal >> uimm;
    writeRegister(&cpuState, inst.rd, writeVal);
    RV32RET(1);
}

RV32_executionReturn_t fun_srai(RV32_decodedInstruction_t inst) {
    uint32_t uimm = ((uint32_t) inst.I_imm << 20) >> 20; // remove sign bits
    uimm &= 0x1F; // lower 5  bits
    int32_t readVal = (int32_t) getRegister(cpuState, inst.rs1);
    int32_t writeVal = readVal >> uimm;
    writeRegister(&cpuState, inst.rd, (uint32_t) writeVal);
    RV32RET(1);
}

RV32_executionReturn_t fun_ori(RV32_decodedInstruction_t inst) {
    int32_t  imm = inst.I_imm;
    int32_t  readVal = (int32_t) getRegister(cpuState, inst.rs1);
    uint32_t writeVal = readVal | imm;
    writeRegister(&cpuState, inst.rd, writeVal);
    RV32RET(1);
}

RV32_executionReturn_t fun_andi(RV32_decodedInstruction_t inst) {
    int32_t  imm = inst.I_imm;
    int32_t  readVal = (int32_t) getRegister(cpuState, inst.rs1);
    uint32_t writeVal = readVal & imm;
    writeRegister(&cpuState, inst.rd, writeVal);
    RV32RET(1);
}

RV32_executionReturn_t fun_jalr(RV32_decodedInstruction_t inst) {
    // instead of the default 4, the offset is added
    cpuState.addToPc = ((int32_t) getRegister(cpuState, inst.rs1) + inst.I_imm) - cpuState.PC;
    cpuState.addToPc &= ~0x01; // mask out the LSB, only allow 2 byte alignment
    writeRegister(&cpuState, inst.rd, cpuState.PC + 4);
    RV32RET(1);
}

RV32_executionReturn_t fun_ecall(RV32_decodedInstruction_t inst) {
    // TODO: add control transfer to the OS
    RV32RET(1);
}

// probably unused later, but part of the ISA and still I type
RV32_executionReturn_t fun_ebreak(RV32_decodedInstruction_t inst) {
    // TODO: pretty obvious
    RV32RET(1);
}
RV32_executionReturn_t fun_fence(RV32_decodedInstruction_t inst) {
    // unimplemented here, no need
    RV32RET(1);
}


// R type
RV32_executionReturn_t fun_add(RV32_decodedInstruction_t inst) {
    uint32_t result = getRegister(cpuState, inst.rs1) + getRegister(cpuState, inst.rs2);
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

RV32_executionReturn_t fun_sub(RV32_decodedInstruction_t inst) {
    uint32_t result = getRegister(cpuState, inst.rs1) - getRegister(cpuState, inst.rs2);
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

RV32_executionReturn_t fun_sll(RV32_decodedInstruction_t inst) {
    uint8_t shamt   = getRegister(cpuState, inst.rs2) & 0x1F; // 5 lsb
    uint32_t result = getRegister(cpuState, inst.rs1) << shamt;
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

RV32_executionReturn_t fun_slt(RV32_decodedInstruction_t inst) {
    uint32_t result = ((int32_t) getRegister(cpuState, inst.rs1) <
                       (int32_t) getRegister(cpuState, inst.rs2) ? 1 : 0);
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

RV32_executionReturn_t fun_sltu(RV32_decodedInstruction_t inst) {
    uint32_t result = (getRegister(cpuState, inst.rs1) <
                       getRegister(cpuState, inst.rs2) ? 1 : 0);
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

RV32_executionReturn_t fun_xor(RV32_decodedInstruction_t inst) {
    uint32_t result = getRegister(cpuState, inst.rs1) ^ getRegister(cpuState, inst.rs2);
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

RV32_executionReturn_t fun_srl(RV32_decodedInstruction_t inst) {
    uint8_t shamt   = getRegister(cpuState, inst.rs2) & 0x1F; // 5 lsb
    uint32_t result = getRegister(cpuState, inst.rs1) >> shamt;
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

RV32_executionReturn_t fun_sra(RV32_decodedInstruction_t inst) {
    uint8_t shamt   = getRegister(cpuState, inst.rs2) & 0x1F; // 5 lsb
    int32_t result  = (int32_t) getRegister(cpuState, inst.rs1) >> shamt;
    writeRegister(&cpuState, inst.rd, (uint32_t) result);
    RV32RET(1);
}

RV32_executionReturn_t fun_or(RV32_decodedInstruction_t inst) {
    uint32_t result = getRegister(cpuState, inst.rs1) | getRegister(cpuState, inst.rs2);
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

RV32_executionReturn_t fun_and(RV32_decodedInstruction_t inst) {
    uint32_t result = getRegister(cpuState, inst.rs1) & getRegister(cpuState, inst.rs2);
    writeRegister(&cpuState, inst.rd, result);
    RV32RET(1);
}

// S type
RV32_executionReturn_t fun_sb(RV32_decodedInstruction_t inst) {
    int32_t  imm  = (int32_t) inst.S_imm;
    uint32_t reg  = getRegister(cpuState, inst.rs1);
    uint32_t addr = (uint32_t) ((int32_t) reg + imm);
    writeByte(addr, getRegister(cpuState, inst.rs2) & 0xFF);
    RV32RET(1);
}

RV32_executionReturn_t fun_sh(RV32_decodedInstruction_t inst) {
    int32_t  imm  = (int32_t) inst.S_imm;
    uint32_t reg  = getRegister(cpuState, inst.rs1);
    uint32_t addr = (uint32_t) ((int32_t) reg + imm);
    writeHalf(addr, getRegister(cpuState, inst.rs2) & 0xFFFF);
    RV32RET(1);

}
RV32_executionReturn_t fun_sw(RV32_decodedInstruction_t inst) {
    int32_t  imm  = (int32_t) inst.S_imm;
    uint32_t reg  = getRegister(cpuState, inst.rs1);
    uint32_t addr = (uint32_t) ((int32_t) reg + imm);
    writeWord(addr, getRegister(cpuState, inst.rs2));
    RV32RET(1);
}

// B type
RV32_executionReturn_t fun_beq(RV32_decodedInstruction_t inst) {
    uint8_t cond = (int32_t) getRegister(cpuState, inst.rs1) ==
                   (int32_t) getRegister(cpuState, inst.rs2);
    if (cond) cpuState.addToPc = (int32_t) inst.B_imm;
    RV32RET(1);
}

RV32_executionReturn_t fun_bne(RV32_decodedInstruction_t inst) {
    uint8_t cond = (int32_t) getRegister(cpuState, inst.rs1) !=
                   (int32_t) getRegister(cpuState, inst.rs2);
    if (cond) cpuState.addToPc = (int32_t) inst.B_imm;
    RV32RET(1);
}

RV32_executionReturn_t fun_blt(RV32_decodedInstruction_t inst) {
    uint8_t cond = (int32_t) getRegister(cpuState, inst.rs1) <
                   (int32_t) getRegister(cpuState, inst.rs2);
    if (cond) cpuState.addToPc = (int32_t) inst.B_imm;
    RV32RET(1);
}
RV32_executionReturn_t fun_bge(RV32_decodedInstruction_t inst) {
    uint8_t cond = (int32_t) getRegister(cpuState, inst.rs1) >=
                   (int32_t) getRegister(cpuState, inst.rs2);
    if (cond) cpuState.addToPc = (int32_t) inst.B_imm;
    RV32RET(1);
}

RV32_executionReturn_t fun_bltu(RV32_decodedInstruction_t inst) {
    uint8_t cond = getRegister(cpuState, inst.rs1) <
                   getRegister(cpuState, inst.rs2);
    if (cond) cpuState.addToPc = (int32_t) inst.B_imm;
    RV32RET(1);
}

RV32_executionReturn_t fun_bgeu(RV32_decodedInstruction_t inst) {
    uint8_t cond = getRegister(cpuState, inst.rs1) >=
                   getRegister(cpuState, inst.rs2);
    if (cond) cpuState.addToPc = (int32_t) inst.B_imm;
    RV32RET(1);
}

// U type
RV32_executionReturn_t fun_auipc(RV32_decodedInstruction_t inst) {
    // U_imm lsb already zeroed in decode
    writeRegister(&cpuState, inst.rd, inst.U_imm + cpuState.PC);
    RV32RET(1);
}

RV32_executionReturn_t fun_lui(RV32_decodedInstruction_t inst) {
    writeRegister(&cpuState, inst.rd, inst.U_imm);
    RV32RET(1);
}

// J type
RV32_executionReturn_t fun_jal(RV32_decodedInstruction_t inst) {
    cpuState.addToPc = (int32_t) inst.J_imm;
    cpuState.addToPc &= ~0x01; // mask out the LSB, only allow 2 byte alignment
    writeRegister(&cpuState, inst.rd, cpuState.PC + 4);
    RV32RET(1);
}