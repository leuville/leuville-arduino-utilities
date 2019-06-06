# leuville-arduino-utilities
Various programming utilities

 - ISRWrapper.h: object-oriented ISR wrapper
 
 Example:

     class Device: private ISRWrapper<A3> {
        public:
        	using ISRWrapper<A3>::ISRWrapper; // inherits constructor
        	
        	virtual void ISR_callback(uint8_t pin) {
        		// do something compatible with ISR spirit
        	}
     };
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTEzMTMzNjA3NDNdfQ==
-->