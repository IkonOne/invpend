#include <iostream>
#include <exception>
#include "iplib/Address.h"
#include "iplib/SocketUDP.h"

using namespace std;
using namespace iplib;

int main() {
	const int PORT = 30000;
	const net::Address LOOPBACK(127,0,0,1, PORT);
	const char data[] = "Hello World!\n";
	unsigned char buffer[256];

	net::SocketUDP socket;
	socket.Open(PORT);
	socket.Send(LOOPBACK, data, sizeof(data));

	while (true) {
		net::Address src;
		int bytes_read = socket.Receive(src, buffer, sizeof(buffer));

		if (!bytes_read)
			continue;

		cout << buffer;
		break;
	}

	socket.Close();

	return 0;
}
