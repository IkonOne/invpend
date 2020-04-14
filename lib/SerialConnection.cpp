#include "SerialConnection.h"
#include "globals.h" // byte

#ifndef ARDUINO
#include <string>
#endif

using namespace std;

namespace iplib {
namespace net {

SerialConnection::~SerialConnection() {
    Close();
}

SerialConnection::SerialConnection(const unsigned long baud) {
    _impl.SetBaud(baud);
}

#ifndef ARDUINO
    SerialConnection::SerialConnection(string device, const unsigned long baud)
        : SerialConnection(baud)
    {
        _impl.SetDevice(device);
    }

    void SerialConnection::SetDevice(string device) {
        _impl.SetDevice(device);
    }

    string SerialConnection::GetDevice() const {
        return _impl.GetDevice();
    }

    int SerialConnection::GetBytesAvailable() const {
        return _impl.GetBytesAvailable();
    }

    void SerialConnection::Open(string device, const unsigned long baud) {
        _impl.SetDevice(device);
        Open(baud);
    }

#endif

void SerialConnection::SetBaud(const unsigned long baud) {
    _impl.SetBaud(baud);
}

unsigned long SerialConnection::GetBaud() const {
    return _impl.GetBaud();
}

void SerialConnection::Open(const unsigned long baud) {
    _impl.SetBaud(baud);
    Open();
}

void SerialConnection::Open() {
    _impl.Open();
}

void SerialConnection::Close() {
    _impl.Close();
}

int SerialConnection::Transmit(const ::byte *data, int length) {
    return _impl.Transmit(data, length);
}

int SerialConnection::Receive(::byte *buffer, int length) {
    return _impl.Receive(buffer, length);
}

}   //  net
}   //  iplib