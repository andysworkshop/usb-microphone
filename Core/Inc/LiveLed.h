/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

struct LiveLed: Led {
    LiveLed() :
        Led(GpioPin(LIVE_LED_GPIO_Port, LIVE_LED_Pin)) {

      off();
    }
};
