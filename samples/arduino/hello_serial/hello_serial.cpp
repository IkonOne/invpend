#include "Arduino.h"

// #include "src/iplib/arduino/SerialClient.h"
#include "src/iplib/Protocol.h"
#include "src/iplib/SerialConnection.h"

using namespace iplib;

serial::SerialConnection _connection;
const char msg[] = "Hello Serial\n";

void setup() {
    _connection.Open(serial::Baud::_115200);
}

void loop() {
    _connection.Transmit(msg, sizeof(msg));
}