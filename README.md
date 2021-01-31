This repository contains a collection of arduino sketches that allow using espnow in a networked enviornment. The project cosists of 3 main parts:


- sender_8266

The sender is a low powered device that performs an action on a timer (or a user request) and sends the results to the receiver

- receiver receives messages from senders and packages them as serial messages, which are sent to esp_server

- esp_server - a device connected to the wifi network, which also connects to an mqtt server. This device is connected to the espnow receiver over serial interface (currently software serial as there are issues connected over standard tx/rx on wemos boards while connected to USB), and when a message is received, it forwards it formats it as an MQTT message and sends it to the MQTT server. 


Each message sent over the ESPNOW system has a fixed header, which indicates how the message is subsequently processed by the receiver. This allows new message and sensor types to be added to the network without modifying software on existing devices (other than the esp_server). 

The structure below shows the header of each message:


    typedef struct MsgHeader {
      int sensorId;
      int msgType;

    } MsgHeader;
    
    
