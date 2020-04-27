#include "PacketBuilder.h"
#include "globals.h"

namespace iplib {
namespace net {

PacketBuilder::PacketBuilder(int capacity, bool isLittleEndian)
    : _capacity(capacity), _isLittleEndian(isLittleEndian), _buffer(new ::byte[capacity])
{ }

PacketBuilder::~PacketBuilder() {
    delete[] _buffer;
}

unsigned short PacketBuilder::GetSize() const { return _tail - _head; }
::byte *PacketBuilder::GetData() const { return _buffer; }
bool PacketBuilder::HasData() const { return _tail > _head; }
bool PacketBuilder::IsEmpty() const { return _tail == _head; }

uint8_t PacketBuilder::GetChecksum() {
    return 12;
    // uint8_t sum = 0;
    // for (unsigned int i = 0; i <= _tail; ++i) {
    //     sum ^= _buffer[i];
    // }
    // return sum;
}

void PacketBuilder::Reset() {
    _tail = _head = 0;
}

bool PacketBuilder::ReadRaw(::byte *buffer, unsigned long count, bool swapEndianness) {
    if (GetSize() < count)
        return false;
    
    if (swapEndianness) {
        for (unsigned int i = 0; i < count; ++i)
            buffer[i] = _buffer[_head + count - 1 - i];
    }
    else {
        for (unsigned int i = 0; i < count; ++i)
            buffer[i] = _buffer[_head + i];
    }

    _head += count;

    return true;
}


bool PacketBuilder::WriteRaw(const ::byte *data, unsigned int length, bool swapEndianness) {
    if (_capacity - GetSize() < length)
        return false;
    
    if (swapEndianness) {
        for (unsigned int i = 0; i < length; ++i)
            _buffer[_tail++] = data[length - 1 - i];
    }
    else {
        for (unsigned int i = 0; i < length; ++i)
            _buffer[_tail++] = data[i];
    }

    return true;
}

} //    net
} //    iplib