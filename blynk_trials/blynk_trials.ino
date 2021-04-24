#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WifiUDP.h>
#include <String.h>
#include <BlynkSimpleEsp8266.h>
#include "icons.h"
#include "MAX30105.h"  //Get it here: http://librarymanager/All#SparkFun_MAX30105
#include "heartRate.h"
#define BLYNK_PRINT Serial
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
MAX30105 particleSensor;

char auth[] = "rP3GaX8cfZg8S-328aNV6dWvJo82gvnx";

const char* ssid = "wifi hotspot";
const char* password = "1234asdf";

//const char* ssid = "Zeptogreens";
//const char* password = "zeptogreens20";

int bootup = 0;
int value = 0;
double thermistor=0;
double bpm_blynk=0;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;


float bpm_val; 
int abpm_val; 
long ir_val;
int contact_val;

double adc,k,c,f;

BlynkTimer timer;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
 
void setup() 
{
  bootup=1;
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  } 
    if (particleSensor.begin(Wire, I2C_SPEED_FAST) == false) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  
  display.clearDisplay();
  logo();
  boot();  
  Blynk.begin(auth, ssid, password,  IPAddress(188,166,206,43), 8080);
  timer.setInterval(500L, myTimerEvent);
  wifistatus();
  }
 
  
void loop()
{



bpm(bpm_val,abpm_val,ir_val,contact_val);
temp(adc,k,c,f);
thermistor=f;
bpm_blynk=bpm_val;

display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE,BLACK);
display.setCursor(4,0);
display.println(" ORINCI LABS PVT LTD");
display.drawBitmap(0, 10, image_data_thermometer, 16,41 , 1);

display.setTextColor(WHITE, BLACK); 
display.setTextSize(1);
display.setCursor(18,20);
display.print(f);
display.println(" F");
display.setCursor(18,29);
display.print(c);
display.println(" C");

display.setCursor(18,29+8);
display.print(bpm_val);
display.println(" BPM");

display.setCursor(18,29+8+8);
display.print(abpm_val);
display.println(" Avg BPM");

display.setCursor(18,29+8+8+8);
  if (contact_val == 0)
    display.println("No body contact!");


//display.setTextSize(1);
//display.setCursor(0,57);
//display.print(WiFi.localIP());
display.display();
 
Blynk.run();
timer.run();
}



void logo()
{
  display.clearDisplay();
  display.drawBitmap(20, 0, image_data_orinci, 89, 64, 1);
  display.setTextSize(1);
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0,55);
  display.println(" ORINCI LABS PVT LTD");
  display.display();
  delay(5000);  
  display.clearDisplay();
}

void boot()
{
  {
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(500);
  display.clearDisplay();
  display.display();
  delay(500);
  display.clearDisplay();
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(500);
  display.clearDisplay();
  display.display();
  delay(500);
  }
}

void temp(double &adc_value,double &k,double &c,double &f)
{
  double Vout, Rth; 
  const double VCC = 3.3;             // NodeMCU on board 3.3v vcc
  const double R2 = 10000;            // 10k ohm series resistor
  const double adc_resolution = 1024; // 10-bit adc
  const double A = 0.001129148;       // thermistor equation parameters
  const double B = 0.000234125;
  const double C = 0.0000000876741; 

  adc_value = analogRead(A0);
  //Vout = analogRead(A0) * (VCC / adc_resolution); 
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;

/*  Steinhart-Hart Thermistor Equation:
 *  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
 *  where A = 0.001129148, B = 0.000234125 and C = 8.76741*10^-8  */
  
  k = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));   // Temperature in kelvin
  c = k - 273.15;  // Temperature in degree celsius
  f = (c * 9/5) +32; //  Temperature in degree celsius fahrenheit | (0°C × 9/5) + 32 = 32°F
  
 }  


void wifistatus()
{
  Serial.println("");
  display.setTextSize(1);
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0,0);
  display.println("Connected to WiFi.");
  Serial.print(WiFi.localIP());
  Serial.println(""); 
  display.println("Welcome");
  
  display.display();
  delay(1000);
  display.clearDisplay();
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


void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
 Blynk.virtualWrite(V0, thermistor);
 Blynk.virtualWrite(V1, bpm_blynk);
}
