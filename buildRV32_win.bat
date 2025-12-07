@echo off
REM ============================================================
REM  RISC-V RV32 Assembly Build Script
REM  Usage: build_rv32.bat input.s
REM ============================================================

if "%~1"=="" (
    echo Usage: %0 file.s
    exit /b 1
)

set SRC=%~1
set NAME=%~n1

REM ------------------------------------------------------------
REM  Assemble -> main.o
REM ------------------------------------------------------------
echo Assembling %SRC% ...
riscv64-unknown-elf-as -march=rv32im -mabi=ilp32 -o "%NAME%.o" "%SRC%"
if errorlevel 1 exit /b 1

REM ------------------------------------------------------------
REM  Link -> main.elf
REM ------------------------------------------------------------
echo Linking...
riscv64-unknown-elf-ld -m elf32lriscv -Ttext=0x0 -o "%NAME%.elf" "%NAME%.o"
if errorlevel 1 exit /b 1

REM ------------------------------------------------------------
REM  Convert -> main.bin
REM ------------------------------------------------------------
echo Creating raw binary...
riscv64-unknown-elf-objcopy -O binary "%NAME%.elf" "%NAME%.bin"
if errorlevel 1 exit /b 1

echo.
echo Build complete!
echo Output files:
echo   %NAME%.o
echo   %NAME%.elf
echo   %NAME%.bin
echo.
