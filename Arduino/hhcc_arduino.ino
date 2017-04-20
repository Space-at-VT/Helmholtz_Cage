#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

String input;
double magData[3];
int counter;

void setup() {
    magData[0] = 0;
    magData[1] = 0;
    magData[2] = 0; 
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0){
    input = Serial.readStringUntil('\n');
    Serial.println("apple");
/*
    Serial.print(magData[0]/counter); Serial.print(",");
    Serial.print(magData[1]/counter); Serial.print(",");
    Serial.println(magData[2]/counter);

    counter = 0;
    magData[0] = 0;
    magData[1] = 0;
    magData[2] = 0;
    
    if (input.equals("000")){
      //Relay 1&2
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
      //Relay 3&4
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      //Relay 5&6
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);}
    else if (input.equals("001")){
      //Relay 1&2
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
      //Relay 3&4
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      //Relay 5&6
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);}
    else if (input.equals("010")){
      //Relay 1&2
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
      //Relay 3&4
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      //Relay 5&6
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);}
    else if (input.equals("011")){
      //Relay 1&2
      digitalWrite(6, HIGH);
      digitalWrite(7, LOW);
      //Relay 3&4
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      //Relay 5&6
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);}
    else if (input.equals("100")){
      //Relay 1&2
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      //Relay 3&4
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      //Relay 5&6
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);}
    else if (input.equals("101")){
      //Relay 1&2
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      //Relay 3&4
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      //Relay 5&6
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);}
    else if (input.equals("110")){
      //Relay 1&2
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      //Relay 3&4
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      //Relay 5&6
      digitalWrite(10, HIGH);
      digitalWrite(11, LOW);}
    else if (input.equals("111")){
      //Relay 1&2
      digitalWrite(6, LOW);
      digitalWrite(7, HIGH);
      //Relay 3&4
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      //Relay 5&6
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);}
    else{
      //Relay 1&2
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      //Relay 3&4
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      //Relay 5&6
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);}}
    else{
        sensors_event_t event; 
        mag.getEvent(&event);
        magData[0] = event.magnetic.x + magData[0];
        magData[1] = event.magnetic.y + magData[1];
        magData[2] = event.magnetic.z + magData[2];
        counter = counter + 1;*/
    }
}
