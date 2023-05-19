/*************************************************************************************************
 *
 *************************************************************************************************/

#ifndef __WATER_TANK_H__
#define __WATER_TANK_H__

#define WATERTANK_DEBUG true

#define WATERTANK_ALARM_CALLBACK_SIGNATURE std::function<void(boolean, float)> lowVolumeAlarmCallback
#define WATERTANK_TIMEOUT 20000
#define WATERTANK_HYSTERESIS_LOW_VOLUME_ALARM 20 // Liter
class WaterTank {
	private:
		uint8_t pinTrig, pinEcho;
		float tankArea;
		float tankHeight;
		float measureDistance();
    WATERTANK_ALARM_CALLBACK_SIGNATURE;
    

    struct {
      boolean active;
      float trigLevel;
      std::function<void(boolean, float)> callback;
    } alarmLowVolume;
    
	public:
		float getTankVolume(); // Measured volume in liter
//    WaterTank(uint8_t pinTrig, uint8_t pinEcho, float area, float height);
    WaterTank(uint8_t pinTrig, uint8_t pinEcho, float area, float height, float lowLevelAlarm, WATERTANK_ALARM_CALLBACK_SIGNATURE);
};
 
#endif
