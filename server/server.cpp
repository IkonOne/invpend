#include <iostream>
#include <functional>
#include <stdexcept>
#include <thread>

#include <iplib/Address.h>
#include <iplib/globals.h>
#include <iplib/MathHelpers.h>
#include <iplib/Peer.h>
#include <iplib/SocketUDP.h>
#include <iplib/SerialConnection.h>

using namespace std;
using namespace iplib;

typedef net::Peer<net::SocketUDP> PeerUDP;
typedef net::Peer<net::SerialConnection> PeerSerial;

constexpr int PORT_SERVER = 30000;
constexpr int PORT_CLIENT = 30001;
constexpr int PORT_SIM = 30002;
const net::Address ADDRESS_CLIENT(127,0,0,1, PORT_CLIENT);
const net::Address ADDRESS_SIM(127,0,0,1, PORT_SIM);

union {
    net::ipsrv_pos_t ipsrv_pos;
    net::clisrv_cart_pos_t clisrv_cart_pos;
} packet;

int main() {
// int main(int argc, char *argv[]) {
    // auto prog_name = argv[0];

    // if (argc != 3) {
    //     cout << "Usage: " << prog_name << " <connection type> <port>\n";
    //     cout << "\tValid <connections types> are: { serial, udp }.\n";
    //     cout << "\t<ports> are the colloquial ports for each type of connection.\n";

    //     throw new runtime_error("invalid number of arguments - "s + to_string(argc));
    // }

    PeerUDP endpointUDP(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());
    endpointUDP.GetConnection().SetTransmitAddress(ADDRESS_CLIENT);
    endpointUDP.GetConnection().Open(PORT_SERVER);
    endpointUDP.GetConnection().SetBlocking(true);

    while (true) {
        while (endpointUDP.IsPacketReady()) {
            switch(endpointUDP.GetPacketType()) {
                case net::PacketType::IPSRV_POS:
                    endpointUDP.Receive(packet.ipsrv_pos);
                    endpointUDP.GetConnection().SetTransmitAddress(ADDRESS_CLIENT);
                    endpointUDP.Transmit(&packet.ipsrv_pos);
                    break;
                
                case net::PacketType::CLISRV_CART_POS:
                    endpointUDP.Receive(packet.clisrv_cart_pos);
                    endpointUDP.GetConnection().SetTransmitAddress(ADDRESS_SIM);
                    endpointUDP.Transmit(&packet.clisrv_cart_pos);
            }
        }

        this_thread::sleep_for(chrono::milliseconds(1));
    }

    endpointUDP.GetConnection().Close();

    return 0;
}