#include <errno.h>
#include <exception>
#include <iostream>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h> // strerror
#include <string>

#include "Address.h"
#include "SocketUDP.h"

using namespace std;

namespace iplib {
namespace net {

SocketUDP::~SocketUDP() {
	Close();
}

bool SocketUDP::IsOpen() const { return _sockfd > 0; }

void SocketUDP::Open(unsigned short port) {
	_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (_sockfd <= 0)
		throw runtime_error("failed to create a socket - "s + strerror(errno) );
	
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	// htons == host to network byte order
	addr.sin_port = htons(port);

	if (::bind(_sockfd, (const sockaddr*) &addr, sizeof(sockaddr_in)) == -1)
		throw runtime_error("failed to bind socket - "s + strerror(errno));

	// const int non_blocking = 1;
	// if (fcntl(_sockfd, F_SETFL, O_NONBLOCK, non_blocking) == -1)
	// 	throw runtime_error("failed to set non-blocking - "s + strerror(errno));
}

void SocketUDP::Close() {
	if (close(_sockfd) == -1)
		throw runtime_error("failed to close - "s + strerror(errno));

	_sockfd = 0;
}

int SocketUDP::Transmit(const void *data, int size) {
	if (!IsOpen())
		throw runtime_error("socket is not open");

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(_tx_address.GetAddress());
	addr.sin_port = htons(_tx_address.GetPort());

	int sent_bytes = sendto(_sockfd, (const char *)data, size, 0, (sockaddr*) &addr, sizeof(sockaddr_in));

	if (sent_bytes == -1)
		throw runtime_error("failed to send packet - "s + strerror(errno));

	return sent_bytes;
}

int SocketUDP::Receive(void *data, int size) {
	if (!IsOpen())
		throw runtime_error("socket is not open");

	sockaddr_in from;
	socklen_t from_len = sizeof(sockaddr_in);
	int bytes = recvfrom(_sockfd, (char*)data, size, 0, (sockaddr*) &from, &from_len);

	if (bytes != -1) {
		_rx_address = Address(ntohl(from.sin_addr.s_addr), ntohs(from.sin_port));
		return bytes;
	}

	return 0;
}

void SocketUDP::SetBlocking(bool block) {
	const int non_blocking = block ? 1 : 0;
	if (fcntl(_sockfd, F_SETFL, O_NONBLOCK, non_blocking) == -1)
		throw runtime_error("failed to set blocking - "s + strerror(errno));
}

} // net
} // iplib