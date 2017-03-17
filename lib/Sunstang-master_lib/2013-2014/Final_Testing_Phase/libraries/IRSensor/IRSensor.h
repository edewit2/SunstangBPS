#ifndef IRSENSOR_H
#define IRSENSOR_H

#include "Arduino.h"


namespace IRSensor {
	struct IRSensorPowerRegressionProperties {
		float amplitude, exponent;
		IRSensorPowerRegressionProperties(float c, float e) : amplitude(c), exponent(e) {}
	}
	const SHARP_2Y0A700(83948, -1.072),
		  SHARP_GP2D120(3241.9, -1.039);
}

enum Unit { RAW, CM, IN, FT, M, DM };

class IRSensor {
public:
    IRSensor(uint8_t _pin, const IRSensor::IRSensorPowerRegressionProperties& sensorProperties);
    inline int getRaw();
    float getDistance(Unit unit);

private:
    uint8_t pin;
    const float &amplitude, &exponent;

    inline float cmToIn(float cm);
    inline float inToCm(float in);
    inline float rawToCm(int raw);
    inline float rawToIn(int raw);
};

#endif