/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

/**
 * Service class to manage the API to the ST GREQ library. This is provided as a closed source
 * but free-to-use library by ST Micro. See UM1798 for details.
 */

class GraphicEqualizer {

  private:
    uint8_t *_greqPersistent;
    uint8_t *_greqScratch;

    buffer_t _greqInput;
    buffer_t _greqOutput;

    // the range of the 10 bands is -12..+12 in 1dB steps
    greq_dynamic_param_t _dynamicParam;

  public:
    GraphicEqualizer();

    void setBand(int8_t index, int8_t value);
    const int16_t* getGainsPerBand() const;

    void process(int16_t *iobuffer, int32_t nSamples);
};

/**
 * Constructor
 */

extern "C" {
extern greq_dynamic_param_t *pEqualizerParams;
}

inline GraphicEqualizer::GraphicEqualizer() {

  // the USB 'C' interface needs access to this

  pEqualizerParams = &_dynamicParam;

  // allocate space required by the SVC library

  _greqPersistent = new uint8_t[greq_persistent_mem_size];  // 548
  _greqScratch = new uint8_t[greq_scratch_mem_size];  // 3840

  // reset the library

  if (greq_reset(_greqPersistent, _greqScratch) != GREQ_ERROR_NONE) {
    Error_Handler();
  }

  greq_static_param_t staticParam;

  // set the number of bands to 10

  staticParam.nb_bands = 10;

  if (greq_setParam(&staticParam, _greqPersistent) != SVC_ERROR_NONE) {
    Error_Handler();
  }

  // buffer constants

  _greqInput.nb_channels = _greqOutput.nb_channels = 2;
  _greqInput.nb_bytes_per_Sample = _greqOutput.nb_bytes_per_Sample = 2;
  _greqInput.mode = _greqOutput.mode = INTERLEAVED;

  // set the bands

  _dynamicParam.enable = 1;
  _dynamicParam.user_gain_per_band_dB[0] = -3;  // 62
  _dynamicParam.user_gain_per_band_dB[1] = -3;  // 115
  _dynamicParam.user_gain_per_band_dB[2] = -3;  // 214
  _dynamicParam.user_gain_per_band_dB[3] = 3;   // 399
  _dynamicParam.user_gain_per_band_dB[4] = 3;   // 742
  _dynamicParam.user_gain_per_band_dB[5] = 3;   // 1380
  _dynamicParam.user_gain_per_band_dB[6] = 3;   // 2567
  _dynamicParam.user_gain_per_band_dB[7] = 3;   // 4775
  _dynamicParam.user_gain_per_band_dB[8] = 3;   // 8882
  _dynamicParam.user_gain_per_band_dB[9] = 3;   // 16520
  _dynamicParam.gain_preset_idx = 0;

  greq_setConfig(&_dynamicParam, _greqPersistent);
}

/**
 * Get a pointer to the gains per band
 */

inline const int16_t* GraphicEqualizer::getGainsPerBand() const {
  return _dynamicParam.user_gain_per_band_dB;
}

/**
 * Update the value of a band
 */

inline void GraphicEqualizer::setBand(int8_t index, int8_t value) {
  _dynamicParam.user_gain_per_band_dB[index] = value;
  greq_setConfig(&_dynamicParam, _greqPersistent);
}

/**
 * Process a sample buffer
 */

inline void GraphicEqualizer::process(int16_t *iobuffer, int32_t nSamples) {

  // initialise the buffer parameters

  _greqInput.buffer_size = _greqOutput.buffer_size = nSamples;

  _greqInput.data_ptr = iobuffer;
  _greqOutput.data_ptr = iobuffer;

  // call the library method

  int32_t error = greq_process(&_greqInput, &_greqOutput, _greqPersistent);
  if (error != GREQ_ERROR_NONE) {
    Error_Handler();
  }
}
