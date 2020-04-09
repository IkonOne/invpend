#include "PacketBuffer.h"

namespace iplib {
namespace net {

PacketBuffer::PacketBuffer(int capacity)
    : _capacity(capacity), _buffer(new char[capacity])
{ }

PacketBuffer::~PacketBuffer() {
    delete[] _buffer;
}

int PacketBuffer::GetSize() const { return _size; }
char *PacketBuffer::GetData() const { return _buffer; }
bool PacketBuffer::HasData() const { return _size != 0; }
bool PacketBuffer::IsEmpty() const { return _size == 0; }
bool PacketBuffer::IsFull() const { return _size == _capacity; }

void PacketBuffer::Reset() {
    _size = _head = _tail = 0;
}

bool PacketBuffer::ReadRaw(char *buffer, int length) {
    if (_size < length)
        return false;
    
    for (int i = 0; i < length; ++i) {
        buffer[i] = _buffer[_head];
        _head = (_head + 1) % _capacity;
    }

    _size -= length;
    return true;
}

bool PacketBuffer::WriteRaw(const char *data, int length) {
    if (_capacity - _size < length)
        return false;
    
    for (int i = 0; i < length; ++i) {
        _buffer[_tail] = data[i];
        _tail = (_tail + 1) % _capacity;
    }

    _size -= length;
    return true;
}

} //    net
} //    iplib