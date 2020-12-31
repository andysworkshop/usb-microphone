#include "main.h"
#include "stm32f4xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_spi1_rx;

/**
 * @brief This function handles Non maskable interrupt.
 */

void NMI_Handler() {
  while (1);
}

/**
 * @brief This function handles Hard fault interrupt.
 */

void HardFault_Handler() {
  while (1);
}

/**
 * @brief This function handles Memory management fault.
 */

void MemManage_Handler() {
  while (1);
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */

void BusFault_Handler() {
  while (1);
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */

void UsageFault_Handler() {
  while (1);
}

/**
 * @brief This function handles System service call via SWI instruction.
 */

void SVC_Handler() {
}

/**
 * @brief This function handles Debug monitor.
 */

void DebugMon_Handler() {
}

/**
 * @brief This function handles Pendable request for system service.
 */

void PendSV_Handler() {
}

/**
 * @brief This function handles System tick timer.
 */

void SysTick_Handler() {
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler() {
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
}

/**
 * @brief This function handles USB On The Go FS global interrupt.
 */

void OTG_FS_IRQHandler() {
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}
