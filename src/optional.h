#pragma once

template <typename T>
class optional {
private:
    T *_object = nullptr;
public:
    operator bool() {
        return _object != nullptr;
    }
    T * operator *() {
        return _object;
    }
};