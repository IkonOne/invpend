#include "src/iplib/QuadEncoderSensor.h"

QuadEncoderSensor sensor(2, A0, A1);

void setup() {
    Serial.begin(115200);

    sensor.Calibrate(8000);
}

void loop() {

}