#ifndef MODEM_STATE_H
#define MODEM_STATE_H

#include <stdint.h>

#define MODEM_PAUSE_BETWEEN_SWITCHES_VALUE 	3000
#define MODEM_PWRKEY_SWITCH_TIME_LIMIT 		2000
#define MODEM_DELAY_BETWEEN_MESSAGES		1000

struct modem_cmd_result {

	uint32_t value_1;
	uint32_t value_2;
};


struct MODEM {

	uint8_t phy_is_powered;						// powers modem phisically by setting ON/OFF pin of LM2596R-ADJ
	struct modem_cmd_result  (*phy_SwitchPower)(uint8_t newState);	// setting ON/OFF pin of LM2596R-ADJ
	uint16_t pause_between_switches;

	//Customer can power on  modem by pulling down the PWRKEY pin for at least 1   second and release
	//Customer can power off modem by pulling down the PWRKEY pin for at least 1.5 second and release
	void(*Press_PWRKEY)(void);
	uint8_t IsPressed;
	uint16_t PressingTime;
	void(*Finish_Press_PWRKEY)(void);

	uint8_t STATUS;			// Power on status;	relates to pin42 of modem ; Operating Status Indication
	uint8_t Send_new_status;
	uint16_t Delay_between_messages;
	void (*Update_Status)(uint8_t NewState);

	void (*Main)(void);
	void (*Systick_Handler)(void);
};

#endif
