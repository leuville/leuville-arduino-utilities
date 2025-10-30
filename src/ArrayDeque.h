#pragma once

#include <Arduino.h>

namespace leuville {
namespace simple_template_library {

/*
 * Fixed-size deque (fifo) implemented with a circular array. 
 * 
 * if SYNC is true, the object is protected against interrupts
 */
template <typename T, bool SYNC = false, uint8_t SIZ = 20>
class ArrayDeque {
protected:

    T _data[SIZ];
    int8_t _front = SIZ-1;
    int8_t _back = 0;
    uint8_t _size = 0;
    uint8_t _fullPolicy = BLOCK;

    static void shift(int8_t& pos, int8_t step) {
        pos += step;
        if (pos < 0)
            pos = SIZ - 1;
        else if (pos > SIZ - 1)
            pos = 0;
    }

    bool isRoomAvailable() {
        if (full()) {
            switch (_fullPolicy) {
                case KEEP_FRONT: 
                    pop_back(); 
                    return true;
                case KEEP_BACK: 
                    pop_front(); 
                    return true;
                case BLOCK: 
                    return false;
            }
        }
        return true;
    }

public:

    /*
     * fullPolicy is policy to apply when deque is full
     * KEEP_FRONT = discard back to make room available
     * KEEP_BACK = discard front to make room available
     * BLOCK = no room available
     */
    enum { KEEP_FRONT, KEEP_BACK, BLOCK };

    ArrayDeque(uint8_t fullPolicy = BLOCK): _fullPolicy(fullPolicy) {}

    constexpr uint8_t max_size() const {
        return SIZ;
    }

    uint8_t size() const {
        return _size;
    }

    bool empty() const {
        if (SYNC) noInterrupts();
        bool res = (size() == 0);
        if (SYNC) interrupts();
        return res;
    }

    bool full() const {
        if (SYNC) noInterrupts();
        bool res = (_size == SIZ);
        if (SYNC) interrupts();
        return res;
    }

    bool push_front(const T& elt) {
        if (! isRoomAvailable()) {
            return false;
        }
        if (SYNC) noInterrupts();
        _data[_front] = elt;
        shift(_front, -1);
        _size++;
        if (SYNC) interrupts();
        return true;
    }

    bool push_back(const T& elt) {
        if (! isRoomAvailable()) {
            return false;
        }
        if (SYNC) noInterrupts();
        _data[_back] = elt;
        shift(_back, +1);
        _size++;
        if (SYNC) interrupts();
        return true;
    }

    const T& front() const {
        if (SYNC) noInterrupts();
        int8_t pos = _front;
        shift(pos, +1);
        const T& res = _data[pos];
        if (SYNC) interrupts();
        return res;
    }

    const T& back() const {
        if (SYNC) noInterrupts();
        int8_t pos = _back;
        shift(pos, -1);
        const T& res = _data[pos];
        if (SYNC) interrupts();
        return res;
    }

    T* frontPtr() {
        if (_size == 0)
            return nullptr;
        if (SYNC) noInterrupts();
        int8_t pos = _front;
        shift(pos, +1);
        T* res = & _data[pos];
        if (SYNC) interrupts();
        return res;
    }

    T* backPtr() {
        if (_size == 0)
            return nullptr;
        if (SYNC) noInterrupts();
        int8_t pos = _back;
        shift(pos, -1);
        T* res = & _data[pos];
        if (SYNC) interrupts();
        return res;
    }

    void pop_front() {
        if (SYNC) noInterrupts();
        shift(_front, +1);
        _size--;
        if (SYNC) interrupts();
    }

    void pop_back() {
        if (SYNC) noInterrupts();
        shift(_back, -1);
        _size--;
        if (SYNC) interrupts();
    }
    
};

}
}
