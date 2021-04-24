

#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0x0A); //Turn off Green LED
 
}

void loop()
{
float bpm_val,tempc_val,tempf_val;
int abpm_val, contact_val, wait;
long ir_val;  
bpm(bpm_val,abpm_val,ir_val,contact_val);
  Serial.print(" IR =");
  Serial.print(ir_val);
  Serial.print(" BPM =");
  Serial.print(bpm_val);
  Serial.print(" AVG BPM =");
  Serial.print(abpm_val);
  Serial.print(" Body contact =");
  Serial.print(contact_val);
  Serial.println(" ");

//    {
//    particleSensor.enableDIETEMPRDY();
//    temp(tempc_val,tempf_val);
//    Serial.print(" Body Temp C=");
//    Serial.print(tempc_val);
//    Serial.print(" Body Temp F=");
//    Serial.print(tempf_val);
//    Serial.println(" ");
//    particleSensor.disableDIETEMPRDY();
//    }
    
  delay(1);
}

void bpm(float &bpm_, int &abpm_, long &ir_, int &contact_)
{
  long irValue = particleSensor.getIR();
   
  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  ir_=irValue;
  bpm_=beatsPerMinute;
  abpm_=beatAvg;
  
  if (irValue < 50000)
    contact_=0;
  else
    contact_=1;

  }


void temp(float &temp_c,float &temp_f)
{
  temp_c = particleSensor.readTemperature();
  temp_f = particleSensor.readTemperatureF();
  }
