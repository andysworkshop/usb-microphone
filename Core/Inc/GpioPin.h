/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

class GpioPin {

  private:
    GPIO_TypeDef *_port;
    uint16_t _pin;

  private:
    void assign(const GpioPin &src);

  public:
    GpioPin(GPIO_TypeDef *port, uint16_t pin);
    GpioPin(const GpioPin &src);

    void set() const;
    void reset() const;
    void setState(bool state) const;
    bool getState() const;

    GpioPin& operator=(const GpioPin &src);
};

inline GpioPin::GpioPin(GPIO_TypeDef *port, uint16_t pin) {
  _port = port;
  _pin = pin;
}

inline GpioPin::GpioPin(const GpioPin &src) {
  assign(src);
}

inline void GpioPin::set() const {
  HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_SET);
}

inline void GpioPin::reset() const {
  HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);
}

inline bool GpioPin::getState() const {
  return HAL_GPIO_ReadPin(_port, _pin) == GPIO_PIN_SET;
}

inline void GpioPin::setState(bool state) const {
  HAL_GPIO_WritePin(_port, _pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

inline GpioPin& GpioPin::operator=(const GpioPin &src) {
  assign(src);
  return *this;
}

inline void GpioPin::assign(const GpioPin &src) {
  _port = src._port;
  _pin = src._pin;
}
