// 
// 
// 

#include "dht_mos.h"
#include <DHT.h>

#define DHT_NR_SENSORS 1
#define DHT_0_PIN D4
#define DHT_0_TYPE DHT11


dht_sensor_strucht dht_sensor = { DHT_0_PIN ,0,0,DHT_0_TYPE };

DHT dht(DHT_0_PIN, DHT_0_TYPE);


void dht_get_sensor_value()
{
	dht_sensor.temp = dht.readTemperature();
	dht_sensor.humidity = dht.readHumidity();
	

	if (isnan(dht_sensor.humidity) || isnan(dht_sensor.temp))
	{
		Serial.println("Failed to read from DHT sensor!");
	}
	else
	{
		Serial.println(dht_sensor.temp);
		Serial.println(dht_sensor.humidity);
	}

}


void dht_setup() 
{
	dht.begin();
	/*
	for (uint8_t i = 0; i < DHT_NR_SENSORS; i++)
	{
		pinMode(dht_sensor[i].pin, OUTPUT);
		//digitalWrite(relay[i].pin, LOW);			// set all relays to off at boot
	} */

}


void dht_run()
{

	dht_get_sensor_value();

}



