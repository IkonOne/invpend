#ifndef ARDUINO

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
	void Open(unsigned short port);
	void Close();
	bool IsOpen() const;
	void Send(const Address &dest, const void *data, int size);
	int Receive(Address &src_out, void *buffer, int buff_size);

  private:
	int _sockfd;
};

} // net
} // iplib

#endif // _SOCKETUDP_H_

#endif // ARDUINO