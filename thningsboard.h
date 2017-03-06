// thningsboard.h

#ifndef _THNINGSBOARD_h
#define _THNINGSBOARD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include "IPAddress.h"
struct thingsboard_Struct
{
	char		mqtt_token[32];			// the Hostname & AP mode SSID
	IPAddress	ip_mqtt_server;				// What NTP server to use in IP 
	uint16_t	port_mqtt_server;
	boolean		enabled;
	
};






#endif

