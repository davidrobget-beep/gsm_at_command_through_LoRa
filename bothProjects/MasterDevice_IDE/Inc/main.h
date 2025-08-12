
#ifndef MAIN_H_
#define MAIN_H_


//#define HSI_CLOCK         		16000000U
//#define SYSTICK_TIM_CLK   		HSI_CLOCK

#include "stm32f0xx_hal.h"

#include "stm32f0xx_ll_crc.h"
#include "stm32f0xx_ll_crs.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_gpio.h"

#define E220_900T30D_TX_Pin LL_GPIO_PIN_2
#define E220_900T30D_TX_GPIO_Port GPIOA
#define E220_900T30D_RX_Pin LL_GPIO_PIN_3
#define E220_900T30D_RX_GPIO_Port GPIOA
#define E220_900T30D_M1_Pin LL_GPIO_PIN_4
#define E220_900T30D_M1_GPIO_Port GPIOA
#define E220_900T30D_AUX_Pin LL_GPIO_PIN_5
#define E220_900T30D_AUX_GPIO_Port GPIOA
#define E220_900T30D_M0_Pin LL_GPIO_PIN_6
#define E220_900T30D_M0_GPIO_Port GPIOA

#define BUZZER_Pin LL_GPIO_PIN_7
#define BUZZER_GPIO_Port GPIOA
#define START_BUZZER_ (LL_GPIO_SetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin))
#define STOP_BUZZER_  (LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin))

#define BUTTON_Pin LL_GPIO_PIN_1
#define BUTTON_GPIO_Port GPIOB

void Error_Handler(void);


#endif /* MAIN_H_ */
