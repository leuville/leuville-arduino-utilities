/*
 * Module: ISRTimer
 *
 * Function: Timer based on RTCZero
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
 * ISRTimer class
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
	uint32_t	_alarmEpoch = 0;

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
	 * Returns RTCZero instance reference
	 */
	RTCZero &  getRTC() {
		return _rtc;
	}

	/*
	 * Init RTC
	 */ 
	virtual void begin(bool resetTime = true) {
		_rtc.begin(resetTime);
	}

	/*
	 * Activates the timer
	 */
	virtual void enable() {
		_rtc.attachInterrupt(ISRTimer::ISR_timer);
		_enabled = true;
		_alarmEpoch = _rtc.getEpoch() + _timeout;
		_rtc.setAlarmEpoch(_alarmEpoch);
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
	 * Modify the system time and updates the alarm if needed
	 */
	void setEpoch(uint32_t epoch) {
		uint32_t now = _rtc.getEpoch();
		_rtc.setEpoch(epoch);
		if (_enabled) {
			_rtc.setAlarmEpoch(_alarmEpoch + epoch - now);
		}
	}

	/*
	 * Returns true if a time (H,M,S) is in the future
	 */
	virtual bool isNowBefore(uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0) {
		uint32_t now = _rtc.getEpoch();
		uint32_t midnight = now - _rtc.getHours()*3600 - _rtc.getMinutes()*60 - _rtc.getSeconds();
		uint32_t timeToCheck = midnight + (3600 * hour) + (60 * minute) + second;
		return (timeToCheck > now);
	}

	/*
	 * Set the timeout value at future time expressed as hour of the day
	 *
	 * returns true of timeout set
	 * false if time in the past
	 */
	virtual bool setTimeout(uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0) {
		uint32_t now = _rtc.getEpoch();
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

	uint32_t getTimeout() {
		return _timeout;
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
