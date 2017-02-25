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
	bool	readOk;
	float   temp;
	float	humidity;
	uint8_t type;		// 0 = DHT 22 ; 1 = DHT 11
	uint8_t relay_no;	// what relay are we controlling
	uint8_t mintemp;
	uint8_t HumiDiff;	// how much lower is the humidity alloed to fall before switching off
	uint8_t minHumid;


	
};



#endif

