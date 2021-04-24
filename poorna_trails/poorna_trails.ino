float sensor=A0;
float sen;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial.print("sensor reading:");
pinMode(sensor,INPUT);
//digitalWrite(sensor,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
sen=analogRead(sensor);
Serial.println(sen);
delay(1000);
}
