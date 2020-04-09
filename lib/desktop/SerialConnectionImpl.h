#ifndef SERIALCONNECTIONIMPL_H
#define SERIALCONNECTIONIMPL_H

#include <termios.h>
#include "../globals.h"

namespace iplib {
namespace serial {

class SerialConnectionImpl {
  public:
    SerialConnectionImpl();
    ~SerialConnectionImpl();

    void SetDevice(char *device);
    const char *GetDevice() const;

    void SetBaud(const unsigned long baud);
    unsigned long GetBaud() const;

    void Open();
    void Close();

    int Transmit(const char *data, int length);
    int Receive(char *data, int length);

  private:
    unsigned long _baud;
    char *_device;
    int _fd;
    struct termios _tty;
};

}   //  serial
}   //  iplib

#endif // SERIALCONNECTIONIMPL_H