#pragma once

#include <Arduino.h>
#include <lmic/oslmic.h>
#include <functor.h>

namespace lstl = leuville::simple_template_library;
using namespace lstl;

namespace leuville {
namespace lora {

template <typename T>
using Callback = MemberFunction<T,void>;

/*
 * Stores a function map (key, function pointer)
 */
template <typename T, uint8_t SIZ = 10>
class CallbackRegister
{
	using MemberFuncPtr = void(T::*)();	// callback type (member function pointer)

    private:
        osjob_t     _jobs[SIZ];
        Callback<T> _callbacks[SIZ];

    public:

        void set(uint8_t pos, T * target, MemberFuncPtr ptrF) {
            _callbacks[pos] = Callback<T>(target, ptrF);            
        }

        void execute(osjob_t * job) {
            for (uint8_t i = 0; i < SIZ; i++) {
                if (&(_jobs[i]) == job) {
                    _callbacks[i]();
                    return;
                }
            }
         }

        osjob_t * operator[](uint8_t pos) {
            return & _jobs[pos];
        }
};

}
}
