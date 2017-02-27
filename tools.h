// tools.h

#ifndef _TOOLS_h
#define _TOOLS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
										// DEFAULT settings if not loaded from the SPIFFS
#define DEF_DEBUG_OUT true				// serial debugging
#define DEF_WIFI_MODE true				// true = client
#define DEF_OTA_SERVER true				// enable the OTA server ?
#define DEF_STATIC_IP_ENABLED true		// set static ip for startup  ?
#define DEF_HTTP_ENABLED true			// enable the HTTP server ?
#define DEF_ARTNET_ENABLE false			// enable Artnet  ?
#define DEF_AUTO_FFT false				// enalbe auto FFT ?
#define DEF_LED_TYPE 1					// led type 1 = WS2812b, 0 = APA102
#define DEF_DEBUG_TELNET true			// debug to TELNET?
#define DEF_FFT_MASTER_SEND false		//  if in master mode send out the UDP Multicast packets?

#define NR_GLOBAL_OPTIONS_BYTES 2     // to hold the bools in bits not bytes! Enum below must fit in here!!!
enum GLOBAL_OPTIONS_ENUM
{
	DEBUG_OUT			= 0		// Debug to Serial ?
	,WIFI_MODE			= 1 	// True = Client, False = AP
	,OTA_SERVER			= 2		// Enable Arduino OTA
	,STATIC_IP_ENABLED	= 3		// Static IP?
	,HTTP_ENABLED		= 4		// enable the HTTP server
	,ARTNET_ENABLE		= 5		// Artnet mode?
	,OSC_EDIT			= 6		// alow Edditing of NR LEds and Start Led ?
	,AUTOMAT_MASTER		= 7		// Master FFT-Server ? if not = slave!
	,FFT_ENABLE			= 8		// FFT enabled
	,BLEND_INVERT		= 9		// invert all blend modes!
	,OSC_MC_SEND		= 10	// send OSC to slave OSC devices ?
	,UPDATE_LEDS		= 11	// 
	,BPM_COUNTER		= 12	// Pallete BPM mode ?
	,FFT_AUTO			= 13	// Auto FFT mode ?
	,DEBUG_TELNET		= 14	// Debug to telnet ?
	,FFT_MASTER_SEND	= 15	// if in master mode send out the UDP Multicast packets?

};


#endif

