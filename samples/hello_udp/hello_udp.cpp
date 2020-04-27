#include <iostream>
#include <exception>
#include "iplib/Address.h"
#include "iplib/SocketUDP.h"

using namespace std;
using namespace iplib;

int main() {
	const int PORT_TX = 30000;
	const int PORT_RX = 30001;
	const net::Address ADDRESS_TX(127,0,0,1, PORT_TX);
	const net::Address ADDRESS_RX(127,0,0,1, PORT_RX);
	const char data[] = "Hello World!\n";
	unsigned char buffer[256];

	net::SocketUDP socket_rx;
	socket_rx.Open(PORT_RX);

	net::SocketUDP socket_tx;
	socket_tx.Open(PORT_TX);
	socket_tx.SetTransmitAddress(ADDRESS_RX);
	socket_tx.Transmit(data, sizeof(data));

	int bytes_read = 0;
	while (0 < (bytes_read = socket_rx.Receive(buffer, 255))) {
		cout << bytes_read << endl;
		cout << buffer << endl;;

		// int bytes_read = socket_rx.Receive(buffer, sizeof(buffer));

		// if (!bytes_read)
		// 	continue;

		// cout << buffer;
		// break;
	}

	return 0;
}
