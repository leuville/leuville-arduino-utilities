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
#include <RTCZero.h>

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
			long int now = micros();
			if (now > _instance->_lastEventTmst + _instance->_delay*1000) {
				_instance->_lastEventTmst = now;
				_instance->ISR_callback(PIN);
			}
		}
	}

protected:

	const uint32_t _pin 	= PIN;
	const uint32_t _mode 	= INPUT;
	uint32_t _reason 		= CHANGE;
	const uint32_t _delay 	= 200;
	long int _lastEventTmst = 0;
	bool _enabled			= false;

public:

	static const ISRWrapper<PIN> *instance() { return _instance; }

	/*
	 * Constructor: pinMode(PIN, REASON) is called
	 *
	 * mode = INPUT, INPUT_PULLUP
	 * reason = CHANGE, LOW, HIGH, RISING, FALLING
	 */
	ISRWrapper(uint32_t mode = INPUT, uint32_t reason = CHANGE, uint32_t delay = 100)
		: _mode(mode), _reason(reason), _delay(delay) {

		assert(_instance == nullptr);
		_instance = this;
	}

	virtual ~ISRWrapper() = default;

	virtual void begin() {
		pinMode(_pin, _mode);
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
		attachInterrupt(digitalPinToInterrupt(_pin), ISRWrapper::ISR_commonCB, _reason);
	}

	/*
	 * Detach interrupt
	 */
	virtual void disable() {
		if (!_enabled)
			return;
		_enabled = false;
		detachInterrupt(_pin);
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

/*
 * Timer based on RTCZero
 */
class ISRTimer {

	static ISRTimer* _instance;

	static void ISR_timer() {
		_instance->disable();
		_instance->ISR_timeout();
		if (_instance->_repeated) {
			_instance->enable();
		}
	}

protected:

	RTCZero		& _rtc;
	uint32_t	_timeout = 60*60;
	bool 		_enabled = false;
	bool 		_repeated = false;

public:

	static const ISRTimer *instance() { return _instance; }

	/*
	 * Construct a timer
	 * timeout defaults to 1 hour with no repetition
	 * pre-existing instance is disabled
	 */
	ISRTimer(RTCZero & rtc, uint32_t timeout = 60*60, boolean repeated = false)
		: _rtc(rtc), _timeout(timeout), _repeated(repeated) {

		if (_instance != nullptr) {
			_instance->disable();
		}
		_instance = this;
	}

	virtual ~ISRTimer() {
		disable();
	}

	/*
	 * Init RTC
	 */ 
	virtual void begin(bool resetTime = true) {
		_rtc.begin(resetTime);
		_rtc.setDate(1, 1, 0);
	}

	/*
	 * Activates the timer
	 */
	virtual void enable() {
		_rtc.attachInterrupt(ISRTimer::ISR_timer);
		_enabled = true;
		_rtc.setAlarmEpoch(_rtc.getEpoch() + _timeout);
		_rtc.enableAlarm(_rtc.MATCH_YYMMDDHHMMSS);
	}

	/*
	 * Disable (deactivate) the timer
	 */
	virtual void disable() {
		_enabled = false;
		_rtc.disableAlarm();
		_rtc.detachInterrupt();
	}

	/*
	 * Set the timeout value
	 */
	virtual void setTimeout(uint32_t timeout) {
		boolean enabled = _enabled;
		if (_enabled) {
			disable();
		}
		_timeout = timeout;
		if (enabled) {
			enable();
		}
	}

	/*
	 * Print informations (debug purpose)
	 */
	void printStatus() {
		Serial.print(F("[ "));
		Serial.print(F("time="));
			Serial.print(_rtc.getHours()); Serial.print(F(":")); Serial.print(_rtc.getMinutes());Serial.print(F(":")); Serial.print(_rtc.getSeconds());Serial.print(F(" "));
		Serial.print(F("date="));
			Serial.print(_rtc.getDay()); Serial.print(F("/")); Serial.print(_rtc.getMonth());Serial.print(F("/")); Serial.print(_rtc.getYear());Serial.print(F(" "));
		Serial.print(F("alarm="));
			Serial.print(_rtc.getAlarmHours()); Serial.print(F(":")); Serial.print(_rtc.getAlarmMinutes());Serial.print(F(":")); Serial.print(_rtc.getAlarmSeconds());Serial.print(F(" "));
			Serial.print(_rtc.getAlarmDay());Serial.print(F("/"));Serial.print(_rtc.getAlarmMonth());Serial.print(F("/"));Serial.print(_rtc.getAlarmYear());Serial.print(F(" "));
		Serial.println(F("]"));
	}

	/*
	 * Called by interrupt
	 * To override with respect for the ISR mechanism constraints
	 */
	virtual void ISR_timeout() = 0;
};

/*
 * Declares and init the singleton / static variable (link purpose)
 */
ISRTimer * ISRTimer::_instance = nullptr;
