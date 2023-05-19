/*************************************************************************************************
 *
 *************************************************************************************************/
 
#include <Arduino.h>
#include "WaterTank.h"

float WaterTank::measureDistance() {
	int airTime;
  float distance;
	
	delay(10);
	digitalWrite(pinTrig, LOW);
	delayMicroseconds(2);
	digitalWrite(pinTrig, HIGH);
	delayMicroseconds(10);
	digitalWrite(pinTrig, LOW);
	airTime = pulseIn(pinEcho, HIGH, WATERTANK_TIMEOUT)/2;

  distance = airTime/291.0;
  
  #ifdef WATERTANK_DEBUG
    Serial.print("Measured distance = ");
    Serial.print(distance);
    Serial.println(" dm (One way)");
  #endif
  if ((distance > 0) && (distance <= tankHeight))
  	return(distance);
  else
    return(tankHeight*1.0);
}

float WaterTank::getTankVolume() {
	float tankVolume = tankArea*(tankHeight-measureDistance());

  #ifdef WATERTANK_DEBUG
    Serial.print("Measured volume = ");
    Serial.print(tankVolume);
    Serial.print(" (");
    Serial.print(tankHeight);
    Serial.print(" , ");
    Serial.print(tankArea);
    Serial.println(")");
  #endif

  if (alarmLowVolume.active && (tankVolume > (alarmLowVolume.trigLevel + WATERTANK_HYSTERESIS_LOW_VOLUME_ALARM))) {
    //
    // Cease alarm
    //
    alarmLowVolume.active = false;
    alarmLowVolume.callback(false, tankVolume);
  } else if (!alarmLowVolume.active && (tankVolume < alarmLowVolume.trigLevel)) {
    //
    // Raise alarm
    //
    alarmLowVolume.active = true;
    alarmLowVolume.callback(true, tankVolume);
  }
	return(tankVolume);
}

// void WaterTank::WaterTank(uint8_t trigPin, uint8_t echoPin, float area, float height) {
//   WaterTank(trigPin, echoPin, area, height, null);
// }

WaterTank::WaterTank(uint8_t trigPin, uint8_t echoPin, float area, float height, float lowVolumeLevel, WATERTANK_ALARM_CALLBACK_SIGNATURE) {
  pinTrig = trigPin;
  pinEcho = echoPin;
    
  pinMode(pinTrig,OUTPUT);
  pinMode(pinEcho,INPUT);
  
  tankArea = area;
  tankHeight = height;

  alarmLowVolume.active = false;
  alarmLowVolume.trigLevel = lowVolumeLevel;
  alarmLowVolume.callback = lowVolumeAlarmCallback;

  alarmLowVolume.callback(alarmLowVolume.active, 0.0);

}
 
