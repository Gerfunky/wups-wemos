// 
// 
// 



#ifdef _MSC_VER  
#include <ESP8266WiFi\src\ESP8266WiFi.h>
#include <arduino_589948\src\PubSubClient.h>
#include <arduino_981649\ArduinoJson.h>

#else
#include <ESP8266WiFi.h>				// REquired for other libs
#include <PubSubClient.h>
#include <ArduinoJson.h>		

#endif


#include "thningsboard.h"



#define MQTT_TOKEN "8wGm3u263ss1y8dGHy61"
#define THINGSBOARD_DHT_ALIAS "HomeMos"
#define DEV_ID "c9b43e60-0086-11e7-bd27-63324ef75de3"
#define THINGSBOARD_MQTT_PORT 1883
#define DEF_TB_IP_SERVER		{172,16,222,4}
#define DEF_TB_PORT 1883
#define DEF_TB_ENABLED true;

thingsboard_Struct tboard;



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


extern boolean FS_thingsboard_read(uint8_t conf_nr = 0);
extern void	FS_thingsboard_write(uint8_t conf_nr = 0);

extern String ntp_get_resettime();

IPAddress thingsboard_server_ip = DEF_TB_IP_SERVER; //"172.16.222.4";


WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

#define MMQT_ATTRIBUTES_URL "v1/devices/me/attributes"
#define MMQT_TELEMETRY_URL "v1/devices/me/telemetry"

void mmqt_send_attributes()
{
	if (tboard.enabled)
	{
		// Prepare gpios JSON payload string
		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& data = jsonBuffer.createObject();

		data["Name"] = "Hello_MOS";
		data["Errors"] = dht_sensor[0].totalErrors;
		data["LastReset"] = ntp_get_resettime();
		
		char payload[256];
		data.printTo(payload, sizeof(payload));
		String strPayload = String(payload);
		Serial.print("Get attributes status: ");
		Serial.println(strPayload);
		//return strPayload;

		mqtt_client.publish(MMQT_ATTRIBUTES_URL, strPayload.c_str());
	}	
}

void mmqt_send_dht_temp()
{
	if (tboard.enabled)
	{
		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& data = jsonBuffer.createObject();

		data["temperature"] = dht_sensor[0].temp;
		data["max_temp"] = dht_get_temp_stage0_Max();
		data["min_temp"] = dht_get_temp_stage0_Min();
		data["Errors"] = dht_sensor[0].totalErrors;
		char payload[256];
		data.printTo(payload, sizeof(payload));
		String strPayload = String(payload);
		Serial.print("Get TELEMETRY temp: ");
		Serial.println(strPayload);

		mqtt_client.publish(MMQT_TELEMETRY_URL, strPayload.c_str());
	}
}

void mmqt_send_dht_humid()
{
	if (tboard.enabled)
	{
		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& data = jsonBuffer.createObject();

		data["humidity"] = dht_sensor[0].humidity;
		data["max_humid"] = dht_get_humid_stage0_Max();
		data["min_humid"] = dht_get_humid_stage0_Min();
		char payload[256];
		data.printTo(payload, sizeof(payload));
		String strPayload = String(payload);
		Serial.print("Get TELEMETRY humid : ");
		Serial.println(strPayload);

		mqtt_client.publish(MMQT_TELEMETRY_URL, strPayload.c_str());
	}
}




void thingsboard_connect()
{
	if (true == tboard.enabled)
	{
		debugMe("Connecting to Thingsboard node ...", false );
		// Attempt to connect (clientId, username, password)
		if (mqtt_client.connect("Esp8266 Device", tboard.mqtt_token, NULL)) {
			debugMe("[DONE]");
		}
		else {
			debugMe("[FAILED] [ rc = ", false);
			debugMe(mqtt_client.state(), false);
			debugMe(" : retrying in 5 seconds]");
			// Wait 5 seconds before retrying
			delay(3000);
		}
	}
}







void thingsboard_setup()
{

	if (false == FS_thingsboard_read())
	{
		tboard.ip_mqtt_server = DEF_TB_IP_SERVER;
		tboard.port_mqtt_server = DEF_TB_PORT;
		tboard.enabled = DEF_TB_ENABLED;

		String dev_mqqt_token = MQTT_TOKEN;

		dev_mqqt_token.toCharArray(tboard.mqtt_token, dev_mqqt_token.length() + 1);

		FS_thingsboard_write();

	}
	mqtt_client.setServer(tboard.ip_mqtt_server, tboard.port_mqtt_server);
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
