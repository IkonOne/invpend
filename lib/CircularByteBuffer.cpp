#include <iostream>
#include "CircularByteBuffer.h"

namespace iplib {

CircularByteBuffer::~CircularByteBuffer() {
    delete[] _scratch;
}

CircularByteBuffer::CircularByteBuffer(int size)
    : CircularBuffer(size), _scratch(new ::byte[size])
{ }

::byte *CircularByteBuffer::GetScratch() {
    return _scratch;
}

void CircularByteBuffer::CaptureScratch(int size) {
    int i = 0;
    while (i < size && Put(_scratch[i++]));
}

}   // iplib