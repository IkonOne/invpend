#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "PacketBuilder.h"
#include <stdint.h>

namespace iplib {
namespace net {

typedef uint16_t size_t;
typedef uint8_t id_t;
typedef uint16_t sequence_t;
typedef uint16_t protocol_id_t;

size_t GetPacketSize(unsigned char packetType);

namespace PacketType {
    constexpr id_t NONE            = 0;

    constexpr id_t NULL_PACKET     = 5;

    // three-way handshake
    constexpr id_t SYN             = 10;
    constexpr id_t SYN_ACK         = 11;
    constexpr id_t ACK             = 12;

    // Server -> Inverted Pendulum
    // reserved 50-99


    // Inverted Pendulum -> Server
    // reserved 100-149

    constexpr id_t IPSRV_READY     = 100;
    constexpr id_t IPSRV_POS       = 101;


    // Server -> Client
    // reserved 150-199

    // Client -> Server
    // reserved 200-249

    constexpr id_t CLISRV_CART_POS  = 201;

}  //  PacketType

typedef struct packet_header {
    protocol_id_t pid;
    id_t type;   // type of packet sent
    size_t dataSize;

    void ReadFrom(PacketBuilder &pb) {
        pb.Read(&pid);
        pb.Read(&type);
        pb.Read(&dataSize);
    }

    void WriteTo(PacketBuilder &pb) const {
        pb.Write(&pid);
        pb.Write(&type);
        pb.Write(&dataSize);
    }

    static constexpr size_t GetSize() {
        return
            sizeof(pid) +
            sizeof(type) +
            sizeof(dataSize);
    }
} packet_header_t;

// No idea what this is for
typedef struct NULL_PACKET {
    static constexpr unsigned char TYPE = PacketType::NULL_PACKET;

    packet_header_t header;
    char nothing;

    void ReadFrom(PacketBuilder &pb) { pb.Read(&nothing); }
    void WriteTo(PacketBuilder &pb) const { pb.Write(&nothing); }
    static constexpr size_t GetSize() { return sizeof(nothing); }
} null_packet_t;

typedef struct SYN {
    static constexpr unsigned char TYPE = PacketType::SYN;

    packet_header_t header;
    uint32_t val;

    void ReadFrom(PacketBuilder &pb) { pb.Read(&val); }
    void WriteTo(PacketBuilder &pb) const { pb.Write(&val); }
    static constexpr size_t GetSize() { return sizeof(val); }
} syn_t;

typedef struct SYN_ACK {
    static constexpr unsigned char TYPE = PacketType::SYN_ACK;

    packet_header_t header;
    uint32_t val;

    void ReadFrom(PacketBuilder &pb) { pb.Read(&val); }
    void WriteTo(PacketBuilder &pb) const { pb.Write(&val); }
    static constexpr size_t GetSize() { return sizeof(val); }
} syn_ack_t;

typedef struct ACK {
    static constexpr unsigned char TYPE = PacketType::ACK;

    packet_header_t header;
    uint32_t val;

    void ReadFrom(PacketBuilder &pb) { pb.Read(&val); }
    void WriteTo(PacketBuilder &pb) const { pb.Write(&val); }
    static constexpr size_t GetSize() { return sizeof(val); }
} ack_t;

// Inverted Pendulum

typedef struct IPSRV_READY {
    static constexpr unsigned char TYPE = PacketType::IPSRV_READY;

    packet_header_t header;
    int32_t ready;

    void ReadFrom(PacketBuilder &pb) { pb.Read(&ready); }
    void WriteTo(PacketBuilder &pb) const { pb.Write(&ready); }
    static constexpr size_t GetSize() { return sizeof(ready); }
} ipsrv_ready_t;

typedef struct IPSRV_POS {
    static constexpr unsigned char TYPE = PacketType::IPSRV_POS;

    packet_header_t header;
    float pend_theta;
    float cart_x;

    void ReadFrom(PacketBuilder &pb) {
        pb.Read(&pend_theta);
        pb.Read(&cart_x);
    }

    void WriteTo(PacketBuilder &pb) const {
        pb.Write(&pend_theta);
        pb.Write(&cart_x);
    }

    static constexpr size_t GetSize() {
        return
            sizeof(pend_theta) +
            sizeof(cart_x);
    }
} ipsrv_pos_t ;

typedef struct CLISRV_CART_POS {
    static constexpr unsigned char TYPE = PacketType::CLISRV_CART_POS;

    packet_header_t header;
    float cart_x;

    void ReadFrom(PacketBuilder&pb) {
        pb.Read(&cart_x);
    }

    void WriteTo(PacketBuilder &pb) const {
        pb.Write(&cart_x);
    }

    static constexpr size_t GetSize() {
        return sizeof(cart_x);
    }
} clisrv_cart_pos_t;

} // net
} // iplib

#endif // PROTOCOL_H