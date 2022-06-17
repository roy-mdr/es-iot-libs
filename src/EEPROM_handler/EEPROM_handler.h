#ifndef EEPROM_HANDLER_H
#define EEPROM_HANDLER_H



#include <Arduino.h>
#include <EEPROM.h>





String EEPROM_READ(int addr);

void EEPROM_ERASE(int addr);

void EEPROM_WRITE(int addr, String string);

bool EEPROM_CELL_IS_EMPTY(int addr);





#endif