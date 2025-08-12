#include <buzzer.h>
#include <main.h>

static void buzzer_addNoise(uint8_t item);
static void buzzer_systick(void);

static void buzzer_addToQueue(uint8_t item);
static void buzzer_getfromQueue(int8_t * item);
static void buzzer_main(void);
static void buzzer_stopRepeat(void);

// see/fix/check "BUZZER_NOISE..." defines in buzzer.h
const struct noise_types noiseTypes[BUZZER_NOISE_LENGTH] = {

	 {.type = BUZZER_NOISE_TYPE_SINGLE, .period = 170, .pause = 100 },
	 {.type = BUZZER_NOISE_TYPE_SINGLE, .period = 300, .pause = 100 },
	 {.type = BUZZER_NOISE_TYPE_REPEAT, .period = 400, .pause = 350 },

};

struct Buzzers buzzer = {
	
	.freeSpace = BUZZER_QUEUE_LENGTH,
	.FirstIndex = 0,
	.LastIndex = 0,
	
	.addNoise = &buzzer_addNoise,					// user api
	.systick = &buzzer_systick,
	.main = &buzzer_main,
	.stopRepeat = &buzzer_stopRepeat,
	._addToQueue = &buzzer_addToQueue,
	._getFromQueue = &buzzer_getfromQueue,
};


static void buzzer_addNoise(uint8_t item){										// USER API

	if ( noiseTypes[item].type == BUZZER_NOISE_TYPE_SINGLE ){
		
		buzzer._addToQueue(item);
		
	} else {		// == BUZZER_NOISE_TYPE_REPEAT
		buzzer.Q_REPEAT = 1;
	}
	
	if ( !buzzer.isON ){
		if ( buzzer.period )
			buzzer.period = 0;
		buzzer.isON = 1;
	}	

}

static void buzzer_stopRepeat(void){								  			// USER API

	buzzer.Q_REPEAT = 0;
	
}

static void buzzer_systick(void){
		
	if ( buzzer.isON ){
	
		if ( buzzer.period ){
		
			buzzer.period--;
			
			if ( buzzer.state == 0 && buzzer.period < buzzer.pause ){
				buzzer.state = 1;
				STOP_BUZZER_;
			}
		
		}
		
	}
	
	
	/*
	if ( buzzer.turnedOn ){
		if ( buzzer.period )
			buzzer.period--;
		if ( !buzzer.period ){
			LL_GPIO_ResetOutputPin(Buzzer_GPIO_Port, Buzzer_Pin);				// delete from Systick into main func ???
			buzzer.turnedOn = 0;
		}	
	
	}

	*/
	
}

static void buzzer_addToQueue(uint8_t item){

	if ( buzzer.freeSpace ){	// > 0

		buzzer.freeSpace--;
	
		buzzer.QUEUE[buzzer.LastIndex] = item;
		
		if ( buzzer.LastIndex < BUZZER_QUEUE_LENGTH-1 )
			buzzer.LastIndex++;
		else
			buzzer.LastIndex = 0;
		
				
	}
	
}	

static void buzzer_getfromQueue(int8_t * item){

	if ( buzzer.freeSpace != BUZZER_QUEUE_LENGTH ){		// there is even one item in the queue.
	
		buzzer.freeSpace++;
		*item = buzzer.QUEUE[buzzer.FirstIndex];
		
		if ( buzzer.FirstIndex < BUZZER_QUEUE_LENGTH-1 )
			buzzer.FirstIndex++;
		else
			buzzer.FirstIndex = 0;
		
	} else if ( buzzer.Q_REPEAT ) {	// No item is in the queue.
	
		*item = BUZZER_NOISE_REPEAT_400_350;
	} else {
	
		*item = -1;
	}
}

static void buzzer_main(void){

		static int8_t nextItem;
	
		if ( buzzer.isON ){
			if ( !buzzer.period ){		// == 0;  period is gone
			
				
				buzzer._getFromQueue(&nextItem);
				if ( !(nextItem == -1) ){
				
					buzzer.pause = noiseTypes[nextItem].pause;
					buzzer.state = 0;
					START_BUZZER_;
					buzzer.period = noiseTypes[nextItem].period;
					
				} else {	// empty
					
					buzzer.isON = 0;	// Stop
					STOP_BUZZER_;			// not necessary
				}
			
			}
		}
	
}

