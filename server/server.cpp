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

constexpr int PORT_SERVER = 30000;
constexpr int PORT_CLIENT = 30001;
constexpr int PORT_SIM = 30002;
net::Address ADDRESS_CLIENT(127,0,0,1, PORT_CLIENT);
net::Address ADDRESS_SIM(127,0,0,1, PORT_SIM);
// const net::Address ADDRESS_SIM(127,0,0,1, PORT_SIM);
void UpdateNet() {
    union {
        net::ipsrv_pos_t ipsrv_pos;
        net::clisrv_cart_pos_t clisrv_cart_pos;
    } packet;

    net::Peer<net::SocketUDP> endpointUDP(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());
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
}

// int main() {
int main(int argc, char *argv[]) {
    auto prog_name = argv[0];

    if (argc > 2) {
        cout << "Usage: " << prog_name << " <client ip>\n";
        throw new runtime_error("invalid number of arguments - "s + to_string(argc));
    }

    if (argc == 2)
        ADDRESS_CLIENT = net::Address::fromString(argv[1]);
    
    cout << "Client IP: " << ADDRESS_CLIENT << endl;

    thread t(UpdateNet);
    t.join();

    return 0;
}