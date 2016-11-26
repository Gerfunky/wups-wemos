// dht_mos.h

#ifndef _DHT_MOS_h
#define _DHT_MOS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct dht_sensor_strucht
{
	uint8_t pin;
	float   temp;
	float	humidity;
	uint8_t type;		// 0 = DHT 22 ; 1 = DHT 11
};



#endif

