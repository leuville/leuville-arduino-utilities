#pragma once

#include <Arduino.h>
#include <ArduinoSTL.h>
#include <vector>

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
inline uint64_t convertMacAddressToUint64(String mac) {
	const uint8_t siz = 6;
	uint8_t data[siz];
	char buf[64];
	strcpy(buf, (char*)mac.c_str());
	char *ptr = &(buf[0]);

	char * tok;
	int i = 0;

	while (tok = strtok_r(ptr, ":", &ptr)) {
		data[i++] = strtoul(tok, nullptr, 16);
	}

	return convertUint8ArrayToUint64(data, siz);
}

/*
 * Convert a string containing mac address to a vector of uint8_t
 */
inline std::vector<uint8_t> convertMacAddressToUint8Array(String mac) {
	std::vector<uint8_t> res;
	res.reserve(6);
	char buf[64];
	strcpy(buf, (char*)mac.c_str());
	char *ptr = &(buf[0]);

	char * tok;
	int i = 0;

	while (tok = strtok_r(ptr, ":", &ptr)) {
		res.push_back((uint8_t)strtoul(tok, nullptr, 16));
	}
	return res;
}

/*
 * Convert a std::string containing HEX to uint8_t array
 */
/*
void convertHexStringToIntArray(const std::string& hex, uint8_t * tab) {
	for (unsigned int i = 0; i < hex.length(); i += 2) {
		std::string byteString = hex.substr(i, 2);
		uint8_t val = (uint8_t) strtol(byteString.c_str(), NULL, 16);
		tab[i] = val;
	}
}
*/
/*
 * Switch OFF unused pins
 *
 */
inline void switchOffPins(std::initializer_list<uint8_t> list) {
	for(uint8_t pin: list) {
		pinMode(pin, INPUT_PULLUP);
		digitalWrite(pin, HIGH);
	}
}

/*
 * Wait for a given amount of ms
 * NOT USE delay() or millis()
 */
inline void loopFor(uint32_t delay) {
	unsigned long endtime = micros() + delay * 1000;
	while (micros() < endtime) {}
}
