#pragma once

#include <Arduino.h>

/*
 * Fixed-size deque (fifo) implemented with a circular array. 
 * 
 * if SYNC is true, the object is protected against interrupts
 */
template <typename T, bool SYNC = false, uint8_t SIZ = 20>
class deque
{
protected:

    T _data[SIZ];
    int8_t _front = SIZ-1;
    int8_t _back = 0;
    uint8_t _size = 0;

    static void shift(int8_t& pos, int8_t step) {
        pos += step;
        if (pos < 0)
            pos = SIZ - 1;
        else if (pos > SIZ - 1)
            pos = 0;
    }

public:

    constexpr uint8_t max_size() const {
        return SIZ;
    }

    uint8_t size() const {
        if constexpr(SYNC) noInterrupts();
        uint8_t res = _size;
        if constexpr(SYNC) interrupts();
        return res;
    }

    bool empty() const {
        if constexpr(SYNC) noInterrupts();
        bool res = (size() == 0);
        if constexpr(SYNC) interrupts();
        return res;
    }

    bool full() const {
        if constexpr(SYNC) noInterrupts();
        bool res =  (_size == SIZ);
        if constexpr(SYNC) interrupts();
    }

    void push_front(const T& elt) {
        if constexpr(SYNC) noInterrupts();
        _data[_front] = elt;
        shift(_front, -1);
        _size++;
        if constexpr(SYNC) interrupts();
    }

    void push_back(const T& elt) {
        if constexpr(SYNC) noInterrupts();
        _data[_back] = elt;
        shift(_back, +1);
        _size++;
        if constexpr(SYNC) interrupts();
    }

    const T& front() const {
        if constexpr(SYNC) noInterrupts();
        int8_t pos = _front;
        shift(pos, +1);
        const T& res = _data[pos];
        if constexpr(SYNC) interrupts();
        return res;
    }

    const T& back() const {
        if constexpr(SYNC) noInterrupts();
        int8_t pos = _back;
        shift(pos, -1);
        const T& res = _data[pos];
        if constexpr(SYNC) interrupts();
        return res;
    }

    T* frontPtr() {
        if (_size == 0)
            return nullptr;
        if constexpr(SYNC) noInterrupts();
        int8_t pos = _front;
        shift(pos, +1);
        T* res = & _data[pos];
        if constexpr(SYNC) interrupts();
        return res;
    }

    T* backPtr() {
        if (_size == 0)
            return nullptr;
        if constexpr(SYNC) noInterrupts();
        int8_t pos = _back;
        shift(pos, -1);
        T* res = & _data[pos];
        if constexpr(SYNC) interrupts();
        return res;
    }

    void pop_front() {
        if constexpr(SYNC) noInterrupts();
        shift(_front, +1);
        _size--;
        if constexpr(SYNC) interrupts();
    }

    void pop_back() {
        if constexpr(SYNC) noInterrupts();
        shift(_back, -1);
        _size--;
        if constexpr(SYNC) interrupts();
    }
    
    void print() {
        Serial.print("deque: ");
        Serial.print(_front); Serial.print(" ");Serial.print(_back);Serial.print(" ");
        Serial.println(size());
    }
};


