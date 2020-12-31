/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

/**
 * Audio management functionality. Many of the methods in here are called from the
 * usbd_audio_if.cpp file.
 */

class Audio {

  private:
    // 20ms of 64 bit samples

    volatile int32_t *_sampleBuffer;
    int16_t *_processBuffer;
    int16_t *_sendBuffer;

    const MuteButton &_muteButton;
    const LiveLed &_liveLed;
    GraphicEqualizer &_graphicEqualiser;
    VolumeControl &_volumeControl;
    bool _running;

  public:
    static Audio *_instance;

  public:
    Audio(const MuteButton &muteButton, const LiveLed &liveLed, GraphicEqualizer &graphicEqualiser,
        VolumeControl &volumeControl);

    void setLed() const;
    void setVolume(int16_t volume);

    void i2s_halfComplete();
    void i2s_complete();

    int8_t start();
    int8_t stop();
    int8_t pause();
    int8_t resume();

    const GraphicEqualizer& getGraphicEqualizer() const;

  private:
    void sendData(volatile int32_t *data_in, int16_t *data_out);
};

/**
 * Constructor
 */

inline Audio::Audio(const MuteButton &muteButton, const LiveLed &liveLed, GraphicEqualizer &graphicEqualiser,
    VolumeControl &volumeControl) :
    _muteButton(muteButton), _liveLed(liveLed), _graphicEqualiser(graphicEqualiser), _volumeControl(volumeControl) {

  // initialise variables

  Audio::_instance = this;
  _running = false;

  // allocate buffers

  _sampleBuffer = new int32_t[MIC_SAMPLES_PER_PACKET * 2];      // 7680 bytes (*2 because samples are 64 bit)
  _processBuffer = new int16_t[MIC_SAMPLES_PER_PACKET];         // 1920 bytes
  _sendBuffer = new int16_t[MIC_SAMPLES_PER_PACKET];            // 1920 bytes

  // set LR to low (it's pulled low anyway)

  HAL_GPIO_WritePin(LR_GPIO_Port, LR_Pin, GPIO_PIN_RESET);
}

/**
 * Start the I2S DMA transfer (called from usbd_audio_if.cpp)
 */

inline int8_t Audio::start() {

  HAL_StatusTypeDef status;

  // HAL_I2S_Receive_DMA will multiply the size by 2 because the standard is 24 bit Philips.

  if ((status = HAL_I2S_Receive_DMA(&hi2s1, (uint16_t*) _sampleBuffer, MIC_SAMPLES_PER_PACKET * 2)) == HAL_OK) {
    _running = true;
  }

  return status;
}

/**
 * Stop the I2S DMA transfer
 */

inline int8_t Audio::stop() {

  HAL_StatusTypeDef status;

  if ((status = HAL_I2S_DMAStop(&hi2s1)) == HAL_OK) {
    _running = false;
  }
  return status;
}

/**
 * Pause I2S DMA transfer (soft-mute)
 */

inline int8_t Audio::pause() {

  HAL_StatusTypeDef status;

  if ((status = HAL_I2S_DMAPause(&hi2s1)) == HAL_OK) {
    _running = false;
  }

  return status;
}

/**
 * Resume I2S DMA transfer
 */

inline int8_t Audio::resume() {

  HAL_StatusTypeDef status;

  if ((status = HAL_I2S_DMAResume(&hi2s1)) == HAL_OK) {
    _running = true;
  }
  return status;
}

/**
 * Light the live LED if we are unmuted (hard and soft)
 */

inline void Audio::setLed() const {
  _liveLed.setState(_running && !_muteButton.isMuted());
}

/**
 * Set the volume gain
 */

inline void Audio::setVolume(int16_t volume) {

  // reduce resolution from 1/256dB to 1/2dB

  volume /= 128;

  // ensure SVC library limits are respected

  if (volume < -160) {
    volume = -160;
  } else if (volume > 72) {
    volume = 72;
  }

  _volumeControl.setVolume(volume);
}

/**
 * Get a reference to the graphic equalizer
 */

inline const GraphicEqualizer& Audio::getGraphicEqualizer() const {
  return _graphicEqualiser;
}

/**
 * 1. Transform the I2S data into 16 bit PCM samples in a holding buffer
 * 2. Use the ST GREQ library to apply a graphic equaliser filter
 * 3. Use the ST SVC library to adjust the gain (volume)
 * 4. Transmit over USB to the host
 *
 * We've got 10ms to complete this method before the next DMA transfer will be ready.
 */

inline void Audio::sendData(volatile int32_t *data_in, int16_t *data_out) {

  // only do anything at all if we're not muted and we're connected

  if (!_muteButton.isMuted() && _running) {

    // transform the I2S samples from the 64 bit L/R (32 bits per side) of which we
    // only have data in the L side. Take the most significant 16 bits, being careful
    // to respect the sign bit.

    int16_t *dest = _processBuffer;

    for (uint16_t i = 0; i < MIC_SAMPLES_PER_PACKET / 2; i++) {
      *dest++ = data_in[0];     // left channel has data
      *dest++ = data_in[0];     // right channel is duplicated from the left
      data_in += 2;
    }

    // apply the graphic equaliser filters using the ST GREQ library then
    // adjust the gain (volume) using the ST SVC library

    _graphicEqualiser.process(_processBuffer, MIC_SAMPLES_PER_PACKET / 2);
    _volumeControl.process(_processBuffer, MIC_SAMPLES_PER_PACKET / 2);

    // we only want the left channel from the processed buffer

    int16_t *src = _processBuffer;
    dest = data_out;

    for (uint16_t i = 0; i < MIC_SAMPLES_PER_PACKET / 2; i++) {
      *dest++ = *src;
      src += 2;
    }

    // send the adjusted data to the host

    if (USBD_AUDIO_Data_Transfer(&hUsbDeviceFS, data_out, MIC_SAMPLES_PER_PACKET / 2) != USBD_OK) {
      Error_Handler();
    }
  }
}

/**
 * Override the I2S DMA half-complete HAL callback to process the first MIC_MS_PER_PACKET/2 milliseconds
 * of the data while the DMA device continues to run onward to fill the second half of the buffer.
 */

inline void Audio::i2s_halfComplete() {
  sendData(_sampleBuffer, _sendBuffer);
}

/**
 * Override the I2S DMA complete HAL callback to process the second MIC_MS_PER_PACKET/2 milliseconds
 * of the data while the DMA in circular mode wraps back to the start of the buffer
 */

inline void Audio::i2s_complete() {
  sendData(&_sampleBuffer[MIC_SAMPLES_PER_PACKET], &_sendBuffer[MIC_SAMPLES_PER_PACKET / 2]);
}
