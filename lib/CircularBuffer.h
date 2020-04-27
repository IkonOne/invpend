#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

namespace iplib {

template <typename T>
class CircularBuffer {
  public:
    CircularBuffer() = delete;

    ~CircularBuffer() {
        delete[] _data;
    }

    explicit CircularBuffer(int size) 
        : _data(new T[size]), _capacity(size)
    { }

    bool GetIsFull() const { return GetSize() == GetCapacity(); }
    bool GetIsEmpty() const { return _head == _tail; }
    int GetCapacity() const { return _capacity; }

    int GetSize() const {
        if (_tail >= _head)
            return _tail - _head;
        return _tail + _capacity - _head;
    }

    void Clear() {
        _head = _tail = 0;
    }

    bool Put(T val) {
        if (GetIsFull())
            return false;
        
        _tail = (_tail + 1) % _capacity;
        _data[_tail] = val;
        return true;
    }

    int Put(T *data, int count) {
        int i = 0;
        while (i < count && Put(data[i++]));
        return i;
    }

    bool GetFront(T &out) {
        if (GetIsEmpty())
            return false;
        
        out = _data[_head];
        _head = (_head + 1) %_capacity;
        return true;
    }

    int GetFront(T *buffer, int count) {
        int i = 0;
        while (i < count) {
            if(!GetFront(buffer[i++])) {
                i--;
                break;
            }
        }

        return i;
    }

    bool GetBack(T &out) {
        if (GetIsEmpty())
            return false;
        
        out = _data[_tail];
        _tail = (_tail - 1) % _capacity;
        return true;
    }

    int GetBack(T *buffer, int count) {
        int i = 0;
        while (i < count) {
            if (!GetBack(buffer[i++])) {
                i--;
                break;
            }
        }

        return i;
    }

  private:
    T *_data;
    const int _capacity;
    int _head;
    int _tail;
};

}   // iplib

#endif // CIRCULARBUFFER_H