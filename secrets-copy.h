//

#ifndef __secrets_h__
#define __secrets_h__

#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"

#define MQTT_SERVER "broker"
#define MQTT_PORT 1883
#define MQTT_USER "user"
#define MQTT_PASSWORD "password"

#define MQTT_TOPIC_LOC "home"
#define MQTT_TOPIC_AREA "freshwater"
#define MQTT_TOPIC_SENSOR_1 "tank"
#define MQTT_TOPIC_SENSOR_1_DATA "volume"
#define MQTT_TOPIC_SENSOR_1_ALARM "alarm"
#define MQTT_TOPIC_STATUS "status"

// Used physical pins

#define PIN_SONAR_TRIG D0
#define PIN_SONAR_ECHO D1

// Water tank specific information

#define DESALINATED_WATER_TANK_AREA             28.3  // Square dm
#define DESALINATED_WATER_TANK_HEIGHT           11.0  // Height of the water tank in dm
#define DESALINATED_WATER_TANK_ALARM_LOW_VOLUME 50    // Volume in Liter left in the tank when the alarm is asserted

#endif
