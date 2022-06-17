# LED_BUILTIN Blink Asynchronous

Controll the `LED_BUILTIN` blinking without blocking the program

## Includes

`<Arduino.h>`

## API

### Inverts the output HIGH - LOW

`setLedModeInverted(bool mode);`

Set to `true` if no external LED is connected to the `LED_BUILTIN` pin

### Set blink speed in *milliseconds*

`setLedBlink(unsigned int speed);`

### Immediatly start new blink speed (reset = true)

`setLedBlink(unsigned int speed, bool reset);`

### Immediatly start new blink speed (reset = true) and start HIGH or LOW

`setLedBlink(unsigned int speed, bool reset, bool init);`

### Set LED off

`setLedOff();`

### Set LED on

`setLedOn();`

### Escape blink

`escapeLedBlink();`

## Usage

```c++

#include <blink_async.h>

setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  setLedModeInverted(true);
}

loop() {
  handleLedBlink();
}

funcionX() {

  setLedBlink(500); // Blink speed in ms
  setLedBlink(500, true); // Cancel blink timer and force change (DON'T USE THIS INSIDE LOOP WHERE handleLedBlink() IS CALLED)
  setLedBlink(500, false, 1); // Don't cancel blink timer and Set initial state to 1

  setLedOff();

  setLedOn(); 

  escapeLedBlink();

}

```

## TO DOs

