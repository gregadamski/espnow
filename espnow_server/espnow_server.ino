#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "pwd"
#endif


#define SERVER_ID "EspNowServer1"

const char* ssid = STASSID;
const char* password = STAPSK;

const char* mqttServer = "mqtt_host";
const int mqttPort = 1883;
const char* mqttUser = "mqtt_user";
const char* mqttPassword = "mqtt_pwd";

ESP8266WebServer server(80);
SoftwareSerial swSerial(D6, D7); // RX, TX


WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);



// used to hold the message from the serial interface; per espnow the message cannot be larger than
// 200 bytes, so 255 should be fine
byte buf[255]; 
int bufferLen;


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


MsgHeader msgHeader;


void handleRoot() {
  String resp("hello from esp8266!: ");
  resp = resp + bufferLen + ": test";
  server.send(200, "text/plain", resp);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {

  Serial.begin(115200);
  swSerial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  // connecting to mqtt
  mqttClient.setServer(mqttServer, mqttPort);

  while (!mqttClient.connected()) {
      Serial.println("Connecting to MQTT...");
   
      if (mqttClient.connect("ESP8266Client", mqttUser, mqttPassword )) {
   
        Serial.println("connected");  
   
      } else {
   
        Serial.print("failed with state ");
        Serial.print(mqttClient.state());
        delay(2000);
   
      }
  }


}

void loop(void) {


  if (swSerial.available()) 
  {
    bufferLen = swSerial.read();
    Serial.println("Len: ");
    Serial.println(bufferLen);
    swSerial.readBytes(buf, bufferLen);

    memcpy(&msgHeader, buf, sizeof(msgHeader));

  Serial.println(msgHeader.msgType);
  Serial.println(msgHeader.sensorId);
  
    switch (msgHeader.msgType)
    {
      case 1:
        handleTempMsg(buf);
        break;
      case 2:
        handleTempHumMsg(buf);
        break;
      default:
        Serial.println("Unknown message type received. Skipping");
        break;
    }
  }
  
  server.handleClient();
  MDNS.update();
}

void handleTempMsg(byte * buf)
{
  MsgTemp msgTemp;
  memcpy(&msgTemp, buf, sizeof(msgTemp));
  publishMqttMsg("temp", msgTemp.msgHeader.sensorId,  String(msgTemp.temp));
}


void handleTempHumMsg(byte * buf)
{
  MsgTempHum msgTempHum;
  memcpy(&msgTempHum, buf, sizeof(msgTempHum));
  publishMqttMsg("temp", msgTempHum.msgHeader.sensorId, String(msgTempHum.temp));
  publishMqttMsg("humidity", msgTempHum.msgHeader.sensorId, String(msgTempHum.humidity));
}

void publishMqttMsg(String topic, int sensorId, String value)
{
  String msg = SERVER_ID;
  msg += String("/") + String(sensorId);
  msg += String("/") + String(topic);
  Serial.println(msg);
  Serial.println(value);
  
  mqttClient.publish(msg.c_str(), value.c_str());
}
