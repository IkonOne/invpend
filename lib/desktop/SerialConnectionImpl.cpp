#include <stdexcept>
#include <string>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "../SerialConnection.h"

using namespace std;

namespace iplib {
namespace serial {

SerialConnectionImpl::SerialConnectionImpl()
    : _baud(Baud::DEFAULT_RATE)
{ }

SerialConnectionImpl::~SerialConnectionImpl() {
    Close();
}

void SerialConnectionImpl::SetDevice(char *device) { _device = device; }
const char *SerialConnectionImpl::GetDevice() const { return _device; }
void SerialConnectionImpl::SetBaud(const unsigned long baud) { _baud = baud; }
unsigned long SerialConnectionImpl::GetBaud() const { return _baud; }

void SerialConnectionImpl::Open() {
    // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#full-example
    // https://en.wikibooks.org/wiki/Serial_Programming/termios

    if (-1 == (_fd = open(_device, O_RDWR)))
        throw new runtime_error("Error " + to_string(errno) + " from open: " + strerror(errno));

    memset(&_tty, 0, sizeof(_tty));
    if(tcgetattr(_fd, &_tty) != 0)
        throw new runtime_error("Error " + to_string(errno) + " from tcgetattr: " + strerror(errno));

    _tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    _tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    _tty.c_cflag |= CS8; // 8 bits per byte (most common)
    _tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    _tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    _tty.c_lflag &= ~ICANON;
    _tty.c_lflag &= ~ECHO; // Disable echo
    _tty.c_lflag &= ~ECHOE; // Disable erasure
    _tty.c_lflag &= ~ECHONL; // Disable new-line echo
    _tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    _tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    _tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    _tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    _tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // _tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // _tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    _tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    _tty.c_cc[VMIN] = 0;

    if (-1 == cfsetispeed(&_tty, _baud))
        throw new runtime_error("Error " + to_string(errno) + " from cfsetispeed: " + strerror(errno));

    if (-1 == cfsetospeed(&_tty, _baud))
        throw new runtime_error("Error " + to_string(errno) + " from cfsetospeed: " + strerror(errno));
}

void SerialConnectionImpl::Close() {
    if (-1 == close(_fd))
        throw new runtime_error("Error " + to_string(errno) + " from close: " + strerror(errno));
    _fd = 0;
}

int SerialConnectionImpl::Transmit(const char *data, int length) {
    int result = write(_fd, data, length);
    if (-1 == result)
        throw new runtime_error("Error " + to_string(errno) + " from write: " + strerror(errno));
    
    return result;
}

int SerialConnectionImpl::Receive(char *buffer, int length) {
    int result = read(_fd, buffer, length);
    if (-1 == result)
        throw new runtime_error("Error " + to_string(errno) + " from read: " + strerror(errno));
    
    return result;
}

}   //  serial
}   //  iplib