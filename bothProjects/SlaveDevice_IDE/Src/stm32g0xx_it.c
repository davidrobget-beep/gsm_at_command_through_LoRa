/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
#include <buzzer.h>

#include <e220_uart2.h>
#include "modem_uart1.h"
#include "big_messages.h"
#include "modem_state.h"
#include <buttons.h>
#include "adc_measurement.h"

#include "sl_debug.h"

extern struct UART_INSTANCE E220_UART2_;
extern struct UART_INSTANCE MODEM_UART1_;
extern struct Buzzers buzzer;
extern struct Big_Message_Process assembling_big_message;
extern struct MODEM modem_;
extern struct All_Buttons all_buttons;
extern struct ADC_CALCS_ ADC_data;


//extern uint16_t debug_send;				// delete


/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVCall_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{

	E220_UART2_.SysTickHandler();

	MODEM_UART1_.SysTickHandler();

#ifndef _DEBUG_SWD_EN
	buzzer.systick();
#endif

	assembling_big_message.Systick_Handler();

	modem_.Systick_Handler();

	all_buttons.SysTick_Handler();

/*	if ( debug_send )
		debug_send--;
*/

}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_CH1_IRQHandler(void)
{
	ADC_data.Dma_Handler();

}


/**
  * @brief This function handles DMA1 channel 4, channel 5 and DMAMUX1 interrupts.
  */
void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler(void)
{

	E220_UART2_.DMA_Handler();

	// DMAMUX1


}

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART2_LPUART2_IRQHandler(void)
{

	E220_UART2_.UART_IRQHandler();

}


/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_CH2_3_IRQHandler(void)
{

	MODEM_UART1_.DMA_Handler();
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
	MODEM_UART1_.UART_IRQHandler();
}




/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_7) != RESET)
  {
    LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_7);		// GSM_STATUS_Pin
    modem_.Update_Status(0);
  }

  if (LL_EXTI_IsActiveRisingFlag_0_31(LL_EXTI_LINE_7) != RESET)
  {
    LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_LINE_7);		// GSM_STATUS_Pin
    modem_.Update_Status(1);
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
