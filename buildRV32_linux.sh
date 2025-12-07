#!/bin/bash
# ============================================================
# RISC-V RV32 Assembly Build Script
# Usage: ./build_rv32.sh input.s
# ============================================================

if [ -z "$1" ]; then
    echo "Usage: $0 file.s"
    exit 1
fi

SRC="$1"
NAME="${SRC%.*}"

# ------------------------------------------------------------
# Assemble -> main.o
# ------------------------------------------------------------
echo "Assembling $SRC ..."
riscv64-unknown-elf-as -march=rv32im -mabi=ilp32 -o "${NAME}.o" "$SRC"
if [ $? -ne 0 ]; then
    exit 1
fi

# ------------------------------------------------------------
# Link -> main.elf
# ------------------------------------------------------------
echo "Linking..."
riscv64-unknown-elf-ld -m elf32lriscv -Ttext=0x0 -o "${NAME}.elf" "${NAME}.o"
if [ $? -ne 0 ]; then
    exit 1
fi

# ------------------------------------------------------------
# Convert -> main.bin
# ------------------------------------------------------------
echo "Creating raw binary..."
riscv64-unknown-elf-objcopy -O binary "${NAME}.elf" "${NAME}.bin"
if [ $? -ne 0 ]; then
    exit 1
fi

echo
echo "Build complete!"
echo "Output files:"
echo "  ${NAME}.o"
echo "  ${NAME}.elf"
echo "  ${NAME}.bin"
echo
