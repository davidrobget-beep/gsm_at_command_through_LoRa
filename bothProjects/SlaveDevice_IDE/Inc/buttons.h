#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <main.h>
#include "stdint.h"

#include "sl_debug.h"


// * * * * * *   S E T   M E,   U S E R   * * * * * * * * * *

#define BUTTONS_NUMBER 		2


//#define BTN_PING_INDEX		0
//#define BTN_SECOND_INDEX	1			// change name
#define BUTTON_PRESSING_TIME	2000

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#define BUTTON_DELAY_FOR_STEP   50       // ms

// 
#define BTN_LOG_STATE_UP        (uint8_t)0      //logical
#define BTN_LOG_STATE_DOWN      (uint8_t)1      //logical
#define BTN_LOG_STATE_PRESS     (uint8_t)2      //logical
#define BTN_LOG_STATE_CLICK     (uint8_t)3      //logical
//#define BTN_PHISICAL_STATE      (uint8_t)4      //phisical

#define BTN_EVENT_DOWN		1
#define BTN_EVENT_PRESSED	2
#define BTN_EVENT_CLICKED	4

struct any_button{
  //uint8_t (*GET_CurState)(uint8_t);         //
  uint8_t currentState;                 // buttons's state machine
  uint8_t pressing;                     // 1 or 0
  uint32_t pressingTime;                // how long the button is pressed;

  //
  void (* Down_Callback)(void);
  //void (* Press_Callback)(void);
  //void (* Click_Callback)(void);
  GPIO_TypeDef * gpio_port;
  uint32_t		 gpio_pin;

};



struct All_Buttons {

	struct any_button buttons[BUTTONS_NUMBER];
	const  uint16_t pressing_time_[BUTTONS_NUMBER];

	uint8_t remainTimeToStep;
	uint16_t try_to_get_event;

	void (* Init)(void);

	uint8_t (* get_btn_State)(uint8_t btn_index);
	void (*SysTick_Handler)(void);

};




void checkButtons_logic( void );

//****************************************************************

//     EventSubscription
/*#define PING_Btn_DOWN_01               0  //
#define PING_Btn_PRESS_01              1  //
#define PING_Btn_CLICK_01              2  //

#define EVENT_SUBSCRIPTION_NUMBER 3
*/
//****************************************************************

#endif
