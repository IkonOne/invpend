#ifndef _SOCKETUDP_H_
#define _SOCKETUDP_H_

#include "AbstractSocket.h"

namespace iplib {
namespace net {

/**
 *	Implements a UDP Socket.
 */
class SocketUDP : public AbstractSocket {
  public:
	~SocketUDP();
	void Open(unsigned short port) override;
	void Close() override;
	bool IsOpen() const override;
	int Transmit(const void *data, int size) override;
	int Receive(void *buffer, int buff_size) override;

	void SetBlocking(bool block=true);

  private:
	int _sockfd;
};

} // net
} // iplib

#endif // _SOCKETUDP_H_