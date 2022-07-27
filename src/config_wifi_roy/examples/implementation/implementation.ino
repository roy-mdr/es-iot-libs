#include <config_wifi_roy.h>





ESP8266WebServer server(80);

#define EEPROM_ADDR_CONNECTED_SSID 1       // Start saving connected network SSID from this memory address
#define EEPROM_ADDR_CONNECTED_PASSWORD 30  // Start saving connected network Password from this memory address
#define AP_SSID "ESP8266_AP"               // Set your own Network Name (SSID)
#define AP_PASSWORD "12345678"             // Set your own password



String pin5value = "0";





///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// HELPERS /////////////////////////////////////////////

/////////////////////////////////////////////////
//////////////// DETECT CHANGES ////////////////

bool changed(String gotStat, String &compareVar) {
  if(gotStat != compareVar){
    compareVar = gotStat;
    return true;
  } else {
    compareVar = gotStat;
    return false;
  }
}





///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// SETUP //////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  EEPROM.begin(4096);
  Serial.print("...STARTING...");
  Serial.println();

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(5, INPUT);     // Initialize the DIGITAL 5 pin as an input

  setupWifiConfigServer(server, EEPROM_ADDR_CONNECTED_SSID, EEPROM_ADDR_CONNECTED_PASSWORD);

  Serial.println("Starting server anyway xD ...");
  ESP_AP_STA(server, AP_SSID, AP_PASSWORD);

}





///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// LOOP //////////////////////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:
  
  wifiConfigLoop(server);
  
  if (WiFi.status() == WL_CONNECTED) {
    // Do while WiFi is connected
    //Serial.println("WiFi connected");
  } else {
    // Do while WiFi is NOT connected
    //Serial.println("disconnected");
  }

  if ( changed((String)digitalRead(5), pin5value) ) {
    Serial.print("cambio pin 5 a: ");
    Serial.println(pin5value);
    pin5value == "1" ? ESP_AP_STA(server, AP_SSID, AP_PASSWORD) : ESP_STATION(server, true);
  }
  
}
