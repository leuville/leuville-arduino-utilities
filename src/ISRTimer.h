/*
 * Module: ISRTimer
 *
 * Function: Timer & lowpower based on RTCZero & ArduinoLowPower
 *
 * Copyright and license: See accompanying LICENSE file.
 *
 * Author: Laurent Nel
 */

#pragma once

#include <LowPowerClock.h>

/*
 * ISRTimer class
 */
class ISRTimer {

	static ISRTimer* _instance;

	static void ISR_timer() {
		_instance->disable();
		_instance->_timeout = _instance->ISR_timeout(); // timeout may be adapted
		if (_instance->_repeated) {
			_instance->enable();
		}
	}

protected:

	uint32_t	_timeout = 60*60;	// 1 hour
	bool 		_enabled = false;
	bool 		_repeated = false;
	bool		_beginDone = false;
	RTCZero	&	_rtc;

public:

	enum REPEATED : bool { OFF = false, ON = true };

	static ISRTimer *instance() { return _instance; }

	/*
	 * Construct a timer
	 * timeout defaults to 1 hour with no repetition
	 * pre-existing instance is disabled
	 */
	ISRTimer(uint32_t timeout = 60*60, bool repeated = REPEATED::ON)
		: _timeout{timeout}, _repeated{repeated}, _rtc(lowPowerClock.getRTC()) {

		if (_instance != nullptr) {
			_instance->disable();
		}
		_instance = this;
	}

	virtual ~ISRTimer() {
		disable();
	}

	virtual void begin() {
		lowPowerClock.begin();
	}

	/*
	 * Becomes public
	 */
	virtual void attachInterruptWakeup(uint32_t pin, voidFuncPtr callback, irq_mode mode) {
		lowPowerClock.attachInterruptWakeup(pin, callback, mode);
	}

	/*
	 * Activates the timer
	 */
	virtual void enable() {
		_enabled = true;
		attachInterruptWakeup(RTC_ALARM_WAKEUP, ISRTimer::ISR_timer, CHANGE);
		lowPowerClock.setAlarmIn(_timeout);
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
	 * Modify the system time and updates the alarm if needed
	 */
	void setEpoch(uint32_t epoch) {
		auto wasEnabled = _enabled;
		if (_enabled) {
			disable();
		}
		_rtc.setEpoch(epoch);
		if (wasEnabled) {
			enable();
		}
	}

	/*
	 * Set the timeout value at future time expressed as hour of the day
	 *
	 * returns true of timeout set
	 * false if time in the past
	 */
	virtual bool setTimeout(uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0) {
		uint32_t now = lowPowerClock.getEpoch();
		uint32_t midnight = now - _rtc.getHours()*3600 - _rtc.getMinutes()*60 - _rtc.getSeconds();
		uint32_t timeout = midnight + 3600 * hour + 60 * minute + second;
		return setTimeout(timeout - now);
	}

	/*
	 * Set the timeout value
	 */
	virtual bool setTimeout(uint32_t timeout) {
		if (timeout <= 0) {
			return false;
		} else {
			bool enabled = _enabled;
			if (_enabled) {
				disable();
			}
			_timeout = timeout;
			if (enabled) {
				enable();
			}
			return true;
		}
	}

	virtual void standbyMode() {
		lowPowerClock.standbyMode();
	}

	RTCZero & getRTC() {
		return _rtc;
	}

	uint32_t getTimeout() {
		return _timeout;
	}
	
	/*
	 * Called by interrupt
	 * To override with respect for the ISR mechanism constraints
	 */
	virtual uint32_t ISR_timeout() = 0;

};

/*
 * Declares and init the singleton / static variable (link purpose)
 */
ISRTimer * ISRTimer::_instance = nullptr;
