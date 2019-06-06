# leuville-arduino-utilities
Various programming utilities for Arduino devices.

## Contents

 - ISRWrapper.h:
	 - ISRWrapper<>: object-oriented ISR wrapper
	 - ISRTimer : base class for timer based on RTCZero
 - energy.h
	 - StandbyMode: base class to provide standby mode
 
## Example 1: ISRWrapper<>
 This code builds a new class with a button connected on pin A3. Each time the button is pressed, the virtual function ISR_callback is called. The pin number is a template parameter.
 
     class Device: private ISRWrapper<A3> {
        public:
        	using ISRWrapper<A3>::ISRWrapper; // inherits constructor
        	
        	virtual void ISR_callback(uint8_t pin) override {
        		// do something compatible with ISR spirit
        	}
     };
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE0MjIzMTk1MTAsMTc2MDkzMTkzM119
-->