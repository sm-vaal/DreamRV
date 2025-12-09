.section .text
.globl _start

.equ AUDIO_BASE,      0x90008
.equ CH_WIDTH,        0x20

_start:
    li s0, AUDIO_BASE   # s0 = Base Address
    li s1, 0            # s1 = Channel Index (0-3)
    li s2, 0            # s2 = Instrument Index (0-8)

test_loop:
    # 1. Calculate Current Channel Address
    #    Address = Base + (Index * 32)
    li t0, CH_WIDTH
    mul t0, s1, t0      # t0 = Index * 0x20
    add s3, s0, t0      # s3 = CURRENT Channel Address (e.g. 0x90008, 0x90028...)

    # 2. Write Sound Parameters
    li t0, 500          # Duration: 500ms
    sw t0, 0(s3)

    li t0, 440          # Frequency: 440Hz
    sw t0, 4(s3)

    sw s2, 20(s3)       # Waveform: s2 (Cycles 0-8)

    li t0, 200          # Volume: 200
    sw t0, 16(s3)

    # 3. Trigger Sound
    sw zero, 12(s3)     # Clear Status (Set to 0/Busy) BEFORE trigger, to prevent issues
    
    li t0, 1
    sw t0, 8(s3)        # Enable = 1

    # 4. Wait for Sound to Finish
wait_busy:
    lw t0, 12(s3)       # Read Status
    beqz t0, wait_busy  # Loop while 0 (Busy)

    # 5. Update Loops
    
    # Increment Channel (0 -> 1 -> 2 -> 3 -> 0)
    addi s1, s1, 1
    li t0, 4
    rem s1, s1, t0      # Modulo 4

    # Increment Instrument (0 -> ... -> 8 -> 0)
    addi s2, s2, 1
    li t0, 9
    rem s2, s2, t0      # Modulo 9

    j test_loop