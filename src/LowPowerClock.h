/*
 * Module: LowPowerClock
 *
 * Function: Lowpower based on ArduinoLowPower (cannot be subclassed beacuse of private setAlarmIn())
 *
 * Copyright and license: See accompanying LICENSE file.
 *
 * Author: Laurent Nel
 */

#pragma once

#include <RTCZero.h>

#define RTC_ALARM_WAKEUP	0xFF
#if defined(ARDUINO_API_VERSION)
using irq_mode = PinStatus;
#else
using irq_mode = uint32_t;
#endif

/*
 * LowPowerClock class
 */
class LowPowerClock {

protected:

 	RTCZero		_rtc;
	bool		_beginDone = false;

	/*
	 * Init clock
	 */
	void configGCLK6() {
		// enable EIC clock
		GCLK->CLKCTRL.bit.CLKEN = 0; //disable GCLK module
		while (GCLK->STATUS.bit.SYNCBUSY);

		GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK6 | GCLK_CLKCTRL_ID( GCM_EIC )) ;  //EIC clock switched on GCLK6
		while (GCLK->STATUS.bit.SYNCBUSY);

		GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(6));  //source for GCLK6 is OSCULP32K
		while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

		GCLK->GENCTRL.bit.RUNSTDBY = 1;  //GCLK6 run standby
		while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

		/* Errata: Make sure that the Flash does not power all the way down
			* when in sleep mode. */

		NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;
	}

public:

	/*
	 * Activate RTC & init in mode 0
	 */ 
	void begin() {
		if (!_beginDone) {
			attachInterruptWakeup(RTC_ALARM_WAKEUP, nullptr, CHANGE);
			_beginDone = true;
		}
	}

	void attachInterruptWakeup(uint32_t pin, voidFuncPtr callback, irq_mode mode) {
		if (pin > PINS_COUNT) {
			// check for external wakeup sources
			// RTC library should call this API to enable the alarm subsystem
			switch (pin) {
				case RTC_ALARM_WAKEUP:
					_rtc.begin(false);
					_rtc.attachInterrupt(callback);
				/*case UART_WAKEUP:*/
			}
			return;
		}	
		EExt_Interrupts in = g_APinDescription[pin].ulExtInt;
		if (in == NOT_AN_INTERRUPT || in == EXTERNAL_INT_NMI)
				return;
	
		//pinMode(pin, INPUT_PULLUP);
		attachInterrupt(pin, callback, mode);
		configGCLK6();
		// Enable wakeup capability on pin in case being used during sleep
		EIC->WAKEUP.reg |= (1 << in);
	}

	void setAlarmIn(uint32_t seconds) {
		if (!_rtc.isConfigured()) {
			attachInterruptWakeup(RTC_ALARM_WAKEUP, NULL, (irq_mode)0);
		}
		_rtc.setAlarmEpoch(_rtc.getEpoch() + seconds);
		_rtc.enableAlarm(_rtc.MATCH_YYMMDDHHMMSS);
	}
	
	/*
     * Stand by mode
     */
    void standbyMode() {
		sleep();
	}

	void sleep() {
		bool restoreUSBDevice = false;
		if (SERIAL_PORT_USBVIRTUAL) {
			USBDevice.standby();
		} else {
			USBDevice.detach();
			restoreUSBDevice = true;
		}
		// Disable systick interrupt:  See https://www.avrfreaks.net/forum/samd21-samd21e16b-sporadically-locks-and-does-not-wake-standby-sleep-mode
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;	
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
		__DSB();
		__WFI();
		// Enable systick interrupt
		SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	
		if (restoreUSBDevice) {
			USBDevice.attach();
		}
	}
	
	/*
	 * Sleep with duration
	 */
	void sleep(uint32_t t) {
		setAlarmIn(t);
		sleep();
	}

	void idle() {
		SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
		PM->SLEEP.reg = 2;
		__DSB();
		__WFI();
	}
	
	void idle(uint32_t t) {
		setAlarmIn(t);
		idle();
	}

	/*
	 * Return RTC
	 */
	RTCZero & getRTC() {
		return _rtc;
	}

	uint32_t getEpoch() {
		return _rtc.getEpoch();
	}

	void setEpoch(uint32_t epoch) {
		_rtc.setEpoch(epoch);
	}
};

/*
 * "singleton"
 */
LowPowerClock lowPowerClock;
