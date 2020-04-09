#ifndef PACKETBUFFER_H
#define PACKETBUFFER_H

namespace iplib {
namespace net {

/**
 * Circular buffer of raw fundamental types.
 */
class PacketBuffer {
  public:
    PacketBuffer(int capacity);
    ~PacketBuffer();

    int GetSize() const;
    char *GetData() const;

    bool HasData() const;
    bool IsFull() const;
    bool IsEmpty() const;

    void Reset();

    bool ReadRaw(char *buffer, int length);

    bool WriteRaw(const char *data, int length);

    template <typename T>
    bool Read(T *out) {
        return ReadRaw(reinterpret_cast<char*>(out), sizeof(T));
    }

    template<typename T>
    bool Write(const T data) {
        return WriteRaw(reinterpret_cast<const char*>(&data), sizeof(T));
    }


  private:
    int _capacity;
    char *_buffer;
    int _size;
    int _head;
    int _tail;
};

} // net
} // iplib

#endif // PACKETBUFFER_H