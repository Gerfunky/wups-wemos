// wemos_relay.h

#ifndef _WEMOS_RELAY_h
#define _WEMOS_RELAY_h

#ifdef RELAY_BOARD

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif




#define NR_OF_RELAYS 8
#define DEF_RELAY_PIN_0 D0	//wemos 0
#define DEF_RELAY_PIN_1 D1	// 1
#define DEF_RELAY_PIN_2 D2	// 2...
#define DEF_RELAY_PIN_3 D3
							// D4 for direct sensor
#define DEF_RELAY_PIN_4 D5
#define DEF_RELAY_PIN_5 D6
#define DEF_RELAY_PIN_6 D7
#define DEF_RELAY_PIN_7 D8

struct relay_cfg_struct
{
	uint8_t pin;
	bool	state;
};


enum RELAY_ENUM
{
	RELAY_PIN = 0		// Debug to Serial ?
	, RELAY_STATE = 1 	// True = ON, False = OF
};




#endif



#endif

