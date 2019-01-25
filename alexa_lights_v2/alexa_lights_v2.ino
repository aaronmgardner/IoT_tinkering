/*
  A fun project with my son. This is a simple program that
  runs on a NodeMCU ESP8266 board and communicates with
  an amazon echo dot over the wifi network.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

// Network credentials 
#define WIFI_SSID "waaaaahoooooo"
#define WIFI_PASS "deadbeef76"    

#define SERIAL_BAUDRATE 115200

// WeMo emulation library 
fauxmoESP fauxmo;

// Set Relay Pins 
#define RELAY_1 D5
#define RELAY_2 D6

void setup() 
{
   Serial.begin(SERIAL_BAUDRATE);
   //setup and wifi connection
   wifiSetup();
 
   //Set relay pins to outputs
   pinMode(RELAY_1, OUTPUT);
   pinMode(RELAY_2, OUTPUT);
    
   // Device Names for Simulated Wemo switches
   fauxmo.addDevice("Light One");
   fauxmo.addDevice("Light Two");
   fauxmo.onMessage(callback); 
}

void loop() 
{
  fauxmo.handle();
}

// Main callback function for wemo event handler
void callback(uint8_t device_id, const char * device_name, bool state) 
{
  Serial.print("Device "); Serial.print(device_name); 
  Serial.print(" state: ");
  if (state) 
  {
    Serial.println("ON");
  } 
  else 
  {
    Serial.println("OFF");
  }
  
  //Switching action on detection of device name
  
  if ( (strcmp(device_name, "Light One") == 0) ) 
  {
    if (!state) 
    {
      digitalWrite(RELAY_1, LOW);
    } 
    else 
    {
      digitalWrite(RELAY_1, HIGH);
    }
  }

  if ( (strcmp(device_name, "Light Two") == 0) ) 
  {
    if (!state) 
    {
      digitalWrite(RELAY_2, LOW);
    } 
    else 
    {
      digitalWrite(RELAY_2, HIGH);
    }
  }
  
  if ( (strcmp(device_name, "All Lights") == 0) ) 
  {
    if (!state) 
    {
      digitalWrite(RELAY_1, LOW);
      digitalWrite(RELAY_2, LOW);
    } 
    else 
    {
      digitalWrite(RELAY_1, HIGH);
      digitalWrite(RELAY_2, HIGH);
    }
  }
}
    
// Wifi setup 
void wifiSetup() 
{
   // Set WIFI module to STA mode
   WiFi.mode(WIFI_STA);

   // Connect
   Serial.println ();
   Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
   Serial.println();
   WiFi.begin(WIFI_SSID, WIFI_PASS);

   // Wait
   while (WiFi.status() != WL_CONNECTED) 
   {
      Serial.print(".");
      delay(100);
   }
   Serial.print(" ==> CONNECTED!" );
   Serial.println();

   // Connected!
   Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
   Serial.println();
}
