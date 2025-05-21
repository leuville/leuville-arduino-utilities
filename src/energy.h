/*
 * Module: energy
 *
 * Function: computes battery level
 *
 * Copyright and license: See accompanying LICENSE file.
 *
 * Author: Laurent Nel
 */

#pragma once

#include <initializer_list>

/*
 * PIN: battery pin
 * DIV : power divider
 * VMIN : min voltage in millivolts
 * VMAX : max voltage in millivolts
 */
template <uint16_t PIN = 0, uint8_t DIV = 0, uint16_t VMIN = 3200, uint16_t VMAX = 4200>
class EnergyController {

	const float	_vMin = VMIN / 1000;
	const float	_vMax = VMAX / 1000;

public:

	/*
	 * Return the voltage in V
	 * 
	 * pin depends on board type used
	 * feather_m0 = (A7)
	 * 
	 * https://forum.sparkfun.com/viewtopic.php?f=117&t=49246&p=221795&hilit=battery+level#p221795
	 * https://www.omzlo.com/articles/your-arduino-samd21-adc-is-wrong-did-you-notice
	 */
	constexpr float getVoltage() {
		return PIN == 0 ? _vMax : ((float)analogRead(PIN) * DIV * 3.3f) / 1023.0f;
	}
	/*
	 * Return the  battery level between 0 and 100%
	 */
	uint8_t getBatteryPower(uint8_t scale = 100) {
		auto voltage = getVoltage();
		if (voltage >= _vMax) 
			return scale;
		else 
			if (voltage <= _vMin) 
				return 0;
			else 
				return (uint8_t)roundf(((voltage - _vMin) / (_vMax - _vMin)) * scale);
	}

	/* 
	 * pullup unused pins to save energy
	 */
	void setUnusedPins(std::initializer_list<uint8_t> unusedPins) {
		for(auto pin: unusedPins) {
			setUnusedPin(pin);
		}
	}

	void setUnusedPin(uint8_t unusedPin) {
		pinMode(unusedPin, OUTPUT);
		digitalWrite(unusedPin, LOW);
	}

};
