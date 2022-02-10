/*
 * Module: energy
 *
 * Function: utilities to save energy
 *
 * Copyright and license: See accompanying LICENSE file.
 *
 * Author: Laurent Nel
 */

#pragma once

#include <RTCZero.h>

/*
 * Provides standbymode based on RTCZero
 *
 * A device with standby feature should inherit from this class
 */
class EnergyController {

	RTCZero & 		_rtc;
	const uint16_t	_vMin = 2200;
	const uint16_t	_vMax = 3200;

	/*
 	 * SAMD 32kHz clock initialization for standby modes
 	 * Must be called after a first call to attachInterrupt()
 	 */
	void setupClock() {

		// Set the XOSC32K to run in standby
		SYSCTRL->XOSC32K.bit.RUNSTDBY = 1;

		// The RTCZero library will setup generic clock 2 to XOSC32K/32
		// and we'll use that for the EIC.

		GCLK->CLKCTRL.reg = uint16_t(
			GCLK_CLKCTRL_CLKEN |
			GCLK_CLKCTRL_GEN_GCLK2 |
			GCLK_CLKCTRL_ID( GCLK_CLKCTRL_ID_EIC_Val )
		);

		// Anti-bouncing
		EIC->CONFIG[1].bit.FILTEN1 = 1;

		while (GCLK->STATUS.bit.SYNCBUSY) {}
	}

public:

	EnergyController(RTCZero & rtc, uint16_t vMin = 2200, uint16_t vMax = 3200)
		: _rtc(rtc), _vMin(vMin), _vMax(vMax) 
	{

	}

	void begin() {
	}

	void enable() {
		setupClock();
	}

	/*
	 * Stand by mode
	 */
	void standby() {
		_rtc.standbyMode();
	}

	/*
	 * Return the voltage in mV & battery level, between 0 and 100%
	 * 
	 * pin depends on board type used
	 * feather_m0 = (A7)
	 * sparkfun_prorf = (A5)
	 * 
	 * https://forum.sparkfun.com/viewtopic.php?f=117&t=49246&p=221795&hilit=battery+level#p221795
	 * https://www.omzlo.com/articles/your-arduino-samd21-adc-is-wrong-did-you-notice
	 */
	template <uint16_t PIN=BATTPIN>
	uint16_t getVoltage() {
  		return (uint16_t)roundf((3300.0f / 1023.0f) * (4.7f + 10.0f) / 10.0f * (float)analogRead(PIN));
	}

	template <uint16_t PIN=BATTPIN>
	uint8_t getBatteryPower() {
		uint16_t range = _vMax - _vMin;
		uint16_t vcurrent =  max(getVoltage<PIN>() - _vMin, 0);
		vcurrent = min(vcurrent, range);

		return (uint8_t)round((double)(100.0 * (double)vcurrent) / (double)range);
	}

	/* 
	 * pullup unused pins to save energy
	 */
	EnergyController & pullupPin(uint8_t unusedPin) {
		pinMode(unusedPin, INPUT_PULLUP);
		return *this;
	}

};
