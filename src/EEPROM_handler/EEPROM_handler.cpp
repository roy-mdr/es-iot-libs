#include "EEPROM_handler.h"

String EEPROM_READ(int addr) {
  String newString;
  int value;
  int valueSize = EEPROM.read(addr);
  for (int i = 0; i < valueSize; i++) {
    addr++;
    value = EEPROM.read(addr);
    if (value != 0) {
      newString += (char)value;
    }
  }
  return newString;
}

void EEPROM_ERASE(int addr) {
  int valueSize = EEPROM.read(addr);
  for (int i = 0; i < valueSize; i++) {
    EEPROM.write(i, -1);
  }
  EEPROM.commit();
}

void EEPROM_WRITE(int addr, String string) {
  int strSize = (string.length() + 1);
  char inchar[strSize - 1];
  string.toCharArray(inchar, strSize);
  EEPROM.write(addr, strSize);
  for (int i = 0; i < strSize; i++) {
    addr++;
    EEPROM.write(addr, inchar[i]);
  }
  EEPROM.commit();
}

bool EEPROM_CELL_IS_EMPTY(int addr) {
  return EEPROM.read(addr) == 255;
}

bool EEPROM_CLEAR() {
  for (int E = 0; E < EEPROM.length(); E++) {
    EEPROM.write(E, -1); // Itera por toda la EEPROM y reemplaza cualquier valor que esté en algún byte por cero (lo cambié a -1)
  }
  return EEPROM.commit();
}