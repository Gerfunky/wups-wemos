/*
	Wups -Wemos
	
	TODO: more info here!

*/

#include "wemos_relay.h"
#include "Header.h"				// add the main Header file

#define DEF_BOOT_DEBUGING  true  // Set to true to get DEbuging info on serial port during boot. else set to false  TODO put this in epprom
#define DEF_SERIAL_SPEED 57600   // teensy - ESP8266 working on 57600

		// add the Debug functions   --     send to debug   MSG to  Serial or telnet --- Line == true  add a CR at the end.
		extern void debugMe(String input, boolean line = true);				// debug funtions from wifi-ota 
		//extern void debugMe(float input, boolean line = true);
		//extern void debugMe(uint8_t input, boolean line = true);
		//extern void debugMe(int input, boolean line = true);


		// add wifi funtions from wifi-ota.cpp
		extern void wifi_setup();
		extern void wifi_loop();



		// add SPIFFS-Functions from config-fs.cpp
		extern void FS_setup_SPIFFS();


		
		// From tools.cpp
		//extern boolean get_bool(uint8_t bit_nr);
		extern void write_bool(uint8_t bit_nr, boolean value);


		// from relay.cpp
		extern void relay_setup();
		extern void relay_set(uint8_t relay_nr, bool state);
		extern void relay_set_all(boolean state);

		unsigned long update_time = 0;



 
void setup()
{
	write_bool(DEBUG_OUT, DEF_BOOT_DEBUGING);   // Set the Boot debuging level will be overwriten when loading config from SPIFFS
	
	
	if (DEF_BOOT_DEBUGING == true)
	{

		Serial.begin(DEF_SERIAL_SPEED);

		//Serial.begin(DEF_SERIAL_SPEED);						// enable serial for debugging nand CMDmesanger if using local FFT from teensy
		debugMe(ESP.getResetReason());
		debugMe(ESP.getResetInfo());
		debugMe("Starting Setup - Light Fractal");
	}

	FS_setup_SPIFFS();  // includes loadbool()
	
	

	wifi_setup();


	relay_setup();

	//setup_comms(DEF_BOOT_DEBUGING, DEF_SERIAL_SPEED);   // Start CMDmessanger and the Serial if DEF_BOOT_DEBUGING == false
	debugMe("DONE Setup");


}

bool rel_state = true;

void loop() 
{

	wifi_loop();
	
	if (millis() >= update_time )
	{
		rel_state = !rel_state;
		update_time = millis() + 1000;
		
		relay_set_all( rel_state);

		//relay_set( 1, rel_state);
		//Serial.println("Doit Baby");
		//Serial.println(rel_state);


	}


}
