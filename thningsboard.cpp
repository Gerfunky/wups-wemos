// 
// 
// 

#include "thningsboard.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>


#define MQTT_TOKEN "8wGm3u263ss1y8dGHy61"
#define THINGSBOARD_DHT_ALIAS "HomeMos"
#define DEV_ID "c9b43e60-0086-11e7-bd27-63324ef75de3"
#define THINGSBOARD_MQTT_PORT 1883
#define DEF_IP_SERVER		{172,16,222,4}


extern void debugMe(String input, boolean line = true);
extern void debugMe(float input, boolean line = true);
extern void debugMe(uint8_t input, boolean line = true);
extern void debugMe(int input, boolean line = true);



#include "dht_mos.h"
extern dht_sensor_strucht dht_sensor[DHT_NR_SENSORS];
extern float dht_get_temp_stage0_Min();
extern float dht_get_temp_stage0_Avg();
extern float dht_get_temp_stage0_Max();
extern float dht_get_humid_stage0_Max();
extern float dht_get_humid_stage0_Min();
extern float dht_get_humid_stage0_Avg();






IPAddress thingsboard_server_ip = DEF_IP_SERVER; //"172.16.222.4";


WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;




void mqtt_getAndSendTemperatureAndHumidityData()
{
	if (mqtt_client.connected())
	{
		Serial.println("Collecting temperature data.");

		// Reading temperature or humidity takes about 250 milliseconds!
		float h = dht_sensor[0].humidity;
		// Read temperature as Celsius (the default)
		float t = dht_sensor[0].temp;

		// Check if any reads failed and exit early (to try again).
	//	if (isnan(h) || isnan(t)) {
		//Serial.println("Failed to read from DHT sensor!");
		//return;
		//}



		String temperature = String(t);
		String humidity = String(h);

		/*
		// Just debug messages
		Serial.print("Sending temperature and humidity : [");
		Serial.print(temperature); Serial.print(",");
		Serial.print(humidity);
		Serial.print("]   -> ");
		*/
		// Prepare a JSON payload string
		String payload = "{";
						payload += "\"temperature\":"; payload += temperature; 
		//payload += ","; payload += "\"humidity\":"; payload += humidity; 
		payload += ","; payload += "\"max_temp\":"; payload += String(dht_get_temp_stage0_Max()) ;  
		payload += ","; payload += "\"min_temp\":"; payload += String(dht_get_temp_stage0_Min()); 
		//payload += ","; payload += "\"max_humid\":"; payload += String(dht_get_humid_stage0_Max()); 
		//payload += ","; payload += "\"min_humid\":"; payload += String(dht_get_humid_stage0_Min()); 
		payload += ","; payload += "\"DHT_Errors\":"; payload += String(dht_sensor[0].totalErrors);
						payload += "}";

		// Send payload
		char attributes[100];
		payload.toCharArray(attributes, 100);
		mqtt_client.publish("v1/devices/me/telemetry", attributes);


		    payload = "{";
		//payload += "\"temperature\":"; payload += temperature;
					 payload += "\"humidity\":"; payload += humidity;
		//payload += ","; payload += "\"max_temp\":"; payload += String(dht_get_temp_stage0_Max());
		//payload += ","; payload += "\"min_temp\":"; payload += String(dht_get_temp_stage0_Min());
		payload += ","; payload += "\"max_humid\":"; payload += String(dht_get_humid_stage0_Max());
		payload += ","; payload += "\"min_humid\":"; payload += String(dht_get_humid_stage0_Min()); 
		//payload += ","; payload += "\"DHT_Errors\":"; payload += String(dht_sensor[0].totalErrors);
		payload += "}";

		char attributes2[100];
		payload.toCharArray(attributes2, 100);
		mqtt_client.publish("v1/devices/me/telemetry", attributes2);

		debugMe(attributes);
	}
}








void thingsboard_connect()
{
	debugMe("Connecting to Thingsboard node ...");
	// Attempt to connect (clientId, username, password)
	if (mqtt_client.connect("Esp8266 Device", MQTT_TOKEN, NULL )) {
		debugMe("[DONE]");
	}
	else {
		debugMe("[FAILED] [ rc = ", false);
		debugMe(mqtt_client.state(),false);
		debugMe(" : retrying in 5 seconds]");
		// Wait 5 seconds before retrying
		delay(3000);
	}
}


void thingsboard_setup()
{
	mqtt_client.setServer(thingsboard_server_ip, THINGSBOARD_MQTT_PORT);
	thingsboard_connect();

	
}

void thingsboard_loop()
{
	if (!mqtt_client.connected()) {
		thingsboard_connect();
	}
	

	//if (millis() - lastSend > dht_sensor[0].update_sec * 1000) { // Update and send only after 1 seconds
	//	mqtt_getAndSendTemperatureAndHumidityData();
	//	lastSend = millis();
	//}


	mqtt_client.loop();
}
