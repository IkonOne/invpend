#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#ifdef ARDUINO
#include "arduino/SerialConnectionImpl.h"
#else
#include <string>
#include "desktop/SerialConnectionImpl.h"
#endif

#include "PacketBuilder.h"
#include "globals.h" // byte

namespace iplib {
namespace net {

class SerialConnection {

#ifndef ARDUINO
  private:
    std::string _device;

  public:
    SerialConnection(std::string device, const unsigned long baud);

    int GetBytesAvailable() const;
    void SetDevice(std::string device);
    std::string GetDevice() const;
    void Open(std::string device, const unsigned long baud);
#endif

  public:
    SerialConnection() = default;
    ~SerialConnection();
    SerialConnection(const unsigned long baud);
    void SetBaud(const unsigned long baud);
    unsigned long GetBaud() const;

    void Open();
    void Open(const unsigned long baud);
    void Close();

    int Transmit(const ::byte *data, int length);
    int Receive(::byte *buffer, int length);

  private:
    SerialConnectionImpl _impl;
};

}   //  net
}   //  iplib

#endif // SERIALCONNECTION_H