#include "distance_test.h"
#include <stdint.h>
//#include <math.h>
#include <buzzer.h>

extern struct Buzzers buzzer;

uint16_t Current_distance_number_milliseconds = 2211;


void Second_Function_button_down_handler(void){

	uint8_t noise_number;

	noise_number = Current_distance_number_milliseconds / 1000 + 1;

	for ( uint8_t ii = 0; ii < noise_number; ii++ )
		buzzer.addNoise(BUZZER_NOISE_REPEAT_600_500);


}

