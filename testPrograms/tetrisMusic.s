.section .data

# ==============================================================================
# TETRIS THEME (TYPE A) - CHIPTUNE ARRANGEMENT
# Format: .word FREQ_HZ, .word DURATION_MS, .word WAVEFORM_ID
#
# Waveform Legend:
# 3 = Noise (Percussion)
# 5 = Sawtooth (Heavy Bass)
# 6 = Pulse 25% (Game Boy Lead)
# 8 = NES Triangle (Flute/Hollow)
# ==============================================================================
tetris_song:
    # --- INTRO: GRITTY BASS (Sawtooth) ---
    .word 82,  200, 5   # E2
    .word 0,   50,  0   # Rest
    .word 123, 200, 5   # B2
    .word 0,   50,  0   # Rest
    .word 165, 200, 5   # E3
    .word 0,   50,  0   # Rest
    .word 208, 200, 5   # G#3
    .word 0,   100, 0   # Longer Rest

    # --- DRUM FILL (Noise) ---
    .word 440, 60,  3   # Snare
    .word 440, 60,  3   # Snare
    .word 0,   100, 0   # Rest

    # --- SECTION A: MAIN THEME (Pulse 25% - Game Boy Style) ---
    # E5 - B4 - C5 - D5
    .word 659, 400, 6   # E5
    .word 494, 200, 6   # B4
    .word 523, 200, 6   # C5
    .word 587, 400, 6   # D5
    
    # C5 - B4 - A4
    .word 523, 200, 6   # C5
    .word 494, 200, 6   # B4
    .word 440, 400, 6   # A4
    
    # A4 - C5 - E5
    .word 440, 200, 6   # A4 (Short hop)
    .word 523, 200, 6   # C5
    .word 659, 400, 6   # E5
    
    # D5 - C5 - B4
    .word 587, 200, 6   # D5
    .word 523, 200, 6   # C5
    .word 494, 400, 6   # B4
    
    # B4 - C5 - D5 - E5
    .word 494, 200, 6   # B4
    .word 0,   10,  0   # tiny gap
    .word 523, 200, 6   # C5
    .word 587, 400, 6   # D5
    .word 659, 400, 6   # E5

    # C5 - A4 - A4
    .word 523, 400, 6   # C5
    .word 440, 400, 6   # A4
    .word 0,   50,  0   # Articulation gap
    .word 440, 400, 6   # A4
    .word 0,   200, 0   # Rest

    # --- SECTION B: THE BRIDGE (NES Triangle - Flute Style) ---
    # D5 - F5 - A5 (High reach)
    .word 587, 300, 8   # D5
    .word 698, 300, 8   # F5
    .word 880, 400, 8   # A5

    # G5 - F5 - E5
    .word 783, 200, 8   # G5
    .word 698, 200, 8   # F5
    .word 659, 600, 8   # E5 (Sustain)

    # C5 - E5 - D5
    .word 523, 200, 8   # C5
    .word 659, 400, 8   # E5
    .word 587, 200, 8   # D5
    .word 523, 200, 8   # C5
    
    # B4 - B4 - C5
    .word 494, 400, 8   # B4
    .word 0,   50,  0   # Gap
    .word 494, 200, 8   # B4
    .word 523, 200, 8   # C5
    
    # D5 - E5
    .word 587, 400, 8   # D5
    .word 659, 400, 8   # E5
    
    # C5 - A4 - A4 (Back to Pulse for the finish)
    .word 523, 400, 6   # C5 (Pulse)
    .word 440, 400, 6   # A4 (Pulse)
    .word 440, 400, 6   # A4 (Pulse)

    # --- LOOP ---
    .word 0, 0, 0       # Terminator

.section .text
.globl _start

# ==============================================================================
# MMIO MAP
# ==============================================================================
.equ REG_DURATION,  0x90008
.equ REG_FREQ,      0x9000C
.equ REG_ENABLE,    0x90010
.equ REG_FINISHED,  0x90014
.equ REG_VOLUME,    0x90018
.equ REG_WAVEFORM,  0x9001C

_start:
    # 1. Set Global Volume
    li t0, 200          # ~80% Volume
    li t1, REG_VOLUME
    sw t0, 0(t1)

replay_song:
    la s0, tetris_song  # Load song data

next_note:
    # 2. Read Parameters
    lw s1, 0(s0)        # Frequency
    lw s2, 4(s0)        # Duration
    lw s3, 8(s0)        # Waveform ID

    # 3. Check Terminator
    or t0, s1, s2
    beqz t0, replay_song

    # 4. Write to Registers
    li t0, REG_FREQ
    sw s1, 0(t0)

    li t0, REG_DURATION
    sw s2, 0(t0)

    li t0, REG_WAVEFORM
    sw s3, 0(t0)        # Change Instrument!

    # 5. Clear Finished Flag (Race Condition Fix)
    li t0, REG_FINISHED
    sw zero, 0(t0)

    # 6. Trigger
    li t0, REG_ENABLE
    li t1, 1
    sw t1, 0(t0)

    # 7. Wait
wait_loop:
    li t0, REG_FINISHED
    lw t1, 0(t0)
    beqz t1, wait_loop

    # 8. Advance (12 bytes per note)
    addi s0, s0, 12
    j next_note