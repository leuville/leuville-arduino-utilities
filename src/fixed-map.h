#pragma once

#include <Arduino.h>

namespace leuville {
namespace simple_template_library {

#define NO_INTERRUPT_IF(SYNC) if constexpr(SYNC) noInterrupts()
#define INTERRUPT_IF(SYNC) if constexpr(SYNC) interrupts()

/*
 * Fixed-size map
 */
template <typename K = String, typename V = uint8_t, bool SYNC = false, uint8_t SIZ = 20>
class mapArray
{
    struct Pair {
        K _key;
        V _value;
    };

    Pair _data[SIZ];
    uint8_t _size = 0;

    Pair* get(const K & key) {
        for (uint8_t i = 0; i < _size; i++) {
            Pair & pair = _data[i];
            if (pair._key == key)
                return &pair;
        }
        return nullptr;
    }

    public:
    
    bool put(const K key, const V value) {
        if (_size == SIZ)
            return false;
        Pair* pair = get(key);
        NO_INTERRUPT_IF(SYNC);
        if (pair == nullptr) {
            _data[_size++] = { key, value };
        } else {
            pair->_value = value;    
        }  
        INTERRUPT_IF(SYNC); 
        return true; 
    }

    V & operator[](const K & key) {
        Pair* pair = get(key);
        return pair->_value;
    }

    K & key(uint8_t pos) {
        Pair &pair = _data[pos];
        return pair._key;
    }

    V & value(uint8_t pos) {
        Pair &pair = _data[pos];
        return pair._value;
    }
};

}
}
