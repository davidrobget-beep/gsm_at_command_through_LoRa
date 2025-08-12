#ifndef _BUZZER_H
#define _BUZZER_H

/*
 * 		# 0. add defines START_BUZZER_ and STOP_BUZZER_ to manage Buzzer's pin
		# 1. add buzzer.main(); 	into main loop
		# 2. add buzzer.systick();  into Systick handler

*/

#include <stdint.h>

//#define BUZZER_NOISE_PERIOD_70 70
//#define BUZZER_NOISE_PERIOD_200 200

#define BUZZER_NOISE_LENGTH 3
#define BUZZER_NOISE_SINGLE_170_100 0			//70ms noise --- 100ms pause
#define BUZZER_NOISE_SINGLE_300_100 1
#define BUZZER_NOISE_REPEAT_400_350 2


#define BUZZER_NOISE_TYPE_SINGLE ((uint8_t)0)
#define BUZZER_NOISE_TYPE_REPEAT ((uint8_t)1)

struct noise_types{

	 uint8_t type;
	 uint16_t period;
	 uint16_t pause;
};

#define BUZZER_QUEUE_LENGTH 7

struct Buzzers {

	uint8_t isON;			// Global ON
	//uint8_t turnedOn;	// Local  ON; full cycle, whole period.
	uint16_t period;
	uint16_t pause;
	uint8_t  state;			// 0 - noise; 1 - pause

	// ----------------
	uint8_t freeSpace;										// this var can be deleted ! Use ( if ( .FirstIndex != .LastIndex ) ) instead of it.
	uint8_t QUEUE[BUZZER_QUEUE_LENGTH];		// for single noises
	uint8_t Q_REPEAT;											// flag for repeated noises
	uint8_t FirstIndex;
	uint8_t LastIndex;
	
	// ----------------
	void (*addNoise)(uint8_t item);
	void (*systick)(void);
	void (*main)(void);		// put to Main while
	void (*stopRepeat)(void);
	// ----------------
	void (*_addToQueue)(uint8_t item);
	void (*_getFromQueue)(int8_t * item);
	
};

#endif
