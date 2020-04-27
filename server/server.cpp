#include <iostream>
#include <stdexcept>

#include <iplib/Address.h>
#include <iplib/globals.h>
#include <iplib/MathHelpers.h>
#include <iplib/Peer.h>
#include <iplib/SocketUDP.h>
// #include <iplib/SerialConnection.h>

using namespace std;
using namespace iplib;

constexpr int PORT_SERVER = 30000;
constexpr int PORT_CLIENT = 30001;
constexpr int PORT_SIM = 30002;
const net::Address ADDRESS_CLIENT(127,0,0,1, PORT_CLIENT);
const net::Address ADDRESS_SIM(127,0,0,1, PORT_SIM);

union {
    net::ipsrv_pos_t ipsrv_pos;
} rx_packet;

int main(int argc, char *argv[]) {
    ((void)argc);
    auto prog_name = argv[0];

    // if (argc != 3) {
    //     cout << "Usage: " << prog_name << " <connection type> <port>\n";
    //     cout << "\tValid <connections types> are: { serial, udp }.\n";
    //     cout << "\t<ports> are the colloquial ports for each type of connection.\n";

    //     throw new runtime_error("invalid number of arguments - "s + to_string(argc));
    // }

    net::Peer<net::SocketUDP> peer(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());
    peer.GetConnection().Open(PORT_SERVER);

    while(true) {
        if (peer.IsPacketReady()) {
            switch(peer.GetPacketType()) {
                case net::PacketType::IPSRV_POS:
                    peer.Receive(rx_packet.ipsrv_pos);
                    cout << rx_packet.ipsrv_pos.pend_theta << '\n';
                    break;
                
                default:
                    cout << "shit\n";
            }
        }
    }

    peer.GetConnection().Close();

    return 0;
}