# No-Poll subscriber

> This library is a client clibrary of https://github.com/roy-mdr/es-web-notify and should be updated in sync with it.

Manages the client logic of the notification server.

## Includes

`<Arduino.h>`

`<ESP8266WiFi.h>`

## API

### Configures the host and start listening

`handleNoPollSubscription(WiFiClient &client, String subHost, int subPort, String subPath, String subConfigJSON, String userAgent, void (*NPS_doInLoop)(void), void (*NPS_onConnected)(void), void (*NPS_onParsed)(String));`

> NOTE: This function will hog the whole loop() function. So move your loop code to doInLoopFunction()...

- **NPS_doInLoop** - Triggered on each iteration of the while() function
- **NPS_onConnected** - Triggered on succesful connection to the notification server
- **NPS_onParsed** - Trigggered on data received. Will pass the raw JSON string

## Usage

- make function: `void doInLoop() {}` This function will contain all the original logic inside the loop.
- make function: `void onConnected() {}` Maybe grab some state!
- make function: `void onParsed(String) {}` Will receive a raw JSON string. So parsing should be done here.

- In `loop()`: `handleNoPollSubscription(..., doInLoop, onConnected, onParsed);`

## TO DOs

- [ ] Implement the [actual logic](https://github.com/roy-mdr/es-web-notify/blob/main/client/CLIENT.LOGIC.md) (with memory and counting bytes). At the moment it just skips the message size part... So errors may be expected if the payload is too large.