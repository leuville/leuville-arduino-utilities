/*
 * Module: ISRWrapper
 *
 * Function: classes for Object-Oriented approach of ISR
 *
 * Copyright and license: See accompanying LICENSE file.
 *
 * Author: Laurent Nel
 */

#pragma once

#include <Arduino.h>
#define __ASSERT_USE_STDERR
#include <assert.h>

/*
 * For object-oriented approach of ISR
 * subclass should implement ISR_callback() virtual function
 *
 * This class is template to provide many singletons as needed
 * (one per PIN as there is one interrupt per PIN)
 */
template <uint32_t PIN>
class ISRWrapper {
private:

	static ISRWrapper<PIN> * _instance;

	/*
	 * Calls member function ISR_callback()
	 * on if delay between this interrupt and the previous one is more than _delay
	 */
	static void ISR_commonCB() {
		if (_instance->_delay == 0)
			_instance->ISR_callback(PIN);
		else {
			unsigned long now = micros();
			if (now > _instance->_lastEventTmst + _instance->_delay) {
				_instance->_lastEventTmst = now;
				_instance->ISR_callback(PIN);
			}
		}
	}

protected:

	const uint32_t _mode 			= INPUT_PULLUP;
	uint32_t _reason 				= CHANGE;
	const unsigned long _delay 		= 250*1000; // us
	unsigned long _lastEventTmst 	= 0;
	bool _enabled					= false;

public:

	static const ISRWrapper<PIN> *instance() { return _instance; }

	/*
	 * Constructor
	 *
	 * mode = INPUT, INPUT_PULLUP
	 * reason = CHANGE, LOW, HIGH, RISING, FALLING
	 */
	ISRWrapper(uint32_t mode = INPUT_PULLUP, uint32_t reason = CHANGE, unsigned long delay = 250000)
		: _mode(mode), _reason(reason), _delay(delay) {

		assert(_instance == nullptr);
		_instance = this;
	}

	virtual ~ISRWrapper() = default;

	/*
	 * pinMode(PIN, REASON) is called
	 */
	virtual void begin() {
		pinMode(PIN, _mode);
	}

	virtual void enable() {
		enable(_reason);
	}

	/*
	 * Attach interrupt for a given reason (CHANGE, HIGH, LOW, FALLING, RISING)
	 *
	 */
	virtual void enable(uint32_t reason) {
		if (_enabled)
			return;
		_enabled = true;
		_reason = reason;
		attachInterrupt(digitalPinToInterrupt(PIN), ISRWrapper::ISR_commonCB, _reason);
	}

	/*
	 * Detach interrupt
	 */
	virtual void disable() {
		if (!_enabled)
			return;
		_enabled = false;
		detachInterrupt(PIN);
	}

	/*
	 * Called by interrupt
	 * To override with respect for the ISR mechanism constraints
	 * pin parameter may be useful in case of multiple inheritance
	 */
	virtual void ISR_callback(uint8_t pin) = 0;
};

/*
 * Declares and init the singleton / static variable (link purpose)
 */
template <uint32_t PIN>
ISRWrapper<PIN> * ISRWrapper<PIN>::_instance = nullptr;

/*
 * Example of use
 *
 * A device connected to pin #A3
 */
/*
class Device: private ISRWrapper<A3> {
public:
	using ISRWrapper<A3>::ISRWrapper; // inherits constructor

	virtual void ISR_callback(uint8_t pin) {
		// do something compatible with ISR spirit
	}

};
*/

/*
 * ISRWrapper with delegate
 * May be used without subclassing
 */
template <typename T, uint32_t PIN>
class ISRDelegate: public ISRWrapper<PIN> {

	typedef void (T::*ptrFM)(uint8_t);	// callback type (member function pointer)

protected:

	T & _delegate;
	ptrFM _func;

public:

	ISRDelegate(T & delegate, ptrFM func, uint32_t mode = INPUT, uint32_t reason = CHANGE, uint32_t delay = 100)
		: ISRWrapper<PIN>(mode, reason, delay), _delegate(delegate), _func(func) {
	}

	virtual void ISR_callback(uint8_t pin) override {
		(_delegate.*_func)(pin);
	}
};

