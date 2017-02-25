// 
// 
// 

#include "dht_mos.h"
#include "wemos_relay.h" // for relay checking

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

#define DHT_DEF_MINTEMP 5
#define DHT_RELAY_NO 0
#define DHT_DEF_HUM_DIFF 10
#define DHT_DEF_MIN_HUMID 60

dht_sensor_strucht dht_sensor[DHT_NR_SENSORS] = { { false ,0,0,DHT_0_TYPE,DHT_DEF_MINTEMP, DHT_RELAY_NO,DHT_DEF_HUM_DIFF, DHT_DEF_MIN_HUMID } }; // , { false ,0,0,DHT_0_TYPE } };

DHT dht(DHT_0_PIN, DHT_0_TYPE);

// External
extern relay_cfg_struct relay[NR_OF_RELAYS];
extern void relay_set(uint8_t relay_nr, bool state);

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

	//dht_temp_master_stage0[sensor_nr].addValue(dht_sensor[sensor_nr].temp);
	//dht_humidity_master_stage0[sensor_nr].addValue(dht_sensor[sensor_nr].humidity);
}


void dht_get_sensor_value(uint8_t sensorNR = 0)
{
	dht_sensor[sensorNR].temp  = dht.readTemperature();
	dht_sensor[sensorNR].humidity = dht.readHumidity();
	

	if (isnan(dht_sensor[sensorNR].humidity) || isnan(dht_sensor[sensorNR].temp))	// check if sensor reading is ok if not do ...	
	{
		dht_sensor[sensorNR].readOk = false;
		debugMe("Failed to read from DHT sensor!");
	}
	else				// sensor reading ok
	{
		dht_sensor[sensorNR].readOk = true;
		Serial.println(dht_sensor[sensorNR].temp);
		Serial.println(dht_sensor[sensorNR].humidity);

		dht_temp_stage0.addValue(dht_sensor[sensorNR].temp);
		dht_humidity_stage0.addValue(dht_sensor[sensorNR].humidity);

		debugMe("avg_temp ", false);
		debugMe(float(dht_temp_stage0.getAverage()), true);
		debugMe("avg_humidity ", false);
		debugMe(float(dht_humidity_stage0.getAverage()), true);
	}

}



void dht_process_values(uint8_t sensorNR = 0)
{
	if (dht_sensor[sensorNR].readOk == true)
	{
		if (relay[dht_sensor[sensorNR].relay_no].state == true)  // relay is on 
		{
			// check the temp and humidity and switch off if below temp or humidity
			if ((dht_sensor[sensorNR].temp <= dht_sensor[sensorNR].mintemp - 1) || dht_sensor[sensorNR].humidity < (dht_sensor[sensorNR].minHumid - dht_sensor[sensorNR].HumiDiff))
			{
				relay_set(dht_sensor[sensorNR].relay_no, false);
			}
		}
		else if (relay[dht_sensor[sensorNR].relay_no].state == false)  // relay is on 
		{	
			// if the relay is off check temp and humidity and power on ir required
			if ((dht_sensor[sensorNR].temp >= dht_sensor[sensorNR].mintemp) || dht_sensor[sensorNR].humidity >= dht_sensor[sensorNR].minHumid)
			{
				relay_set(dht_sensor[sensorNR].relay_no, true);
			}
		}
	}
	else
	{
		// check how ofter false reading send out a msg to inform on x failure of readings.
		;
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
	dht_process_values();


}



