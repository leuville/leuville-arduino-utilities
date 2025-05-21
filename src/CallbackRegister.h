#pragma once

#include <Arduino.h>
#include <functor.h>
#include <fixed-map.h>

namespace lstl = leuville::simple_template_library;
using namespace lstl;

namespace leuville {
namespace lora {

template <typename T>
using Callback = MemberFunction<T,void>;

/*
 * Stores a function map (key, function pointer)
 */
template <typename K, typename V, uint8_t SIZ = 10>
class CallbackRegister
{
	using MemberFuncPtr = void(V::*)();	// callback type (member function pointer)

    private:
        mapArray<K, Callback<V>> _callbacks;

    public:

        void set(K key, V * target, MemberFuncPtr ptrF) {
            _callbacks.put(key, Callback<V>(target, ptrF));        
        }

        void execute(K key) {
            _callbacks[key]();
        }

};

}
}
