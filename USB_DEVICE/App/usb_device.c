#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio_in.h"
#include "usbd_audio_if.h"


USBD_HandleTypeDef hUsbDeviceFS;


void MX_USB_DEVICE_Init() {

  USBD_AUDIO_Init_Microphone_Descriptor(&hUsbDeviceFS, MIC_SAMPLE_FREQUENCY, MIC_NUM_CHANNELS);

  /* Init Device Library, add supported class and start the library. */

  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK ||
      USBD_RegisterClass(&hUsbDeviceFS, &USBD_AUDIO) != USBD_OK ||
      USBD_AUDIO_RegisterInterface(&hUsbDeviceFS, &USBD_AUDIO_fops) != USBD_OK ||
      USBD_Start(&hUsbDeviceFS) != USBD_OK) {
    Error_Handler();
  }
}
