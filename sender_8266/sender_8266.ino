
#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xA4, 0xCF, 0x12, 0xC5, 0xFC, 0x7D};

#define SENSOR_ID 1

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


MsgTempHum myData;

unsigned long lastTime = 0;  
unsigned long startTime; 

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("Last Packet Send Status: ");
    if (sendStatus == 0){
      Serial.println("Delivery success");
    }
    else{
      Serial.println("Delivery fail");
    }

    lastTime = millis();

    unsigned long elapsedTime = lastTime - startTime;
    Serial.println("Elapsed time: ");
    Serial.println(elapsedTime);
    ESP.deepSleep(30e6); 
}


void setup() {
  startTime = millis();
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  myData.msgHeader.sensorId = SENSOR_ID;
  myData.msgHeader.msgType = 2;
  myData.temp =  getTemperature();
  myData.humidity = getHumidity();
  
  // Send message via ESP-NOW
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

}

float getTemperature()
{
  return random(15,40); 
}

float getHumidity()
{
  return random(30,80);
}

 
void loop() 
{

}
