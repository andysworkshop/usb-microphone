/**
  ******************************************************************************
  * @file    svc_glo.h
  * @author  MCD Application Team
  * @version V1.0.4
  * @date    15-Oct-2017
  * @brief   This file contains API for Smart Volume Control library (SVC)
  *          For more details about this Library, please refer to document
  *          "UM1642 User manual". 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Image SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_image_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SVC_GLO_H
#define SVC_GLO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "audio_fw_glo.h"

/* Exported constants --------------------------------------------------------*/

#define SVC_MUTE_NOT_ACTIVATED              0
#define SVC_MUTE_ACTIVATED                 (SVC_MUTE_NOT_ACTIVATED + 1)
#define SVC_DISABLE_EFFECT                  0
#define SVC_ENABLE_EFFECT                  (SVC_DISABLE_EFFECT + 1)


/*      E R R O R   V A L U E S                                               */
#define SVC_ERROR_NONE                              0
#define SVC_UNSUPPORTED_DELAY_LENGTH               -1
#define SVC_UNSUPPORTED_VOLUME                     -2
#define SVC_UNSUPPORTED_MUTE_MODE                  -3
#define SVC_UNSUPPORTED_QUALITY_MODE               -4        
#define SVC_UNSUPPORTED_JOINT_STEREO_MODE          -5        
#define SVC_UNSUPPORTED_NUMBER_OF_BYTEPERSAMPLE    -6
#define SVC_BAD_HW                                 -7

/* Exported types ------------------------------------------------------------*/

struct svc_dynamic_param {
    int16_t target_volume_dB;    /* in dB    */
    int16_t mute;                /* 0 = unmute, 1 = mute */
    int16_t enable_compr;
    int32_t attack_time;
    int32_t release_time;
    int16_t quality;             /* 0:STANDARD, 1:HIGH_Q */
};
typedef struct svc_dynamic_param svc_dynamic_param_t;

struct svc_static_param {
  int16_t delay_len;
  int16_t joint_stereo;
};
typedef struct svc_static_param svc_static_param_t;

/* External variables --------------------------------------------------------*/

extern const uint32_t svc_scratch_mem_size;
extern const uint32_t svc_persistent_mem_size;

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

/* svc_reset() : initializes static memory, states machines, ... */
extern int32_t svc_reset(void *persistent_mem_ptr, void *scratch_mem_ptr);
/* svc_setConfig() : use to change dynamically some parameters */
extern int32_t svc_setConfig(svc_dynamic_param_t *input_dynamic_param_ptr, void *persistent_mem_ptr);
/* svc_getConfig() : use to get values of dynamic parameters */
extern int32_t svc_getConfig(svc_dynamic_param_t *input_dynamic_param_ptr, void *persistent_mem_ptr);
/* svc_setParam() : use to set parameters that won't change during processing */
extern int32_t svc_setParam(svc_static_param_t *input_static_param_ptr, void *persistent_mem_ptr);
/* svc_getParam() : use to get values of static parameters */
extern int32_t svc_getParam(svc_static_param_t *input_static_param_ptr, void *persistent_mem_ptr);
/* svc_process() : this is the main processing routine */
extern int32_t svc_process(buffer_t *input_buffer, buffer_t *output_buffer, void *persistent_mem_ptr);

#ifdef __cplusplus
}
#endif

#endif // SVC_GLO_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
