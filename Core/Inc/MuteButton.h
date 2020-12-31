/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

class MuteButton: public Button {

  private:
    bool _pressed;
    bool _muted;

  public:
    MuteButton();

    void run();
    bool isMuted() const;
};

inline MuteButton::MuteButton() :
    Button(GpioPin(MUTE_GPIO_Port, MUTE_Pin), true) {

  _pressed = false;
  _muted = false;
}

inline void MuteButton::run() {

  // only return true the first time that the button is detected as down.
  // further calls will return false until the button is released again.

  if (isPressed()) {
    if (_pressed == false) {
      _pressed = true;
      _muted ^= true;
    }
  } else {
    _pressed = false;
  }
}

inline bool MuteButton::isMuted() const {
  return _muted;
}
