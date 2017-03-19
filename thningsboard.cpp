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
#define DEF_TB_UPDATE_MIN_TIMEOUT 1; // send every x min.

thingsboard_Struct tboard;



extern void debugMe(String input, boolean line = true);
extern void debugMe(float input, boolean line = true);
extern void debugMe(uint8_t input, boolean line = true);
extern void debugMe(int input, boolean line = true);

#include "wifi-ota.h"
extern wifi_Struct wifi_cfg;
extern String get_ip_str();

#include "dht_mos.h"
extern dht_sensor_strucht dht_sensor[DHT_NR_SENSORS];
extern float dht_get_temp_stage0_Min();
extern float dht_get_temp_stage0_Avg();
extern float dht_get_temp_stage0_Max();
extern float dht_get_humid_stage0_Max();
extern float dht_get_humid_stage0_Min();
extern float dht_get_humid_stage0_Avg();

#include "wemos_relay.h"
extern relay_cfg_struct relay[NR_OF_RELAYS];


extern boolean FS_thingsboard_read(uint8_t conf_nr = 0);
extern void	FS_thingsboard_write(uint8_t conf_nr = 0);

extern String ntp_get_resettime();
extern String TimeNow();

IPAddress thingsboard_server_ip = DEF_TB_IP_SERVER; //"172.16.222.4";


WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

#define MMQT_ATTRIBUTES_URL "v1/devices/me/attributes"
#define MMQT_TELEMETRY_URL "v1/devices/me/telemetry"


String mqtt_TimeNow()
{
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& data = jsonBuffer.createObject();
	data["LastUpdate"] = TimeNow();
	data["SendMinUpdate"] = tboard.updateMinTimeOut;
	data["ReadDHTsec"] = dht_sensor[0].update_sec;

	char payload[256];
	data.printTo(payload, sizeof(payload));
	String strPayload = String(payload);
	return strPayload;

}



String mmqt_get_attributes()
{

	// Prepare gpios JSON payload string
	StaticJsonBuffer<300> jsonBuffer;
	JsonObject& data = jsonBuffer.createObject();

	data["Name"] = wifi_cfg.APname;
	data["Errors"] = dht_sensor[0].totalErrors;
	data["LastReset"] = ntp_get_resettime();
	data["LocalIP"] = get_ip_str();

	char payload[350];
	data.printTo(payload, sizeof(payload));
	String strPayload = String(payload);

	debugMe("Get attributes status: ",false);
	debugMe(strPayload);
	return strPayload;


}

void mmqt_send_attributes()
{
	if (tboard.enabled)
	{

		mqtt_client.publish(MMQT_ATTRIBUTES_URL, mmqt_get_attributes().c_str());
	}
}

boolean mmqt_send_dht_temp()
{

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& data = jsonBuffer.createObject();

		if (255 != dht_get_temp_stage0_Avg())	 data["temperature"] = dht_get_temp_stage0_Avg(); else return false;
		if (255 != dht_get_temp_stage0_Max())	 data["max_temp"] = dht_get_temp_stage0_Max(); else return false;
		if (255 != dht_get_temp_stage0_Min())	 data["min_temp"] = dht_get_temp_stage0_Min(); else return false;
		data["Errors"] = dht_sensor[0].totalErrors;

		{
			char payload[256];
			data.printTo(payload, sizeof(payload));
			String strPayload = String(payload);
			debugMe("send TELEMETRY temp: ");
			//debugMe(strPayload);
			mqtt_client.publish(MMQT_TELEMETRY_URL, strPayload.c_str());
			return true;
		}
	
}

boolean mmqt_send_dht_humid()
{

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& data = jsonBuffer.createObject();

	if (255 != dht_get_humid_stage0_Avg())	 data["humidity"] = dht_get_humid_stage0_Avg(); else return false;
	if (255 != dht_get_humid_stage0_Max())   data["max_humid"] = dht_get_humid_stage0_Max(); else return false;
	if (255 != dht_get_humid_stage0_Min())	 data["min_humid"] = dht_get_humid_stage0_Min(); else return false;
	//if (255 != data["humidity"] && 255 != data["max_humid"] && 255 != data["min_humid"])
	{
		char payload[256];
		data.printTo(payload, sizeof(payload));
		String strPayload = String(payload);

		debugMe("send TELEMETRY humid : ");
		//debugMe(strPayload);
		mqtt_client.publish(MMQT_TELEMETRY_URL, strPayload.c_str());

		return true;
	}

}



boolean mmqt_send_relay_status()
{
	if (tboard.enabled)
	{
		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& data = jsonBuffer.createObject();

		data["Relay"] = String(relay[0].state);



		{
			char payload[256];
			data.printTo(payload, sizeof(payload));
			String strPayload = String(payload);
			debugMe("send TELEMETRY temp: ");
			//debugMe(strPayload);
			mqtt_client.publish(MMQT_TELEMETRY_URL, strPayload.c_str());
			return true;
		}

		mmqt_send_dht_temp();
		mmqt_send_dht_humid();
	}
}

boolean mmqt_send_dht_settings()
{

	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& data = jsonBuffer.createObject();

	 data["OnTemp"] = dht_sensor[0].mintemp;
	 data["OffTemp"] = dht_sensor[0].mintemp - dht_sensor[0].tempDiff;
	
	 data["OnHumid"] = dht_sensor[0].minHumid;
	 data["OffHumid"] = dht_sensor[0].minHumid - dht_sensor[0].HumiDiff;

	 data["SensorUpdateSeconds"] = dht_sensor[0].update_sec ;
	//if (255 != data["humidity"] && 255 != data["max_humid"] && 255 != data["min_humid"])
	{
		char payload[256];
		data.printTo(payload, sizeof(payload));
		String strPayload = String(payload);

		debugMe("send TELEMETRY humid : ");
		//debugMe(strPayload);
		mqtt_client.publish(MMQT_TELEMETRY_URL, strPayload.c_str());

		return true;
	}

}


/*
void mmqt_on_message(const char* topic, byte* payload, unsigned int length)
{
	debugMe("MMQT_callback");

	char json[length + 1];
	strncpy(json, (char*)payload, length);
	json[length] = '\0';

	debugMe("Topic: ", false);
	debugMe(topic);
	debugMe("Message: ", false);
	debugMe(json);

	// Decode JSON request
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& data = jsonBuffer.parseObject((char*)json);

	if (!data.success())
	{
		debugMe("parseObject() failed");
		return;
	}

	// Check request method
	String methodName = String((const char*)data["method"]);

	if (methodName.equals("getAttributes")) {
		// Reply with GPIO status
		String responseTopic = String(topic);
		responseTopic.replace("request", "response");
		mqtt_client.publish(responseTopic.c_str(), mmqt_get_attributes().c_str());
	}
	



}

*/

#define TB_CONNECT_TIMEOUT_DEF 1;
unsigned long TBconnect_timoutUpdateTime = 0;

void thingsboard_connect()
{
	if (true == tboard.enabled && TBconnect_timoutUpdateTime < millis())
	{
		TBconnect_timoutUpdateTime = (millis() + 1 * 1000 * 60);
		debugMe("Connecting to Thingsboard node ...", false);
		// Attempt to connect (clientId, username, password)
		if (mqtt_client.connect("Esp8266 Device", tboard.mqtt_token, NULL)) {
			debugMe("[DONE]");
			mmqt_send_attributes();
			mmqt_send_dht_settings();
			mmqt_send_relay_status();
		}
		else {
			debugMe("[FAILED] [ rc = ", false);
			debugMe(mqtt_client.state(), false);
			
			// Wait 5 seconds before retrying
			//delay(1000);
		}
	}
}


void thingsboard_send_loop()
{
	if (millis() >= tboard.updateTime && (tboard.enabled))
	{
		if (mmqt_send_dht_humid() && mmqt_send_dht_temp())
		{
			mqtt_client.publish(MMQT_ATTRIBUTES_URL, mqtt_TimeNow().c_str());
			//mmqt_send_relay_status(); //also when changing status send it.
			tboard.updateTime = millis() + 1000 * 60 * tboard.updateMinTimeOut;
			
		}
		else 
			tboard.updateTime = millis() + 1000 * 10;
	}



}




void thingsboard_setup()
{

	if (false == FS_thingsboard_read())
	{
		tboard.ip_mqtt_server = DEF_TB_IP_SERVER;
		tboard.port_mqtt_server = DEF_TB_PORT;
		tboard.enabled = DEF_TB_ENABLED;
		tboard.updateMinTimeOut = DEF_TB_UPDATE_MIN_TIMEOUT;
		String dev_mqqt_token = MQTT_TOKEN;

		dev_mqqt_token.toCharArray(tboard.mqtt_token, dev_mqqt_token.length() + 1);

		FS_thingsboard_write();

	}
	tboard.updateTime = 0;
	mqtt_client.setServer(tboard.ip_mqtt_server, tboard.port_mqtt_server);
	//mqtt_client.setCallback(mmqt_on_message);
	thingsboard_connect();


}

void thingsboard_loop()
{
	if (!mqtt_client.connected()) {
		thingsboard_connect();
	}
	thingsboard_send_loop();


	//if (millis() - lastSend > dht_sensor[0].update_sec * 1000) { // Update and send only after 1 seconds
	//	mqtt_getAndSendTemperatureAndHumidityData();
	//	lastSend = millis();
	//}


	mqtt_client.loop();
}
