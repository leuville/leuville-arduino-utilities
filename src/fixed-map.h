#pragma once

#include <Arduino.h>

/*
 * Fixed-size map
 */
template <typename K, typename V, uint8_t SIZ = 20>
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
    
    bool put(const K & key, const V & value) {
        if (_size == SIZ)
            return false;
        Pair* pair = get(key);
        if (pair == nullptr) {
            _data[_size++] = { key, value };
        } else {
            pair->_value = value;    
        }      
        return true; 
    }

    V * operator[](const K & key) {
        Pair* pair = get(key);
        if (pair == nullptr) {
            return nullptr;
        } else {
            return & pair->_value;
        }
    }
};
