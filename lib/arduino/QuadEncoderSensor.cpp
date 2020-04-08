#include "QuadEncoderSensor.h"

// Core iplib libraries
#include "../MathHelpers.h"

/**
 * QuadEncoderSensor Class Definition
 */

namespace iplib {
namespace arduino {

QuadEncoderSensor::QuadEncoderSensor(uint8_t emitterPin, uint8_t sensorPin0, uint8_t sensorPin1)
    : _mapMin(-1.0f), _mapMax(1.0f)
{
    _sensorPins[0] = sensorPin0;
    _sensorPins[1] = sensorPin1;
    _qtr.setTypeAnalog();
    _qtr.setSensorPins(_sensorPins, 2);
    _qtr.setEmitterPin(emitterPin);
    _qtr.setSamplesPerSensor(1);
}

void QuadEncoderSensor::Calibrate(uint16_t duration_ms) {
    // analogRead() takes about 0.1ms on avr
    // 0.1ms * 1 sample per sensor * 2 sensors
    // * 10 reads per calibrate() call = ~2ms per calibrate()
    for (uint16_t i = 0; i < duration_ms / 2; ++i)
        _qtr.calibrate();
}

void QuadEncoderSensor::Update() {
    _qtr.read(_values);
}

void QuadEncoderSensor::SetMapRange(float min, float max) {
    _mapMin = min;
    _mapMax = max;
}

float QuadEncoderSensor::ReadMappedValue(int pin) {
    return iplib::fmap(
        _values[pin],
        _qtr.calibrationOn.minimum[pin], _qtr.calibrationOn.maximum[pin],
        _mapMin, _mapMax
    );
}

uint16_t QuadEncoderSensor::ReadCalibratedValue(int pin) {
    return _values[pin];
}

}   // arduino
}   // iplib