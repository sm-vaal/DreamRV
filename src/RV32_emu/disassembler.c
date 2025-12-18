//
// Created by sergi on 18/12/2025.
//

#include "disassembler.h"

void getDisassembledInstruction(RV32_instruction_t binaryInstruction, char* buf, int bufSize) {
    RV32_decodedInstruction_t decoded = decodeInstruction(binaryInstruction);
    RV32_operationType_t opType = getOperationType(binaryInstruction);

    switch (opType) {
        case OP_REG:
            disassembleOpReg(decoded, buf, bufSize);
            break;
        case OP_LOAD:
            disassembleOpLoad(decoded, buf, bufSize);
            break;
        case OP_IMM:
            disassembleOpImm(decoded, buf, bufSize);
            break;
        case OP_JALR:
            disassembleOpJalr(decoded, buf, bufSize);
            break;
        case OP_ECALL:
            disassembleOpEcall(decoded, buf, bufSize);
            break;
        case OP_STORE:
            disassembleOpStore(decoded, buf, bufSize);
            break;
        case OP_BRANCH:
            disassembleOpBranch(decoded, buf, bufSize);
            break;
        case OP_AUIPC:
            disassembleOpAuipc(decoded, buf, bufSize);
            break;
        case OP_LUI:
            disassembleOpLui(decoded, buf, bufSize);
            break;
        case OP_JAL:
            disassembleOpJal(decoded, buf, bufSize);
            break;
        default:
            *buf = '\0';

    }


}

void disassembleOpReg(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    static const char* mnemonics[8][3] = {
        {"add", "sub", "mul"   },
        {"sll",  NULL, "mulh"  },
        {"slt",  NULL, "mulhsu"},
        {"sltu", NULL, "mulhu" },
        {"xor",  NULL, "div"   },
        {"srl", "sra", "divu"  },
        {"or",   NULL, "rem"   },
        {"and",  NULL, "remu"  }
    };

    uint8_t alt = 0xFF;
    if (decoded.funct7 == 0) {
        alt = 0;
    }else if (decoded.funct7 == 0x20) {
        alt = 1;
    } else if (decoded.funct7 == 0x01) {
        alt = 2;
    }

    if (alt == 0xFF) {
        snprintf(buf, bufSize, "UNKNOWN REG");
        return;
    }

    const char* mnem = mnemonics[decoded.funct3][alt];

    if (mnem == NULL) {
        snprintf(buf, bufSize, "UNKNOWN REG");
    } else {
        // "mnem  rd, rs1, rs2"
        snprintf(buf, bufSize, "%s x%d, x%d, x%d",
                  mnem, decoded.rd, decoded.rs1, decoded.rs2);
    }

}

void disassembleOpLoad(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    static const char* mnemonics[8] = {
         "lb",  "lh", "lw", NULL, // 0b011 invalid
        "lbu", "lhu", NULL, NULL  // out of bounds
    };

    const char* mnem = mnemonics[decoded.funct3];
    if (mnem == NULL) {
        snprintf(buf, bufSize, "UNKNOWN LOAD");
    } else {
        // mnem rd, off(rs1)
        snprintf(buf, bufSize, "%s x%d, %d(x%d)",
            mnem, decoded.rd, decoded.I_imm, decoded.rs1);
    }
}

void disassembleOpImm(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    static const char* mnemonics[8][2] = {
        {"addi",  "addi" },
        {"slli",   NULL  },
        {"slti",  "slti" },
        {"sltiu", "sltiu"},
        {"xori",  "xori" },
        {"srli",  "srai" },
        {"ori",   "ori"  },
        {"andi",  "andi" }
    };

    // doesn't check if zero for srli, only non-srai.
    // This is deliberate for simplicity, binaries will always be valid anyway
    uint8_t alt = decoded.funct7 == 0x20;
    const char* mnem = mnemonics[decoded.funct3][alt];

    if (mnem == NULL) {
        snprintf(buf, bufSize, "UNKNOWN IMM");
    } else {
        // mnem rd, rs1, imm
        if (decoded.funct3 == 1 || decoded.funct3 == 5) {
            uint32_t shamt = decoded.rs2; // 5-bit in RV32 for shifts
            snprintf(buf, bufSize, "%s x%d, x%d, %u",
                     mnem, decoded.rd, decoded.rs1, shamt);
        } else {
            snprintf(buf, bufSize, "%s x%d, x%d, %d",
                     mnem, decoded.rd, decoded.rs1, decoded.I_imm);
        }
    }
}

void disassembleOpJalr(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    snprintf(buf, bufSize, "jalr x%d, %d(x%d)",
                decoded.rd, decoded.I_imm, decoded.rs1);
}

void disassembleOpEcall(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    snprintf(buf, bufSize, "ecall");
}

void disassembleOpStore(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    static const char* mnemonics[8] = {
      "sb", "sh", "sw", NULL, NULL, NULL, NULL, NULL
    };

    const char* mnem = mnemonics[decoded.funct3];

    if (mnem == NULL) {
        snprintf(buf, bufSize, "UNKNOWN STORE");
    } else {
        // mne rs2, imm(rs1)
        snprintf(buf, bufSize, "%s x%d, %d(x%d)",
                 mnem, decoded.rs2, decoded.S_imm, decoded.rs1);
    }
}

void disassembleOpBranch(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    static const char* mnemonics[8] = {
        "beq", "bne",   NULL,  NULL,
        "blt", "bge", "bltu", "bgeu"
    };

    const char* mnem = mnemonics[decoded.funct3];

    if (mnem == NULL) {
        snprintf(buf, bufSize, "UNKNOWN BRANCH");
    } else {
        // mnem rs1, rs2, label
        snprintf(buf, bufSize, "%s x%d, x%d, %d",
                 mnem, decoded.rs1, decoded.rs2, (int32_t) decoded.B_imm);
    }
}

void disassembleOpAuipc(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    snprintf(buf, bufSize, "auipc x%d, %d", decoded.rd, decoded.U_imm);
}

void disassembleOpLui(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    snprintf(buf, bufSize, "lui x%d, %d", decoded.rd, decoded.U_imm);
}

void disassembleOpJal(RV32_decodedInstruction_t decoded, char* buf, int bufSize) {
    snprintf(buf, bufSize, "jal x%d, %d", decoded.rd, (int32_t) decoded.J_imm);
}

