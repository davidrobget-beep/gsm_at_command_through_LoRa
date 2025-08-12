/**
  ******************************************************************************
  * @file    main.c
  * @author  
  * @version V1.0
  * @brief   Default main function.
  ******************************************************************************
*/

#include "main.h"
#include <stdint.h>
#include "stm32f070x6.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#include "my_uart.h"
#include "e220_900t30d.h"
#include "buzzer.h"
//#include "crc_.h"
#include "my_usb.h"
#include "queue_m.h"

extern struct E220_900T30D e220_900t30d;
extern struct Buzzers buzzer;
extern struct MY_USB_ myUsb_;
extern struct QUEUE_MANAGER queueManager;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CRC_Init(void);


//void Parse_bytes_main(void);


//test
#include "crc_.h"
uint8_t deb12[7] = {0x01, 0x00, 0x05 , 0x06, 0x07, 0x08, 0x09};
static volatile union paramLL CRC_VAL_12;

int main(void)
{
	
	HAL_Init();

	NVIC_SetPriority(SysTick_IRQn, 3);

	SystemClock_Config();



	MX_DMA_Init();
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	e220_900t30d.Init();
	MX_USB_DEVICE_Init();
	MX_CRC_Init();

	LL_SYSTICK_EnableIT();

	buzzer.addNoise(BUZZER_NOISE_SINGLE_170_100);
  

	//test
	CalculateCRC_32bit(deb12, 7, ((union paramLL *)&CRC_VAL_12));				// delete

	for(;;){

		e220_900t30d.Receive();					// uart

		buzzer.main();

		myUsb_.Myusb_receive_task();
		//myUsb_.Myusb_transmission_task();

		queueManager.task();		// transmission_task();

/*		if ( debug_send == 0 ){
			debug_send = TTT4000;

			debug_mes[debug_index] = debug_Value++;
			CalculateCRC_32bit(debug_mes, 6, &CRC_VAL);
			debug_mes[debug_index+1] = CRC_VAL.par_bytes[0];
			debug_mes[debug_index+2] = CRC_VAL.par_bytes[1];
			debug_mes[debug_index+3] = CRC_VAL.par_bytes[2];
			debug_mes[debug_index+4] = CRC_VAL.par_bytes[3];

			e220_900t30d.Transmit((char*)debug_mes, 10, TR_TYPE_TRANSPARENT);


		}
*/
			//                    CDC_Transmit_FS(testMess, 8);




	}

}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_6, LL_RCC_PREDIV_DIV_1);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(48000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL);
}


static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* Peripheral clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_NONE);
  LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_NONE);
  LL_CRC_SetInitialData(CRC, LL_CRC_DEFAULT_CRC_INITVALUE);
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}


static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(E220_900T30D_M1_GPIO_Port, E220_900T30D_M1_Pin);	//   E220 in
  LL_GPIO_ResetOutputPin(E220_900T30D_M0_GPIO_Port, E220_900T30D_M0_Pin);	//  transmission mode


  /**/
  LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);

  /**/
  GPIO_InitStruct.Pin = E220_900T30D_M1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(E220_900T30D_M1_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = E220_900T30D_AUX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(E220_900T30D_AUX_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = E220_900T30D_M0_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(E220_900T30D_M0_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

void HAL_MspInit(void)
{

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();


  __HAL_REMAP_PIN_ENABLE(HAL_REMAP_PA11_PA12);

}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
