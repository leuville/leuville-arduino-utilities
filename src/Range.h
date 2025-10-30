/*
 * Module: Range
 *
 * Function: various utilities to handle numeric values between min & max
 *
 * Copyright and license: See accompanying LICENSE file
 *
 * Author: Laurent Nel
 */

#pragma once

#include <Arduino.h>

template<typename T>
struct Range {
    T min;
    T max;
};

template<typename T>
struct RangedValue {
    T 			value;
    Range<T> 	range;
	operator T() const {return value; };
};

/*
 * generic scaling
 */
template<typename T, typename U>
U scaleValue(T input, T inMin, T inMax, U outMin, U outMax) {
	if (input < inMin) input = inMin;
	if (input > inMax) input = inMax;
	return static_cast<U>(
		outMin + (static_cast<double>(input - inMin) * (outMax - outMin)) / (inMax - inMin)
	);
}

template<typename T, typename U>
U scaleValue(const RangedValue<T> & input, const Range<U> & outputRange) {
	return scaleValue(input.value, input.range.min, input.range.max, outputRange.min, outputRange.max);
}

template <typename T>
bool isBelowPercent(T value, float percent, T min, T max) {
  	T threshold = min + (max - min) * (percent / 100.0f);
  	return value < threshold;
}

template <typename T>
bool isBelowPercent(const RangedValue<T> & rangedValue, float percent) {
	return isBelowPercent(rangedValue.value, percent, rangedValue.range.min, rangedValue.range.max);
}


