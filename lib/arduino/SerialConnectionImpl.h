#ifndef SERIALCONNECTIONIMPL_H
#define SERIALCONNECTIONIMPL_H

#include "../globals.h"
#include "Arduino.h"

namespace iplib {
namespace net {

class SerialConnectionImpl {
  public:
    SerialConnectionImpl() = default;
    ~SerialConnectionImpl();

    void SetBaud(const unsigned long baud);
    const unsigned long GetBaud() const;

    void Open();
    void Close();

    int Transmit(const byte *data, int length);
    int Receive(byte *buffer, int length);

  private:
    unsigned long _baud;
};

}   //  net
}   //  iplib

#endif // SERIALCONNECTIONIMPL_H