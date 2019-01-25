/*
  Arduino program that runs on an Arduino UNO
  Takes input from a PLC (Siemens Hydroranger)
  PLC sets pin high when water level is low.
  We then transmit that the cistern needs water to the well
  via HC12 radio chips.
  Also sends heartbeat signal to well arduino.
*/

#include <SPI.h>
#include <printf.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
int hydroRangerPin = 2;
byte incomingByte;
String readBuffer = "";
bool pumpAck = false;
bool stopAck = false;
int heartBeatCounter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(hydroRangerPin, INPUT_PULLUP); // Hydro ranger contact closure 
  HC12.begin(9600);               // Serial port to HC12
}

void loop() {
  // first see if we have any acknowledgements
  while (HC12.available()) {             // If HC-12 has data
    incomingByte = HC12.read();          // Store each icoming byte from HC-12
    readBuffer += char(incomingByte);    // Add each byte to ReadBuffer string variable
  }
  if (readBuffer.length() > 0) {
    Serial.print("readBuffer=");
    Serial.println(readBuffer);
    if (readBuffer.equals("P-OK")) {
      pumpAck = true;
      stopAck = false;
    } else if (readBuffer.equals("S-OK")) {
      stopAck = true;
      pumpAck = false;
    }
    readBuffer = "";
  }
  delay(100);  // give the cpu a breath...
  int sensorValue = digitalRead(hydroRangerPin);
  if (sensorValue == LOW) {
    // see if we have an ack yet, if not keep sending pump signal
    //if (!pumpAck) {
      HC12.print("P");
      Serial.println("pump");
    //}
  } else {
    if (!stopAck) {
      HC12.print("S");
      Serial.println("stop");      
    }
  }
  delay(1000);
  // every XX seconds send a heartbeat
  if (heartBeatCounter >= 10) {
    HC12.print("H");
    heartBeatCounter = 0;
  } else {
    heartBeatCounter++;
  }
}


