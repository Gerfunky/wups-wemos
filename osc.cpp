// 
// 
// 

#ifndef BLAHBLAH
//  Todo : 




#include "osc.h"


#ifndef USE_OSC
// clapse all ctl+m +o    +l = expand

//#ifdef ESP8266

//#define OSC_MC_SERVER_DISABLED



#ifdef _MSC_VER
	
	#include <ESP8266WiFi\src\WiFiUdp.h>
	#include <ESP8266WiFi\src\ESP8266WiFi.h>
	#include <OSC\OSCMessage.h>
	#include <OSC\OSCBundle.h>
	#include <OSC\OSCData.h>
	#include <QueueArray\QueueArray.h>

#else
	#include <WiFiUdp.h>
	#include <ESP8266WiFi.h>
	#include <OSCMessage.h>
	#include <OSCBundle.h>
	#include <OSCData.h>
	#include <QueueArray.h>
#endif

#include "tools.h"								// include the Tools enums for reading and writing bools
#include "wifi-ota.h"




// External Functions  and Variables

// From tools.cpp
extern boolean get_bool(uint8_t bit_nr);
extern void write_bool(uint8_t bit_nr, boolean value);
extern float byte_tofloat(uint8_t value, uint8_t max_value = 255);




// from config.fs
extern void FS_wifi_write(uint8_t conf_nr);
extern void FS_Bools_write(uint8_t conf_nr);
extern void FS_osc_delete_all_saves();
extern void	FS_dht_write(uint8_t conf_nr);
/*extern void FS_artnet_write(uint8_t conf_nr);
extern boolean FS_play_conf_read(uint8_t conf_nr);
extern void FS_play_conf_write(uint8_t conf_nr);
extern void FS_FFT_write(uint8_t conf_nr);
extern boolean FS_FFT_read(uint8_t conf_nr);
*/

// from wifi


	//extern void WIFI_FFT_toggle_master(boolean value);
	//extern void WIFI_FFT_toggle(boolean mode_value);
	//extern fft_data_struct fft_data[7];
	extern wifi_Struct wifi_cfg;
	//extern artnet_struct artnet_cfg;

	// add the Debug functions   --     send to debug   MSG to  Serial or telnet --- Line == true  add a CR at the end.
	extern void debugMe(String input, boolean line = true);
	extern void debugMe(float input, boolean line = true);
	extern void debugMe(uint8_t input, boolean line = true);
	extern void debugMe(int input, boolean line = true);


// from httpd
extern void httpd_toggle_webserver();

// from leds
/*extern float LEDS_get_FPS();
extern led_cfg_struct led_cfg;

extern led_Copy_Struct copy_leds[NR_COPY_STRIPS];
extern Strip_FL_Struct part[NR_STRIPS];
extern form_Part_FL_Struct form_part[NR_FORM_PARTS];
extern byte  copy_leds_mode[NR_COPY_LED_BYTES];
extern byte strip_menu[_M_NR_STRIP_BYTES_][_M_NR_OPTIONS_];
extern byte form_menu[_M_NR_FORM_BYTES_][_M_NR_FORM_OPTIONS_];
extern uint8_t global_strip_opt[_M_NR_STRIP_BYTES_][_M_NR_GLOBAL_OPTIONS_];
extern led_cfg_struct led_cfg;
extern byte fft_menu[3];
extern fft_led_cfg_struct fft_led_cfg;
//extern CRGBPalette16 LEDS_pal_cur[NR_PALETTS];
extern uint8_t LEDS_pal_read(uint8_t pal, uint8_t no, uint8_t color);
extern void LEDS_pal_write(uint8_t pal, uint8_t no, uint8_t color, uint8_t value);
extern void LEDS_pal_reset_index();
extern void LEDS_pal_load(uint8_t pal_no, uint8_t pal_menu);
//struct OSC_buffer_float master_rgb = { 255,255,255 };


//from coms
extern void comms_S_FPS(uint8_t fps);

*/


// from relay
//void relay_set(uint8_t relay_nr, bool state);

QueueArray <char> osc_out_float_addr;
QueueArray <float> osc_out_float_value;



osc_cfg_struct osc_cfg = { OSC_IPMULTI_ ,OSC_PORT_MULTI_,OSC_OUTPORT, OSC_INPORT, 0,1 };




	WiFiUDP osc_server;				// the normal osc server



void OSC_setup()
{
	osc_server.begin(osc_cfg.inPort);

}




// OSC gerneral functions
float osc_byte_tofloat(byte value, uint8_t max_value = 255) {

	float float_out = float(value) / max_value;

	return float_out;
}

void osc_queu_MSG_float(String addr_string, float value) {
	IPAddress ip_out(osc_server.remoteIP());
	osc_cfg.return_ip_LB = ip_out[3];

	//char address_out[20];
	//OSC_buffer_float msg_out_data;


	//addr_string.toCharArray(address_out, addr_string.length() + 1); //address_out 
	//msg_out_data.addr = addr_string;
	//msg_out_data.value = value;

	for (int i = 0; i < addr_string.length(); i++)
		osc_out_float_addr.enqueue(addr_string.charAt(i));

	osc_out_float_addr.enqueue(0);	// add a null on the end 
	osc_out_float_value.enqueue(value);


	/*OSCMessage msg_out(address_out);
	msg_out.add(value);
	osc_server.beginPacket(osc_server.remoteIP(), 9000);
	msg_out.send(osc_server);
	osc_server.endPacket();
	msg_out.empty();*/

}

void osc_send_MSG_String(String addr_string, String value) {
	char address_out[20];
	char message[20];



	addr_string.toCharArray(address_out, addr_string.length() + 1); //address_out 

	value.toCharArray(message, value.length() + 1);

	OSCMessage msg_out(address_out);
	msg_out.add(message);
	osc_server.beginPacket(osc_server.remoteIP(), 9000);
	msg_out.send(osc_server);
	osc_server.endPacket();
	msg_out.empty();


}
// Other Functions like Sending loop
void osc_send_out_float_MSG_buffer() 
{
	if (osc_out_float_value.isEmpty() != true)
	{

		char address_out[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		uint8_t i = 0;
		float value = 0;



		//msg_out_data = osc_out_float.dequeue();

		//addr_string.toCharArray(address_out, addr_string.length() + 1); //address_out 
		//msg_out_data.addr.toCharArray(address_out, msg_out_data.addr.length() + 1); //address_out
		if (osc_out_float_value.count() >= OSC_BUNDLE_SEND_COUNT)
		{
			OSCBundle bundle_out;
			//IPAddress ip_out(WiFi.localIP());
			IPAddress ip_out(osc_server.remoteIP());
			//ip_out[3] = osc_cfg.return_ip_LB;

			for (uint8_t z = 0; z <16; z++)
			{
				i = 0;
				while (osc_out_float_addr.peek() != 0 && i <20) {
					address_out[i] = osc_out_float_addr.dequeue();
					i++;
				}
				address_out[i] = osc_out_float_addr.dequeue(); // get the null char for end of string as well. so that we can fetch the next msg next time
				value = osc_out_float_value.dequeue();

				bundle_out.add(address_out).add(float(value));

				yield();
				memset(address_out, 0, 20);

}
			osc_server.beginPacket(ip_out, 9000); //{172,16,222,104}, 8001) ; //
			bundle_out.send(osc_server);
			osc_server.endPacket();
			bundle_out.empty();
			yield();
			//delay(1);

		}
		else
		{
			OSCBundle bundle_out;

			for (uint8_t z = 0; z <osc_out_float_value.count(); z++)
			{
				i = 0;
				while (osc_out_float_addr.peek() != 0 && i <20) {
					address_out[i] = osc_out_float_addr.dequeue();
					i++;
				}
				address_out[i] = osc_out_float_addr.dequeue(); // get the null char for end of string as well. so that we can fetch the next msg next time
				value = osc_out_float_value.dequeue();

				bundle_out.add(address_out).add(float(value));
				memset(address_out, 0, 20);

			}
			osc_server.beginPacket(osc_server.remoteIP(), 9000); //{172,16,222,104}, 8001) ; //
			bundle_out.send(osc_server);
			osc_server.endPacket();
			bundle_out.empty();
			yield();




		}
	}
}



/*
#ifndef OSC_MC_SERVER_DISABLED
void osc_mc_send(String addr, uint8_t value)
{
	if (get_bool(OSC_MC_SEND) == true)
	{
		//debugMe("osc_mcI_send:", false);
		//debugMe(addr);
		char address_out[20];
		int out_value = int(value);

		addr.toCharArray(address_out, addr.length() + 1); //address_out 
		OSCMessage msg_out(address_out);
		msg_out.add(out_value);
		osc_mc_server.beginPacketMulticast(osc_cfg.ipMulti, osc_cfg.portMulti, WiFi.localIP());
		msg_out.send(osc_mc_server);
		osc_mc_server.endPacket();
	}
}

void osc_mc_send(String addr, float value)
{
	if (get_bool(OSC_MC_SEND) == true)
	{
		//debugMe("osc_mcF_send:",false);
		//debugMe(addr);

		char address_out[20];
		//int out_value = int(value);

		addr.toCharArray(address_out, addr.length() + 1); //address_out 
		OSCMessage msg_out(address_out);
		msg_out.add(value);
		osc_mc_server.beginPacketMulticast(osc_cfg.ipMulti, osc_cfg.portMulti, WiFi.localIP());
		msg_out.send(osc_mc_server);
		osc_mc_server.endPacket();
	}
}
#endif
*/
// OSC Settings MSG:/s



// Strips
/*
void osc_strips_G_toggle_rec(OSCMessage &msg, int addrOffset) {
	// OSC MESSAGE :/strips/GO:/?/Row/collum  

	String collum_string;
	String row_string;
	char address[4];					// to pick info aut of the msg address
										//char address_out[20];	
	int select_bit = 0;
	String select_bit_string;
	bool switch_bool = false;			// for toggels to get row and collum

	String outbuffer = "/s/ANL";		// OSC return address
	float outvalue;						// return value to labels

	String out_add_label;				// address label



	msg.getAddress(address, addrOffset - 1, 1);					// get the select-bit info	
	select_bit_string = select_bit_string + address[0];
	select_bit = select_bit_string.toInt();
	

	memset(address, 0, sizeof(address));				// rest the address to blank


	msg.getAddress(address, addrOffset + 3);		// get the address for row / collum


	for (byte i = 0; i < sizeof(address); i++)
	{
		if (address[i] == '/') {
			switch_bool = true;
		}
		else if (switch_bool == false)
		{
			row_string = row_string + address[i];
		}
		else
			collum_string = collum_string + address[i];
	}

	int row = row_string.toInt() - 1;
	int collum = collum_string.toInt() - 1;  // Whit CRGB value in the pallete

	//debugMe(address);
	memset(address, 0, sizeof(address));
	byte msg_size = msg.size();

	//debugMe("row: " + String(row) + " col: " + String(collum));

	bitWrite(global_strip_opt[select_bit][row], collum, bool(msg.getFloat(0)));


	//outbuffer = String("/s" + out_add_label);
	//osc_send_MSG(outbuffer, outvalue);

}


void osc_strips_settings_rec(OSCMessage &msg, int addrOffset) {
	// OSC MESSAGE :/strip/??:/bit/row/collum   xx/?/?
	// :/bit/+-option/strip



	//byte addrOffset = 7+6+1 ;
	String bit_string;
	String option_string;
	String strip_string;

	int bit_int;
	int option_int;
	int strip_int;


	char address[3];
	char address_out[20];
	bool switch_bool = false;

	String outbuffer = "/strips/sX/SIL/X";
	float outvalue = 255;

	msg.getAddress(address, addrOffset + 1, 1);
	bit_string = bit_string + address[0];

	memset(address, 0, sizeof(address));

	msg.getAddress(address, addrOffset + 3);


	for (byte i = 0; i < sizeof(address); i++) {
		if (address[i] == '/') {
			switch_bool = true;

		}
		else if (switch_bool == false) {
			option_string = option_string + address[i];

		}
		else
			strip_string = strip_string + address[i];

	}

	bit_int = bit_string.toInt();
	option_int = option_string.toInt() - 1;
	strip_int = strip_string.toInt() - 1;

	memset(address, 0, sizeof(address));

	msg.getAddress(address, addrOffset - 2, 2);


	byte msg_size = msg.size();

	char full_addr[msg_size + 1 - 4];
	//hmsg.getAddress(full_addr, 0, msg_size-4 ) ;

	//full_addr[sizeof(full_addr)- 2] = 'X'; 


	if (bool(msg.getFloat(0)) == true) {  // only on pushdown
		if (address[0] == 'S' && address[1] == 'I') 
		{
			switch (option_int) 
			{
				case 0:
					part[strip_int + bit_int * 8].index_start -= osc_cfg.conf_multiply;
					break;
				case 2:
					part[strip_int + bit_int * 8].index_start += osc_cfg.conf_multiply;
					break;
			}

			outvalue = float(part[strip_int + bit_int * 8].index_start);

		}

		if (address[0] == 'I' && address[1] == 'A') 
		{
			switch (option_int) 
			{
				case 0:
					part[strip_int + bit_int * 8].index_add -= osc_cfg.conf_multiply;
					break;
				case 2:
					part[strip_int + bit_int * 8].index_add += osc_cfg.conf_multiply;
					break;
			}
			outvalue = float(part[strip_int + bit_int * 8].index_add);

		}

		if (address[0] == 'I' && address[1] == 'F') 
		{
			switch (option_int) 
			{
				case 0:
					part[strip_int + bit_int * 8].index_add_pal -= osc_cfg.conf_multiply;
					break;
				case 2:
					part[strip_int + bit_int * 8].index_add_pal += osc_cfg.conf_multiply;
					break;
			}
			outvalue = float(part[strip_int + bit_int * 8].index_add_pal);


		}

		if (address[0] == 'S' && address[1] == 'L') 
		{
			if (get_bool(OSC_EDIT) == true)
				switch (option_int) 
				{
					case 0:
						part[strip_int + bit_int * 8].start_led = constrain(part[strip_int + bit_int * 8].start_led - osc_cfg.conf_multiply, 0, NUM_LEDS - part[strip_int + bit_int * 8].nr_leds);
						break;
					case 1:
						if (strip_int !=0 )
							part[strip_int + bit_int * 8].start_led = part[strip_int - 1 + bit_int * 8].start_led + part[strip_int-1 + bit_int * 8].nr_leds;
						else  if (bit_int != 0)
							part[strip_int + bit_int * 8].start_led = part[ 7  + (bit_int- 1) * 8].start_led + part[7 + (bit_int-1) * 8].nr_leds;
					break;
					case 2:
						part[strip_int + bit_int * 8].start_led = constrain(part[strip_int + bit_int * 8].start_led + osc_cfg.conf_multiply, 0, NUM_LEDS - part[strip_int + bit_int * 8].nr_leds);
						break;
				}
			outvalue = float(part[strip_int + bit_int * 8].start_led);
		}

		if (address[0] == 'N' && address[1] == 'L') 
		{
			if (get_bool(OSC_EDIT) == true)
				switch (option_int) 
				{
					case 0:
						part[strip_int + bit_int * 8].nr_leds = constrain(part[strip_int + bit_int * 8].nr_leds - osc_cfg.conf_multiply, 0, NUM_LEDS - part[strip_int + bit_int * 8].start_led);
						break;
					case 2:
						part[strip_int + bit_int * 8].nr_leds = constrain(part[strip_int + bit_int * 8].nr_leds + osc_cfg.conf_multiply, 0, NUM_LEDS - part[strip_int + bit_int * 8].start_led);
						break;
				}
			outvalue = float(part[strip_int + bit_int * 8].nr_leds);
		}

		outbuffer = String("/strips/s" + String(bit_int) + "/" + String(address) + "L/" + String(strip_int + 1));
		osc_queu_MSG_float(outbuffer, outvalue);
	}

}

void osc_strips_settings_send(byte y) {
	// OSC MESSAGE OUT :/strip/s?/xx/?/?

	for (int i = 0; i < 8; i++) {

		osc_queu_MSG_float(String("/strips/s" + String(y) + "/SLL/" + String(i + 1)), float(part[i + (y * 8)].start_led));
		osc_queu_MSG_float(String("/strips/s" + String(y) + "/NLL/" + String(i + 1)), float(part[i + (y * 8)].nr_leds));
		osc_queu_MSG_float(String("/strips/s" + String(y) + "/IAL/" + String(i + 1)), float(part[i + (y * 8)].index_add));
		osc_queu_MSG_float(String("/strips/s" + String(y) + "/IFL/" + String(i + 1)), float(part[i + (y * 8)].index_add_pal));
		osc_queu_MSG_float(String("/strips/s" + String(y) + "/SIL/" + String(i + 1)), float(part[i + (y * 8)].index_start));

		for (uint8_t row = 0; row < _M_NR_GLOBAL_OPTIONS_; row++)
		{
			osc_queu_MSG_float(String("/strips/GO/" + String(y) + "/" + String(row + 1) + String("/") + String(i + 1)), float(bitRead(global_strip_opt[y][row], i)));
		}

		yield(); //delay(1); 
	}


}

void osc_strips_toggle_rec(OSCMessage &msg, int addrOffset) {
	// recive OSC message for STRIPS
	// OSC MESSAGE :/strip/T:/bit/row/collum   T/?/?
	//						:/bit/option/strip

	String bit_string;
	String option_string;
	String strip_string;

	int bit_int;
	int option_int;
	int strip_int;

	char address[6];
	bool switch_bool = false;

	msg.getAddress(address, addrOffset + 1, 1);
	bit_string = bit_string + address[0];
	bit_int = bit_string.toInt();

	msg.getAddress(address, addrOffset + 3);

	for (byte i = 0; i < sizeof(address); i++)
	{
		if (address[i] == '/')
			switch_bool = true;
		else if (switch_bool == false)
			option_string = option_string + address[i];
		else
			strip_string = strip_string + address[i];
	}

	option_int = option_string.toInt() - 1;
	strip_int = strip_string.toInt() - 1;



	bitWrite(strip_menu[bit_int][option_int], strip_int, bool(msg.getFloat(0)));

}

void osc_strips_toggle_send(byte y)
{
	// OSC MESSAGE OUT :/strip/s?/T/?/?
	for (int i = 0; i < 8; i++)
	{
		for (int z = 0; z < _M_NR_OPTIONS_; z++)
		{
			osc_queu_MSG_float(String("/strips/T/" + String(y) + "/" + String(z + 1) + "/" + String(i + 1)), float(bitRead(strip_menu[y][z], i)));
		}
		osc_queu_MSG_float(String("/strips/s" + String(y) + "/SLL/" + String(i + 1)), float(part[i + (y * 8)].start_led));
		osc_queu_MSG_float(String("/strips/s" + String(y) + "/NLL/" + String(i + 1)), float(part[i + (y * 8)].nr_leds));
	}
}

void osc_strips_routing(OSCMessage &msg, int addrOffset) {
	// OSC MESSAGE :/strip
	msg.route("/T",		osc_strips_toggle_rec, addrOffset);
	msg.route("/SI",	osc_strips_settings_rec, addrOffset);				// Start Index
	msg.route("/IA",	osc_strips_settings_rec, addrOffset);
	msg.route("/IF",	osc_strips_settings_rec, addrOffset);				// Index add
	msg.route("/SL",	osc_strips_settings_rec, addrOffset);
	msg.route("/NL",	osc_strips_settings_rec, addrOffset);
	msg.route("/GO",	osc_strips_G_toggle_rec, addrOffset);

	if (msg.fullMatch("/s0/refresh",		addrOffset) && bool(msg.getFloat(0)) == true) { osc_strips_toggle_send(0); osc_strips_toggle_send(1); }
	if (msg.fullMatch("/s2/refresh",		addrOffset) && bool(msg.getFloat(0)) == true) { osc_strips_toggle_send(2); osc_strips_toggle_send(3); }
	if (msg.fullMatch("/s0/conf_refesh",	addrOffset) && bool(msg.getFloat(0)) == true) { osc_strips_settings_send(0); osc_strips_settings_send(1); }
	if (msg.fullMatch("/s2/conf_refesh",	addrOffset) && bool(msg.getFloat(0)) == true) { osc_strips_settings_send(2); osc_strips_settings_send(3); }
}


// OSC Forms 
void osc_forms_send(byte y) {
	// OSC MESSAGE OUT :/form/f"y"/T/?/i
	debugMe("Forms - refesh Start ");
	debugMe(String("y=" + String(y)));
	for (int i = 0; i < 8; i++) 
	{
		//debugMe(String("i=" + String(i)));


		for (int z = 0; z < _M_NR_FORM_OPTIONS_ ; z++)
		{
			osc_queu_MSG_float(String("/form/T/" + String(y) + "/" + String(z + 1) + "/" + String(i + 1)), float(bitRead(form_menu[y][z], i)));

		}
		//debugMe(String("for 1 done"), true);
		
		osc_queu_MSG_float(String("/form/f" + String(y) + "/SLL/" + String(i + 1)), float(form_part[i + (y * 8)].start_led));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/NLL/" + String(i + 1)), float(form_part[i + (y * 8)].nr_leds));

		osc_queu_MSG_float(String("/form/FA/" + String(y) + "/" + String(i + 1)), osc_byte_tofloat(form_part[i + (y * 8)].fade_value, MAX_FADE_VALUE));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/FAL/" + String(i + 1)), float(form_part[i + (y * 8)].fade_value));
		osc_queu_MSG_float(String("/form/GL/" + String(y) + "/" + String(i + 1)), osc_byte_tofloat(form_part[i + (y * 8)].glitter_value, MAX_GLITTER_VALUE));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/GLL/" + String(i + 1)), float(form_part[i + (y * 8)].glitter_value));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/JDL/" + String(i + 1)), float(form_part[i + (y * 8)].juggle_nr_dots));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/JSL/" + String(i + 1)), float(form_part[i + (y * 8)].juggle_speed));
		yield();
		osc_send_out_float_MSG_buffer();
		
	}
	//debugMe(String("forms  done"), true);

}

void osc_forms_config_send(byte y) {
	// OSC MESSAGE OUT :/strip/s?/xx/?/?

	for (int i = 0; i < 8; i++) {

		osc_queu_MSG_float(String("/form/f" + String(y) + "/SLL/" + String(i + 1)), float(form_part[i + (y * 8)].start_led));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/NLL/" + String(i + 1)), float(form_part[i + (y * 8)].nr_leds));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/IAL/" + String(i + 1)), float(form_part[i + (y * 8)].index_add));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/IFL/" + String(i + 1)), float(form_part[i + (y * 8)].index_add_pal));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/SIL/" + String(i + 1)), float(form_part[i + (y * 8)].index_start));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/JDL/" + String(i + 1)), float(form_part[i + (y * 8)].juggle_nr_dots));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/JS/" + String(i + 1)), osc_byte_tofloat(form_part[i + (y * 8)].juggle_speed, MAX_JD_SPEED_VALUE));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/JSL/" + String(i + 1)), float(form_part[i + (y * 8)].juggle_speed));
		osc_queu_MSG_float(String("/form/f" + String(y) + "/RFL/" + String(i + 1)), float(form_part[i + (y * 8)].rotate));
 
	}


}

void osc_forms_config_rec(OSCMessage &msg, int addrOffset) {
	// OSC MESSAGE :/form/??:/bit/row/collum   xx/?/?
	//	:/bit/option/form
 

	String bit_string;
	String option_string;
	String form_string;

	int bit_int;
	int option_int;
	int form_int;
	char address[3];
	char address_out[20];
	bool switch_bool = false;

	String outbuffer = "/form/fx/SIL/X";
	float outvalue;

	msg.getAddress(address, addrOffset + 1, 1);
	bit_string = bit_string + address[0];

	memset(address, 0, sizeof(address));

	msg.getAddress(address, addrOffset + 3);
	//debugMe(address);

	for (byte i = 0; i < sizeof(address); i++) {
		if (address[i] == '/') {
			switch_bool = true;

		}
		else if (switch_bool == false) {
			option_string = option_string + address[i];

		}
		else
			form_string = form_string + address[i];

	}
	bit_int = bit_string.toInt();
	option_int = option_string.toInt() - 1;
	form_int = form_string.toInt() - 1;

	memset(address, 0, sizeof(address));

	msg.getAddress(address, addrOffset - 2, 2);

	if (bool(msg.getFloat(0)) == true) 
	{  // only on pushdown
		if (address[0] == 'S' && address[1] == 'I') {
			switch (option_int) {


			case 0:
				form_part[form_int + bit_int * 8].index_start -= osc_cfg.conf_multiply;
				break;
			case 2:
				form_part[form_int + bit_int * 8].index_start += osc_cfg.conf_multiply;
				break;
			}
			outvalue = float(form_part[form_int + bit_int * 8].index_start);
		}

		if (address[0] == 'I' && address[1] == 'A') {
			switch (option_int) {
			case 0:
				form_part[form_int + bit_int * 8].index_add -= osc_cfg.conf_multiply;
				break;
			case 2:
				form_part[form_int + bit_int * 8].index_add += osc_cfg.conf_multiply;
				break;
			}
			//outbuffer = String("/strips/s" + String(z) + "/AIL/" + String(select_bit_int+1));

			outvalue = float(form_part[form_int + bit_int * 8].index_add);
			//osc_send_MSG(outbuffer , float(part[select_bit_int + z *8 ].index_add)) ;   

		}

		if (address[0] == 'I' && address[1] == 'F') {
			switch (option_int) {
			case 0:
				form_part[form_int + bit_int * 8].index_add_pal -= osc_cfg.conf_multiply;
				break;
			case 2:
				form_part[form_int + bit_int * 8].index_add_pal += osc_cfg.conf_multiply;
				break;
			}
			//outbuffer = String("/strips/s" + String(z) + "/AIL/" + String(select_bit_int+1));

			outvalue = float(form_part[form_int + bit_int * 8].index_add_pal);
			//osc_send_MSG(outbuffer , float(part[select_bit_int + z *8 ].index_add)) ;   

		}

		if (address[0] == 'S' && address[1] == 'L') {
			if ((get_bool(OSC_EDIT) == true) && (form_int + bit_int != 0))
				switch (option_int)
				{
				case 0:
					//form_part[select_bit_int + z * 8].start_led -= osc_cfg.conf_multiply;

					form_part[form_int + bit_int * 8].start_led = constrain(form_part[form_int + bit_int * 8].start_led - osc_cfg.conf_multiply, 0, NUM_LEDS - form_part[form_int + bit_int * 8].nr_leds);
					break;
				case 1:
					if (form_int != 0)
						form_part[form_int + bit_int * 8].start_led = form_part[form_int - 1 + bit_int * 8].start_led + form_part[form_int - 1 + bit_int * 8].nr_leds;
					else  if (bit_int != 0)
						form_part[form_int + bit_int * 8].start_led = form_part[7 + (bit_int - 1) * 8].start_led + form_part[7 + (bit_int - 1) * 8].nr_leds;
					break;

				case 2:
					//form_part[select_bit_int + z * 8].start_led += osc_cfg.conf_multiply;
					form_part[form_int + bit_int * 8].start_led = constrain(form_part[form_int + bit_int * 8].start_led + osc_cfg.conf_multiply, 0, NUM_LEDS - form_part[form_int + bit_int * 8].nr_leds);
					break;
				}
			//outbuffer = String("/strips/s" + String(z) + "/AIL/" + String(select_bit_int+1));

			outvalue = float(form_part[form_int + bit_int * 8].start_led);
			//osc_send_MSG(outbuffer , float(part[select_bit_int + z *8 ].index_add)) ;   

		}

		if (address[0] == 'N' && address[1] == 'L') {
			if ((get_bool(OSC_EDIT) == true) && (form_int + bit_int != 0))
				switch (option_int)
				{
				case 0:
					form_part[form_int + bit_int * 8].nr_leds = constrain(form_part[form_int + bit_int * 8].nr_leds - osc_cfg.conf_multiply, 0, NUM_LEDS - form_part[form_int + bit_int * 8].start_led);
					break;
				case 2:
					form_part[form_int + bit_int * 8].nr_leds = constrain(form_part[form_int + bit_int * 8].nr_leds + osc_cfg.conf_multiply, 0, NUM_LEDS - form_part[form_int + bit_int * 8].start_led);
					break;
				}

			outvalue = float(form_part[form_int + bit_int * 8].nr_leds);

		}

		if (address[0] == 'J' && address[1] == 'D') {
			switch (option_int) {
			case 0:
				form_part[form_int + bit_int * 8].juggle_nr_dots--;
				break;
			case 2:
				form_part[form_int + bit_int * 8].juggle_nr_dots++;
				break;
			}


			outvalue = float(form_part[form_int + bit_int * 8].juggle_nr_dots);
			//osc_send_MSG(outbuffer , float(part[select_bit_int + z *8 ].index_add)) ;   

		}

		if (address[0] == 'R' && address[1] == 'F') 
		{
			if (get_bool(OSC_EDIT) == true)
				if (form_part[form_int + bit_int * 8].nr_leds != 0) 
				{
					switch (option_int) 
					{
					case 0:
						form_part[form_int + bit_int * 8].rotate = constrain(form_part[form_int + bit_int * 8].rotate - osc_cfg.conf_multiply, -(form_part[form_int + bit_int * 8].nr_leds) + 1, (form_part[form_int + bit_int * 8].nr_leds) - 1);
						break;
					case 2:

						form_part[form_int + bit_int * 8].rotate = constrain(form_part[form_int + bit_int * 8].rotate + osc_cfg.conf_multiply, -(form_part[form_int + bit_int * 8].nr_leds) + 1, (form_part[form_int + bit_int * 8].nr_leds) - 1);
						break;
					}
				}

			outvalue = float(form_part[form_int + bit_int * 8].rotate);
			//osc_send_MSG(outbuffer , float(part[select_bit_int + z *8 ].index_add)) ;   

		}

		{
			outbuffer = String("/form/f" + String(bit_int) + "/" + String(address) + "L/" + String(form_int + 1));
			osc_queu_MSG_float(outbuffer, outvalue);
		}
	}

}

void osc_forms_fader_rec(OSCMessage &msg, int addrOffset) 
{
	// OSC MESSAGE :/form/FA:/?   

	String select_mode_string;		// Fader NR
	String select_strip_addr;		// form NR
									//String select_bit_string;
	char address[3];
	char address_out[20];
	bool switch_bool = false;
	uint8_t z = 0;						// form NR in uint8_t
	String outbuffer = "/form/fx/FA/X";
	float outvalue;

	msg.getAddress(address, addrOffset + 1, 1);
	select_strip_addr = select_strip_addr + address[0];
	z = select_strip_addr.toInt();
	memset(address, 0, sizeof(address));

	msg.getAddress(address, addrOffset + 3);

	for (byte i = 0; i < sizeof(address); i++) 
	{
		select_mode_string = select_mode_string + address[i];
	}

	//int select_bit_int = select_bit_string.toInt() - 1;
	int select_mode_int = select_mode_string.toInt() - 1;

	memset(address, 0, sizeof(address));

	msg.getAddress(address, addrOffset - 2, 2);

	byte msg_size = msg.size();

	if (address[0] == 'F' && address[1] == 'A') 
	{
		form_part[select_mode_int + z * 8].fade_value = (msg.getFloat(0) * MAX_FADE_VALUE);  //byte(msg.getFloat(0) * 255);
		outvalue = float(form_part[select_mode_int + z * 8].fade_value);
	}





	if (address[0] == 'G' && address[1] == 'L') 
	{				//map(msg.getFloat(0), 0,1,0,MAX_FADE) 

		form_part[select_mode_int + z * 8].glitter_value = (msg.getFloat(0) *  MAX_GLITTER_VALUE); //byte(msg.getFloat(0) * 255);

																								   //outbuffer = String("/strips/s" + String(z) + "/AIL/" + String(select_bit_int+1));

		outvalue = float(form_part[select_mode_int + z * 8].glitter_value);
		//osc_send_MSG(outbuffer , float(part[select_bit_int + z *8 ].index_add)) ;   

	}

	if (address[0] == 'J' && address[1] == 'S') 
	{

		form_part[select_mode_int + z * 8].juggle_speed = msg.getFloat(0) * MAX_JD_SPEED_VALUE;  //byte(msg.getFloat(0) * 255);

																								 //outbuffer = String("/strips/s" + String(z) + "/AIL/" + String(select_bit_int+1));

		outvalue = float(form_part[select_mode_int + z * 8].juggle_speed);
		//osc_send_MSG(outbuffer , float(part[select_bit_int + z *8 ].index_add)) ;   

	}

	{
		outbuffer = String("/form/f" + String(z) + "/" + String(address) + "L/" + String(select_mode_int + 1));
		osc_queu_MSG_float(outbuffer, outvalue);
	}


}

void osc_forms_toggle_rec(OSCMessage &msg, int addrOffset) // OSC: /form/T:/bit/row/collum 
{															
				// :/bit/option/form
				// recive OSC message for STRIPS
				// OSC MESSAGE :/form/f?/   T/?/?

				//DBG_OUTPUT_PORT.println("lol"); 

				//byte addrOffset = 7+6+1 ;																				

	String bit_string;
	String option_string;
	String form_string;

	int bit_int;
	int option_int;
	int form_int;

	char address[6];
	bool switch_bool = false;

	msg.getAddress(address, addrOffset + 1, 1);
	bit_string = bit_string + address[0];


	msg.getAddress(address, addrOffset + 3);
	//DBG_OUTPUT_PORT.println(address);

	for (byte i = 0; i < sizeof(address); i++) {
		if (address[i] == '/') {
			switch_bool = true;

		}
		else if (switch_bool == false) {
			option_string = option_string + address[i];

		}
		else
			form_string = form_string + address[i];

	}

	bit_int = bit_string.toInt();
	option_int = option_string.toInt() - 1;
	form_int = form_string.toInt() - 1;


	bitWrite(form_menu[bit_int][option_int], form_int, bool(msg.getFloat(0)));

}

void osc_forms_routing(OSCMessage &msg, int addrOffset) {
	// OSC MESSAGE :/form

	if (msg.fullMatch("/f/refresh", addrOffset) && bool(msg.getFloat(0)) == true) { osc_forms_send(0); osc_forms_send(1); }
	if (msg.fullMatch("/f/conf_refesh", addrOffset) && bool(msg.getFloat(0)) == true) { osc_forms_config_send(0); osc_forms_config_send(1); }

	//toggle buttons 
	msg.route("/T", osc_forms_toggle_rec, addrOffset);			// form menu toggles
														//msg.route("/f1/T",  osc_rec_forms, addrOffset);

														// push buttons
	msg.route("/SI", osc_forms_config_rec, addrOffset);	// Start Index
	msg.route("/IA", osc_forms_config_rec, addrOffset);	// Index Add led
	msg.route("/IF", osc_forms_config_rec, addrOffset);	// Index Add pal
	msg.route("/SL", osc_forms_config_rec, addrOffset);	// Start Led
	msg.route("/NL", osc_forms_config_rec, addrOffset);	// NR Leds
	msg.route("/JD", osc_forms_config_rec, addrOffset);	// Juggle Dots
	msg.route("/RF", osc_forms_config_rec, addrOffset);

	// Faders
	msg.route("/FA", osc_forms_fader_rec, addrOffset);	// Fade
	msg.route("/GL", osc_forms_fader_rec, addrOffset);	// glitter
	msg.route("/JS", osc_forms_fader_rec, addrOffset);	// juggle speed

														//msg.route("/f1/FA", osc_rec_forms_fader, addrOffset);
														//msg.route("/f0/GL", osc_rec_forms_fader, addrOffset);	// Glitter Value
														//msg.route("/f1/GL", osc_rec_forms_fader, addrOffset);
														//msg.route("/f0/JS", osc_rec_forms_fader, addrOffset);	// Juggle Speed
														//msg.route("/f1/JS", osc_rec_forms_fader, addrOffset);
														//
														//DBG_OUTPUT_PORT.println("yeah");      
}

*/
/*



// MASTER    OSC: /m


void osc_master_conf_play_save(OSCMessage &msg, int addrOffset) {
	// OSC MESSAGE :/m/save/row/coll=1
	if (bool(msg.getFloat(0)) == true) 
	{
		String select_mode_string;
		// String select_bit_string;
		char address[6];
		bool switch_bool = false;

		msg.getAddress(address, addrOffset + 1);
		for (byte i = 0; i < sizeof(address); i++) {
			if (address[i] == '/') {
				switch_bool = true;

			}
			else if (switch_bool == false) {
				select_mode_string = select_mode_string + address[i];

			}

		}

		int select_mode_int = select_mode_string.toInt() - 1;
		FS_play_conf_write(select_mode_int);

		msg.getAddress(address, 0,3);
		if (address[1] != 'x')
		{
#ifndef OSC_MC_SERVER_DISABLED
			//if (get_bool(OSC_MC_SEND) == true)
			osc_mc_send(String("/x/m/save/" + select_mode_string + "/1"), float(1));
			
#endif
			osc_send_MSG_String("/m/INFO", String("Saved :  " + String(select_mode_int)));
		}
	}
}

void osc_master_settings_send()
{
	//float test = float(led_cfg.bri) / float(led_cfg.max_bri);
	// debugMe("ohoho : ");
	//debugMe(String(test));
	//debugMe(led_cfg.bri);
	//debugMe(led_cfg.max_bri);

	osc_queu_MSG_float("/m/bri", byte_tofloat(led_cfg.bri, 255)) ; //float(led_cfg.bri) / float(led_cfg.max_bri) );
	osc_queu_MSG_float("/m/r", byte_tofloat(led_cfg.r,255));
	osc_queu_MSG_float("/m/g", byte_tofloat(led_cfg.g,255));
	osc_queu_MSG_float("/m/b", byte_tofloat(led_cfg.b,255));
	osc_queu_MSG_float("/m/palbri", byte_tofloat(led_cfg.pal_bri, 255));
	osc_queu_MSG_float("/m/ups", byte_tofloat(led_cfg.pal_fps, MAX_PAL_FPS));
	osc_queu_MSG_float("/m/fftups", byte_tofloat(fft_led_cfg.fps, MAX_PAL_FPS));

	osc_queu_MSG_float("/m/rl", float(led_cfg.r));
	osc_queu_MSG_float("/m/gl", float(led_cfg.g));
	osc_queu_MSG_float("/m/bl", float(led_cfg.b));
	osc_queu_MSG_float("/m/bril", float(led_cfg.bri));
	osc_queu_MSG_float("/m/palbril",float(led_cfg.pal_bri));
	osc_queu_MSG_float("/m/upsl", float(led_cfg.pal_fps));
	osc_queu_MSG_float("/m/fftupsl", float(fft_led_cfg.fps));

	osc_queu_MSG_float("/m/fire/coolL", float(led_cfg.fire_cooling));
	osc_queu_MSG_float("/m/fire/sparkL", float(led_cfg.fire_sparking));

	osc_queu_MSG_float("/m/blend", float(get_bool(BLEND_INVERT)));
	yield();
}

void osc_master_basic_reply(String address , uint8_t value)
{
		 // send back label info
#ifndef OSC_MC_SERVER_DISABLED
	//if (get_bool(OSC_MC_SEND) == true)
	osc_mc_send(String("/x" + address), value );
	
#endif
	osc_queu_MSG_float(String(address + "l"), value);
}

void osc_master_bpm_manual(OSCMessage &msg, int addrOffset) 
{
	if (bool(msg.getFloat(0)) == true)			// only on pushdown
	{
		unsigned long curT = millis();

		if (led_cfg.bpm_lastT == 1)			led_cfg.bpm_lastT = curT;
		else if (led_cfg.bpm_diff == 1)		led_cfg.bpm_diff = curT - led_cfg.bpm_lastT;
		else
		{
			unsigned long diff = curT - led_cfg.bpm_lastT;

			if (diff > BMP_MAX_TIME) 	diff = led_cfg.bpm_diff;
			else						led_cfg.bpm_diff = (led_cfg.bpm_diff + diff) / 2;
			
			led_cfg.bpm = 60000 / led_cfg.bpm_diff;
			//DBG_OUTPUT_PORT.println(bpm_counter.bpm);


			led_cfg.bpm_lastT = curT;
			osc_send_MSG_String("/m/bpm/bpml", String(led_cfg.bpm));

			if (get_bool(BPM_COUNTER) == true)
			{
				led_cfg.pal_fps = (led_cfg.bpm * 16) / 60;
				osc_send_MSG_String("/m/upsl", String(led_cfg.pal_fps));
				//DBG_OUTPUT_PORT.println("ups?");
				//DBG_OUTPUT_PORT.println(pal_updates_second);
			}

		}
	} // bool only on pushdown
}

void osc_master_bpm_toggle(OSCMessage &msg, int addrOffset) 
{
	// OSC MESSAGE :/m/bpm/toggle/x/x
	if (bool(msg.getFloat(0)) == true) 
	{
		String select_row_string;			// old mode
		// String select_bit_string;
		char address[6];
		bool switch_bool = false;

		msg.getAddress(address, addrOffset + 1);
		for (byte i = 0; i < sizeof(address); i++) 
		{
			if (address[i] == '/')  switch_bool = true;
			else if (switch_bool == false) 	select_row_string = select_row_string + address[i];
		}
		//int select_bit_int = select_bit_string.toInt()-1;
		int select_row_int = select_row_string.toInt() - 1;
		//DBG_OUTPUT_PORT.print("select_mode ") ;
		//DBG_OUTPUT_PORT.println(select_mode_int) ;

		switch (select_row_int)
		{
		case 0:
			led_cfg.bpm = constrain((led_cfg.bpm - osc_cfg.conf_multiply), 1 , 255)   ;
			break;
		case 1:
			osc_queu_MSG_float("/m/bpm/enabled", float(get_bool(BPM_COUNTER)));

		break;
		case 2:
			led_cfg.bpm = constrain((led_cfg.bpm + osc_cfg.conf_multiply),1,255) ;
			break;
		default:
			break;
		}



		osc_send_MSG_String("/m/bpm/bpml", String(led_cfg.bpm));

			
		if (get_bool(BPM_COUNTER) == true)
		{
			led_cfg.pal_fps = (led_cfg.bpm * 16 )/60 ;
			osc_send_MSG_String("/m/upsl", String(led_cfg.pal_fps));
			//DBG_OUTPUT_PORT.println("ups?");
			//DBG_OUTPUT_PORT.println(pal_updates_second);
		}


	}
}


void osc_master_conf_play_load(OSCMessage &msg, int addrOffset) {
	// OSC MESSAGE :/m/load
	if (bool(msg.getFloat(0)) == true)
	{
		String select_mode_string;
		// String select_bit_string;
		char address[15];
		bool switch_bool = false;

		msg.getAddress(address, addrOffset + 1);
		for (byte i = 0; i < sizeof(address); i++) {
			if (address[i] == '/') {
				switch_bool = true;

			}
			else if (switch_bool == false) {
				select_mode_string = select_mode_string + address[i];

			}
		}
		int select_mode_int = select_mode_string.toInt() - 1;

		FS_play_conf_read(select_mode_int);
		LEDS_pal_reset_index();

		msg.getAddress(address, 0, 3);
		if (address[1] != 'x')
		{
#ifndef OSC_MC_SERVER_DISABLED
			//if (get_bool(OSC_MC_SEND) == true)
			osc_mc_send(String("/x/m/load/" + select_mode_string + "/1"), float(1));

#endif
			osc_send_MSG_String("/m/INFO", String("Loaded :  " + String(select_mode_int)));
		}
		osc_master_settings_send();
	}

}


void osc_master_routing(OSCMessage &msg, int addrOffset)
{

		msg.route("/load", osc_master_conf_play_load, addrOffset);
		msg.route("/save", osc_master_conf_play_save, addrOffset);

		if (msg.fullMatch("/ref", addrOffset) && bool(msg.getFloat(0)) == true)		osc_master_settings_send();

		if (msg.fullMatch("/bri",addrOffset))			{ led_cfg.bri	= byte(msg.getFloat(0)	* 255); osc_master_basic_reply("/m/bri",led_cfg.bri);}// osc_mc_send("/x/bri",		led_brightness);}
        if (msg.fullMatch("/r",addrOffset))				{ led_cfg.r		= byte(msg.getFloat(0)	* 255); osc_master_basic_reply("/m/r", led_cfg.r);}// osc_mc_send("/x/r",		master_rgb.r);}
        if (msg.fullMatch("/g",addrOffset))				{ led_cfg.g		= byte(msg.getFloat(0)	* 255); osc_master_basic_reply("/m/g", led_cfg.g);}// osc_mc_send("/x/g",		master_rgb.g);}
        if (msg.fullMatch("/b",addrOffset))				{ led_cfg.b		= byte(msg.getFloat(0)	* 255); osc_master_basic_reply("/m/b", led_cfg.b);}// osc_mc_send("/x/b" ,		master_rgb.b) ; }
		if (msg.fullMatch("/palbri", addrOffset))		{ led_cfg.pal_bri = byte(msg.getFloat(0) * 255); osc_master_basic_reply("/m/palbri", led_cfg.pal_bri); }// osc_mc_send("/x/b" ,		master_rgb.b) ; }

		if (msg.fullMatch("/blend", addrOffset))		{ write_bool(BLEND_INVERT, bool(msg.getFloat(0))); }   // global_blend_switch = bool(msg.getFloat(0));

		if (msg.fullMatch("/ups", addrOffset))		{ led_cfg.pal_fps = constrain(byte(msg.getFloat(0) * MAX_PAL_FPS), 1, MAX_PAL_FPS); osc_master_basic_reply("/m/ups", led_cfg.pal_fps); }
		if (msg.fullMatch("/fftups", addrOffset)) { fft_led_cfg.fps = constrain(byte(msg.getFloat(0) * MAX_PAL_FPS), 1, MAX_PAL_FPS); osc_queu_MSG_float("/m/fftupsl", float(fft_led_cfg.fps)); yield();  comms_S_FPS(fft_led_cfg.fps); }
		if (msg.fullMatch("/FPS", addrOffset))			osc_queu_MSG_float("/m/FPSL", float(LEDS_get_FPS()));

		if (msg.fullMatch("/fire/cool", addrOffset)) { led_cfg.fire_cooling = constrain(byte(msg.getFloat(0)), 20, 100); osc_queu_MSG_float("/m/fire/coolL", float(led_cfg.fire_cooling)); }
		if (msg.fullMatch("/fire/spark", addrOffset)) { led_cfg.fire_sparking = constrain(byte(msg.getFloat(0)), 20, 100); osc_queu_MSG_float("/m/fire/sparkL", float(led_cfg.fire_sparking)); }
		

		if (msg.fullMatch("/bpm/enabled", addrOffset))	write_bool(BPM_COUNTER, bool(msg.getFloat(0)));
		msg.route("/bpm/toggle",						osc_master_bpm_toggle, addrOffset);
		msg.route("/bpm/man",							osc_master_bpm_manual, addrOffset);

}

*/

// Device Settings OSC:/DS
void osc_DS_refresh()
{
		
			for (uint8_t i = 0; i < 4; i++)
			{
				// IP Config
				osc_queu_MSG_float(String("/DS/SIPL/" + String(i)), float(wifi_cfg.ipStaticLocal[i]));
				osc_queu_MSG_float(String("/DS/SNML/" + String(i)), float(wifi_cfg.ipSubnet[i]));
				osc_queu_MSG_float(String("/DS/DGWL/" + String(i)), float(wifi_cfg.ipDGW[i]));
				osc_queu_MSG_float(String("/DS/DNSL/" + String(i)), float(wifi_cfg.ipDNS[i]));
			}

			osc_send_out_float_MSG_buffer();  // send out some of it and yield
			yield();
			osc_queu_MSG_float(String("/DS/ESIP"), float(get_bool(STATIC_IP_ENABLED)));
			osc_queu_MSG_float(String("/DS/httpd"), float(get_bool(HTTP_ENABLED)));
			osc_queu_MSG_float(String("/DS/debug"), float(get_bool(DEBUG_OUT)));
			osc_queu_MSG_float(String("/DS/TNdebug"), float(get_bool(DEBUG_TELNET)));


			osc_queu_MSG_float(String("/DS/WAP"), float(get_bool(WIFI_MODE)));  //debugMe(get_bool(WIFI_MODE));
			osc_send_MSG_String("/DS/SSID", String(wifi_cfg.ssid));
			osc_send_MSG_String("/DS/WPW", String(wifi_cfg.pwd));
			osc_send_MSG_String("/DS/WAPNL", String(wifi_cfg.APname));

			
			// General
			osc_queu_MSG_float("/DS/EDIT", float(get_bool(OSC_EDIT)));
			osc_queu_MSG_float("/DS/OSC_M", float(get_bool(OSC_MC_SEND)));

			
			osc_send_out_float_MSG_buffer();   // send out some of it and yield
			yield();



		

		//debugMe(wifi_cfg.ipStaticLocal[i]);

	 
}



void osc_DS_ip_in(OSCMessage &msg, int addrOffset)
{	// OSC IN Device settings IP
	// OSC MESSAGE :/DS/?type?:/row/collum   xx/?/?
		// OSC MESSAGE :/DS/???:/bit/row/collum   xx/?/?
		// :/bit/+-option/strip

		//DBG_OUTPUT_PORT.println("lol"); 

		//byte addrOffset = 7+6+1 ;
		String type_string;
		String option_string;
		String byte_string;

		int type_int;
		int option_int;
		int byte_int;


		char address[4];
		char address_out[20];
		bool switch_bool = false;

		String outbuffer = "/strips/sX/SIL/X";
		float outvalue = 255;

		msg.getAddress(address, addrOffset -3 , 3);
		type_string = type_string + address[0];


		//debugMe(address);

		memset(address, 0, sizeof(address));

		msg.getAddress(address, addrOffset + 1);
		//DBG_OUTPUT_PORT.println(address);

		//debugMe(address);

		for (byte i = 0; i < sizeof(address); i++) {
			if (address[i] == '/') {
				switch_bool = true;

			}
			else if (switch_bool == false) {
				option_string = option_string + address[i];

			}
			else
				byte_string = byte_string + address[i];

		}

		type_int = type_string.toInt();
		option_int = option_string.toInt() - 1;
		byte_int = byte_string.toInt() - 1;

		memset(address, 0, sizeof(address));

		msg.getAddress(address, addrOffset - 3, 3);

		/*DBG_OUTPUT_PORT.println(select_mode_int);
		DBG_OUTPUT_PORT.println("--"); */

		//byte msg_size = msg.size();

		//char full_addr[msg_size + 1 - 4];
		


		if (bool(msg.getFloat(0)) == true) {  // only on pushdown
			if (address[0] == 'S' && address[1] == 'I' && address[2] == 'P') {
				switch (option_int) 
				{
					case 0:
						wifi_cfg.ipStaticLocal[byte_int] -= osc_cfg.conf_multiply;
					
						break;
					case 2:
						wifi_cfg.ipStaticLocal[byte_int] += osc_cfg.conf_multiply;
						break;
				}

				outvalue = float(wifi_cfg.ipStaticLocal[byte_int]);


			}

	
			if (address[0] == 'S' && address[1] == 'N' && address[2] == 'M') {
					switch (option_int)
					{
					case 0:
						wifi_cfg.ipSubnet[byte_int] -= osc_cfg.conf_multiply;

						break;
					case 2:
						wifi_cfg.ipSubnet[byte_int] += osc_cfg.conf_multiply;
						break;
					}

					outvalue = float(wifi_cfg.ipSubnet[byte_int]);


				}

			if (address[0] == 'D' && address[1] == 'G' && address[2] == 'W') {
				switch (option_int)
				{
				case 0:
					wifi_cfg.ipDGW[byte_int] -= osc_cfg.conf_multiply;

					break;
				case 2:
					wifi_cfg.ipDGW[byte_int] += osc_cfg.conf_multiply;
					break;
				}

				outvalue = float(wifi_cfg.ipDGW[byte_int]);


			}
			
			if (address[0] == 'D' && address[1] == 'N' && address[2] == 'S') {
				switch (option_int)
				{
				case 0:
					wifi_cfg.ipDNS[byte_int] -= osc_cfg.conf_multiply;

					break;
				case 2:
					wifi_cfg.ipDNS[byte_int] += osc_cfg.conf_multiply;
					break;
				}

				outvalue = float(wifi_cfg.ipDNS[byte_int]);


			}

			outbuffer = String("/DS/" + String(address) + "L/" + String(byte_int ));
			osc_queu_MSG_float(outbuffer, outvalue);
			//debugMe(outbuffer);
		}







}

void osc_device_settings_routing(OSCMessage &msg, int addrOffset) 
{	// routing of Device settings OSC messages

	char address[10];
	msg.getAddress(address);
	debugMe(String(address));

	
	// OSC MESSAGE :/DS
	if (msg.fullMatch("/DEL_ALL", addrOffset) && bool(msg.getFloat(0)) == true)		{ osc_send_MSG_String("/DS/INFO", String("ALL DELETED "));	FS_osc_delete_all_saves(); };
	if (msg.fullMatch("/RESET", addrOffset) && bool(msg.getFloat(0)) == true)		{ osc_send_MSG_String("/DS/INFO", String("Resetting")); delay(100);		ESP.restart(); };

	if (msg.fullMatch("/ref", addrOffset) && bool(msg.getFloat(0)) == true)		osc_DS_refresh();
	if (msg.fullMatch("/httpd", addrOffset))		httpd_toggle_webserver();
		
	if (msg.fullMatch("/debug", addrOffset))		{ osc_send_MSG_String("/DS/INFO", String("Debug : "+ String(bool(msg.getFloat(0)))));	 debugMe(String("Debug : "+ String(bool(msg.getFloat(0))))); write_bool(DEBUG_OUT, bool(msg.getFloat(0)));}
	if (msg.fullMatch("/TNdebug", addrOffset))			{ write_bool(DEBUG_TELNET, bool(msg.getFloat(0))); }
	if (msg.fullMatch("/ESIP", addrOffset))			{ osc_send_MSG_String("/DS/INFO", String("Static IP : " + String(bool(msg.getFloat(0)))));	write_bool(STATIC_IP_ENABLED, bool(msg.getFloat(0))); }
	if (msg.fullMatch("/WAP", addrOffset))			{ write_bool(WIFI_MODE, bool(msg.getFloat(0))); }//debugMe("BLAH!!!");debugMe(get_bool(WIFI_MODE)); }


	if (msg.fullMatch("/IPSAVE", addrOffset) && bool(msg.getFloat(0)) == true) 		{FS_wifi_write(0); osc_send_MSG_String("/DS/INFO", String("IP saved")); }
	if (msg.fullMatch("/BSAVE", addrOffset) && bool(msg.getFloat(0)) == true) { FS_Bools_write(0); osc_send_MSG_String("/DS/INFO", String("Settings saved"));	FS_dht_write(0); }
	if (msg.fullMatch("/RSSI", addrOffset) && bool(msg.getFloat(0)) == true)		osc_queu_MSG_float("/DS/RSSIL", float(WiFi.RSSI()));



	if (msg.fullMatch("/Fheap", addrOffset) && bool(msg.getFloat(0)) == true)		osc_queu_MSG_float("/DS/FheapL", float(ESP.getFreeHeap()));
	if (msg.fullMatch("/heap", addrOffset) && bool(msg.getFloat(0)) == true)		osc_queu_MSG_float("/DS/heapL", float(ESP.getFreeHeap()));


	if (msg.fullMatch("/fft_send_master", addrOffset))  							write_bool(FFT_MASTER_SEND, bool(msg.getFloat(0)));

	if (msg.fullMatch("/OSC_M", addrOffset))										write_bool(OSC_MC_SEND, bool(msg.getFloat(0)));



	if (msg.fullMatch("/EDIT", addrOffset))				write_bool(OSC_EDIT, bool(msg.getFloat(0)));

	msg.route("/SIP", osc_DS_ip_in, addrOffset);
	msg.route("/SNM", osc_DS_ip_in, addrOffset);
	msg.route("/DGW", osc_DS_ip_in, addrOffset);
	msg.route("/DNS", osc_DS_ip_in, addrOffset);

	

	//debugMe("DS routing END");
}



void osc_relay_routing(OSCMessage &msg, int addrOffset)
{
	char address[3];
	//msg.getAddress(address);
	debugMe(String(address));
	uint8_t relay_nr = 0;
	String relay_nr_string;
	msg.getAddress(address, addrOffset + 1, 1);

	
	relay_nr_string = relay_nr_string + address[0];
	relay_nr = relay_nr_string.toInt();
	//relay_set(relay_nr, bool(msg.getFloat(0)));
	


}


// Main OSC loop
void OSC_loop() 
{	// the main osc loop
	// it hast 2 main parts, 
	// part 1 is for the unicast OSC communication
	// part 2 is for the multicast OSC communication


	OSCMessage oscMSG;
	OSCMessage oscMSG_MC;

	int size = osc_server.parsePacket();

	if (size > 0) {
		while (size--) {
			oscMSG.fill(osc_server.read());
		}
		if (!oscMSG.hasError())
		{
			//debugMe("osc: rem , locel");
			//debugMe(osc_mc_server.remoteIP());
			//debugMe(WiFi.localIP());

			//oscMSG.route("/m", osc_master_routing);
			oscMSG.route("/DS", osc_device_settings_routing);
			oscMSG.route("/relay", osc_relay_routing);
			//oscMSG.route("/multipl", osc_multipl_rec);
			
			// if (oscMSG.fullMatch("/FULL_REF", 0) && bool(oscMSG.getFloat(0)) == true) osc_send_all();
			//if (oscMSG.fullMatch("/reset-index", 0) && bool(oscMSG.getFloat(0)) == true) LEDS_pal_reset_index();
		}
		else {
			//error = bundle.getError();
			Serial.print("OSC error: ");
			//Serial.println( bundle.getError());
		}
	}
	osc_send_out_float_MSG_buffer();


}


#endif // OAC
//
#endif //BLAH