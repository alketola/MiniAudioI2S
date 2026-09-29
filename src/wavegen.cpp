#include <Arduino.h>
#include "wavegen.h"
// Waveform generation C++ classes implementation
// waveform varies between +amplitude and -amplitude, at frequency, relative to sample_rate
//
// Antti Ketola 2026
// MIT LICENSE

WaveGen::WaveGen(waveform_e waveform, int frequency, int sample_rate, uint16_t amplitude)
  : waveform(waveform), frequency(frequency), sample_rate(sample_rate),
    amplitude(amplitude), pos(0) {}

void WaveGen::reset() {
  pos = 0;
}

// Sawtooth: linear ramp from -amplitude to +amplitude over one period
SawtoothGen::SawtoothGen(int frequency, int sample_rate, uint16_t amplitude)
  : WaveGen(SAWTOOTH, frequency, sample_rate, amplitude) {}

int32_t SawtoothGen::getSample() {
  int period = sample_rate / frequency;
  int16_t sample = ((int16_t)amplitude * 2 * ((int16_t)pos - period) / period) - amplitude;
  if (++pos >= period){ pos = 0; }
  return sample;
}

// Square: +amplitude for first half period, -amplitude for second half
SquareGen::SquareGen(int frequency, int sample_rate, uint16_t amplitude)
  : WaveGen(SQUARE, frequency, sample_rate, amplitude) {}

int32_t SquareGen::getSample() {
  int32_t period = sample_rate / frequency;
  int32_t sample = (pos < (uint32_t)(period / 2)) ? (int32_t) -amplitude : (int32_t) amplitude;
  if (++pos >= period) reset();
  return sample;
}

// Triangle: rises from -amplitude to +amplitude, then falls back
TriangleGen::TriangleGen(int frequency, int sample_rate, uint16_t amplitude)
  : WaveGen(TRIANGLE, frequency, sample_rate, amplitude) {}

int32_t TriangleGen::getSample() {
  int period = sample_rate / frequency;
  int half = period / 2;
  int32_t sample;
  if (pos < (uint32_t)half) {
    sample = (int32_t)amplitude * (2 * (int32_t)pos - half) / half;
  } else {
    sample = (int32_t)amplitude * (3 * half - 2 * (int32_t)pos) / half;
  }
  if (++pos >= (uint32_t)period) pos = 0;
  return sample;
}

// Sine: rises from -amplitude to +amplitude, then falls back
SineGen::SineGen(int frequency, int sample_rate, uint16_t amplitude)
  : WaveGen(SINE, frequency, sample_rate, amplitude) {}

int32_t SineGen::getSample() {
  int period = sample_rate / frequency;
  int half = period / 2;
  int32_t sample;

  sample = amplitude * sin(2 * M_PI* pos/period);
  if (pos < (uint32_t)half) {
    sample = (int32_t)amplitude * (2 * (int32_t)pos - half) / half;
  } else {
    sample = (int32_t)amplitude * (3 * half - 2 * (int32_t)pos) / half;
  }
  if (++pos >= (uint32_t)period) pos = 0;
  return sample;
}
