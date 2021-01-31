/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <espnow.h>
#include <ESP8266WiFi.h>

#include <SoftwareSerial.h>



typedef struct MsgHeader {
  int sensorId;
  int msgType;
  
} MsgHeader;


typedef struct MsgTemp {
  MsgHeader msgHeader;
  float temp;
  
} MsgTemp;

typedef struct MsgTempHum {
  MsgHeader msgHeader;
  float temp;
  float humidity;
  
} MsgTempHum;



int cnt;


// Create a struct_message called myData
MsgHeader msgHeader;
MsgTemp msgTemp;
SoftwareSerial swSerial(D6, D7); // RX, TX



// callback function that will be executed when data is received
void OnDataRecv(unsigned char * mac, unsigned char *incomingData, unsigned char len) {
   Serial.println("Received message from ESPNOW");

  memcpy(&msgHeader, incomingData, sizeof(msgHeader));

  Serial.println("Received message from ESPNOW");
  Serial.println(len);
  Serial.println(msgHeader.sensorId);
  Serial.println(msgHeader.msgType);

  swSerial.write(len);
  swSerial.write(incomingData, len);
}




void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  swSerial.begin(9600);
  Serial.println(WiFi.macAddress());

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  cnt = 0;
}

void loop() 
{
  cnt++;

   Serial.println("still here");

//  MsgTemp myTemp;
//  myTemp.msgHeader.sensorId = 12;
//  myTemp.msgHeader.msgType = 1;
//  myTemp.temp = (cnt * 2 + 10) % 30 ;
//
//  int len = sizeof(myTemp);
//  unsigned char * buf = (unsigned char *) &myTemp;
//
//  swSerial.write(len);
//  swSerial.write(buf, len);
//
//  delay(500);
//
//  MsgTempHum myTempHum;
//  myTempHum.msgHeader.sensorId = 13;
//  myTempHum.msgHeader.msgType = 2;
//  myTempHum.temp = (cnt * 2 + 10) % 30 ;
//  myTempHum.humidity = (cnt + 10) % 50 ;
//
//  len = sizeof(myTempHum);
//  buf = (unsigned char *) &myTempHum;
//
//  swSerial.write(len);
//  swSerial.write(buf, len);
//
//  
    delay(1000);
}
