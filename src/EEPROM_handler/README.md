# EEPROM handler

Handles basic functions of the EEPROM memory.

## Includes

`<Arduino.h>`

`<EEPROM.h>`

## API

### Read memory address

`EEPROM_READ(int addr);`

returns `String` class with the content

### Erase content from memory address

`EEPROM_ERASE(int addr);`

does not return (void)

### Write String to memory address

`EEPROM_WRITE(int addr, String string);`

does not return (void)

### Checks if memory address is empty

`EEPROM_CELL_IS_EMPTY(int addr);`

returns `true` if is empty (set to 255)

returns `false` if is not empty (other than 255)

### Clear all the EEPROM data

`EEPROM_CLEAR();`

does not return (void)

## TO DOs