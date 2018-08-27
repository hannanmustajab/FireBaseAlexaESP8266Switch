#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include "wifimanagment.h" 
#include<FirebaseArduino.h>
#define FIREBASE_HOST "switchboard-73b51.firebaseio.com"                     //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "5qFI1wTllYaWRnaob3ym2yPV565v7wUa1e3SCFwQ"       //Your Firebase Database Secret goes here
UpnpBroadcastResponder upnpBroadcastResponder;
//#######################################
//#######################################
Switch *lineOne = NULL;
Switch *lineTwo = NULL;
//#######################################
//on/off callbacks 
void lineOneOn();
void lineOneOff();

void lineTwoOn();
void lineTwoOff();
//#######################################
//#######################################

#define Relay1 4 //D6
int val1;

#define Relay2 5 //D2
int val2;



void setup()
{
   EEPROM.begin(memalloc);
   Serial.begin(115200);
   pinMode(Relay1, OUTPUT);
   pinMode(Relay2, OUTPUT);

  // Initialise wifi connection
  //wifiConnected = connectWifi();

  digitalWrite(Relay1,LOW);
  digitalWrite(Relay2,LOW);

  
  if(connectWifi()){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    lineOne = new Switch("line one", 80, lineOneOn, lineOneOff);
    lineTwo = new Switch("line two", 81, lineTwoOn, lineTwoOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*lineOne);
    upnpBroadcastResponder.addDevice(*lineTwo);

  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.setInt("S1",1);                     //Here the varialbe"S1","S2","S3" and "S4" needs to be the one which is used in our Firebase and MIT App Inventor
  Firebase.setInt("S2",1); 
  }else{
    startAP();
  }
}

void firebasereconnect()
{
  Serial.println("Trying to reconnect");
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  }
 
void loop()
{
   if(WiFi.status() == WL_CONNECTED){
      upnpBroadcastResponder.serverLoop();
      
      lineOne->serverLoop();
      lineTwo->serverLoop();

         if (Firebase.failed())
      {
      Serial.print("setting number failed:");
      Serial.println(Firebase.error());
      firebasereconnect();
      return;
      }
      
  val1=Firebase.getInt("S1");                                        //Reading the value of the varialble Status from the firebase
  
  if(val1==1)                                                             // If, the Status is 1, turn on the Relay1
     {
      digitalWrite(Relay1,LOW);
      Serial.println("light 1 ON");
       
     }
    
    else if(val1==0)                                                      // If, the Status is 0, turn Off the Relay1
    {                                      
      digitalWrite(Relay1,HIGH);
      Serial.println("light 1 OFF");
     
    }

  val2=Firebase.getInt("S2");                                        //Reading the value of the varialble Status from the firebase
  
  if(val2==1)                                                             // If, the Status is 1, turn on the Relay2
     {
      digitalWrite(Relay2,LOW);
      Serial.println("light 2 ON");
    }
    else if(val2==0)                                                      // If, the Status is 0, turn Off the Relay2
    {                                      
      digitalWrite(Relay2,HIGH);
      Serial.println("light 2 OFF");
    }



   }else{
      startAP();
   }
}

void lineOneOn() {
    Serial.print("Line 1 turned on ...");
    Firebase.setInt("S1",1); 
    
}

void lineOneOff() {
    Serial.print("Line 1 turned off ...");
    Firebase.setInt("S1",0); 
}

void lineTwoOn() {
    Serial.print("Line 2 turned on ...");
    Firebase.setInt("S2",1); 
}

void lineTwoOff() {
  Serial.print("Line 2 turned off ...");
  Firebase.setInt("S2",0); 
}

