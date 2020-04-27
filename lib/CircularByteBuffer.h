#ifndef CIRCULARBYTEBUFFER_H
#define CIRCULARBYTEBUFFER_H

#include "globals.h"
#include "CircularBuffer.h"

namespace iplib {

class CircularByteBuffer : public CircularBuffer<::byte> {
  public:
    CircularByteBuffer() = delete;
    ~CircularByteBuffer();
    explicit CircularByteBuffer(int size);

    ::byte *GetScratch();
    void CaptureScratch(int size);

  private:
    ::byte *_scratch;
};

} // iplib

#endif // CIRCULARBYTEBUFFER_H