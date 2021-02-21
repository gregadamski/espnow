#include <espnow.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

typedef struct MsgHeader {
  int sensorId;
  int msgType;
  
} MsgHeader;


// Create a struct_message called myData
MsgHeader msgHeader;

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
}

void loop() 
{
}
