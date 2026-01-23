#include "Animation.hpp"
#include <Preferences.h>

// 0 = no Serial spam (recommended for production)
#ifndef ANIM_NVS_DEBUG
#define ANIM_NVS_DEBUG 0
#endif

static void makeKey(char* out, size_t outSize, const char* key, const char* suffix) {
	if (!out || outSize == 0) return;
	if (!key) {
		out[0] = '\0';
		return;
	}
	snprintf(out, outSize, "%s%s", key, suffix);
	out[outSize - 1] = '\0';
}

bool AnimationBase::saveToNVS(const char* key) {
	if (!key) {
#if ANIM_NVS_DEBUG
		Serial.println("Animation: saveToNVS - null key");
#endif
		return false;
	}

	Preferences prefs;
	if (!prefs.begin("anim", false)) {
#if ANIM_NVS_DEBUG
		Serial.println("Animation: saveToNVS - prefs.begin failed");
#endif
		return false;
	}

	char kh[32], ks[32], kv[32];
	makeKey(kh, sizeof(kh), key, "_h");
	makeKey(ks, sizeof(ks), key, "_s");
	makeKey(kv, sizeof(kv), key, "_v");

	bool ok = true;
	ok &= (prefs.putUChar(kh, hue) != 0);
	ok &= (prefs.putUChar(ks, sat) != 0);
	ok &= (prefs.putUChar(kv, val) != 0);
	prefs.end();

#if ANIM_NVS_DEBUG
	Serial.print("Animation: saveToNVS ");
	Serial.print(key);
	Serial.print(" h="); Serial.print(hue);
	Serial.print(" s="); Serial.print(sat);
	Serial.print(" v="); Serial.println(val);
#endif
	return ok;
}

bool AnimationBase::loadFromNVS(const char* key) {
	if (!key) {
#if ANIM_NVS_DEBUG
		Serial.println("Animation: loadFromNVS - null key");
#endif
		return false;
	}

	Preferences prefs;
	if (!prefs.begin("anim", true)) {
		// apply defaults through setter so subclasses see them
		setColorHSV(defaultHue, defaultSat, defaultVal);
		return false;
	}

	char kh[32], ks[32], kv[32];
	makeKey(kh, sizeof(kh), key, "_h");
	makeKey(ks, sizeof(ks), key, "_s");
	makeKey(kv, sizeof(kv), key, "_v");

	uint8_t h = prefs.getUChar(kh, defaultHue);
	uint8_t s = prefs.getUChar(ks, defaultSat);
	uint8_t v = prefs.getUChar(kv, defaultVal);
	prefs.end();

	setColorHSV(h, s, v);

#if ANIM_NVS_DEBUG
	Serial.print("Animation: loadFromNVS ");
	Serial.print(key);
	Serial.print(" h="); Serial.print(h);
	Serial.print(" s="); Serial.print(s);
	Serial.print(" v="); Serial.println(v);
#endif
	return true;
}
