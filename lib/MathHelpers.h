#ifndef MATHHELPERS_H
#define MATHHELPERS_H

namespace iplib {

struct pid_s {
    pid_s(float setpoint, float kp, float ki, float kd)
        : setpoint(setpoint), kp(kp), ki(ki), kd(kd)
    { }

    float setpoint;
    float kp, ki, kd;
    float integral;
    float prev_error;
};

float pid(pid_t &pid, float measured, float dt) {
    // https://en.wikipedia.org/wiki/PID_controller#Pseudocode
    float error = pid.setpoint - measured;
    pid.integral = pid.integral + error * dt;
    float d = (error - pid.prev_error) / dt;
    pid.prev_error = error;

    return pid.kp * error + pid.ki * pid.integral + pid.kd * d;
}

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