/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

/**
 * A debounced button class
 */

class Button: public GpioPin {

  public:
    enum CurrentState {
      None,
      Up,
      Down
    };

  private:

    static const uint32_t DEBOUNCE_UP_DELAY_MILLIS = 100;
    static const uint32_t DEBOUNCE_DOWN_DELAY_MILLIS = 1;

    enum InternalState {
      Idle,                         // nothing happening
      DebounceUpDelay,              // delaying...
      DebounceDownDelay
    };

    bool _pressedIsHigh;            // The button is electrically HIGH when pressed?
    InternalState _internalState;   // Internal state of the class

    bool _lastButtonReading;        // the last state we sampled
    uint32_t _transitionTime;       // the time of the last transition

  public:
    Button(const GpioPin &pin, bool pressedIsHigh);
    CurrentState getAndClearCurrentState();   // retrieve current state and reset to idle
};

inline Button::Button(const GpioPin &pin, bool pressedIsHigh) :
    GpioPin(pin) {

  _transitionTime = 0;
  _lastButtonReading = false;
  _pressedIsHigh = pressedIsHigh;
  _internalState = Idle;
}

/**
 * Get and reset the current state. This should be called in the main application loop.
 * @return The current state. If the current state is one of the up/down pressed states
 * then that state is returned and then internally reset to none so the application only
 * gets one 'notification' that the button is pressed/released.
 */

inline Button::CurrentState Button::getAndClearCurrentState() {

  // read the pin and flip it if this switch reads high when open

  bool buttonReading = getState();
  if (!_pressedIsHigh) {
    buttonReading ^= true;
  }

  const uint32_t now = HAL_GetTick();

  if (_lastButtonReading == buttonReading) {

    // no change in the button reading, we could be exiting the debounce delay

    switch (_internalState) {

    case DebounceUpDelay:
      if (now - _transitionTime > DEBOUNCE_UP_DELAY_MILLIS) {
        _internalState = Idle;
        return Up;
      }
      break;

    case DebounceDownDelay:
      if (now - _transitionTime > DEBOUNCE_DOWN_DELAY_MILLIS) {
        _internalState = Idle;
        return Down;
      }
      break;

    case Idle:
      break;
    }

    return None;

  } else {

    // button reading has changed, this always causes the state to enter the debounce delay

    _transitionTime = now;
    _lastButtonReading = buttonReading;
    _internalState = buttonReading ? DebounceDownDelay : DebounceUpDelay;

    return None;
  }
}
