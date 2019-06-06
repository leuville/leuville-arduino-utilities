# leuville-arduino-utilities
Various programming utilities

 - ISRWrapper.h:
	 - ISRWrapper: object-oriented ISR wrapper
	 - ISRTimer : object-oriented timer based on RCTZero
 - energy.h
	 - StandbyMode: object-oriented class to provide 
 
 Example:

     class Device: private ISRWrapper<A3> {
        public:
        	using ISRWrapper<A3>::ISRWrapper; // inherits constructor
        	
        	virtual void ISR_callback(uint8_t pin) {
        		// do something compatible with ISR spirit
        	}
     };
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTc2MDkzMTkzM119
-->