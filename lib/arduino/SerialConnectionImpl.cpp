#include "Arduino.h"

#include "SerialConnectionImpl.h"

namespace iplib {
namespace serial {

SerialConnectionImpl::~SerialConnectionImpl() {
    Close();
}

void SerialConnectionImpl::SetBaud(const unsigned long baud) {
    _baud = baud;
}

const unsigned long SerialConnectionImpl::GetBaud() const {
    return _baud;
}

void SerialConnectionImpl::Open() {
    Serial.begin(_baud);
}

void SerialConnectionImpl::Close() {
    Serial.end();
}

int SerialConnectionImpl::Transmit(const char *data, int length) {
    return Serial.write(data, length);
}

int SerialConnectionImpl::Receive(char *buffer, int length) {
    return Serial.readBytes(buffer, length);
}

}   //  serial
}   //  iplib