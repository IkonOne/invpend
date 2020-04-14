#ifndef QUADENCODER_H
#define QUADENCODER_H

#include "Arduino.h"
#include "QuadEncoderSensor.h"

class QuadEncoder {
  public:
    QuadEncoder() = delete;
    QuadEncoder(uint8_t emitterPin, uint8_t sensorPin0, uint8_t sensorPin1);

    void Calibrate(int duration_ms);
    void Update();

  private:
    QuadEncoderSensor _sensor;
    long _num_loops;
    float _theta;
    float _prev_theta;
    uint8_t _quadrant;
    uint8_t _prev_quadrant;
    int8_t _quadrant_dir;
};

#endif // QUADENCODER_H