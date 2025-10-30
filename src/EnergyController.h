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

#undef min
#undef max
#include <initializer_list>
#include <functional>
#include <Range.h>

/*
 * VMIN : min voltage in millivolts
 * VMAX : max voltage in millivolts
 */
template <uint16_t VMIN = 3200, uint16_t VMAX = 4200>
class EnergyController {

protected:

	std::function<double(void)> _getVoltage = []() -> double { return VMAX; };

	/*
	 * Function to get current voltage (millivolts) from board 
	 *
	 * returns by default the maximum voltage == 100% battery power
	 * may be overriden by application class (defined as private subclass) 
	 */
	virtual std::function<double(void)> defineGetVoltage() {
		return []() -> double { return VMAX; };
	}

public:

	static constexpr Range<uint8_t> _range100 {0, 100};

	EnergyController() = default;

	EnergyController(std::function<double(void)> getVoltage) : _getVoltage(getVoltage) {}

	/*
	 * Defines the function used to get board voltage (see defineGetVoltage())
	 */
	virtual void begin() {
		_getVoltage = defineGetVoltage();
	}

	/*
	 * Return the  battery level between min and max
	 */
	template <typename T>
	T getBatteryPower(T min = 0, T max = 100) {
		return scaleValue(
			static_cast<uint16_t>(round(_getVoltage())), 
			VMIN, 
			VMAX, 
			static_cast<T>(min), 
			static_cast<T>(max)
		);
 	}

	template <typename T = uint8_t>
	RangedValue<T> getBatteryPower(const Range<T> & range = _range100) {
		return RangedValue<T>{getBatteryPower(range.min, range.max), range};
 	}

	template <typename T>
	bool isBatteryPowerLessThan(RangedValue<T> & value, float percent) {
		return isBelowPercent(value, percent);
	}

	bool isBatteryPowerLessThan(uint8_t power, float percent, uint8_t min, uint8_t max) {
		return isBelowPercent(power, percent, min, max);
	}

	/* 
	 * pullup unused pins to save energy and avoid unwanted interrupts
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
