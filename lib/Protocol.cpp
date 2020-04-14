#include "Protocol.h"

namespace iplib {
namespace net {

size_t GetPacketSize(unsigned char packetType) {
    switch(packetType) {
        case PacketType::NULL_PACKET:   return null_packet_t::GetSize();

        case PacketType::SYN:           return syn_t::GetSize();
        case PacketType::SYN_ACK:       return syn_ack_t::GetSize();
        case PacketType::ACK:           return ack_t::GetSize();

        case PacketType::IPSRV_READY:   return ipsrv_ready_t::GetSize();
        case PacketType::IPSRV_POS:     return ipsrv_pos_t::GetSize();

        case PacketType::NONE:
        default:
            return 0;
    }
}

}   //  net
}   //  iplib