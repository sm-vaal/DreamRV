//
// Created by sergi on 25/11/2025.
//

#include "RV32_core.h"
#include "RV32_I.h"
#include "RV32_extensions/RV32M.h"


RV32_cpuState_t cpuState;

// automatically assigned as null by default
opcodeDispatch_t opcodeDispatchTable[OPCODE_MAX];


void initCPU(uint32_t PCval) {
    cpuState.PC = PCval;
    for (int i = 0; i < 32; i++) {
        cpuState.regFile[i] = 0;
    }
    cpuState.addToPc = 4;
    fillDispatchTable();
}

void setAddToPC(int32_t add) {
    cpuState.addToPc = add;
}

RV32_executionReturn_t invalidInstruction(RV32_decodedInstruction_t inst) {
    /*
    printError(WARNING, "Dispatched invalid instruction\nOpcode: %d\n"
                        "rd: %d\nfunct3: %d\nrs1: %d\nrs2: %d\nfunct7: %d\n",
                        inst.opcode, inst.rd, inst.funct3, inst.rs1, inst.rs2, inst.funct7);
    */
    RV32RET(0);
}

void fillDispatchTable() {
    // RV32I instructions ONLY. The rest is for extensions

    // I TYPE
    opcodeDispatchTable[OP_LOAD].funct3Table[0b000]    = fun_lb;
    opcodeDispatchTable[OP_LOAD].funct3Table[0b001]    = fun_lh;
    opcodeDispatchTable[OP_LOAD].funct3Table[0b010]    = fun_lw;
    opcodeDispatchTable[OP_LOAD].funct3Table[0b100]    = fun_lbu;
    opcodeDispatchTable[OP_LOAD].funct3Table[0b101]    = fun_lhu;
    opcodeDispatchTable[OP_IMM].funct3Table[0b000]     = fun_addi;
    opcodeDispatchTable[OP_IMM].funct7Table[0b001][0]  = fun_slli;
    opcodeDispatchTable[OP_IMM].funct3Table[0b010]     = fun_slti;
    opcodeDispatchTable[OP_IMM].funct3Table[0b011]     = fun_sltiu;
    opcodeDispatchTable[OP_IMM].funct3Table[0b100]     = fun_xori;
    opcodeDispatchTable[OP_IMM].funct7Table[0b101][0]  = fun_srli;
    opcodeDispatchTable[OP_IMM].funct7Table[0b101][32] = fun_srai;
    opcodeDispatchTable[OP_IMM].funct3Table[0b110]     = fun_ori;
    opcodeDispatchTable[OP_IMM].funct3Table[0b111]     = fun_andi;
    opcodeDispatchTable[OP_JALR].funct3Table[0b000]    = fun_jalr;
    opcodeDispatchTable[OP_ECALL].funct3Table[0b000]   = fun_ecall;

    // R TYPE
    opcodeDispatchTable[OP_REG].funct7Table[0b000][0]  = fun_add;
    opcodeDispatchTable[OP_REG].funct7Table[0b000][32] = fun_sub;
    opcodeDispatchTable[OP_REG].funct7Table[0b001][0]  = fun_sll;
    opcodeDispatchTable[OP_REG].funct7Table[0b010][0]  = fun_slt;
    opcodeDispatchTable[OP_REG].funct7Table[0b011][0]  = fun_sltu;
    opcodeDispatchTable[OP_REG].funct7Table[0b100][0]  = fun_xor;
    opcodeDispatchTable[OP_REG].funct7Table[0b101][0]  = fun_srl;
    opcodeDispatchTable[OP_REG].funct7Table[0b101][32] = fun_sra;
    opcodeDispatchTable[OP_REG].funct7Table[0b110][0]  = fun_or;
    opcodeDispatchTable[OP_REG].funct7Table[0b111][0]  = fun_and;

    // S TYPE
    opcodeDispatchTable[OP_STORE].funct3Table[0b000]   = fun_sb;
    opcodeDispatchTable[OP_STORE].funct3Table[0b001]   = fun_sh;
    opcodeDispatchTable[OP_STORE].funct3Table[0b010]   = fun_sw;

    // B TYPE
    opcodeDispatchTable[OP_BRANCH].funct3Table[0b000]  = fun_beq;
    opcodeDispatchTable[OP_BRANCH].funct3Table[0b001]  = fun_bne;
    opcodeDispatchTable[OP_BRANCH].funct3Table[0b100]  = fun_blt;
    opcodeDispatchTable[OP_BRANCH].funct3Table[0b101]  = fun_bge;
    opcodeDispatchTable[OP_BRANCH].funct3Table[0b110]  = fun_bltu;
    opcodeDispatchTable[OP_BRANCH].funct3Table[0b111]  = fun_bgeu;

    // U TYPE
    opcodeDispatchTable[OP_AUIPC].noSubfunc            = fun_auipc;
    opcodeDispatchTable[OP_LUI].noSubfunc              = fun_lui;

    // J TYPE
    opcodeDispatchTable[OP_JAL].noSubfunc              = fun_jal;


    // TODO: ADD CONDITIONALS, WHERE EXTENSIONS ARE ADDED TO THE TABLE IF SELECTED
    // M EXTENSION
    opcodeDispatchTable[OP_REG].funct7Table[0b000][1] = fun_mul;
    opcodeDispatchTable[OP_REG].funct7Table[0b001][1] = fun_mulh;
    opcodeDispatchTable[OP_REG].funct7Table[0b010][1] = fun_mulhsu;
    opcodeDispatchTable[OP_REG].funct7Table[0b011][1] = fun_mulhu;
    opcodeDispatchTable[OP_REG].funct7Table[0b100][1] = fun_div;
    opcodeDispatchTable[OP_REG].funct7Table[0b101][1] = fun_divu;
    opcodeDispatchTable[OP_REG].funct7Table[0b110][1] = fun_rem;
    opcodeDispatchTable[OP_REG].funct7Table[0b111][1] = fun_remu;
}

RV32_executionReturn_t decodeAndRun(RV32_instruction_t instruction) {
    RV32_decodedInstruction_t decInst = decodeInstruction(instruction);

    opcodeDispatch_t* func = &opcodeDispatchTable[decInst.opcode];

    if (func->noSubfunc) {
        return func->noSubfunc(decInst);
    }
    if (func->funct3Table[decInst.funct3]) {
        return func->funct3Table[decInst.funct3](decInst);
    }
    if (func->funct7Table[decInst.funct3][decInst.funct7]) {
        return func->funct7Table[decInst.funct3][decInst.funct7](decInst);
    }
    {
        return invalidInstruction(decInst);
    }

}

RV32_instructionFormat_t getInstructionFormat(RV32_instruction_t instruction) {
    uint8_t opcode = instruction & 0x7F; // mask 7 lsb
    switch (opcode) {
        case OP_REG:    return R_inst;

        case OP_LOAD:
        case OP_IMM:
        case OP_JALR:
        case OP_ECALL:  return I_inst;

        case OP_STORE:  return S_inst;

        case OP_BRANCH: return B_inst;

        case OP_AUIPC:
        case OP_LUI:    return U_inst;

        case OP_JAL:    return J_inst;

        default: return INVALID_inst;
    }
}

RV32_operationType_t getOperationType(RV32_instruction_t instruction) {
    uint8_t opcode = instruction & 0x7F;
    return (RV32_operationType_t) opcode;
}

RV32_decodedInstruction_t decodeInstruction(RV32_instruction_t instruction) {
    RV32_decodedInstruction_t ret;

    ret.opcode =  instruction        & 0x7F;
    ret.rd     = (instruction >> 7)  & 0x1F;
    ret.funct3 = (instruction >> 12) & 0x07;
    ret.rs1    = (instruction >> 15) & 0x1F;
    ret.rs2    = (instruction >> 20) & 0x1F;
    ret.funct7 = (instruction >> 25) & 0x7F;

    ret.I_imm  =  ((int32_t) instruction >> 20); // sign extension

    ret.S_imm  = ((instruction >> 7) & 0x1F) |
                 ((instruction >> 25) & 0x7F) << 5;
    ret.S_imm  = ((int32_t) (ret.S_imm << 20)) >> 20; // signext

    ret.B_imm =
        (((instruction >> 31) & 0x1)  << 12) |
        (((instruction >> 25) & 0x3F) << 5)  |
        (((instruction >> 8)  & 0xF)  << 1)  |
        (((instruction >> 7)  & 0x1)  << 11);
    ret.B_imm = ((int32_t) (ret.B_imm << 19) >> 19); // signext

    ret.U_imm = instruction & 0xFFFFF000; // UPPER immediate

    ret.J_imm =
        (((instruction >> 31) & 0x1)   << 20) |
        (((instruction >> 12) & 0xFF)  << 12) |
        (((instruction >> 20) & 0x1)   << 11) |
        (((instruction >> 21) & 0x3FF) << 1);
    ret.J_imm = ((int32_t)(ret.J_imm << 11) >> 11); // signext

    return ret;
}



uint32_t getRegister(RV32_cpuState_t cpu, uint8_t index) {
    if (index > 31) {
        // should not happen due to masking to 5 bits, but just in case
        printError(WARNING, "Instruction tried to read invalid register\n");
        return 0;
    }
    if (index == 0) {
        return 0;
    }
    return cpu.regFile[index];

}

void writeRegister(RV32_cpuState_t *cpu, uint8_t index, uint32_t value) {
    if (index > 31) {
        printError(WARNING, "Instruction tried to write invalid register\n");
    } else if (index != 0) {
        cpu->regFile[index] = value;
    }
}

void printCPU() {
    printf("PC=%08x | ", cpuState.PC);
    for (int i = 0; i < 32; i++) {
        printf("x%-2d=%08x  ", i, cpuState.regFile[i]);
        if ((i + 1) % 4 == 0) printf("\n             ");
    }
    printf("\n");
}
