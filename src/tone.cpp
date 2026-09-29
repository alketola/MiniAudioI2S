#include <math.h>
#include "tone.h"

// Implementation of Tone related classes
//
// Antti Ketola 2026
// MIT LICENSE

Envelope::Envelope(uint32_t attack_ms, uint32_t decay_ms, uint32_t sustain_ms, uint8_t sustain_level_percent, uint32_t release_ms) {
  this->attack_ms = attack_ms;
  this->decay_ms = decay_ms;
  this->sustain_ms = sustain_ms;
  this->sustain_level_percent = sustain_level_percent;
  this->release_ms = release_ms;
  this->duration_ms = attack_ms + decay_ms + sustain_ms + release_ms;
}

// Returns 0.0-1.0 amplitude multiplier for the ADSR position at sample_index.
// Stages use cumulative time boundaries: attack [0,a), decay [a,a+d),
// sustain [a+d,a+d+s), release [a+d+s,a+d+s+r), silence afterwards.
float Envelope::getLevel(uint32_t sample_index, uint32_t sample_rate) const {
  float t  = (float)sample_index * 1000.0f / (float)sample_rate; // ms elapsed
  float sl = sustain_level_percent / 100.0f;                     // sustain level 0..1
  float a = attack_ms, d = decay_ms, s = sustain_ms, r = release_ms;

  if (t < a)             return (a > 0.0f) ? t / a : 1.0f;            // 0 -> 1
  if (t < a + d)         return 1.0f - (1.0f - sl) * (t - a) / d;     // 1 -> sl
  if (t < a + d + s)     return sl;                                   // sustain hold
  if (t < a + d + s + r) return sl * (1.0f - (t - (a + d + s)) / r);  // sl -> 0
  return 0.0f;
}

Tone::Tone(WaveGen* waveGen, Envelope* envelope, uint16_t master_volume_percent)
  : pWaveGen(waveGen), pEnvelope(envelope), master_volume_percent(master_volume_percent) {
    set_volume(master_volume_percent);
  }

// dB-based log curve: 100% = 1.0, 50% ≈ 0.1 (-20 dB), 0% = silence
float Tone::volumeCurve(uint16_t percent) {
  return (percent == 0)
    ? 0.0f
    : powf(10.0f, (percent / 100.0f - 1.0f) * 2.0f);
}

void Tone::set_volume(uint16_t percent) {
  master_volume_percent = percent;
  logvol = volumeCurve(percent);
}

uint32_t Tone::msToSamples(uint32_t ms) {
    return (uint64_t)ms * this->pWaveGen->getSampleRate() / 1000;
}

void Tone::play(Stream &stream_to) {
  uint32_t rate = pWaveGen->getSampleRate();
  uint32_t length_samples = msToSamples(pEnvelope->getDurationMs());

  pWaveGen->reset();  // start each note at phase 0
  Serial.printf("Length samples: %u ; %.2f ms\n",
    length_samples, (float)length_samples * 1000.0f / (float)rate);

  for (uint32_t i = 0; i < length_samples; i++) {
    int32_t raw = pWaveGen->getSample();
    float env   = pEnvelope->getLevel(i, rate);
    float out   = raw * env * this->logvol;
    int16_t s16 = (int16_t)constrain((int32_t)out, -32768, 32767);
    uint8_t* sample_pointer = (uint8_t*) & s16;
    // Stereo 16-bit: write the same sample to left and right channels
    stream_to.write(sample_pointer, sizeof(int16_t));
    stream_to.write(sample_pointer, sizeof(int16_t));
  }
}

void Tone::plot() {
  uint32_t rate = pWaveGen->getSampleRate();
  int f = pWaveGen->getFrequency();
  uint32_t length_samples = rate/f ;
  Serial.printf("plot: length_samples:%u \n", length_samples);
  Serial.printf("plot: length %.2f ms \n", (float)length_samples * 1000.0f / (float)rate );

  for (uint32_t i = 0; i < length_samples; i++) {
    double env   = pEnvelope->getLevel(i, rate);
    int32_t raw = pWaveGen->getSample();
    double out   = raw; // * logvol;
    int16_t s16 = (int16_t)constrain((int) out, -32768, 32767);

    Serial.print("tone-signal-level:");
    Serial.println(s16);
  }
}

void Tone::plot_env() {
  uint32_t rate = pWaveGen->getSampleRate();
  uint32_t f = pWaveGen->getFrequency();
  uint32_t duration_ms = this->pEnvelope->getDurationMs();
  uint32_t wavelength_samples = rate/f ;
  uint32_t duration_samples = msToSamples(duration_ms);
  Serial.printf("plot env: rate:%u f:%u duration_ms:%u wavelength_samples:%u duration_samples:%u (%.2f ms)\n",
    rate, f, duration_ms, wavelength_samples, duration_samples,
    (float)duration_samples * 1000.0f / (float)rate);

  for (uint32_t i = 0; i < duration_samples; i++) {
    float env   = pEnvelope->getLevel(i, rate);
    Serial.printf("envelope-level: %f \n", env);    
  }
}
