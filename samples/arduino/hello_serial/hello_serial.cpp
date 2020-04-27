#include "Arduino.h"

#include "src/iplib/Protocol.h"
#include "src/iplib/SerialConnection.h"
#include "src/iplib/Peer.h"
#include "src/iplib/PacketBuilder.h"
#include "src/iplib/MathHelpers.h"

using namespace iplib;

net::Peer<net::SerialConnection> _peer(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());
net::syn_t syn;

void setup() {
    _peer.GetConnection().Open(net::Baud::_115200);
    syn.val = 123;
    delay(1000);
}

void loop() {
    _peer.Transmit(&syn);
    delay(500);
}