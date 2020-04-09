#include "SerialConnection.h"

using namespace std;

namespace iplib {
namespace serial {

SerialConnection::~SerialConnection() {
    Close();
}

SerialConnection::SerialConnection(const unsigned long baud) {
    _impl.SetBaud(baud);
}

#ifndef ARDUINO
    SerialConnection::SerialConnection(char *device, const unsigned long baud)
        : SerialConnection(baud)
    {
        _impl.SetDevice(device);
    }

    void SerialConnection::SetDevice(char *device) {
        _impl.SetDevice(device);
    }

    const char *SerialConnection::GetDevice() const {
        return _impl.GetDevice();
    }

    void SerialConnection::Open(char *device, const unsigned long baud) {
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

int SerialConnection::Transmit(const char *data, int length) {
    return _impl.Transmit(data, length);
}

int SerialConnection::Receive(char *buffer, int length) {
    return _impl.Receive(buffer, length);
}

}
}