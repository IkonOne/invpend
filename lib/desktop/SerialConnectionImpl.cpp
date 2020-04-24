#include <iostream>
#include <stdexcept>
#include <string>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stddef.h>
#include "globals.h"    // byte
#include "../SerialConnection.h"

using namespace std;

namespace iplib {
namespace net {

SerialConnectionImpl::SerialConnectionImpl()
    : _baud(net::Baud::_9600), _fd(-1)
{ }

SerialConnectionImpl::~SerialConnectionImpl() {
    Close();
}

void SerialConnectionImpl::SetDevice(string device) { _device = device; }
string SerialConnectionImpl::GetDevice() const { return _device; }
void SerialConnectionImpl::SetBaud(const unsigned long baud) { _baud = baud; }
unsigned long SerialConnectionImpl::GetBaud() const { return _baud; }

void SerialConnectionImpl::Open() {
    // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#full-example
    // https://en.wikibooks.org/wiki/Serial_Programming/termios
    // https://github.com/scream3r/java-simple-serial-connector/blob/f5564869f8a70503c29c0d24609245321ba39b2c/src/cpp/_nix_based/jssc.cpp#L241

    if ((_fd = open(_device.c_str(), O_RDWR | O_NDELAY | O_NOCTTY)) < 0)
        throw new runtime_error("Error " + to_string(errno) + " from open(): " + strerror(errno));
 
    if (tcgetattr(_fd, &_tty) < 0)
        throw new runtime_error("Error " + to_string(errno) + " getting attributes tcgetattr(): " + strerror(errno));

    _tty.c_cflag = _tty.c_iflag = _tty.c_lflag = _tty.c_oflag = 0;
    _tty.c_cflag = CS8 | CREAD | CLOCAL | HUPCL;
    _tty.c_cc[VMIN] = 0;
    _tty.c_cc[VTIME] = 0;

    // Setting the speed before setting attributes is required (at least on mac)
    if (cfsetspeed(&_tty, (speed_t)_baud) < 0)
        throw new runtime_error("Error " + to_string(errno) + " setting baud rate: " + strerror(errno));

    if (tcsetattr(_fd, TCSANOW, &_tty) < 0)
        throw new runtime_error("Error " + to_string(errno) + " setting attributes tcsetattr(): " + strerror(errno));
 
    // set exclusive access
    if (ioctl(_fd, TIOCEXCL) < 0)
        throw new runtime_error("Error " + to_string(errno) + " getting exclusive access to port: " + strerror(errno));
    
    if (tcflush(_fd, TCIOFLUSH) != 0)
        throw new runtime_error("Error " + to_string(errno) + " flushing the port tcflush(): " + strerror(errno));

}

void SerialConnectionImpl::Close() {
    if (_fd == -1)
        return;

    ioctl(_fd, TIOCNXCL);
    if (close(_fd) < 0)
        throw new runtime_error("Error " + to_string(errno) + " from close: " + strerror(errno));
    _fd = -1;
}

int SerialConnectionImpl::Transmit(const ::byte *data, int length) {
    int result = write(_fd, data, length);
    if (-1 == result)
        throw new runtime_error("Error " + to_string(errno) + " from write: " + strerror(errno));
    
    return result;
}

int SerialConnectionImpl::GetBytesAvailable() const {
    int bytesAvailable;
    ioctl(_fd, FIONREAD, &bytesAvailable);
    return bytesAvailable;
}

int SerialConnectionImpl::Receive(::byte *buffer, int length) {
    if (GetBytesAvailable() == 0)
        return 0;
    int result = read(_fd, buffer, length);
    return result;
}

}   //  net
}   //  iplib