#include "QuadEncoder.h"
#include "QuadEncoderSensor.h"

#include <math.h>

/**
 *  QuadEncoder Helpers
 */

static const uint8_t QUADRANT_I    = 0b00000001;
static const uint8_t QUADRANT_II   = 0b00000010;
static const uint8_t QUADRANT_III  = 0b00000100;
static const uint8_t QUADRANT_IV   = 0b00001000;
static const uint8_t QUADRANT_ALL  = 0b00001111;
static const uint8_t QUADRANT_NONE = 0b00000000;

static const int8_t DIR_POS = 1;
static const int8_t DIR_NEG = -1;

uint8_t get_quadrant(float x, float y) {
    if (x >= 0) {
        if (y > 0)
            return QUADRANT_I;
        return QUADRANT_IV;
    }

    if (y > 0)
        return QUADRANT_II;
    return QUADRANT_III;
}

uint8_t get_quadrant(float theta) {
    if (theta >= 0) {
        if (theta < M_PI_2)
            return QUADRANT_I;
        return QUADRANT_II;
    }

    if (theta >= -M_PI_2)
        return QUADRANT_IV;
    return QUADRANT_III;
}

int8_t get_quadrant_dir(uint8_t prevQ, uint8_t currQ) {
    if (prevQ == currQ) return 0; // most common case

    switch (prevQ | currQ) {
        case 0b00000011:
        case 0b00000110:
        case 0b00001100:
            return prevQ < currQ ? DIR_POS : DIR_NEG;

        case 0b00001001:
            return prevQ > currQ ? DIR_POS : DIR_NEG;
    }

    return 0;
}

/**
 * QuadEncoder Class Definition
 */

namespace iplib {
namespace arduino {

QuadEncoder::QuadEncoder(uint8_t emitterPin, uint8_t sensorPin0, uint8_t sensorPin1)
    : _sensor(emitterPin, sensorPin0, sensorPin1)
{}

void QuadEncoder::Calibrate(int duration_ms) {
    _sensor.Calibrate(duration_ms);
}

void QuadEncoder::Update() {
    _sensor.Update();

    float x = _sensor.ReadMappedValue(0);
    float y = _sensor.ReadMappedValue(1);
    float curr_theta = atan2(y, x);
    uint8_t curr_quadrant = get_quadrant(curr_theta);
    int8_t curr_dir = get_quadrant_dir(_prev_quadrant, curr_quadrant);

    if (_prev_quadrant != curr_quadrant) {

        if (_quadrant_dir != curr_dir) {
            _quadrant &= _prev_quadrant;

            if ((_quadrant_dir == DIR_POS && _quadrant == QUADRANT_NONE && _prev_quadrant == QUADRANT_I && curr_quadrant == QUADRANT_IV) |
                (_quadrant_dir == DIR_NEG && _quadrant == QUADRANT_NONE && _prev_quadrant == QUADRANT_IV && curr_quadrant == QUADRANT_I))
            {
                _quadrant_dir = curr_dir;
                _quadrant = curr_quadrant;
            }
        }
        else {
            if (_quadrant == QUADRANT_ALL) {
                _num_loops += _quadrant_dir;
                _quadrant = curr_quadrant;
            }

            if (_prev_quadrant == QUADRANT_IV && curr_quadrant == QUADRANT_I ||
                _prev_quadrant == QUADRANT_I && _prev_quadrant == QUADRANT_IV)
            {
                _quadrant = curr_quadrant;
            }
        }

        _quadrant |= curr_quadrant;
    }

    _theta = 2 * M_PI * _num_loops + curr_theta;
    if (_quadrant_dir == DIR_POS) {
        if (curr_quadrant == QUADRANT_III || curr_quadrant == QUADRANT_IV)
            _theta += 2 * M_PI;
    }
    else {
        if (curr_quadrant == QUADRANT_I || curr_quadrant == QUADRANT_II)
            _theta -= 2 * M_PI;
    }

    _prev_theta = curr_theta;
    _prev_quadrant = curr_quadrant;
}

} // arduino
} // iplib

