/*
 * Module: energy
 *
 * Function: classes for Object-Oriented approach of ISR
 *
 */
#pragma once

#include <RTCZero.h>
#include <ArduinoSTL.h>

/*
 * Energy saving
 */
void pullupPins(std::initializer_list<uint8_t> unusedPins) {
	for (uint8_t pin: unusedPins) {
		pinMode(pin, INPUT_PULLUP);
	}
}

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
 * 32kHz clock initialization for syandby modes
 * Must be called after a first attachInterrupt()
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
 * Enable an ext interrupt the way to wakeup the EIC in standbymode
 */
void activateIntForStandbyMode(std::initializer_list<uint32_t> extIntList) {
	for (uint32_t eint: extIntList) {
		EIC->WAKEUP.vec.WAKEUPEN |= (1<<eint);
	}
}

constexpr uint16_t VMAX = 420;	// 4.20 V
constexpr uint16_t VMIN = 360;	// 3.60 V
constexpr uint16_t BATTPIN = A7;
constexpr uint16_t POWDIVIDER = 2;

//
// voltage obtenu sur A7(D9)
// retourne un pourcentage de charge restante
//
uint8_t getRemainingPower() {
	int vcurrent = (int)((analogRead(BATTPIN)*POWDIVIDER*3.3*100)/1024);
	if (vcurrent > VMAX)
		vcurrent = VMAX;
	else if (vcurrent < VMIN)
		vcurrent = VMIN;
	return (100 * (vcurrent - VMIN))/(VMAX - VMIN);
}

/*
 * Provides standbymode based on RTCZero
 */
class StandbyMode {

	RTCZero & _rtc;

public:

	StandbyMode(RTCZero & rtc): _rtc(rtc) {
	}

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
