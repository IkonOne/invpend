#ifndef _SOCKETINTERFACE_H_
#define _SOCKETINTERFACE_H_

namespace iplib {
namespace net {

/**
 *	A generalized interface so that we can test different types of sockets.
 */
class AbstractSocket {
  public:

	/**
	 *	Opens a socket for communication on the given port.
	 */
	virtual void Open(unsigned short port) = 0;

	/**
	 *	Closes the socket.
	 */
	virtual void Close() = 0;

	/**
	 *	Derp
	 */
	virtual bool IsOpen() const = 0;

	/**
	 *	Sends [data] of [size] to the [dest] address.
	 */
	virtual void Send(const Address &dest, const void *data, int size) = 0;
	
	/**
	 *	Receives data from the destination src_out address.
	 *	- [src_out] is set to the source address upon success.
	 *	- [buffer] is the buffer where the received data is stored.
	 *	- [buff_size] is the size of the buffer.
	 *	- Returns the size of the data received if any.  Otherwise -1.
	 */
	virtual int Receive(Address &src_out, void *buffer, int buff_size) = 0;
};

} // net
} // iplib

#endif // _SOCKETINTERFACE_H_
