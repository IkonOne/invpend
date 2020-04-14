#include "Arduino.h"

#include "SerialConnectionImpl.h"

namespace iplib {
namespace net {

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

int SerialConnectionImpl::Transmit(const byte *data, int length) {
    return Serial.write(data, length);
}

int SerialConnectionImpl::Receive(byte *buffer, int length) {
    return Serial.readBytes(buffer, length);
}

}   //  net
}   //  iplib