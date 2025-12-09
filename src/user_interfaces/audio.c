// Created by sergi on 09/12/2025.

#include "audio.h"
#include <math.h>
#include <stdlib.h>
#include "../RV32_emu/memory.h"

Sound targetSound = {0};
short* waveData = NULL;

static audioChannel_t channels[AUDIO_CHANNEL_COUNT];

void initAudio() {
    InitAudioDevice();
    for (int i = 0; i < AUDIO_CHANNEL_COUNT; i++) {
        channels[i].sound = (Sound) {0};
        channels[i].waveData = NULL;
        channels[i].currentWaveForm = -1;
        channels[i].currentFrequency = 0;
        channels[i].currentDuration = 0;
        channels[i].currentVolume = 0;
    }
}

void updateChannelSound(audioChannel_t* ch, uint32_t durationMs, uint32_t frequency,
                        uint32_t rawVolume, waveForm_t waveFormType) {

    if (durationMs == 0) return;
    // Calculate proper sample count (SampleRate * TimeInSeconds)
    uint32_t sampleRate = 44100;
    uint32_t sampleCount = (uint32_t)((float)sampleRate * ((float)durationMs / 1000.0f));

    // regenerate if changed
    if (ch->currentFrequency != frequency || ch->currentDuration != durationMs ||
        ch->currentVolume != rawVolume || ch->currentWaveForm != waveFormType) {

        // Clean up old data if it exists
        if (ch->sound.stream.buffer != NULL) {
            UnloadSound(ch->sound);
        }
        if (ch->waveData != NULL) {
            free(ch->waveData);
        }

        // Allocate new buffer (Use short for signed 16-bit audio)
        ch->waveData = (short*)malloc(sampleCount * sizeof(short));

        if (rawVolume > 255) rawVolume = 255;
        float volumeScale = (float)rawVolume / 255.0f;
        float amplitude = 32000.0f * volumeScale;

        // Generate wave
        switch (waveFormType) {
            case WAVEFORM_SINE: default: {
                for (uint32_t i = 0; i < sampleCount; i++) {
                    // Standard sine wave formula
                    ch->waveData[i] = (short)(amplitude * sinf(2.0f * PI * (float) frequency
                                            * (float) i / (float) sampleRate));
                }
                break;
            }
            case WAVEFORM_SQUARE: {
                for (uint32_t i = 0; i < sampleCount; i++) {
                    // square wave formula
                    float period = (float)sampleRate / (float)frequency;
                    ch->waveData[i] = (fmodf(i, period) < (period / 2.0f)) ?
                    (short)amplitude : (short)-amplitude;
                }
                break;
            }
            case WAVEFORM_TRIANGLE: {
                for (uint32_t i = 0; i < sampleCount; i++) {
                    float t = (2.0f * PI * frequency * i) / sampleRate;
                    ch->waveData[i] = (short)((2.0f * amplitude / PI) * asinf(sinf(t)));
                }
                break;
            }
            case WAVEFORM_NOISE: {
                for (uint32_t i = 0; i < sampleCount; i++) {
                    // cool retro chiptune trick
                    static uint16_t lfsr = 0xACE1u; // static: preserve
                    lfsr ^= lfsr << 7;
                    lfsr ^= lfsr >> 9;
                    lfsr ^= lfsr << 8;
                    ch->waveData[i] = (lfsr & 1) ? amplitude : -amplitude;
                }
                break;
            }
            case WAVEFORM_ATARI_BASS: {
                // sequence used by TIA for bass
                // repeats every 31 "ticks", treat this as one full wave cycle.
                static const uint8_t poly5_lut[31] = {
                    0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0,
                    0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1
                };

                for (uint32_t i = 0; i < sampleCount; i++) {
                    // Calculate which "bit" of the 31-bit pattern we are on
                    // Formula: (Time * Freq * PatternLength) / SampleRate
                    // This stretches the 31 bits to fit exactly 1 second / frequency

                    float phase = ((float)i * (float)frequency * 31.0f) / (float)sampleRate;

                    // Wrap around the array size (modulo 31)
                    int bitIndex = (int)phase % 31;

                    // Output amplitude based on the bit state
                    ch->waveData[i] = poly5_lut[bitIndex] ? (short)amplitude : (short)-amplitude;
                }
                break;
            }

            case WAVEFORM_SAWTOOTH: {
                for (uint32_t i = 0; i < sampleCount; i++) {
                    float period = (float)sampleRate / (float)frequency;

                    // Normalize 'i' to a 0.0 - 1.0 range within the period
                    float phase = fmodf(i, period) / period;

                    // Convert 0.0->1.0 to -1.0->1.0 (Bipolar audio)
                    // Formula: (phase * 2) - 1
                    ch->waveData[i] = (short)(amplitude * ((phase * 2.0f) - 1.0f));
                }
                break;
            }

            case WAVEFORM_PULSE_25:
            case WAVEFORM_PULSE_12: {
                for (uint32_t i = 0; i < sampleCount; i++) {
                    float period = (float)sampleRate / (float)frequency;
                    float duty = (waveFormType == WAVEFORM_PULSE_25) ? 0.25f : 0.125f;

                    // If current phase < duty cycle % * period, output High, else Low
                    if (fmodf(i, period) < (period * duty)) {
                        ch->waveData[i] = (short)amplitude;
                    } else {
                        ch->waveData[i] = (short)-amplitude;
                    }
                }
                break;
            }

            case WAVEFORM_NES_TRI: {
                // The NES triangle has 32 steps total (16 up, 16 down)
                for (uint32_t i = 0; i < sampleCount; i++) {
                    float period = (float)sampleRate / (float)frequency;

                    // Where are we in the cycle? (0.0 to 1.0)
                    float phase = fmodf(i, period) / period;
                    float sample = 0.0f;
                    if (phase < 0.5f) {
                        // Rising: 0.0 -> 1.0 becomes -1.0 -> 1.0
                        sample = (phase * 4.0f) - 1.0f;
                    } else {
                        // Falling: 0.5 -> 1.0 becomes 1.0 -> -1.0
                        sample = 1.0f - ((phase - 0.5f) * 4.0f);
                    }

                    // quantizing!
                    // NES has 16 volume steps (4-bit).
                    // We multiply by 15, round, then divide back.
                    float steps = 15.0f;
                    sample = roundf(sample * steps) / steps;

                    ch->waveData[i] = (short)(amplitude * sample);
                }
                break;
            }
        }


        Wave wave = { 0 };
        wave.frameCount = sampleCount;
        wave.sampleRate = sampleRate;
        wave.sampleSize = 16;
        wave.channels = 1;
        wave.data = ch->waveData;

        // Upload to Audio Card
        ch->sound = LoadSoundFromWave(wave);

        // Update cache
        ch->currentFrequency = frequency;
        ch->currentDuration  = durationMs;
        ch->currentVolume    = rawVolume;
        ch->currentWaveForm  = waveFormType;
    }

    PlaySound(ch->sound);
}

void closeAudio() {
    for (int i = 0; i < AUDIO_CHANNEL_COUNT; i++) {
        if (channels[i].sound.stream.buffer != NULL) {
            UnloadSound(channels[i].sound);
        }
        if (channels[i].waveData != NULL) {
            free(channels[i].waveData);
        }
    }
    CloseAudioDevice();
}

void playSoundFromMemory() {
    for (int i = 0; i < AUDIO_CHANNEL_COUNT; i++) {
        uint32_t channelBase = AUDIO_BASE_ADDR + (i * AUDIO_CHANNEL_WIDTH);
        uint32_t enableAddr = channelBase + REG_OFFSET_ENABLE;

        bool justTriggered = false;

        // on trigger:
        if (getWord(enableAddr)) {
            // Acknowledge trigger
            writeWord(enableAddr, 0);

            // Set Finished to 0 (Busy)
            writeWord(channelBase + REG_OFFSET_FINISHED, 0);

            // Get params
            uint32_t duration = getWord(channelBase + REG_OFFSET_DURATION);
            uint32_t freq     = getWord(channelBase + REG_OFFSET_FREQ);
            uint32_t vol      = getWord(channelBase + REG_OFFSET_VOLUME);
            uint32_t wave     = getWord(channelBase + REG_OFFSET_WAVEFORM);

            if (wave > 8) wave = 0; // prevent invalid instruments

            updateChannelSound(&channels[i], duration, freq, vol, wave);

            justTriggered = true;
        }

        if (!justTriggered) {
            if (!IsSoundPlaying(channels[i].sound)) {
                writeWord(channelBase + REG_OFFSET_FINISHED, 0xFFFFFFFF);
            } else {
                writeWord(channelBase + REG_OFFSET_FINISHED, 0);
            }
        }
    }
}