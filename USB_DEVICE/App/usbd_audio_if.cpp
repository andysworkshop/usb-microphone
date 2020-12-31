#include "Application.h"

static int8_t Audio_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);
static int8_t Audio_DeInit(uint32_t options);
static int8_t Audio_Record();
static int8_t Audio_VolumeCtl(int16_t Volume);
static int8_t Audio_MuteCtl(uint8_t cmd);
static int8_t Audio_Stop();
static int8_t Audio_Pause();
static int8_t Audio_Resume();
static int8_t Audio_CommandMgr(uint8_t cmd);

USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops = { Audio_Init, Audio_DeInit, Audio_Record, Audio_VolumeCtl, Audio_MuteCtl,
    Audio_Stop, Audio_Pause, Audio_Resume, Audio_CommandMgr, };

/**
 * @brief  Initializes the AUDIO media low layer over USB FS IP
 * @param  AudioFreq: Audio frequency used to play the audio stream.
 * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
 * @param  options: Reserved for future use
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */

static int8_t Audio_Init(uint32_t AudioFreq, uint32_t Volume, uint32_t options) {
  HAL_GPIO_WritePin(LINK_LED_GPIO_Port, LINK_LED_Pin, GPIO_PIN_SET);
  return USBD_OK;
}

/**
 * @brief  De-Initializes the AUDIO media low layer
 * @param  options: Reserved for future use
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */

static int8_t Audio_DeInit(uint32_t options) {
  return USBD_OK;
}

/**
 * @brief  Start audio recording engine
 * @retval BSP_ERROR_NONE in case of success, AUDIO_ERROR otherwise
 */

static int8_t Audio_Record() {
  return Audio::_instance->start();
}

/**
 * @brief  Controls AUDIO Volume.
 * @param  vol: volume level
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */

static int8_t Audio_VolumeCtl(int16_t vol) {
  Audio::_instance->setVolume(vol);
  return USBD_OK;
}

/**
 * @brief  Controls AUDIO Mute.
 * @param  cmd: command opcode
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */

static int8_t Audio_MuteCtl(uint8_t cmd) {
  return USBD_OK;
}

/**
 * @brief  Stops audio acquisition
 * @param  none
 * @retval BSP_ERROR_NONE in case of success, AUDIO_ERROR otherwise
 */

static int8_t Audio_Stop() {
  return Audio::_instance->stop();
}

/**
 * @brief  Pauses audio acquisition
 * @param  none
 * @retval BSP_ERROR_NONE in case of success, AUDIO_ERROR otherwise
 */

static int8_t Audio_Pause() {
  return Audio::_instance->pause();
}

/**
 * @brief  Resumes audio acquisition
 * @param  none
 * @retval BSP_ERROR_NONE in case of success, AUDIO_ERROR otherwise
 */

static int8_t Audio_Resume() {
  return Audio::_instance->resume();
}

/**
 * @brief  Manages command from usb
 * @param  None
 * @retval BSP_ERROR_NONE in case of success, AUDIO_ERROR otherwise
 */

static int8_t Audio_CommandMgr(uint8_t cmd) {
  return USBD_OK;
}

/**
 * Implement the HAL interrupt callbacks that process completed milliseconds of data
 */

extern "C" {

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
  Audio::_instance->i2s_halfComplete();
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
  Audio::_instance->i2s_complete();
}

}

