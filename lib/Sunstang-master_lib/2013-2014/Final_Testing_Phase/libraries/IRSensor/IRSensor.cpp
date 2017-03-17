#include "IRSensor.h"

IRSensor::IRSensor(uint8_t _pin, const IRSensorPowerRegressionProperties& sensorProperties) :
    pin(_pin),
    amplitude(sensorProperties.amplitude),
    exponent(sensorProperties.exponent) {}

int IRSensor::getRaw() {
    return analogRead(pin);
}

float IRSensor::cmToIn(float cm) {
    return cm*0.390625; // 1/2.56 -- mulitplication is more efficient than division
}

float IRSensor::inToCm(float in) {
    return in*2.56;
}

float IRSensor::rawToCm(int raw) {
    return amplitude*pow(raw, exponent); // power regression function
}

float IRSensor::rawToIn(int raw) {
    return cmToIn(rawToCm(raw));
}

float IRSensor::getDistance(Unit unit) {
    switch (unit) {
    case CM:  return rawToCm(getRaw());
    case DM:  return rawToCm(getRaw()) * 0.1;
    case M:   return rawToCm(getRaw()) * 0.01;
    case IN:  return rawToIn(getRaw());
    case FT:  return rawToIn(getRaw()) * 0.083333;
    case RAW:
    default:  return getRaw();
    }
}