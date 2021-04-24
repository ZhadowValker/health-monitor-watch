/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/


void setup() {
  Serial.begin(9600);  /* Define baud rate for serial communication */
}

void loop() {
 char* str; 
 double adc,temp_k,temp_c,temp_f;
 temp(adc,temp_k,temp_c,temp_f);
 Serial.print(adc);
 Serial.println(" ADC");
 Serial.print(temp_k);
 Serial.println(" °K");
 Serial.print(temp_c);
 Serial.println(" °C");
 Serial.print(temp_f);
 Serial.println(" °F");

 sprintf(str, "%f", temp_f);

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

  delay(1000);
 }
