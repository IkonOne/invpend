#ifndef PEER_H
#define PEER_H

#include "Protocol.h"
#include "PacketBuilder.h"
#include "globals.h" // byte
#include <assert.h>
#include <string.h> // memset()

#ifndef ARDUINO
#include <iostream>
#include <iomanip>
#endif

namespace iplib {
namespace net {

template <typename TConnection>
class Peer {
  public:
    Peer() = delete;
    ~Peer();

    explicit Peer(int maxPacketSize, protocol_id_t protocolID, bool isLittleEndian = false);

    TConnection &GetConnection() { return _connection; }

    template <typename TPacket>
    int Transmit(const TPacket *packet) {
        _txBuilder.Reset();
        WriteHeader(TPacket::TYPE);

        packet->WriteTo(_txBuilder);

        WriteChecksum();
        return _connection.Transmit(_txBuilder.GetData(), _txBuilder.GetSize());
    }

    bool IsPacketReady();
    id_t GetPacketType() { return _rxHeader.type; }

    template <typename TPacket>
    void Receive(TPacket &packet) {
        packet.header = _rxHeader;
        packet.ReadFrom(_rxBuilder);
        ResetReceive();
    }

  private:
    void WritePreamble();
    void WriteHeader(unsigned char packetType);
    void WriteChecksum();

    bool ReadProtocol();
    bool ReadHeader();
    bool ReadPacket();
    bool ValidateChecksum();
    void ResetReceive();

    TConnection _connection;
    bool _isLittleEndian;
    PacketBuilder _txBuilder;
    packet_header_t _txHeader;
    PacketBuilder _rxBuilder;
    packet_header_t _rxHeader;

    protocol_id_t _protocolID;

    ::byte _protocolIDHead;
    ::byte _protocolIDTail;
    ::byte _lastProtocolByte;

    // Sequential read state
    ::byte *_readBuff;
    uint8_t _readPktCS;
    unsigned char _readState;
    static constexpr char READ_STATE_PROTO      = 0;
    static constexpr char READ_STATE_HEAD       = 1;
    static constexpr char READ_STATE_PKT        = 2;
    static constexpr char READ_STATE_CHECKSUM   = 3;
    static constexpr char READ_STATE_COMPLETE   = 4;
};

template <typename TConnection>
Peer<TConnection>::Peer(int maxPacketSize, protocol_id_t protocolID, bool isLittleEndian)
    : _isLittleEndian(isLittleEndian),
        _txBuilder(maxPacketSize * 4, isLittleEndian),
        _rxBuilder(maxPacketSize * 4, isLittleEndian),
        _protocolID(protocolID),
        _protocolIDHead(protocolID >> 8),
        _protocolIDTail(protocolID & 0x00FF),
        _lastProtocolByte(~_protocolIDHead),
        _readBuff(new ::byte[maxPacketSize]),
        _readState(READ_STATE_PROTO)
{
    assert(_protocolIDHead != _protocolIDTail);
}

template <typename TConnection>
Peer<TConnection>::~Peer() {
    if (_readBuff) delete[] _readBuff;
}

template <typename TConnection>
void Peer<TConnection>::WriteHeader(unsigned char packetType) {
    _txHeader.pid = _protocolID;
    _txHeader.type = packetType;
    _txHeader.dataSize = GetPacketSize(packetType);
    _txHeader.WriteTo(_txBuilder);
}

template <typename TConnection>
void Peer<TConnection>::WriteChecksum() {
    uint8_t checksum = _txBuilder.GetChecksum();
    _txBuilder.Write(&checksum);
}

template <typename TConnection>
bool Peer<TConnection>::ReadProtocol() {
    ::byte readByte = 0;

    if (_readState > READ_STATE_PROTO)
        return true;

    while (0 != _connection.Receive(&readByte, 1)) {
        #if !defined(ARDUINO) && defined(PACKET_DEBUG)
        std::cout << "PID  " << (unsigned int)readByte << std::endl;
        #endif
        // This assumes that both bytes of the protocolID are unique
        // which is asserted in the constructor.
        if (_lastProtocolByte == _protocolIDHead && readByte == _protocolIDTail) {
            _rxBuilder.Reset();
            _rxBuilder.Write(&_protocolID);
            _lastProtocolByte = ~_protocolIDHead;
            _readState++;
            return true;
        }

        _lastProtocolByte = readByte;
    }

    return false;
}

template <typename TConnection>
bool Peer<TConnection>::ReadHeader() {
    static constexpr int headerSize = packet_header_t::GetSize();

    if (_readState > READ_STATE_HEAD)
        return true;

    int bytesReceived = _connection.Receive(_readBuff, headerSize - _rxBuilder.GetSize());
    _rxBuilder.WriteRaw(_readBuff, bytesReceived);

    #if !defined(ARDUINO) && defined(PACKET_DEBUG)
    for (int i = 0; i < bytesReceived; ++i)
        std::cout << "HEAD " << (unsigned int)_readBuff[i] << std::endl;
    #endif

    if (_rxBuilder.GetSize() < headerSize)
        return false;

    _rxHeader.ReadFrom(_rxBuilder);
    _readState++;
    return true;
}

template <typename TConnection>
bool Peer<TConnection>::ReadPacket() {
    if (_readState > READ_STATE_PKT)
        return true;
    
    int bytesReceived = _connection.Receive(_readBuff, _rxHeader.dataSize - _rxBuilder.GetSize());
    _rxBuilder.WriteRaw(_readBuff, bytesReceived);

    #if !defined(ARDUINO) && defined(PACKET_DEBUG)
    for (int i = 0; i < bytesReceived; ++i)
        std::cout << "PKT  " << (unsigned int)_readBuff[i] << ' ' << _rxHeader.dataSize << ' ' << _rxBuilder.GetSize() << std::endl;
    #endif

    if (_rxBuilder.GetSize() < _rxHeader.dataSize)
        return false;
    
    _readState++;
    return true;
}

template <typename TConnection>
bool Peer<TConnection>::ValidateChecksum() {
    if (_readState > READ_STATE_CHECKSUM)
        return true;

    if (_connection.Receive(_readBuff, 1) < 1)
        return false;
 
    _readPktCS = _rxBuilder.GetChecksum();

    #if !defined(ARDUINO) && defined(PACKET_DEBUG)
    std::cout << "CRC  " << "read " << (unsigned int)_readBuff[0] << " actual " << (unsigned int)_readPktCS << std::endl;
    #endif
    
    if (_readBuff[0] != _readPktCS) {
        ResetReceive();
        return false;
    }

    return true;
}

template <typename TConnection>
void Peer<TConnection>::ResetReceive() {
    _lastProtocolByte = ~_protocolIDHead;
    _readState = READ_STATE_PROTO;
    _rxBuilder.Reset();
}

template <typename TConnection>
bool Peer<TConnection>::IsPacketReady() {
    if (!ReadProtocol()) return false;
    if (!ReadHeader()) return false;
    if (!ReadPacket()) return false;
    return ValidateChecksum();
}


}   //  net
}   //  iplib

#endif // PEER_H