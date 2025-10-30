/*
 * Module: LowPowerClock
 *
 * Function: Lowpower based on ArduinoLowPower & RTCZero
 *
 * Copyright and license: See accompanying LICENSE file.
 *
 * Author: Laurent Nel
 */

#pragma once

#include <RTCZero.h>

/*
 * LowPowerClock class
 */
class LowPowerClock: public RTCZero {

public:

	void begin(bool resetTime= false) {
		if (! isConfigured()) {
			RTCZero::begin(resetTime);
		}
	}

	/*
     * Stand by mode
     */
    void standbyMode() {
		bool restoreUSBDevice = false;
		if (SERIAL_PORT_USBVIRTUAL) {
			USBDevice.standby();
		} else {
			USBDevice.detach();
			restoreUSBDevice = true;
		}
		// Disable systick interrupt:  See https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;	

		RTCZero::standbyMode();

		// Enable systick interrupt
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	
		if (restoreUSBDevice) {
			USBDevice.attach();
		}
	}
};

/*
 * "singleton"
 */
LowPowerClock lowPowerClock;
