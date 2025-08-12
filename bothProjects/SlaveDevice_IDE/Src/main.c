/**
  ******************************************************************************
  * @file    main.c
  * @author
  * @version V1.0
  * @brief   Default main function.
  ******************************************************************************
*/

#include "main.h"
//#include "stm32g031xx.h"
#include "e220_uart2.h"
#include "modem_uart1.h"

#include "sl_debug.h"
#include "buzzer.h"
#include "queue_m.h"
#include "adc_measurement.h"
#include "modem_state.h"
#include <buttons.h>

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_CRC_Init(void);


extern struct Buzzers buzzer;
extern struct UART_INSTANCE E220_UART2_;
extern struct UART_INSTANCE MODEM_UART1_;
extern struct QUEUE_MANAGER queueManager;
extern struct ADC_CALCS_ ADC_data;
extern struct MODEM modem_;


/*#define TTT4000 4000
uint16_t debug_send = TTT4000;
*/
int main(void)
{



  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriority(SysTick_IRQn, 3);

  SystemClock_Config();



  MX_GPIO_Init();
  MX_DMA_Init();
  E220_UART2_.Init();
  MODEM_UART1_.Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_CRC_Init();

  LL_SYSTICK_EnableIT();


  E220_UART2_.ReceiveEnable(E220_UART2_RECEIVE_RING_BUFFER_LEN);
  MODEM_UART1_.ReceiveEnable(MODEM_UART1_RECEIVE_RING_BUFFER_LEN);

#ifndef _DEBUG_SWD_EN
  buzzer.addNoise(BUZZER_NOISE_SINGLE_170_100);
#endif

  LL_TIM_EnableCounter(TIM3);

  ADC_data.Init();

  while (1)
  {


	  E220_UART2_.Receive();
	  MODEM_UART1_.Receive();

#ifndef _DEBUG_SWD_EN
	  buzzer.main();
#endif

	  queueManager.task();		// transmission_task();

	  modem_.Main();

	  checkButtons_logic();

	  ADC_data.Main_Handler();


/*	  if ( debug_send == 0 ){
	  	debug_send = TTT4000;

	  			//e220_900t30d.Transmit("rast874351", 10, TR_TYPE_TRANSPARENT);

	  	MY_UART_SetTransmissionBuffer("rast874351", 10);
	  	MY_UART_Transmit(10);


	  		}
*/

  }

}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  }

  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_4);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Sysclk activation on the main PLL */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  LL_Init1msTick(32000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

	  /* USER CODE BEGIN ADC1_Init 0 */

// direction
// adc_buffer[0]	LL_ADC_CHANNEL_4           KTY_81_110_Pin
// adc_buffer[1]	LL_ADC_CHANNEL_6           LiPo_2S_checkVoltage_Pin
// adc_buffer[2]	LL_ADC_CHANNEL_TEMPSENSOR
// adc_buffer[3]	LL_ADC_CHANNEL_VREFINT

	  /* USER CODE END ADC1_Init 0 */

	  LL_ADC_InitTypeDef ADC_InitStruct = {0};
	  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

	  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* Peripheral clock enable */
	  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);

	  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	  /**ADC1 GPIO Configuration
	  PA4   ------> ADC1_IN4
	  PA6   ------> ADC1_IN6
	  */
	  GPIO_InitStruct.Pin = KTY_81_110_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  LL_GPIO_Init(KTY_81_110_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LiPo_2S_checkVoltage_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  LL_GPIO_Init(LiPo_2S_checkVoltage_GPIO_Port, &GPIO_InitStruct);

	  /* ADC1 DMA Init */

	  /* ADC1 Init */
	  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMAMUX_REQ_ADC1);

	  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);

	  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);

	  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

	  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

	  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

	  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

	  /* USER CODE BEGIN ADC1_Init 1 */

	  /* USER CODE END ADC1_Init 1 */

	  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	  */

	   #define ADC_CHANNEL_CONF_RDY_TIMEOUT_MS ( 1U)
	   #if (USE_TIMEOUT == 1)
	   uint32_t Timeout ; /* Variable used for Timeout management */
	   #endif /* USE_TIMEOUT */

	  ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
	  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
	  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
	  ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
	  LL_ADC_Init(ADC1, &ADC_InitStruct);

	  LL_ADC_REG_SetSequencerConfigurable(ADC1, LL_ADC_REG_SEQ_FIXED);

	  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_EXT_TIM3_TRGO;
	  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
	  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
	  //temporary because the Calibration must be complete;
	  // it will be set to LL_ADC_REG_DMA_TRANSFER_UNLIMITED the Calibration will be completed
	  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;	// LL_ADC_REG_DMA_TRANSFER_UNLIMITED;

	  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
	  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

	  LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
	  LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
	  LL_ADC_SetTriggerFrequencyMode(ADC1, LL_ADC_CLOCK_FREQ_MODE_HIGH);
	  LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_4|LL_ADC_CHANNEL_6
	                              |LL_ADC_CHANNEL_TEMPSENSOR|LL_ADC_CHANNEL_VREFINT);

	   /* Poll for ADC channel configuration ready */
	   #if (USE_TIMEOUT == 1)
	   Timeout = ADC_CHANNEL_CONF_RDY_TIMEOUT_MS;
	   #endif /* USE_TIMEOUT */
	   while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0)
	     {
	   #if (USE_TIMEOUT == 1)
	   /* Check Systick counter flag to decrement the time-out value */
	   if (LL_SYSTICK_IsActiveCounterFlag())
	     {
	   if(Timeout-- == 0)
	         {
	   Error_Handler();
	         }
	     }
	   #endif /* USE_TIMEOUT */
	     }
	   /* Clear flag ADC channel configuration ready */
	   LL_ADC_ClearFlag_CCRDY(ADC1);
	  LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);
	  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_AWD_CH_VREFINT_REG|LL_ADC_AWD_CH_TEMPSENSOR_REG);
	  LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_1, LL_ADC_SAMPLINGTIME_160CYCLES_5);
	  LL_ADC_DisableIT_EOC(ADC1);
	  LL_ADC_DisableIT_EOS(ADC1);

	   /* Enable ADC internal voltage regulator */
	   LL_ADC_EnableInternalRegulator(ADC1);
	   /* Delay for ADC internal voltage regulator stabilization. */
	   /* Compute number of CPU cycles to wait for, from delay in us. */
	   /* Note: Variable divided by 2 to compensate partially */
	   /* CPU processing cycles (depends on compilation optimization). */
	   /* Note: If system core clock frequency is below 200kHz, wait time */
	   /* is only a few CPU processing cycles. */
	   uint32_t wait_loop_index;
	   wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
	   while(wait_loop_index != 0)
	     {
	   wait_loop_index--;
	     }


	   LL_ADC_StartCalibration(ADC1);
	   while ( LL_ADC_IsCalibrationOnGoing(ADC1) )
		   ;
	   //LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES		// lines below is instead of this one  ( before LL_ADC_Enable )

	   // only after the Calibration
	   //   see NOTE in 15.5.5 Managing converted data using the DMA of stm32g031 RefManual
	   LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);			// !!!

  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, (uint32_t)(&(ADC1->DR)), (uint32_t)ADC_data.adc_buffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, ADC_BUFFER_LEN);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    LL_ADC_Enable(ADC1);

    LL_ADC_REG_StartConversion(ADC1);

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
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
  LL_CRC_SetPolynomialCoef(CRC, LL_CRC_DEFAULT_CRC32_POLY);
  LL_CRC_SetPolynomialSize(CRC, LL_CRC_POLYLENGTH_32B);
  LL_CRC_SetInitialData(CRC, LL_CRC_DEFAULT_CRC_INITVALUE);
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TIM_InitStruct.Prescaler = 31999;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 332;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_UPDATE);
  LL_TIM_EnableMasterSlaveMode(TIM3);
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}


/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */


/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
  /* DMA1_Ch4_5_DMAMUX1_OVR_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Ch4_5_DMAMUX1_OVR_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Ch4_5_DMAMUX1_OVR_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /**/
  GSM_V_REGULATOR_TURN_OFF;

  /**/
  LL_GPIO_ResetOutputPin(E220_900T30D_M0_GPIO_Port, E220_900T30D_M0_Pin);		//   E220 in
  LL_GPIO_ResetOutputPin(E220_900T30D_M1_GPIO_Port, E220_900T30D_M1_Pin);		//  transmission mode

  /**/
  LL_GPIO_ResetOutputPin(GSM_POWER_KEY_GPIO_Port, GSM_POWER_KEY_Pin); // button is not pressed

  /**/
  LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);

  /**/
  GPIO_InitStruct.Pin = GSM_V_REGULATOR_Enable_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GSM_V_REGULATOR_Enable_GPIO_Port, &GPIO_InitStruct);

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
  GPIO_InitStruct.Pin = E220_900T30D_M1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(E220_900T30D_M1_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = PING_BUTTON_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(PING_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = GSM_POWER_KEY_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GSM_POWER_KEY_GPIO_Port, &GPIO_InitStruct);

  /**/

  /**/
    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA, LL_EXTI_CONFIG_LINE7);

    /**/
    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_7;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    /**/
    LL_GPIO_SetPinPull(GSM_STATUS_GPIO_Port, GSM_STATUS_Pin, LL_GPIO_PULL_DOWN);

    /**/
    LL_GPIO_SetPinMode(GSM_STATUS_GPIO_Port, GSM_STATUS_Pin, LL_GPIO_MODE_INPUT);

    /* EXTI interrupt init*/
    NVIC_SetPriority(EXTI4_15_IRQn, 0);
    NVIC_EnableIRQ(EXTI4_15_IRQn);


#ifndef _DEBUG_SWD_EN
  GPIO_InitStruct.Pin = SECOND_FUNCTION_BUTTON_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(SECOND_FUNCTION_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);
#endif

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
