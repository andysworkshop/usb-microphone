/*
 * This file is part of the firmware for the Andy's Workshop USB Microphone.
 * Copyright 2021 Andy Brown. See https://andybrown.me.uk for project details.
 * This project is open source subject to the license published on https://andybrown.me.uk.
 */

#pragma once

#include "stm32f4xx_hal.h"

void Error_Handler();

#define MUTE_Pin GPIO_PIN_2
#define MUTE_GPIO_Port GPIOA
#define LINK_LED_Pin GPIO_PIN_12
#define LINK_LED_GPIO_Port GPIOB
#define LIVE_LED_Pin GPIO_PIN_13
#define LIVE_LED_GPIO_Port GPIOB
#define LR_GPIO_Port GPIOB
#define LR_Pin GPIO_PIN_1
