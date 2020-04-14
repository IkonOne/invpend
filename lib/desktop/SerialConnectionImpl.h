#ifndef SERIALCONNECTIONIMPL_H
#define SERIALCONNECTIONIMPL_H

#include <string>
#include <termios.h>
#include <stddef.h>
#include "../globals.h"

namespace iplib {
namespace net {

class SerialConnectionImpl {
  public:
    SerialConnectionImpl();
    ~SerialConnectionImpl();

    void SetDevice(std::string device);
    std::string GetDevice() const;

    void SetBaud(const unsigned long baud);
    unsigned long GetBaud() const;

  int GetBytesAvailable() const;

    void Open();
    void Close();

    int Transmit(const ::byte *data, int length);
    int Receive(::byte *data, int length);

  private:
    unsigned long _baud;
    std::string _device;
    int _fd;
    struct termios _tty;
};

}   //  net
}   //  iplib

#endif // SERIALCONNECTIONIMPL_H