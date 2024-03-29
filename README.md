# es-iot-libs

Estudio Sustenta IoT projects custom libraries

## Install

### Arduino 1.x

1. Double click `copy_install-arduino_v1.x.bat` (don't run as Administrator)
1. Drag and drop `Arduino.exe` from the installation folder
1. Verify the code that is about to run
1. Press any key

- To uninstall run the same process with the file `copy_uninstall-arduino_v1.x.bat`

### Arduino 2.x

1. Run `copy_install-arduino_v2.x.bat`

- To uninstall run the same process with the file `copy_uninstall-arduino_v1.x.bat`

## Install with Symbolic Links

### Arduino 1.x

1. Double click `symlink_install-arduino_v1.x.bat` (don't run as Administrator)
1. Drag and drop `Arduino.exe` from the installation folder
1. Verify the code that is about to run
1. Press any key

- To uninstall run the same process with the file `symlink_uninstall-arduino_v1.x.bat`

### Arduino 2.x

1. Run `symlink_install-arduino_v2.x.bat`

- To uninstall run the same process with the file `symlink_uninstall-arduino_v1.x.bat`

## TO DOs

- [x] Make symlink installer / uninstaller for Arduino 2.x
- [x] Make copy installer / uninstaller for Arduino 1.x
- [x] Make copy installer / uninstaller for Arduino 2.x
- [ ] Make library called `nopoll` that handles all the connection with the notification server (like a MQTT client) and only exposes the function to evaluate each event from the server.
    - [ ] Allow HTTP
    - [ ] Allow HTTPs
    - [ ] Allow WebSockets
    - [ ] Allow raw TCP (maybe?)
