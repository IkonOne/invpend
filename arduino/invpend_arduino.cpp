#include "src/iplib/arduino/QuadEncoder.h"

// iplib net communicaiton
#include "src/iplib/PacketBuilder.h"
#include "src/iplib/Peer.h"
#include "src/iplib/Protocol.h"
#include "src/iplib/SerialConnection.h"
#include "src/iplib/MathHelpers.h"
#include "src/iplib/MathHelpers.h"

using namespace iplib;

QuadEncoder encoder_pend(2, A0, A1);

net::Peer<net::SerialConnection> _peer(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());

net::ipsrv_ready_t ipsrv_ready;
net::ipsrv_pos_t ipsrv_pos;

void setup() {
    _peer.GetConnection().Open(net::Baud::_115200);

    delay(100);

    encoder_pend.Calibrate(4000);
    encoder_pend.Initialize();
}

void loop() {
    encoder_pend.Update();
    ipsrv_pos.pend_theta = encoder_pend.GetTheta();
    _peer.Transmit(&ipsrv_pos);
}