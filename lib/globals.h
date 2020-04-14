#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef BYTE_T
#define BYTE_T
#include <stdint.h>
using byte = uint8_t;
#endif

namespace iplib {

constexpr unsigned int SERIAL_BUFFER_SIZE = 196;
constexpr unsigned int IPLIB_MAX_PACKET_SIZE = 128;
constexpr unsigned int IPLIB_PROTOCOL_ID = 0xFFA7;

namespace net {
namespace Baud {
    const unsigned long _9600 = 9600;
    const unsigned long _115200 = 115200;
}
}

}   //  iplib

#endif // GLOBALS_H