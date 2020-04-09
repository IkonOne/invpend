#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "PacketBuffer.h"

namespace iplib {
namespace net {

typedef unsigned int size_t;
typedef unsigned char id_t;
typedef unsigned int checksum_t;
typedef unsigned short sequence_t;

enum class PacketType : unsigned char {
    NONE            = 0,


    NULL_PACKET     = 5,

    // three-way handshake
    SYN             = 10,
    SYN_ACK         = 11,
    ACK             = 12,

    // Server -> Inverted Pendulum
    // reserved 50-99


    // Inverted Pendulum -> Server
    // reserved 100-149

    IPSRV_READY     = 100,
    IPSRV_POS       = 101


    // Server -> Client
    // reserved 150-199

    // Client -> Server
    // reserved 200-249
};

typedef struct packet_header {
    id_t sid;    // sender id
    PacketType type;   // type of packet sent

    void Read(PacketBuffer &pb) {
        pb.Read(&sid);
        pb.Read(&type);
    }

    void Write(PacketBuffer &pb) {
        pb.Write(&sid);
        pb.Write(&type);
    }

    checksum_t GetChecksum() const {
        return
            (checksum_t)sid +
            (checksum_t)type;
    }

    size_t GetSize() const {
        return
            sizeof(sid) +
            sizeof(type);
    }
} packet_header_t;

typedef struct packet_footer {
    checksum_t checksum;  // checksum of data for validation
    sequence_t sequence;  // where this packet is in the sequence

    void Read(PacketBuffer &pb) {
        pb.Read(&checksum);
        pb.Read(&sequence);
    }

    void Write(PacketBuffer &pb) {
        pb.Write(&checksum);
        pb.Write(&sequence);
    }

    checksum_t GetChecksum() {
        return
            (checksum_t)checksum +
            (checksum_t)sequence;
    }

    size_t GetSize() {
        return
            sizeof(checksum) +
            sizeof(sequence);
    }
} packet_footer_t;

// No idea what this is for
typedef struct NULL_PACKET {
    void Read(PacketBuffer &pb) {}
    void Write(PacketBuffer &pb) {}
    checksum_t GetChecksum() { return 0; }
    size_t GetSize() { return 0; }
} null_packet_t;

typedef struct SYN {
    unsigned int val;

    void Read(PacketBuffer &pb) { pb.Read(&val); }
    void Write(PacketBuffer &pb) { pb.Write(&val); }
    checksum_t GetChecksum() { return val; }
    size_t GetSize() { return sizeof(val); }
} syn_t;

typedef struct SYN_ACK {
    unsigned int val;

    void Read(PacketBuffer &pb) { pb.Read(&val); }
    void Write(PacketBuffer &pb) { pb.Write(&val); }
    checksum_t GetChecksum() { return val; }
    size_t GetSize() { return sizeof(val); }
} syn_ack_t;

typedef struct ACK {
    unsigned int val;

    void Read(PacketBuffer &pb) { pb.Read(&val); }
    void Write(PacketBuffer &pb) { pb.Write(&val); }
    checksum_t GetChecksum() { return val; }
    size_t GetSize() { return sizeof(val); }
} ack_t;

// Inverted Pendulum

typedef null_packet_t ipsrv_ready_t;

typedef struct IPSRV_POS {
    float pend_theta;
    // float motor_pos;

    void Read(PacketBuffer &pb) {
        pb.Read(&pend_theta);
        // pb.Read(&motor_pos);
    }

    void Write(PacketBuffer &pb) {
        pb.Write(&pend_theta);
        // pb.Write(&motor_pos);
    }

    checksum_t GetChecksum() {
        return
            (checksum_t)pend_theta;
    }

    size_t GetSize() {
        return
            sizeof(pend_theta);
    }
} ipsrv_pos_t ;

} // net
} // iplib

#endif // PROTOCOL_H