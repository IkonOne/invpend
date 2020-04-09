#ifndef SERIALCONNECTIONIMPL_H
#define SERIALCONNECTIONIMPL_H

#include "../globals.h"

namespace iplib {
namespace serial {

class SerialConnectionImpl {
  public:
    SerialConnectionImpl() = default;
    ~SerialConnectionImpl();

    void SetBaud(const unsigned long baud);
    const unsigned long GetBaud() const;

    void Open();
    void Close();

    int Transmit(const char *data, int length);
    int Receive(char *buffer, int length);

  private:
    unsigned long _baud;
};

}   //  serial
}   //  iplib

#endif // SERIALCONNECTIONIMPL_H