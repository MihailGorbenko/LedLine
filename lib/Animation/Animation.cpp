#include "Animation.hpp"
#include <Preferences.h>

bool AnimationBase::saveToNVS(const char* key) {
	if (!key) {
		Serial.println("Animation: saveToNVS - null key");
		return false;
	}
	Serial.print("Animation: saveToNVS - begin for key: "); Serial.println(key);
	Preferences prefs;
	if (!prefs.begin("anim", false)) {
		Serial.println("Animation: saveToNVS - prefs.begin failed");
		return false;
	}
	String kh = String(key) + "_h";
	String ks = String(key) + "_s";
	String kv = String(key) + "_v";
	bool ok = true;
	ok &= prefs.putUShort(kh.c_str(), (uint16_t)hue);
	ok &= prefs.putUShort(ks.c_str(), (uint16_t)sat);
	ok &= prefs.putUShort(kv.c_str(), (uint16_t)val);
	prefs.end();
	Serial.print("Animation: saveToNVS - saved h="); Serial.print(hue);
	Serial.print(" s="); Serial.print(sat);
	Serial.print(" v="); Serial.println(val);
	Serial.print("Animation: saveToNVS - result: "); Serial.println(ok ? "OK" : "FAIL");
	return ok;
}

bool AnimationBase::loadFromNVS(const char* key) {
	if (!key) {
		Serial.println("Animation: loadFromNVS - null key");
		return false;
	}
	Serial.print("Animation: loadFromNVS - begin for key: "); Serial.println(key);
	Preferences prefs;
	if (!prefs.begin("anim", true)) {
		Serial.println("Animation: loadFromNVS - prefs.begin failed, applying default color");
		hue = defaultHue;
		sat = defaultSat;
		val = defaultVal;
		return false;
	}
	String kh = String(key) + "_h";
	String ks = String(key) + "_s";
	String kv = String(key) + "_v";
	uint16_t h = prefs.getUShort(kh.c_str(), (uint16_t)defaultHue);
	uint16_t s = prefs.getUShort(ks.c_str(), (uint16_t)defaultSat);
	uint16_t v = prefs.getUShort(kv.c_str(), (uint16_t)defaultVal);
	prefs.end();
	hue = (uint8_t)h;
	sat = (uint8_t)s;
	val = (uint8_t)v;
	Serial.print("Animation: loadFromNVS - loaded h="); Serial.print(hue);
	Serial.print(" s="); Serial.print(sat);
	Serial.print(" v="); Serial.println(val);
	return true;
}
