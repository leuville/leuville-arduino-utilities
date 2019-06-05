/*
 * Led clignotante
 */
#define LED_HIGH 	HIGH
#define LED_LOW		LOW

struct BlinkingLed {
	const uint8_t _pin;
	const unsigned long _interval;
	unsigned long _previousTmst = 0;
	int _state = LED_LOW;

	BlinkingLed(uint8_t pin = LED_BUILTIN, uint32_t inter = 250)
		: _pin(pin), _interval(inter*1000) {
	}

	void begin() {
		pinMode(_pin, OUTPUT);
	}

	void blink() {
		unsigned long currentTmst = micros();
		if (currentTmst - _previousTmst >= _interval) {
		    _previousTmst = currentTmst;
		    _state = (_state == LED_LOW ? LED_HIGH : LED_LOW);
		}
		digitalWrite(_pin, _state);
	}

	void off() {
		_state = LED_LOW;
		updateState();
	}

	void on() {
		_state = LED_HIGH;
		updateState();
	}

	void updateState() {
		digitalWrite(_pin, _state);
		_previousTmst = 0;
	}

};
