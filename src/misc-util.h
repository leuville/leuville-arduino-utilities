/*
 * Module: misc-util
 *
 * Function: various utilities
 *
 * Copyright and license: See accompanying LICENSE file
 *
 * Author: Laurent Nel
 */

#pragma once

#include <Arduino.h>

/*
 * Returns the capacity in terms of number of elements of a C array
 */
template <typename T>
size_t arrayCapacity(const T & tab) {
  return sizeof tab / sizeof tab[0];
}

/*
 * Utilitaires de gestion du TEMPS
 */
#define UINT32_SECONDS(h,m,s) (uint32_t)(UINT16_MINUTES(h,m)*60+s)
#define UINT16_MINUTES(h,m) (uint16_t)(60*h+m)

inline uint32_t timeAsSeconds(uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0) {
	return hour*3600 + minute*60 + second;
}

/*
 * Encodage d'un tableau de N entiers de type uint8_t en String
 * La String contient la valeur en HEXA
 */
inline String convertUint8ArrayToString(const uint8_t *data, const uint8_t numBytes) {
	String result;
	for (uint8_t i=0; i < numBytes; i++) {
	    if (data[i] < 0x10) {
			result += "0";
		}
      	result += String(data[i], HEX);
	}
    return(result);
}

/*
 * Encodage d'un tableau de N entiers de type uint8_t en uint64_t
 */
inline uint64_t convertUint8ArrayToUint64(const uint8_t *data, const uint8_t numBytes) {
	uint64_t res = (((uint64_t)data[numBytes-1]) << 0);
	for (uint8_t i = 1; i < numBytes; i++) {
		res = res | (((uint64_t)data[numBytes-(i+1)]) << (i*8));
	}
	return res;
}

/*
 * Convert MAC address to uint64_t
 */
/*
inline uint64_t convertMacAddressToUint64(String mac) {
	const uint8_t siz = 6;
	uint8_t data[siz];
	char buf[64];
	strcpy(buf, (char*)mac.c_str());
	char *ptr = &(buf[0]);

	char * tok;
	int i = 0;

	while ((tok = strtok_r(ptr, ":", &ptr)) != nullptr) {
		data[i++] = strtoul(tok, nullptr, 16);
	}

	return convertUint8ArrayToUint64(data, siz);
}
*/

/*
 * Switch OFF unused pins
 *
 */
/*
inline void switchOffPins(std::initializer_list<uint8_t> list) {
	for(uint8_t pin: list) {
		pinMode(pin, INPUT_PULLUP);
		digitalWrite(pin, HIGH);
	}
}
*/

/*
 * Wait for a given amount of ms
 * NOT USE delay() or millis()
 */
inline void loopFor(uint32_t delay) {
	unsigned long endtime = micros() + delay * 1000;
	while (micros() < endtime) {}
}

inline byte nibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;  // Not a valid hexadecimal character
}

inline void hexCharacterStringToBytes(const String & hexString, byte *byteArray) {
  bool oddLength = hexString.length() & 1;

  byte currentByte = 0;
  byte byteIndex = 0;

  for (byte charIndex = 0; charIndex < hexString.length(); charIndex++)
  {
    bool oddCharIndex = charIndex & 1;

    if (oddLength)
    {
      // If the length is odd
      if (oddCharIndex)
      {
        // odd characters go in high nibble
        currentByte = nibble(hexString[charIndex]) << 4;
      }
      else
      {
        // Even characters go into low nibble
        currentByte |= nibble(hexString[charIndex]);
        byteArray[byteIndex++] = currentByte;
        currentByte = 0;
      }
    }
    else
    {
      // If the length is even
      if (!oddCharIndex)
      {
        // Odd characters go into the high nibble
        currentByte = nibble(hexString[charIndex]) << 4;
      }
      else
      {
        // Odd characters go into low nibble
        currentByte |= nibble(hexString[charIndex]);
        byteArray[byteIndex++] = currentByte;
        currentByte = 0;
      }
    }
  }
}

inline String loraString(const char* hexString) {
	String res;
	size_t n = strlen(hexString);
	for (int8_t i = n-2; i >= 0; i -= 2) {
		res.concat(hexString[i]);
		res.concat(hexString[i+1]);
	}
	return res;
}

/*
 * USB print
 */
template <typename STYPE>
struct USBPrinter {
	STYPE &_serial;

	USBPrinter(STYPE &serial): _serial(serial) {};

	void begin(uint32_t baud) {
		_serial.begin(baud);
	}

	int available() {
		return _serial.available();
	}

	template <typename T>
	void print(const T data) {
		_serial.print(data);
	}

	template <typename T>
	void print(const T data, const int base) {
  		_serial.print(data, base);
	}

	template <typename T>
	void println(const T data) {
		_serial.println(data);
	}

	template <typename T>
	void println(const T data, const int base) {
  		_serial.println(data, base);
	}

	template <typename T>
	void print(const char* msg, const T data) {
		_serial.print(msg);
		_serial.print(data);
	}

	template <typename T>
	void print(const char* msg, const T data, const int base) {
  		_serial.print(msg);
  		_serial.print(data, base);
	}

	template <typename T>
	void println(const char* msg, const T data) {
		_serial.print(msg);
		_serial.println(data);
	}

	template <typename T>
	void println(const char* msg, const T data, const int base) {
  		_serial.print(msg);
  		_serial.println(data, base);
	}
	
	/*
	 * Prints a hexadecimal value in plain characters
	 */
	void printHex(const uint8_t* data, const uint32_t numBytes) {
		for (uint8_t i = 0; i < numBytes; i++) {
			if (data[i] < 0x10) {
				_serial.print("0");
			}
			_serial.print(data[i], HEX);
		}
		_serial.println("");
	}

};

/*
 * String generic concatenation
 */
template <typename T>
void concat(String & str, T data) {
	str.concat(data);
}

template <typename T, typename... Args>
void concat(String & str, T data, Args... args) {
	concat(str, data);
	concat(str, args...);
}