#include <UIPEthernet.h>
#include "PubSubClient.h"

#define CLIENT_ID       "ESP32MQTT"
#define INTERVAL        3000 // 3 sec delay between publishing
#define GATE_ID "/gate1"

uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};

EthernetClient ethClient;
PubSubClient mqttClient;

long previousMillis;


void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("barrierclient")) {
      Serial.println("connected");
      // Subscribe
      mqttClient.subscribe("gate/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int len){
  String topic_s = topic;
  String subtopic = topic_s.substring(15);
  
  String payload_s = (char*)payload;
  payload_s = payload_s.substring(0,len);

  Serial.println("receive message");

  if(subtopic == GATE_ID){
    if(payload_s == "open"){
      Serial.println("gate open");
    }
   if(payload_s=="close"){
      Serial.println("gate close");
    }
  }
}

void setup() {
  // setup serial communication
  Serial.begin(115200);
  // setup ethernet communication using DHCP
  //Ethernet.begin(mac);
  if(Ethernet.begin(mac) == 0) {
    Serial.println(F("Ethernet configuration using DHCP failed"));
    for(;;);
  }
  Serial.println(Ethernet.localIP());
  // setup mqtt client
  mqttClient.setClient(ethClient);
  mqttClient.setServer("test.mosquitto.org",1883);
  mqttClient.setCallback(mqttCallback);
  //mqttClient.setServer("192.168.1.xxx",1883); //for using local broker
  //mqttClient.setServer("broker.hivemq.com",1883);
  //Serial.println(F("MQTT client configured"));

 previousMillis = millis();
}

void loop() {
    // check interval
  if(millis() - previousMillis > INTERVAL) {
    sendData();
    previousMillis = millis();
  }
  if(!mqttClient.loop()){
    reconnect();
  }
}

void sendData() {
  mqttClient.publish("test","hello");
  Serial.println("send data");
 //hal=hallway, DICHT=Closed, kamer=room, licht=light
}
