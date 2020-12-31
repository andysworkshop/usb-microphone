/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

/**
 * Main program class
 */

class Program {

  private:

    MuteButton _muteButton;
    LiveLed _liveLed;
    Audio _audio;
    GraphicEqualizer _graphicEqualiser;
    VolumeControl _volumeControl;

  public:
    Program();

    void run();
};

inline Program::Program() :
    _audio(_muteButton, _liveLed, _graphicEqualiser, _volumeControl) {
}

inline void Program::run() {

  // infinite loop

  for (;;) {

    // process the mute button

    _muteButton.run();
    _audio.setLed();
  }
}
