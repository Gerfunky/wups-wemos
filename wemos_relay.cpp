// 
// 
// 

#include "wemos_relay.h"



relay_cfg_struct relay[NR_OF_RELAYS] = {
		{ DEF_RELAY_PIN_0  , false},
		{ DEF_RELAY_PIN_1  , false},
		{ DEF_RELAY_PIN_2  , false},
		{ DEF_RELAY_PIN_3  , false},
		{ DEF_RELAY_PIN_4  , false},
		{ DEF_RELAY_PIN_5  , false},
		{ DEF_RELAY_PIN_6  , false},
		{ DEF_RELAY_PIN_7  , false}
		};


void relay_setup()
{
	for (uint8_t i = 0; i < NR_OF_RELAYS; i++)
	{
		pinMode(relay[i].pin, OUTPUT);
		//digitalWrite(relay[i].pin, LOW);			// set all relays to off at boot
	}

}

void relay_set(uint8_t relay_nr, bool state)
{
	if (relay_nr < NR_OF_RELAYS && relay_nr >= 0)
	{
		relay[relay_nr].state = state;
		digitalWrite(relay[relay_nr].pin, relay[relay_nr].state);
	}

}

void relay_set_all(boolean state)
{
	for (uint8_t i = 0; i < NR_OF_RELAYS; i++)
	{
		
		digitalWrite(relay[i].pin, state);
	}

}


