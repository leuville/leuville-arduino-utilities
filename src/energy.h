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
 * Energy saving: pull up pins
 */
/*
void pullupPins(std::initializer_list<uint8_t> unusedPins) {
	for (uint8_t pin: unusedPins) {
		pinMode(pin, INPUT_PULLUP);
	}
}
*/
/*
 * Stand by mode
 */
void standbyMode(RTCZero & rtc) {
	bool restoreUSBDevice = false;
	if (Serial) {
		USBDevice.standby();
	} else {
		USBDevice.detach();
		restoreUSBDevice = true;
	}
	rtc.standbyMode();
	if (restoreUSBDevice) {
		USBDevice.attach();
	}
}

/*
 * 32kHz clock initialization for standby modes
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

/*
 * Return the current voltage, between 0 and 100%
 */
template <uint16_t BATTPIN = A7, uint16_t POWDIVIDER = 2>
uint8_t getBatteryPower() {
	constexpr uint16_t VMAX = 420;	// 4.20 V
	constexpr uint16_t VMIN = 360;	// 3.60 V

	int vcurrent = (int)((analogRead(BATTPIN)*POWDIVIDER*3.3*100)/1024);
	if (vcurrent > VMAX)
		vcurrent = VMAX;
	else if (vcurrent < VMIN)
		vcurrent = VMIN;
	return (100 * (vcurrent - VMIN))/(VMAX - VMIN);
}

/*
 * Provides standbymode based on RTCZero
 *
 * A device with standby feature should inherit from this class
 */
class StandbyMode {

	RTCZero & _rtc;

public:

	StandbyMode(RTCZero & rtc): _rtc(rtc) {}

	void begin() {
		setupClock();
	}

	/*
	 * Stand by mode
	 */
	void standby() {
		standbyMode(_rtc);
	}

};
