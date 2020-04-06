#ifndef QUADENCODERSENSOR_H
#define QUADENCODERSENSOR_H

#include <QTRSensors.h>

class QuadEncoderSensor {
  public:
    QuadEncoderSensor() = delete;

    QuadEncoderSensor(uint8_t emitterPin, uint8_t sensorPin0, uint8_t sensorPin1);

    void Calibrate(uint16_t duration_ms = 2000);
    void Update();
    void SetMapRange(float min, float max);
    float ReadMappedValue(int pin);
    uint16_t ReadCalibratedValue(int pin);

  private:
    QTRSensors _qtr;
    uint8_t _sensorPins[2];
    uint16_t _values[2];
    float _mapMin;
    float _mapMax;
};

#endif //QUADENCODERSENSOR_H