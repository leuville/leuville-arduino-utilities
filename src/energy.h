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

	RTCZero & _rtc;

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

	EnergyController(RTCZero & rtc): _rtc(rtc) {}

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
	 * Return the current voltage, between 0 and 100%
	 * 
	 * pin & divider depends on board type used
	 * feather_m0 = (A7,2)
	 * sparkfun_prorf = (A5,2)
	 * 
	 * https://forum.sparkfun.com/viewtopic.php?f=117&t=49246&p=221795&hilit=battery+level#p221795
	 * https://www.omzlo.com/articles/your-arduino-samd21-adc-is-wrong-did-you-notice
	 */
	template <uint16_t PIN=BATTPIN, uint16_t DIVIDER=POWDIVIDER>
	uint8_t getBatteryPower() {
		constexpr uint16_t VMAX = 420;	// 4.20 V
		constexpr uint16_t VMIN = 360;	// 3.60 V

		int vcurrent = (int)((analogRead(PIN)*DIVIDER*3.3*100)/1024);
		if (vcurrent > VMAX)
			vcurrent = VMAX;
		else if (vcurrent < VMIN)
			vcurrent = VMIN;
		return (100 * (vcurrent - VMIN))/(VMAX - VMIN);
	}

	EnergyController & pullupPin(uint8_t unusedPin) {
		pinMode(unusedPin, INPUT_PULLUP);
		return *this;
	}

};
