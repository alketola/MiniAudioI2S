#ifndef TONE_H
#define TONE_H

#include <Arduino.h>
#include "wavegen.h"

// Headers of on Tone related classes
//
// Antti Ketola 2026
// MIT LICENSE

// Attack Decay Sustain Release (ADSR) volume envelope class definition

class Envelope {
  public:
    Envelope(uint32_t attack_ms, uint32_t decay_ms, uint32_t sustain_ms, uint8_t sustain_level_percent, uint32_t release_ms);
    float getLevel(uint32_t sample_index, uint32_t sample_rate) const;
    uint32_t getDurationMs() const { return duration_ms; }
  private:
    uint32_t attack_ms;
    uint32_t attack;
    uint32_t decay_ms;
    uint32_t decay;
    uint32_t sustain_ms;
    uint32_t sustain;
    uint32_t release_ms;
    uint32_t release;
    uint32_t duration_ms;
    uint32_t duration;
    uint8_t sustain_level_percent;
};

// Tone class;
// A tone includes waveform, envelope, and general sound volume

class Tone {
  public:
    Tone(WaveGen* waveGen, Envelope* envelope, uint16_t master_volume_percent);
    void play(Stream &destination); // plays the tone to stream destination
    void plot(); // outputs data for plotting the tone
    void plot_env(); // outputs data for plotting the envelope
    void set_volume(uint16_t vol_percent);
    uint32_t msToSamples(uint32_t ms);
    uint16_t master_volume_percent;
    float logvol;
  private:
    static float volumeCurve(uint16_t percent); // logaritmic volume curve for human hearing levels
    WaveGen* pWaveGen;
    Envelope* pEnvelope;

};

#endif // TONE_H
