#pragma once

#include "usbd_audio_in.h"

#define MIC_SAMPLE_FREQUENCY 48000
#define MIC_SAMPLES_PER_MS (MIC_SAMPLE_FREQUENCY/1000)  // == 48
#define MIC_NUM_CHANNELS 1
#define MIC_MS_PER_PACKET 20
#define MIC_SAMPLES_PER_PACKET (MIC_SAMPLES_PER_MS * MIC_MS_PER_PACKET) // == 960

extern USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops;
