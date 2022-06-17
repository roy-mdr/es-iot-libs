# Config WiFi

Manages the configuration server and reconnection routines:

- Creates a webpage to connect the ESP8266 to a network (similar to * look for that Github repo * but before I knew it existed)
    - To access it:
        1. Start the ESP as Access Point
        1. Connect your smartphone to the SSID passed to `ESP_AP_STA()` with the password passed to the same function
        1. Make sure Mobile data is deactivated
        1. In your web browser type `192.168.0.1`
        1. Select the scanned network and enter the password
        1. Close the server (start the ESP as Station `ESP_STATION()`)
    - This website is able to work with Javascript deactivted, just the network scan will be manual (and be careful to reload the page with scan param in the URL)

- Creates a webpage to forget a saved network
    - To access it:
        1. Start the ESP as Access Point
        1. Connect your smartphone to the SSID passed to `ESP_AP_STA()` with the password passed to the same function
        1. Make sure Mobile data is deactivated
        1. In your web browser type `192.168.0.1/forget`
        1. Close the server (start the ESP as Station `ESP_STATION()`)

- Handles remembering networks in EEPROM
	- On reset will try to reconnect to last connected network SSID (if set)

- Handles a led blink through the process:
	- **Slow blink:** [ESP in Station mode] Trying to connect to last network SSID (if set) or waiting to connect to a network for the first time.
	- **Fast blink:** [ESP in Access Point mode] Server started to configure a network.
	- **Led On (No blink):** [ESP in Station mode] ESP is connected to a network.

## Includes

`<blink_async.h>`

`<EEPROM_handler.h>`

`<ESP8266WebServer.h>`

## API

### Create the web server to configure ESP

`setupWifiConfigServer(ESP8266WebServer &server, int EEPROM_ADDR_FOR_SSID, int EEPROM_ADDR_FOR_PASSWORD);`

### Handles the runtime interactions

`wifiConfigLoop(ESP8266WebServer &server);`

### Sets the ESP as Station and closes config server

`ESP_STATION(ESP8266WebServer &server);`

### Sets the ESP as Access Point + Station and opens config server

`ESP_AP_STA(ESP8266WebServer &server, char *ssid_AP, char *password_AP);`

## Usage

- In `setup()`: `setupWifiConfigServer()`
- In `loop()`: `wifiConfigLoop()`
- During execution can toggle between `ESP_STATION()` and `ESP_AP_STA()`

## TO DOs

- [ ] Make it able to connect to Enterprise networks (WPA2-ENTERPRISE)