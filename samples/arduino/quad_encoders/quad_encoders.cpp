#include "src/iplib/arduino/QuadEncoder.h"
#include "src/iplib/arduino/QuadEncoderSensor.h"

// QuadEncoderSensor sensor(2, A0, A1);
QuadEncoder encoder(2, A0, A1);

void setup() {
    Serial.begin(115200);

    Serial.println("Calibrating sensor...");
    encoder.Calibrate(8000);
    Serial.println("Calibration complete");

    encoder.Initialize();
}

void loop() {
    encoder.Update();
    auto theta = encoder.GetTheta();

    Serial.print(theta);
    Serial.println();
}