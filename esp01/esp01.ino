void setup() {
  pinMode(2, OUTPUT);          //Led connected to pin 2
}
void loop() {
digitalWrite(2,HIGH); //Led switched on
delay(500);
digitalWrite(2,LOW);  //Led switched off
delay(500);
}
