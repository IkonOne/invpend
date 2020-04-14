#ifndef MATHHELPERS_H
#define MATHHELPERS_H

namespace iplib {

inline float flerp(float t, float r_min, float r_max) {
    return r_min + t * (r_max - r_min);
}

inline float fmap(float val, float in_min, float in_max, float out_min, float out_max) {
    return flerp((val - in_min) / (in_max - in_min), out_min, out_max);
}

inline bool isLittleEndian() {
    uint16_t val = 1;
    return *reinterpret_cast<char*>(&val) == 1;
}

}   // iplib

#endif // MATHHELPERS_H