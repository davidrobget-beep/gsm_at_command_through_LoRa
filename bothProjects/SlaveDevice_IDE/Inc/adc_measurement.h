#ifndef ADC_MEASUREMENT_H_
#define ADC_MEASUREMENT_H_

#include <stdint.h>

#define ADC_BUFFER_LEN 4

#define ADC_CH_KTY_81 		0
#define ADC_CH_LIPO_2S 		1
#define ADC_CH_MCU_TEMP 	2
#define ADC_CH_VREFINT 		3

struct ADC_CALCS_ {

	uint16_t adc_buffer[ADC_BUFFER_LEN];
	float values[ADC_BUFFER_LEN];				// human-readable

	uint8_t try_to_convert;

	int32_t TS_CAL_SUB;							// precalculated value for Internal Temperature calculation

	void (*Init)(void);
	void (*Dma_Handler)(void);
	void (*Main_Handler)(void);

};


#endif
