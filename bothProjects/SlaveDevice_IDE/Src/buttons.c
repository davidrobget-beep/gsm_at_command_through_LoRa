#include <buttons.h>
#include "distance_test.h"

// * * * * * * *   U S E R   P A R T   * * * * * * * * * * * * *
#include "e220_uart2.h"



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


static void initButtons_main( void );
static uint8_t BTN_Get_btn_State(uint8_t btn_index);
static void updateButtonsCounters_systick(void);

struct All_Buttons all_buttons = {

	.buttons = {
			{
				.pressingTime = 0,
				.gpio_port 	  = PING_BUTTON_GPIO_Port,		// * * * * * *   S E T   M E,   U S E R
				.gpio_pin     = PING_BUTTON_Pin,			// * * * * * *   S E T   M E,   U S E R
				.Down_Callback = &PING_button_down_handler, // * * * * * *   S E T   M E,   U S E R

			},
			{
				.pressingTime = 0,
				.gpio_port 	  = SECOND_FUNCTION_BUTTON_GPIO_Port,				// * * * * * *   S E T   M E,   U S E R
				.gpio_pin     = SECOND_FUNCTION_BUTTON_Pin,						// * * * * * *   S E T   M E,   U S E R


#ifndef _DEBUG_SWD_EN
				.Down_Callback = &Second_Function_button_down_handler,			// * * * * * *   S E T   M E,   U S E R
#else
				.Down_Callback = 0,												// debug
#endif

			},
	},

	.pressing_time_ = {
			BUTTON_PRESSING_TIME,
			BUTTON_PRESSING_TIME,
	},

	.remainTimeToStep = BUTTON_DELAY_FOR_STEP,
	.try_to_get_event = 0,

	.get_btn_State   = &BTN_Get_btn_State,
	.SysTick_Handler = &updateButtonsCounters_systick,
	.Init = &initButtons_main,

};

//button`s names which will use in whole application.
/*struct any_button ping_button = {

	.GET_CurState = &ping_btn_State,
	.pressingTime = 0,
	.gpio_port 	  = PING_BUTTON_GPIO_Port,
	.gpio_pin     = PING_BUTTON_Pin,

};*/


//uint8_t EventSubscriprion[EVENT_SUBSCRIPTION_NUMBER];  // correct the number

// buttons's state machine table
// UP - 0
// DOWN - 1
// PRESS - 2
// CLICKED - 3
const uint8_t stateMachineTable_0[4] = {0, 0, 3, 0};      // 
const uint8_t stateMachineTable_1[4] = {1, 2, 2, 1};      //



static void initButtons_main( void ){
      
	for ( uint8_t ii = 0; ii < BUTTONS_NUMBER; ii++ ) {

		// phisical state becomes a logical state
		all_buttons.buttons[ii].currentState = all_buttons.get_btn_State(ii);    // 0 or 1 (UP or DOWN)
	}

}

//

static uint8_t BTN_Get_btn_State(uint8_t btn_index){
  

    return ((all_buttons.buttons[btn_index].gpio_port->IDR &  all_buttons.buttons[btn_index].gpio_pin) == (uint32_t)0);

    
}


static void updateButtonsState_systick2(void){
  
	static uint32_t pressingTimeLoc;
	
	for ( uint8_t ii = 0; ii < BUTTONS_NUMBER; ii++ ) {

      if ( all_buttons.get_btn_State(ii) )
    	  all_buttons.buttons[ii].currentState = stateMachineTable_1[all_buttons.buttons[ii].currentState];
      else
    	  all_buttons.buttons[ii].currentState = stateMachineTable_0[all_buttons.buttons[ii].currentState];
           
      	  	  //private cases    private cases    private cases    private cases
				if ( all_buttons.buttons[ii].pressing ){
				
						pressingTimeLoc = all_buttons.buttons[ii].pressingTime + BUTTON_DELAY_FOR_STEP;
						
						if ( pressingTimeLoc < all_buttons.pressing_time_[ii] )
							all_buttons.buttons[ii].pressingTime = pressingTimeLoc;
						else
							all_buttons.buttons[ii].pressingTime = all_buttons.pressing_time_[ii];
				}
				
			//private cases    private cases    private cases    private cases
			
	}
     
}




static void updateButtonsCounters_systick(void){				// call it from Systick IRQ
  
  if ( all_buttons.remainTimeToStep )
	  all_buttons.remainTimeToStep--;
	else{

		updateButtonsState_systick2();
		
		all_buttons.remainTimeToStep = BUTTON_DELAY_FOR_STEP;


		all_buttons.try_to_get_event = 1;	// try_to_get_event++

/*		if ( ping_button.currentState == BTN_LOG_STATE_DOWN ){    //
				
			EventSubscriprion[PING_Btn_DOWN_01] = 1;               // It must be handled in main
			
				
		} else if ( ping_button.currentState == BTN_LOG_STATE_PRESS ){    //
				
			EventSubscriprion[PING_Btn_PRESS_01] = 1;               // It must be handled in main
			
				
		} else if ( ping_button.currentState == BTN_LOG_STATE_CLICK ){    //
				
			EventSubscriprion[PING_Btn_CLICK_01] = 1;               // It must be handled in main
			
				
		} 
*/

	}
	
}


void checkButtons_logic( void ){							// call it in main loop


	if ( all_buttons.try_to_get_event ){
		all_buttons.try_to_get_event = 0;

		for ( uint8_t ii = 0; ii < BUTTONS_NUMBER; ii++ ) {
		//-------for ( uint8_t ii = 0; ii < 1; ii++ ) {			// delete line; use one above

			if ( all_buttons.buttons[ii].currentState ){	// logic state is NOT "UP"


				if ( all_buttons.buttons[ii].currentState == BTN_LOG_STATE_DOWN ){    //

					all_buttons.buttons[ii].pressingTime = 0;
					all_buttons.buttons[ii].pressing = 1;


					if ( all_buttons.buttons[ii].Down_Callback ){
						all_buttons.buttons[ii].Down_Callback();		// call a User's handler
					}


				} else if ( all_buttons.buttons[ii].currentState == BTN_LOG_STATE_PRESS ){    //

					if ( all_buttons.buttons[ii].pressingTime == all_buttons.pressing_time_[ii] ){


						all_buttons.buttons[ii].pressingTime = 0;


					}


				} else if ( all_buttons.buttons[ii].currentState == BTN_LOG_STATE_CLICK ){    //

					all_buttons.buttons[ii].pressing = 0;


				}

			}

		}
	}

}
