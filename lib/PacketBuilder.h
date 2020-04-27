#ifndef PACKETBUILDER_H
#define PACKETBUILDER_H

#include "globals.h" // byte

namespace iplib {
namespace net {

class PacketBuilder {
  public:
    PacketBuilder() = delete;
    PacketBuilder(int capacity, bool isLittleEndian = false);
    ~PacketBuilder();

    unsigned short GetSize() const;
    ::byte *GetData() const;

    bool HasData() const;
    bool IsEmpty() const;

    uint8_t GetChecksum();

    void Reset();
    bool ReadRaw(::byte *buffer, unsigned long count, bool swapEndianness = false);
    bool WriteRaw(const ::byte *data, unsigned int length, bool swapEndianness = false);

    template <typename T>
    bool Read(T *out) {
        return ReadRaw(reinterpret_cast<::byte*>(out), sizeof(T), _isLittleEndian);
    }

    template<typename T>
    bool Write(const T *data) {
        return WriteRaw(reinterpret_cast<const ::byte*>(data), sizeof(T), _isLittleEndian);
    }

  private:
    unsigned int _capacity;
    bool _isLittleEndian;
    ::byte *_buffer;
    unsigned int _head;
    unsigned int _tail;
};

} // net
} // iplib

#endif // PACKETBUILDER_H