//*************************************************************************************
//*
//*************************************************************************************
//*************************************************************************************
//*
//*************************************************************************************
#include "main.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <string.h>

#include "secrets.h"
#include "WaterTank.h"


//*************************************************************************************
//* Constant declarations
//*************************************************************************************

const char* wifi_ssid = WIFI_SSID;
const char* wifi_password = WIFI_PASSWORD;

const char* mqtt_server = MQTT_SERVER;
const char* mqtt_username = MQTT_USER;
const char* mqtt_password = MQTT_PASSWORD;
/*
const char* mqtt_subscribe_topic = "ssa174/tank/desal/%";
const char* mqtt_data_topic = "ssa174/tank/desal/volume";
const char* mqtt_alarm_topic = "ssa174/tank/desal/alarm";
const char* mqtt_status_topic = "ssa174/tank/desal/status";
*/
//*************************************************************************************
//*
//*************************************************************************************

//*************************************************************************************
//* Global variable declarations - MQTT/WIFI
//*************************************************************************************

WiFiClient wifiEspClient;
  
PubSubClient client(wifiEspClient);

char mqttPayloadMsgBuffer[MQTT_PAYLOAD_MSG_BUFFER_SIZE];

//*************************************************************************************
//* Global variable declarations - WaterTank
//*************************************************************************************

WaterTank desalinatedWaterTank(PIN_SONAR_TRIG, 
                               PIN_SONAR_ECHO, 
                               DESALINATED_WATER_TANK_HEIGHT, 
                               DESALINATED_WATER_TANK_AREA,
                               DESALINATED_WATER_TANK_ALARM_LOW_VOLUME,
                               watertankAlarmlowLevelCallback);

//*************************************************************************************
//*
//*************************************************************************************

//*************************************************************************************
//*
//*************************************************************************************

  void mqttPublish(char* topicPtr, char* payloadPtr) {
    if (!client.connected()) {
      mqttReconnect();
    }
    client.loop();
    
    client.publish(topicPtr,payloadPtr);  
  }
  

//*************************************************************************************
//*
//*************************************************************************************

  void mqttCallback(char* topic, byte* payload, unsigned int length) {
  //  Serial.print("Message arrived [");
  //  Serial.print(topic);
  //  Serial.print("] ");
  
  //  for (int i = 0; i < length; i++) {
  //    Serial.print((char)payload[i]);
  //  }
  //  Serial.println();
/*
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
      // but actually the LED is on; this is because
      // it is active low on the ESP-01)
    } else {
      digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
*/
  }

//*************************************************************************************
//*
//*************************************************************************************

  void mqttReconnect() {
    mqttMsg msg;
    
    // Loop until we're reconnected
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Create a random client ID
      String clientId = "WaterTank-";
      clientId += String(random(0xffff), HEX);
      // Attempt to connect
      if (client.connect(clientId.c_str()), mqtt_username, mqtt_password) {
        Serial.println("connected");
        // Once connected, publish an announcement...

        snprintf(msg.topic,32,"%s/%s/%s",MQTT_TOPIC_LOC,MQTT_TOPIC_AREA, MQTT_TOPIC_STATUS);
        snprintf(msg.payload,31,"up");

        mqttPublish(msg.topic,msg.payload);

        // ... and resubscribe
        
        client.subscribe("inTopic");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }

//*************************************************************************************
//*
//*************************************************************************************

void mqttWifiSetup() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

//*************************************************************************************
//*
//*************************************************************************************

void mqttSetup() {
  mqttWifiSetup();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(mqttCallback);
  client.subscribe(mqtt_subscribe_topic);
}

//*************************************************************************************
//*
//*************************************************************************************

void watertankAlarmlowLevelCallback(boolean activeAlarm, float tankVolume) {
  mqttMsg msg;

  Serial.print("Watertank low level alarm ");
  if (activeAlarm) {
    Serial.print("active");
  } else {
    Serial.print("ceased");    
  }
  Serial.print(" Volume=");
  Serial.print(tankVolume);
  Serial.println(" Liter");

  snprintf(msg.topic,32,"%s/%s/%s/%s",MQTT_TOPIC_LOC,MQTT_TOPIC_AREA, MQTT_TOPIC_SENSOR_1, MQTT_TOPIC_SENSOR_1_ALARM);
  snprintf(msg.payload,31,"{type:\"LowLevel\",status:\"%s\",volume:%.1f}",(activeAlarm? "Active":"Ceased"),tankVolume);

  mqttPublish(msg.topic,msg.payload);
}
//*************************************************************************************
//*
//*************************************************************************************

void getVolume() {
  struct {
    char payload[24];
    char topic[64];
  } mqttMsg;

  float tankVolume;

  if ((tankVolume = desalinatedWaterTank.getTankVolume()) >= 0) {
    snprintf(mqttMsg.payload,24,"%.0f",tankVolume);
    snprintf(mqttMsg.topic,64,"%s/%s/%s/%s",MQTT_TOPIC_LOC,MQTT_TOPIC_AREA, MQTT_TOPIC_SENSOR_1, MQTT_TOPIC_SENSOR_1_DATA);

    mqttPublish(mqttMsg.topic,mqttMsg.payload);
  } else {
    // There are a fault in the measurement
    snprintf(mqttMsg.payload,24,"{volume:%f;status:\"suspect\"}",tankVolume);
    snprintf(mqttMsg.topic,64,"%s/%s/%s/%s",MQTT_TOPIC_LOC,MQTT_TOPIC_AREA, MQTT_TOPIC_SENSOR_1, MQTT_TOPIC_STATUS);

    mqttPublish(mqttMsg.topic,mqttMsg.payload);
  } 

  Serial.print("Water volume = ");
  Serial.println(mqttMsg.payload);
}


//*************************************************************************************
//*
//*************************************************************************************
void waterTankSetup() {
  char mqttPayload[24];
  char mqttTopic[64];

  snprintf(mqttPayload,24,"{status:\"started\"}");
  snprintf(mqttTopic,64,"%s/%s/%s/%s",MQTT_TOPIC_LOC,MQTT_TOPIC_AREA, MQTT_TOPIC_SENSOR_1, MQTT_TOPIC_STATUS);
  
  mqttPublish(mqttTopic,mqttPayload);

  
}

//*************************************************************************************
//*
//*************************************************************************************

unsigned long timeForLastMsg = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  Serial.println("MQTT water tank server starting");
  mqttSetup();
  waterTankSetup();
}

void loop() {
  unsigned long now = millis();
  if (now - timeForLastMsg > 15000) {
    timeForLastMsg = now;

    getVolume();
    

  }
}
