# leuville-arduino-utilities
Various programming utilities

 - ISRWrapper.h:
	 - ISRWrapper: object-oriented ISR wrapper
	 - ISRTimer : base class for timer based on RTCZero
 - energy.h
	 - StandbyMode: base class to provide standby mode
 
## Example
 This code 
 
     class Device: private ISRWrapper<A3> {
        public:
        	using ISRWrapper<A3>::ISRWrapper; // inherits constructor
        	
        	virtual void ISR_callback(uint8_t pin) {
        		// do something compatible with ISR spirit
        	}
     };
<!--stackedit_data:
eyJoaXN0b3J5IjpbNDA5NjM5MzM3LDE3NjA5MzE5MzNdfQ==
-->