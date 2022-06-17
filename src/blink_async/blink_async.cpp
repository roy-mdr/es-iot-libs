#include "blink_async.h"


/** SET TIMEOUT **/
unsigned long blynk_led_timestamp = millis();
unsigned int  blynk_led_track = 0;
unsigned int  blynk_led_timeout = 0; // Blink speed in ms
bool          blynk_led_inverted = false; // Blink speed in ms





void setLedModeInverted(bool mode) {
	blynk_led_inverted = mode;
}

void handleLedBlink() {
	if (blynk_led_timeout > 0) {
		// Should toggle led
		if ( millis() != blynk_led_timestamp ) {
			blynk_led_track++;
			blynk_led_timestamp = millis();
		}

		if ( blynk_led_track > blynk_led_timeout ) {
			// DO TIMEOUT!
			blynk_led_track = 0;
			digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN) ); // TOGGLE LED
		}
	}
}

void setLedBlink(unsigned int speed) {
	blynk_led_timeout = speed;
}

void setLedBlink(unsigned int speed, bool reset) {

	if (reset) {
		escapeLedBlink();
	}

	blynk_led_timeout = speed;
}

void setLedBlink(unsigned int speed, bool reset, bool init) {
	setLedBlink(speed);
	digitalWrite(LED_BUILTIN, init); // START LED BLINKING ON OFF/ON
}

void setLedOff() {
	setLedBlink(0, 1);

	if (blynk_led_inverted) {
		digitalWrite(LED_BUILTIN, HIGH);
	} else {
		digitalWrite(LED_BUILTIN, LOW);
	}
}

void setLedOn() {
	setLedBlink(0, 1);

	if (blynk_led_inverted) {
		digitalWrite(LED_BUILTIN, LOW);
	} else {
		digitalWrite(LED_BUILTIN, HIGH);
	}
}

void escapeLedBlink() {
	blynk_led_track = 0;
}