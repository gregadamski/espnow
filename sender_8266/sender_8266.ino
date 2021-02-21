
#include <ESP8266WiFi.h>
#include <espnow.h>


#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xA4, 0xCF, 0x12, 0xC5, 0xFC, 0x7D};

#define SENSOR_POWER_PIN 13

#define SENSOR_ID 1

ADC_MODE(ADC_VCC);


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

typedef struct MsgTempHumPressure {
  MsgHeader msgHeader;
  float temp;
  float humidity;
  float pressure;
  
} MsgTempHumPressure;

typedef struct MsgTempHumPressureVoltage {
  MsgHeader msgHeader;
  float temp;
  float humidity;
  float pressure;
  float voltage;
  
} MsgTempHumPressureVoltage;


Adafruit_BME280 bme; // I2C


MsgTempHumPressureVoltage myData;

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
    ESP.deepSleep(120e6); 
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
    Serial.println("Start!!");


  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  pinMode(SENSOR_POWER_PIN, OUTPUT);                             // ustawiamy modyfikacje pinu 13 (zasilanie BME280)
  digitalWrite(SENSOR_POWER_PIN, HIGH);                          // ustawiamy modyfikacje pinu 13 (zasilanie BME280)

  bme.begin(0x76);   

  Serial.print(bme.readTemperature());

  myData.msgHeader.sensorId = SENSOR_ID;
  myData.msgHeader.msgType = 4;
  myData.temp =  getTemperature();
  myData.humidity = getHumidity();
  myData.pressure = getPressure();
  myData.voltage = ESP.getVcc() / 1000.0;
  // Send message via ESP-NOW
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  digitalWrite(SENSOR_POWER_PIN, LOW);                   

}

float getTemperature()
{
  return bme.readTemperature();
}

float getHumidity()
{
  return bme.readHumidity();
}

float getPressure()
{
  return bme.readPressure();
}

void loop() 
{

}
