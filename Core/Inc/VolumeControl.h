/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

/**
 * Service class to manage the API to the ST SVC library. This is provided as a closed source
 * but free-to-use library by ST Micro. See UM1642 for details.
 */

class VolumeControl {

  private:
    uint8_t *_svcPersistent;
    uint8_t *_svcScratch;

    buffer_t _svcInput;
    buffer_t _svcOutput;

    svc_dynamic_param_t _dynamicParams;

  public:
    VolumeControl();

    void setMute(bool mute);
    void setVolume(int16_t volume);
    bool isMuted() const;

    void process(int16_t *iobuffer, int32_t nSamples);
};

/**
 * Constructor
 */

inline VolumeControl::VolumeControl() {

  // allocate space required by the SVC library

  _svcPersistent = new uint8_t[svc_persistent_mem_size];  // 1368
  _svcScratch = new uint8_t[svc_scratch_mem_size];  // 2880

  // reset the library

  if (svc_reset(_svcPersistent, _svcScratch) != SVC_ERROR_NONE) {
    Error_Handler();
  }

  // set the static parameters. These defaults are from ST's recommendations.

  svc_static_param_t param;

  param.delay_len = 100;
  param.joint_stereo = 1;

  if (svc_setParam(&param, _svcPersistent) != SVC_ERROR_NONE) {
    Error_Handler();
  }

  // initialise default dynamic params

  _dynamicParams.mute = 0;
  _dynamicParams.target_volume_dB = 72;   // max volume

  // enable compression, high quality, use timings from ST's sample application

  _dynamicParams.enable_compr = 1;
  _dynamicParams.quality = 1;
  _dynamicParams.attack_time = 2103207220;
  _dynamicParams.release_time = 2146924480;

  // buffer constants

  _svcInput.nb_channels = _svcOutput.nb_channels = 2;
  _svcInput.nb_bytes_per_Sample = _svcOutput.nb_bytes_per_Sample = 2;
  _svcInput.mode = _svcOutput.mode = INTERLEAVED;

  // set the initial volume

  setVolume(1);
}

/**
 * Set the volume level. The range is -80db to +36db. Positive values amplify, negative
 * values attenuate. The range is in 0.5dB steps therefore volume parameter range is -160..72
 */

inline void VolumeControl::setVolume(int16_t volume) {

  _dynamicParams.target_volume_dB = volume;

  int32_t error = svc_setConfig(&_dynamicParams, _svcPersistent);
  if (error != SVC_ERROR_NONE) {
    Error_Handler();
  }
}

/**
 * Set the muted state
 */

inline void VolumeControl::setMute(bool mute) {

  _dynamicParams.mute = mute ? 1 : 0;

  int32_t error = svc_setConfig(&_dynamicParams, _svcPersistent);
  if (error != SVC_ERROR_NONE) {
    Error_Handler();
  }
}

/**
 * Get the mute state
 */

inline bool VolumeControl::isMuted() const {
  return _dynamicParams.mute == 1;
}

/**
 * Process a sample buffer
 */

inline void VolumeControl::process(int16_t *iobuffer, int32_t nSamples) {

  // initialise the buffer parameters

  _svcInput.buffer_size = _svcOutput.buffer_size = nSamples;

  _svcInput.data_ptr = iobuffer;
  _svcOutput.data_ptr = iobuffer;

  // call the library method

  int32_t error = svc_process(&_svcInput, &_svcOutput, _svcPersistent);
  if (error != SVC_ERROR_NONE) {
    Error_Handler();
  }
}
