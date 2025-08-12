#include "adc_measurement.h"
#include "main.h"
#include <kty_81_110.h>

extern struct KTY_81_110 kty81_110;


static void ADC_Init(void);
static void ADC_Dma_Handler(void);
static void ADC_Main_Handler(void);

struct ADC_CALCS_ ADC_data = {

	//.adc_buffer
	//.values
	.Init		 = &ADC_Init,					// put into main.c befor main loop
	.Dma_Handler = &ADC_Dma_Handler,			// ? delete
	.Main_Handler = &ADC_Main_Handler,			// put into main loop

	//.try_to_convert

};

static void ADC_Init(void){

	ADC_data.TS_CAL_SUB = *TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR;

}

static void ADC_Dma_Handler(void){

	if ( LL_DMA_IsActiveFlag_TC1(DMA1) ) {

		LL_DMA_ClearFlag_TC1(DMA1);
		ADC_data.try_to_convert = 1;


	} else if ( LL_DMA_IsActiveFlag_TE1(DMA1) ){

		LL_DMA_ClearFlag_TE1(DMA1);

	}


}


static void ADC_Main_Handler(void){			// main loop

	if ( ADC_data.try_to_convert ){
		ADC_data.try_to_convert = 1;

		/* 1. REAL VDD IN MCU */    //// raw adc data into human-readable data
		ADC_data.values[ADC_CH_VREFINT] = (float)((3.0)*((float)(*VREFINT_CAL_ADDR)/(float)ADC_data.adc_buffer[ADC_CH_VREFINT]));


		/* 2. INTERNAL TEMPERATURE */
		{
		//to match 3.0V-calibrated values, I must scale a raw value up ( 3.3 scaled to 3.0 )

		static float TS_CAL_TEMP_SUB = (float)TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP;
		static int32_t adc__data_Scaled;
		//adc__data_new = ADC_data.adc_buffer[ADC_CH_MCU_TEMP] * (int32_t)(1000*ADC_data.values[ADC_CH_VREFINT]) / TEMPSENSOR_CAL_VREFANALOG;	// (X * 3300)/3000
		adc__data_Scaled = (ADC_data.adc_buffer[ADC_CH_MCU_TEMP] * ADC_data.values[ADC_CH_VREFINT]) / 3;										// (X * 3.3)/3

		ADC_data.values[ADC_CH_MCU_TEMP] = ( ( TS_CAL_TEMP_SUB ) * ( adc__data_Scaled - *TEMPSENSOR_CAL1_ADDR ) ) / (  ADC_data.TS_CAL_SUB  ) + TEMPSENSOR_CAL1_TEMP;

		// or this macro
		//ADC_data.values[ADC_CH_MCU_TEMP] = __LL_ADC_CALC_TEMPERATURE( (int32_t)(1000*ADC_data.values[ADC_CH_VREFINT]), ADC_data.adc_buffer[ADC_CH_MCU_TEMP], LL_ADC_RESOLUTION_12B);
		}

		/* 3. KTY-81-110 SENSOR */
		/* if I will use filters
			1. Take 5 raw readings (fast sampling, say every 1s).
			2. Apply median filter to remove spike.
			3. Then average remaining values.
			4. Send result every 5s.
		 */
		kty81_110.CurrentState = kty81_110.get_temp( &(ADC_data.values[ADC_CH_KTY_81]),  ADC_data.adc_buffer[ADC_CH_KTY_81] );


		/* 4. LI-PO 2S Battery Voltage Detector*/
		/* R1 = 10K 1% ( on GND, on ADC )  ; R2 = 27K 1%*/
		/* X - Voltage dropped on R1 and R2  */
		/* Y - V dropped on R1 */
		/* X = Y * 37 / 10    ==> X = Y * 3.7 */
		// Battery Voltage
		ADC_data.values[ADC_CH_LIPO_2S] = 3.7 * (ADC_data.values[ADC_CH_VREFINT] / 4095 * ADC_data.adc_buffer[ADC_CH_LIPO_2S]);

	}


}
