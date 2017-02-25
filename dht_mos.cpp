// 
// 
// 

#include "dht_mos.h"

#ifdef _MSC_VER  
	#include <DHT\DHT.h>
	#include <RunningAverage\RunningAverage.h>
#else
	#include <RunningAverage.h>			// For geting an average 
	#include <DHT.h>
#endif


#define DHT_NR_SENSORS 1
#define DHT_0_PIN D4
#define DHT_0_TYPE DHT11

#define DHT_NR_SLAVES 2

dht_sensor_strucht dht_sensor[DHT_NR_SLAVES] = {{ false ,0,0,DHT_0_TYPE }, { false ,0,0,DHT_0_TYPE }};

DHT dht(DHT_0_PIN, DHT_0_TYPE);


/// running average
#define DHT_NR_STAGE0_SAMPLES 10					// How many samples to take for the FFT average = Stage 1
RunningAverage dht_temp_stage0(DHT_NR_STAGE0_SAMPLES);	// Buffers for the FFT values
RunningAverage dht_humidity_stage0(DHT_NR_STAGE0_SAMPLES);	// Buffers for the FFT values

// master buffers
#define DHT_NR_MASTER_STAGE0_SAMPLES 12					//
RunningAverage dht_temp_master_stage0(DHT_NR_MASTER_STAGE0_SAMPLES);	// Buffers for the
RunningAverage dht_humidity_master_stage0(DHT_NR_MASTER_STAGE0_SAMPLES);	// Buffers 
															// add the Debug functions   --     send to debug   MSG to  Serial or telnet --- Line == true  add a CR at the end.
extern void debugMe(String input, boolean line = true);
extern void debugMe(float input, boolean line = true);
extern void debugMe(uint8_t input, boolean line = true);
extern void debugMe(int input, boolean line = true);


void DHT_recive_wifi_sensor_value(uint8_t sensor_nr, uint8_t temp, uint8_t humidity)
{
	dht_sensor[sensor_nr].temp = float(temp);
	dht_sensor[sensor_nr].temp = float(humidity);
	dht_sensor[sensor_nr].readOk = true;

	dht_temp_master_stage0[sensor_nr].addValue(dht_sensor[sensor_nr].temp);
	dht_humidity_master_stage0[sensor_nr].addValue(dht_sensor[sensor_nr].humidity);
}


void dht_get_sensor_value()
{
	dht_sensor[0].temp = dht.readTemperature();
	dht_sensor[0].humidity = dht.readHumidity();
	

	if (isnan(dht_sensor[0].humidity) || isnan(dht_sensor[0].temp))	// check if sensor reading is ok if not do ...	
	{
		dht_sensor[0].readOk = false;
		debugMe("Failed to read from DHT sensor!");
	}
	else				// sensor reading ok
	{
		dht_sensor[0].readOk = true;
		Serial.println(dht_sensor[0].temp);
		Serial.println(dht_sensor[0].humidity);

		dht_temp_stage0.addValue(dht_sensor[0].temp);
		dht_humidity_stage0.addValue(dht_sensor[0].humidity);

		debugMe("avg_temp ", false);
		debugMe(float(dht_temp_stage0.getAverage()), true);
		debugMe("avg_humidity ", false);
		debugMe(float(dht_humidity_stage0.getAverage()), true);
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



