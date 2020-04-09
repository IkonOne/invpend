#ifndef GLOBALS_H
#define GLOBALS_H

namespace iplib {

constexpr int SERIAL_BUFFER_SIZE = 256;
constexpr int IPLIB_MAX_PACKET_SIZE = 128;


// Arduino Serial.begin(unsigned long baud)
// POSIX csetispeed(tty, speed_t baud) : speed_t == unsigned long
// Arduino doesn't easily support c++11 (or is it 14)
// this should be 'enum class Baud : unsigned long { ... }'
// POSIX termios.h uses defines '#define B9600 9600'
namespace serial {
namespace Baud {
    constexpr unsigned long _9600 = 9600;
    constexpr unsigned long _57600 = 57600;
    constexpr unsigned long _115200 = 115200;

    constexpr unsigned long DEFAULT_RATE = _9600;
}
}


}   //  iplib

#endif // GLOBALS_H