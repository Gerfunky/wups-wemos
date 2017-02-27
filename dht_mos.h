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
	uint8_t pin; 
	uint8_t relay_no;	// what relay are we controlling
	uint8_t mintemp;
	uint8_t HumiDiff;	// how much lower is the humidity alloed to fall before switching off
	uint8_t minHumid;
	uint8_t tempDiff;
	uint16_t totalErrors;
	uint16_t errors;
	uint16_t update_sec;	// how many seconds to wait between each update

	
	//String name; // name it


	
};


#define DHT_NR_SENSORS 1
#define DHT_0_PIN D4
#define DHT_0_TYPE DHT22  // DHT 11 blue DHT22 white

#define DHT_NR_SLAVES 2

#define DHT_DEF_MINTEMP 5
#define DHT_RELAY_NO 0
#define DHT_DEF_HUM_DIFF 10
#define DHT_DEF_MIN_HUMID 60
#define DHT_DEF_TEMP_DIFF 1
#define DHT_DEF_UPDATE_SEC 10

#define DHT_MINTEMP_MIN 3
#define DHT_MINTEMP_MAX 40

#define DHT_TEMPDIF_MIN 1
#define DHT_TEMPDIF_MAX 20

#define DHT_MINHUMID_MIN 10
#define DHT_MINHUMID_MAX 90

#define DHT_HUMDIDDIFF_MIN 5
#define DHT_HUMDIDDIFF_MAX 30

#define DHT_UPDATESEC_MIN 2
#define DHT_UPDATESEC_MAX 360

#endif

