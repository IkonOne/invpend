#include <iostream>
#include <string>
#include <iplib/PacketBuilder.h>
#include <iplib/Protocol.h>
#include <iplib/SerialConnection.h>
#include <iplib/Peer.h>
#include <iplib/MathHelpers.h>

using namespace std;
using namespace iplib;

string _device = "/dev/cu.usbmodem14101";
net::syn_t syn;
net::Peer<net::SerialConnection> _peer(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());

int main() {
	_peer.GetConnection().SetDevice(_device);
	_peer.GetConnection().Open(net::Baud::_115200);

	while (true) {
		cout << "waiting..." << endl;
		while (!_peer.IsPacketReady());
		_peer.Receive(syn);

		cout << "got one" << endl;
		cout << syn.val << endl;
	}

	return 0;
}
