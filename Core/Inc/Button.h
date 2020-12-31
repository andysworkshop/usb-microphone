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

  private:

    static const uint32_t DEBOUNCE_DELAY_MILLIS = 50;

    enum InternalState {
      Idle,                         // nothing happening
      DebounceDelay                 // delaying...
    };

    bool _pressedState;             // The pressed state
    InternalState _internalState;   // Internal state of the class

    uint32_t _lastTime;             // The last time we sampled our button

  public:

    Button(const GpioPin &pin, bool pressedState);
    bool isPressed();
};

inline Button::Button(const GpioPin &pin, bool pressedState) :
    GpioPin(pin) {

  _lastTime = 0;
  _pressedState = pressedState;
  _internalState = Idle;
}

/**
 * Get the current state
 * @return The current state
 */

inline bool Button::isPressed() {

  uint32_t newTime;
  bool state;

  // read the pin and flip it if this switch reads high when open

  state = getState();

  if (_pressedState) {
    state ^= true;
  }

  // if state is low then wherever we were then
  // we are now back at not pressed

  if (!state) {
    _internalState = Idle;
    return false;
  }

  // sample the counter

  newTime = HAL_GetTick();

  // act on the internal state machine

  switch (_internalState) {
  case Idle:
    _internalState = DebounceDelay;
    _lastTime = newTime;
    break;

  case DebounceDelay:
    if (newTime - _lastTime >= DEBOUNCE_DELAY_MILLIS) {

      // been high for at least the debounce time

      return true;
    }
    break;
  }

  // nothing happened at this time

  return false;
}
