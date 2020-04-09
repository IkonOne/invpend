#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#ifdef ARDUINO
#include "arduino/SerialConnectionImpl.h"
#else
#include "desktop/SerialConnectionImpl.h"
#endif

#include "globals.h"

namespace iplib {
namespace serial {

class SerialConnection {
  public:
    SerialConnection() = default;
    ~SerialConnection();
    SerialConnection(const unsigned long baud);

#ifndef ARDUINO
    SerialConnection(char *device, const unsigned long baud);

    void SetDevice(char *device);
    const char *GetDevice() const;
    void Open(char *device, const unsigned long baud);
#endif

    void SetBaud(const unsigned long baud);
    unsigned long GetBaud() const;

    void Open();
    void Open(const unsigned long baud);
    void Close();

    int Transmit(const char *data, int length);
    int Receive(char *buffer, int length);

  private:
    SerialConnectionImpl _impl;
};

}   //  serial
}   //  iplib

#endif // SERIALCONNECTION_H