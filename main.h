
// 

#ifndef __MAIN_H__
#define __MAIN_H__

#define DEBUG true

#define ONE_WIRE_BUS D2
#define MAX_NUMBER_OF_ONEWIRE_DEVICES 16

#define MQTT_PAYLOAD_MSG_BUFFER_SIZE (50)

struct mqttMsg {
  char topic[32];
  char payload[32];
};

void watertankAlarmlowLevelCallback(boolean, float);


#endif
