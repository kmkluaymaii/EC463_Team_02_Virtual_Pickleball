#pragma once
#include "Config.h"

#define I2S_WS 25
#define I2S_BCK 26
#define I2S_DOUT 27
const int SAMPLE_RATE = 44100;
const int amplitude = 20000;

i2s_chan_handle_t tx_handle;

bool beepActive = false;
int beepFreq = 0;
int beepToggleSamples = 0;
int16_t beepSample = amplitude;
int beepCounter = 0;
unsigned long beepEndTime = 0;

void setupI2S() {
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
  i2s_new_channel(&chan_cfg, &tx_handle, NULL);

  i2s_std_config_t std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
    .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
        I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
      .mclk = I2S_GPIO_UNUSED,
      .bclk = (gpio_num_t)I2S_BCK,
      .ws = (gpio_num_t)I2S_WS,
      .dout = (gpio_num_t)I2S_DOUT,
      .din = I2S_GPIO_UNUSED,
    },
  };

  i2s_channel_init_std_mode(tx_handle, &std_cfg);
  i2s_channel_enable(tx_handle);
}

void playBeep(int freqHz, int durationMs) {
  if (freqHz <= 0 || durationMs <= 0) return;

  beepFreq = freqHz;
  beepEndTime = millis() + durationMs;
  beepToggleSamples = max(1, (SAMPLE_RATE / freqHz) / 2);
  beepCounter = 0;
  beepSample = amplitude;
  beepActive = true;
}

void processAudio() {
  if (beepActive) {
    if (millis() >= beepEndTime) {
      beepActive = false;
      beepSample = 0;
    } else {
      beepCounter++;
      if (beepCounter >= beepToggleSamples) {
        beepCounter = 0;
        beepSample = -beepSample;
      }
    }
  } else {
    beepSample = 0;
  }

  size_t written;
  i2s_channel_write(tx_handle, &beepSample, sizeof(beepSample), &written, 0);
}