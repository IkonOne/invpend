#ifndef QUADENCODER_H
#define QUADENCODER_H

#include "Arduino.h"
#include "QuadEncoderSensor.h"

/**
 * [BEWARE] Comments are lies waiting to happen.
 * 
 * void setup() {
 *  Calibrate() // Must be called first.
 *  Initialize()
 * }
 * 
 * void loop() {
 *  Update()
 * }
 */
class QuadEncoder {
  public:
    QuadEncoder() = delete;
    QuadEncoder(uint8_t emitterPin, uint8_t sensorPin0, uint8_t sensorPin1);

    float GetTheta() const;
    long GetNumLoops() const;
    uint8_t GetQuadrants() const;
    int8_t GetQuadrantDir() const;

    void Calibrate(int duration_ms);
    void Initialize();
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