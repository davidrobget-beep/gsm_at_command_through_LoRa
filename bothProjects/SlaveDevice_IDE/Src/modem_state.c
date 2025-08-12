#include "modem_state.h"
#include "main.h"
#include "queue_m.h"
#include "answers_e220_900t30d.h"


extern struct QUEUE_MANAGER queueManager;
extern const struct answer_  e220_900t30d_Answers[];
extern struct __VarialbeAnswer VarialbeAnswer;


static struct modem_cmd_result modem_SwitchPower(uint8_t newState);
static void modem_Press_PWRKEY(void);
static void modem_Finish_Press_PWRKEY(void);
static void modem_Update_Status(uint8_t NewState);
static void modem_main(void);
static void modem_Systick_Handler(void);

struct MODEM modem_ = {

	//.phy_is_powered = 0,
	.phy_SwitchPower = &modem_SwitchPower,
	//.pause_between_switches = 0,

	.Press_PWRKEY    		= &modem_Press_PWRKEY,
	.Finish_Press_PWRKEY 	= &modem_Finish_Press_PWRKEY,

	//.STATUS = 0,
	//.Send_new_status = 0,
	//.Delay_between_messages =		//MODEM_DELAY_BETWEEN_MESSAGES
	.Update_Status            = &modem_Update_Status,

	.Main			 = &modem_main,					// drop into main loop
	.Systick_Handler = &modem_Systick_Handler,		// drop into SystickHandler

};


// powers modem phisically by setting ON/OFF pin of LM2596R-ADJ.
// return: 0 - modem is powered off
//         1 - modem is powered on
//		   2 - pause is not out. it needs to wait.
static struct modem_cmd_result modem_SwitchPower(uint8_t newState){

	struct modem_cmd_result res;

	if ( !modem_.pause_between_switches ){	// == 0
		if ( newState == 0 ){
			GSM_V_REGULATOR_TURN_OFF;
			modem_.phy_is_powered = 0;
			res.value_1 = 0;
		} else{
			GSM_V_REGULATOR_TURN_ON;
			modem_.phy_is_powered = 1;
			res.value_1 = 1;
		}
		modem_.pause_between_switches = MODEM_PAUSE_BETWEEN_SWITCHES_VALUE;
	} else{
		res.value_1 = 3;
	}


	return res;
}

static void modem_Press_PWRKEY(void){

	LL_GPIO_SetOutputPin(GSM_POWER_KEY_GPIO_Port, GSM_POWER_KEY_Pin);
	modem_.PressingTime = MODEM_PWRKEY_SWITCH_TIME_LIMIT;
	modem_.IsPressed = 1;


}

static void modem_Finish_Press_PWRKEY(void){
	LL_GPIO_ResetOutputPin(GSM_POWER_KEY_GPIO_Port, GSM_POWER_KEY_Pin);
}

// NewState: 0 - Falling, 1 - Rising
//
static void modem_Update_Status(uint8_t NewState){						// in IRQ

	//modem_.STATUS = NewState;
	modem_.Send_new_status = 1;
}


static void modem_main(void){

	static struct modem_cmd_result param;

	uint8_t NewState;

	if ( modem_.Send_new_status && !modem_.Delay_between_messages ){

		if ( queueManager.available ) {

			modem_.Send_new_status = 0;

			NewState = LL_GPIO_IsInputPinSet(GSM_STATUS_GPIO_Port, GSM_STATUS_Pin);

			if ( modem_.STATUS != NewState ){
				modem_.STATUS = NewState;

				param.value_1 = modem_.STATUS;
				e220_900t30d_Answer_Fill(&VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_MODEM_STATUS_STATE_INDEX]), &param );			//  CHANGE, FIX

				if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_NOT_REQUIRED ) == Q_result_ER ){
					queueManager.queue_overflows++;
				}

				modem_.Delay_between_messages = MODEM_DELAY_BETWEEN_MESSAGES;
			}

		}
	}

}

static void modem_Systick_Handler(void){

	if ( modem_.pause_between_switches ){
		modem_.pause_between_switches--;
	}

	if ( modem_.IsPressed ){
		if ( modem_.PressingTime )
			modem_.PressingTime--;
		if ( modem_.PressingTime == 0 ){
			modem_.IsPressed = 0;
			modem_.Finish_Press_PWRKEY();
		}

	}

	if ( modem_.Delay_between_messages ){
		modem_.Delay_between_messages--;
	}

}
