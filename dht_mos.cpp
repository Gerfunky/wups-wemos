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


dht_sensor_strucht dht_sensor[DHT_NR_SENSORS] = {  false ,0,0,DHT_0_TYPE,DHT_0_PIN ,  DHT_RELAY_NO,DHT_DEF_MINTEMP,DHT_DEF_HUM_DIFF, DHT_DEF_MIN_HUMID,DHT_DEF_TEMP_DIFF,0,0,10 }; // , { false ,0,0,DHT_0_TYPE } };

//DHT dht(DHT_0_PIN, DHT_0_TYPE);
DHT *myDHT[DHT_NR_SENSORS];

// External
extern relay_cfg_struct relay[NR_OF_RELAYS];
extern void relay_set(uint8_t relay_nr, bool state);
extern boolean FS_dht_read(uint8_t conf_nr = 0);
extern void	FS_dht_write(uint8_t conf_nr);

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


extern void mqtt_getAndSendTemperatureAndHumidityData();
extern void mmqt_send_attributes();
//extern void mmqt_send_dht();
extern void mmqt_send_dht_humid();
extern void mmqt_send_dht_temp();



uint16_t dht_get_sensor_timeout(uint8_t sensor_nr = 0)
{
	uint16_t value = dht_sensor[sensor_nr].update_sec;
	//debugMe(value);
	return value;


}

float dht_get_temp_stage0_Max()
{
	if isnan(dht_temp_stage0.getMax())
		return 255;
	else return dht_temp_stage0.getMax();
}

float dht_get_temp_stage0_Min()
{
	if isnan(dht_temp_stage0.getMin())
		return 255;
	else
	return dht_temp_stage0.getMin();
}

float dht_get_temp_stage0_Avg()
{
	double value = dht_temp_stage0.getAverage();

	if isnan(value)
		return 255;
	else
	return value;
}

float dht_get_humid_stage0_Max()
{
	if isnan(dht_humidity_stage0.getMax())
		return 255;
	else
	return dht_humidity_stage0.getMax();
}

float dht_get_humid_stage0_Min()
{
	if isnan(dht_humidity_stage0.getMin())
		return 255;
	else
	return dht_humidity_stage0.getMin();
}

float dht_get_humid_stage0_Avg()
{
	double value = dht_humidity_stage0.getAverage();
	if isnan(value)
		return 255;
	else return value;
}



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
	/*debugMe("humid ", false);
	debugMe(myDHT[0]->readHumidity(), true);
	debugMe("temp ", false);
	debugMe(myDHT[0]->readTemperature(), true);
	*/

	dht_sensor[sensorNR].temp  = myDHT[0]->readTemperature()  ;
	dht_sensor[sensorNR].humidity = myDHT[0]->readHumidity();
	

	if (isnan(dht_sensor[sensorNR].humidity) || isnan(dht_sensor[sensorNR].temp))	// check if sensor reading is ok if not do ...	
	{
		dht_sensor[sensorNR].readOk = false;
		dht_sensor[sensorNR].errors++;
		dht_sensor[sensorNR].totalErrors++;
		debugMe("Failed to read from DHT sensor! Total errors: ", false);
		debugMe(dht_sensor[sensorNR].totalErrors, false);
		debugMe(" | error: ", false);
		debugMe(dht_sensor[sensorNR].errors);

		dht_sensor[sensorNR].temp = 255;
		dht_sensor[sensorNR].humidity = 255;

		
	}
	else				// sensor reading ok
	{
		dht_sensor[sensorNR].errors =0;
		dht_sensor[sensorNR].readOk = true;

		//mqtt_getAndSendTemperatureAndHumidityData();
		//mmqt_send_dht();
		mmqt_send_dht_humid();
		mmqt_send_dht_temp();
		mmqt_send_attributes();

		dht_temp_stage0.addValue(dht_sensor[sensorNR].temp);
		dht_humidity_stage0.addValue(dht_sensor[sensorNR].humidity);


		debugMe("| temp :", false); 
		debugMe(dht_sensor[sensorNR].temp, false);
		debugMe(" | avg_temp :", false);
		debugMe(float(dht_temp_stage0.getAverage()), false);
		debugMe(" | humidity :", false);
		debugMe(dht_sensor[sensorNR].humidity, false);
		debugMe(" | avg_hum :", false);
		debugMe(float(dht_humidity_stage0.getAverage()), false);

		debugMe(" | TE:", false);
		debugMe(dht_sensor[sensorNR].totalErrors, true);

	}

}



void dht_process_values(uint8_t sensorNR = 0)
{
	if (dht_sensor[sensorNR].readOk == true)
	{
		if (relay[dht_sensor[sensorNR].relay_no].state == true)  // relay is on 
		{
			// check the temp and humidity and switch off if below temp or humidity
			if ((dht_temp_stage0.getAverage() <= dht_sensor[sensorNR].mintemp - dht_sensor[sensorNR].tempDiff) || dht_humidity_stage0.getAverage() < (dht_sensor[sensorNR].minHumid - dht_sensor[sensorNR].HumiDiff))
			{
				//debugMe("relay off");
				//debugMe(relay[dht_sensor[sensorNR].relay_no].state);
				relay_set(dht_sensor[sensorNR].relay_no, false);
				//debugMe(relay[dht_sensor[sensorNR].relay_no].state);
			}
		}
		else if (relay[dht_sensor[sensorNR].relay_no].state == false)  // relay is off 
		{	
			// if the relay is off check temp and humidity and power on ir required
			if ((dht_temp_stage0.getAverage() >= dht_sensor[sensorNR].mintemp) && dht_humidity_stage0.getAverage() >= dht_sensor[sensorNR].minHumid)
			{
				debugMe("relay on");
				debugMe(dht_sensor[sensorNR].temp, false);
				debugMe(" > ",false);
				debugMe(dht_sensor[sensorNR].mintemp, false);
				debugMe(" | ", false);
				debugMe(dht_sensor[sensorNR].humidity, false);
				debugMe(" > ", false);
				debugMe(dht_sensor[sensorNR].minHumid);

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
	//dht.begin();
	if (FS_dht_read(0) == false)
	{
		dht_sensor[0].pin = DHT_0_PIN;
		dht_sensor[0].type = DHT_0_TYPE;
		dht_sensor[0].mintemp = DHT_DEF_MINTEMP;
		dht_sensor[0].tempDiff = DHT_DEF_TEMP_DIFF;
		dht_sensor[0].minHumid = DHT_DEF_MIN_HUMID;
		dht_sensor[0].HumiDiff = DHT_DEF_HUM_DIFF;
		dht_sensor[0].update_sec = DHT_DEF_UPDATE_SEC;
		dht_sensor[0].relay_no = DHT_RELAY_NO;
		dht_sensor[0].totalErrors = 0;
		dht_sensor[0].errors = 0;
		dht_sensor[0].temp = 255;
		dht_sensor[0].humidity = 255;
		FS_dht_write(0);
	}

	myDHT[0] = new DHT(dht_sensor[0].pin, dht_sensor[0].type);


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



