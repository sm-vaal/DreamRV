//
// Created by sergi on 09/12/2025.
//

#ifndef DREAMRV_AUDIO_H
#define DREAMRV_AUDIO_H

#include "raylib.h"
#include <stdint.h>

#define AUDIO_CHANNEL_COUNT 4
#define AUDIO_BASE_ADDR     0x90008
#define AUDIO_CHANNEL_WIDTH 0x20

#define REG_OFFSET_DURATION  0x00  // uint32 msToPlay
#define REG_OFFSET_FREQ      0x04  // uint32 frequency
#define REG_OFFSET_ENABLE    0x08  // nonzero to play sound
#define REG_OFFSET_FINISHED  0x0C  // 0-nonzero
#define REG_OFFSET_VOLUME    0x10  // 0-255
#define REG_OFFSET_WAVEFORM  0x14  // 0-8 (enum)

typedef enum {
    WAVEFORM_SINE       = 0,
    WAVEFORM_SQUARE     = 1,
    WAVEFORM_TRIANGLE   = 2,
    WAVEFORM_NOISE      = 3,
    WAVEFORM_ATARI_BASS = 4,
    WAVEFORM_SAWTOOTH   = 5,
    WAVEFORM_PULSE_25   = 6, // 25% duty cycle square
    WAVEFORM_PULSE_12   = 7, // 12.5%
    WAVEFORM_NES_TRI    = 8  // stepped rectangle, NES style
} waveForm_t;

typedef struct {
    Sound sound;
    short* waveData;

    uint32_t currentFrequency;
    uint32_t currentDuration;
    uint32_t currentVolume;
    waveForm_t currentWaveForm;
} audioChannel_t;

void initAudio();
void closeAudio();
void updateChannelSound(audioChannel_t* ch, uint32_t durationMs, uint32_t frequency,
                        uint32_t rawVolume, waveForm_t waveFormType);
void playSoundFromMemory();

#endif //DREAMRV_AUDIO_H