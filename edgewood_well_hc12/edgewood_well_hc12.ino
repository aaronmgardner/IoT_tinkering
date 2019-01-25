/*
  Receives signals to turn on/off pump from Cistern arduino 
  via HC12 radio chip. 
  Also receives heartbeat signals from cistern. If we have not
  heard from the cistern in <timeout> period, ensure pump is off.
  This is due to issues with tranmission interference and power outages 
  causing the pump to stick on and overflow the cistern. 
*/
#include <printf.h>
#include <SoftwareSerial.h>

int relayPin = 6;
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
byte incomingByte;
String readBuffer = "";
int heartbeatDelta = 0;
int timeoutPeriod = 3600;

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.begin(9600);
  HC12.begin(9600);               // Serial port to HC12
}
void loop() {
  if (HC12.available()) {
    while (HC12.available()) {             // If HC-12 has data
      incomingByte = HC12.read();          // Store each icoming byte from HC-12
      readBuffer += char(incomingByte);    // Add each byte to ReadBuffer string variable
    }
    Serial.println(readBuffer);
    heartbeatDelta = 0;
    // acknowledge receipt of message
    if (readBuffer.equals("P")) {
      HC12.print("P-OK"); // ok were pumping
      if (digitalRead(relayPin) == LOW) {
        digitalWrite(relayPin, HIGH);
      }
    } else if (readBuffer.equals("S")) {
      HC12.print("S-OK"); // ok were stopping
      digitalWrite(relayPin, LOW);
    } else if (readBuffer.equals("H")) {
      // heartbeat ack - reset counter
      Serial.println(readBuffer);
    } else {
      // garbled message???
      Serial.print("garbled message WTF - ");
      Serial.println(readBuffer);
    }
    readBuffer = "";
  } else {
    // if we havent heard from the cistern in a while and we are pumping shut it off
    heartbeatDelta++;
    if (heartbeatDelta >= timeoutPeriod) {
      if (digitalRead(relayPin) == HIGH) {
        Serial.println("Haven't heard from cistern and we are pumping, shut er down!");
        digitalWrite(relayPin, LOW);
      }
    }
  }
  delay(1000);
}
