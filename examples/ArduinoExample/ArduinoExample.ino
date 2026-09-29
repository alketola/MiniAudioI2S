/*
 This example generates sound over I2S at a specified frequency
 and sample rate of a chosen waveform generator defined in wawegen.h

 In the I2S bus there
 MAX08357 I2S Amp Breakout board.
 I2S Circuit:
 * Arduino devikt v1 (diymore)

 Circuit:
 * ESP32 
 * I2S input amplifier MAX 98357A
 * Connect 8 ohm speaker
 * A push button to trigger the sound in the loop()

 * MAX98357A:
   * GND connected GND
   * VIN connected 3.3V
   * LRC connected to pin D23
   * BCLK connected to pin D18
   * DIN connected to pin D4
   * SD connected to pin D27 (optional)

Antti Ketola 2026
 
 MIT LICENSE

 */
#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <ESP_I2S.h>
#include "esp_log.h"
#include "wavegen.h"
#include "tone.h"

// The GPIO pins are selectable, many other pins could be used for the I2S bus.
#define I2S_LRC  25
#define I2S_BCLK 5
#define I2S_DIN  26

// D17, a push button to pull down for a test beeps. 
// Please select a pin with input pull-up resistor option.
#define PUSH_BUTTON_0 17 

const int frequency = 2200;    // frequency of generated wave in Hz
const int amplitude = 32000;   // amplitude of wave
const int sampleRate = 48000;  // sample rate in Hz

i2s_data_bit_width_t bits_per_sample = I2S_DATA_BIT_WIDTH_16BIT;
i2s_mode_t mode = I2S_MODE_STD;
i2s_slot_mode_t slot = I2S_SLOT_MODE_STEREO;

const i2s_port_t i2s_selectedPort = I2S_NUM_0; // Requires ESP32 board support package version 3.3.9 or greater.

I2SClass i2s;
int volume = 80;
i2s_port_t selectedPort = I2S_NUM_AUTO;

SawtoothGen wg = SawtoothGen(frequency, sampleRate, amplitude);
Envelope e = Envelope(5, 30, 5, 40, 400);
Tone t = Tone(&wg, &e, volume);

void setup() {
  // Serial.begin(115200);
      ESP_LOGI("MINIAUDIO", "I2S Sound test with wave generator");
  gpio_config(PUSH_BUTTON_0, INPUT_PULLUP); // the push button is expected to short the pin to GND when pressed

  i2s.setPins(I2S_BCLK, I2S_LRC, I2S_DIN);
  
  if (!i2s.setPort(selectedPort)) {
    ESP_LOGI("MINIAUDIO", "Failed to select I2S port!");
    while (1);
  }

      ESP_LOGI("MINIAUDIO", "I2S port selection"); // Requires ESP32 board support package version 3.3.9 or greater.

  if (!i2s.setPort(i2s_selectedPort)) {
    ESP_LOGI("MINIAUDIO", "Failed to select I2S port!");
    while (1);
  }

  // start I2S at the sample rate with 16-bits per sample
  
  if (!i2s.begin(mode, sampleRate, bits_per_sample, slot)) {
    ESP_LOGI("MINIAUDIO", "Failed to initialize I2S!");
    while (1);  // do nothing if not able to start I2S device
  }
    
}

// Arduino main loop
void loop() {
  bool button_reading = gpio_get_level(PUSH_BUTTON_0); // digitalRead( active low; pushed = FALSE

  if(!button_reading) {  
    ESP_LOGI("MINIAUDIO","Beeping");
    t.play(i2s);
    vTaskDelay(pdMS_TO_TICKS(100));
    t.play(i2s);
    vTaskDelay(pdMS_TO_TICKS(2000));
  } else {
    vTaskDelay(pdMS_TO_TICKS(500)); // making the busy loop less busy
  }

}
