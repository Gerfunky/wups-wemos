

//#include <Arduino.h>

/*----------------------------------------------------------
    MH-Z19 CO2 sensor  SAMPLE
  ----------------------------------------------------------*/

#include <MHZ19_uart.h>

extern void debugMe(String input, boolean line = true);				// debug funtions from wifi-ota 

const int rx_pin = D3;	//Serial rx pin no
const int tx_pin = D4;	//Serial tx pin no

MHZ19_uart mhz19;


int mhz19_co2ppm = 0 ;
int mhz19_temp = 0;

/*----------------------------------------------------------
    MH-Z19 CO2 sensor  setup
  ----------------------------------------------------------*/
void mhz19_setup() {
  //Serial.begin(9600);
  mhz19.begin(rx_pin, tx_pin);
  mhz19.setAutoCalibration(false);
  while( mhz19.isWarming() ) {
    Serial.print("MH-Z19 now warming up...  status:");Serial.println(mhz19.getStatus());
    //debugMe("MH-Z19 now warming up...  status:", false);
    //debugMe(mhz19.getStatus() );
    //delay(1000);
  }
}

/*----------------------------------------------------------
    MH-Z19 CO2 sensor  loop
  ----------------------------------------------------------*/
void mhz19_loop() {
   mhz19_co2ppm = mhz19.getPPM();
   mhz19_temp = mhz19.getTemperature();

  Serial.print("co2: "); Serial.println(mhz19_co2ppm);
  Serial.print("temp: "); Serial.println(mhz19_temp);
  //debugMe("co2: ",false); debugMe(co2ppm);
  //debugMe("temp: ",false); debugMe(temp);

  
  //delay(5000);
}

int getMHZ19ppm()
{
    return mhz19_co2ppm;
}

int getMHZ19temp()
{
    return mhz19_temp;
}
