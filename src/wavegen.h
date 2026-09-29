#ifndef WAVEGEN_H
#define WAVEGEN_H

#include <stdint.h>

// Waveform generation C++ classes header
// waveform varies between +amplitude and -amplitude, at frequency, relative to sample_rate
//
// Antti Ketola 2026
// MIT LICENSE


enum waveform_e { SQUARE, SAWTOOTH, TRIANGLE, SINE };

class WaveGen {
  public:
    WaveGen(waveform_e waveform, int frequency, int sample_rate, uint16_t amplitude);
    void reset();
    virtual int32_t getSample() = 0;
    int getSampleRate() const { return sample_rate; }
    int getFrequency() const { return frequency; }
  protected:
    waveform_e waveform;
    int frequency;
    int sample_rate;
    uint16_t amplitude;
    uint32_t pos;
};

class SawtoothGen : public WaveGen {
  public:
    SawtoothGen(int frequency, int sample_rate, uint16_t amplitude);
    int32_t getSample() override;
};

class SquareGen : public WaveGen {
  public:
    SquareGen(int frequency, int sample_rate, uint16_t amplitude);
    int32_t getSample() override;
};

class TriangleGen : public WaveGen {
  public:
    TriangleGen(int frequency, int sample_rate, uint16_t amplitude);
    int32_t getSample() override;
};

class SineGen : public WaveGen {
  public:
    SineGen(int frequency, int sample_rate, uint16_t amplitude);
    int32_t getSample() override;
};

#endif // WAVEGEN_H
