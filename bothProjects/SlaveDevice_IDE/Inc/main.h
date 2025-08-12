/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_crc.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GSM_RX_Pin LL_GPIO_PIN_7
#define GSM_RX_GPIO_Port GPIOB

#define GSM_V_REGULATOR_Enable_Pin LL_GPIO_PIN_14
#define GSM_V_REGULATOR_Enable_GPIO_Port GPIOC
#define GSM_V_REGULATOR_TURN_OFF  LL_GPIO_SetOutputPin(GSM_V_REGULATOR_Enable_GPIO_Port, GSM_V_REGULATOR_Enable_Pin)
#define GSM_V_REGULATOR_TURN_ON   LL_GPIO_ResetOutputPin(GSM_V_REGULATOR_Enable_GPIO_Port, GSM_V_REGULATOR_Enable_Pin)

#define E220_900T30D_AUX_Pin LL_GPIO_PIN_15
#define E220_900T30D_AUX_GPIO_Port GPIOC
#define E220_900T30D_M0_Pin LL_GPIO_PIN_0
#define E220_900T30D_M0_GPIO_Port GPIOA
#define E220_900T30D_M1_Pin LL_GPIO_PIN_1
#define E220_900T30D_M1_GPIO_Port GPIOA
#define E220_900T30D_TX_Pin LL_GPIO_PIN_2
#define E220_900T30D_TX_GPIO_Port GPIOA
#define E220_900T30D_RX_Pin LL_GPIO_PIN_3
#define E220_900T30D_RX_GPIO_Port GPIOA
#define KTY_81_110_Pin LL_GPIO_PIN_4
#define KTY_81_110_GPIO_Port GPIOA

#define PING_BUTTON_Pin LL_GPIO_PIN_5
#define PING_BUTTON_GPIO_Port GPIOA
#define SECOND_FUNCTION_BUTTON_Pin LL_GPIO_PIN_13
#define SECOND_FUNCTION_BUTTON_GPIO_Port GPIOA

#define LiPo_2S_checkVoltage_Pin LL_GPIO_PIN_6
#define LiPo_2S_checkVoltage_GPIO_Port GPIOA

#define GSM_STATUS_Pin LL_GPIO_PIN_7
#define GSM_STATUS_GPIO_Port GPIOA
#define GSM_STATUS_EXTI_IRQn EXTI4_15_IRQn

#define GSM_POWER_KEY_Pin LL_GPIO_PIN_0
#define GSM_POWER_KEY_GPIO_Port GPIOB

#define GSM_CTS_Pin LL_GPIO_PIN_11
#define GSM_CTS_GPIO_Port GPIOA
#define GSM_RTS_Pin LL_GPIO_PIN_12
#define GSM_RTS_GPIO_Port GPIOA


#define BUZZER_Pin LL_GPIO_PIN_14
#define BUZZER_GPIO_Port GPIOA
#define START_BUZZER_ (LL_GPIO_SetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin))
#define STOP_BUZZER_  (LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin))

#define GSM_TX_Pin LL_GPIO_PIN_6
#define GSM_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
