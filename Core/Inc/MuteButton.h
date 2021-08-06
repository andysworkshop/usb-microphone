/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

class MuteButton: public Button {

  private:
    volatile bool _muted;
    bool _ignoreNextUp;

  public:
    MuteButton();

    void run();
    bool isMuted() const;
};

inline MuteButton::MuteButton() :
    Button(GpioPin(MUTE_GPIO_Port, MUTE_Pin), false) {

  _muted = false;
  _ignoreNextUp = false;
}

inline void MuteButton::run() {

  Button::CurrentState state = getAndClearCurrentState();

  // check for idle

  if (state == Button::CurrentState::None) {
    return;
  }

  if (state == Down) {

    if (!_muted) {
      _muted = true;
      _ignoreNextUp = true;   // the lifting of the button shouldn't exit mute
    }
  }
  else {

    if (_muted) {

      if (_ignoreNextUp) {

        // this is the lifting of the button that went into mute

        _ignoreNextUp = false;
      }
      else {
        _muted = false;
      }
    }
  }
}

inline bool MuteButton::isMuted() const {
  return _muted;
}
