#include "config_wifi_roy.h"


ESP8266WebServer *configWifiServer;

int EEPROM_ADDR_EARLY_UNPLUG = 0;
int EEPROM_ADDR_CONNECTED_SSID = 0;
int EEPROM_ADDR_CONNECTED_PASSWORD = 0;

String tmpSSID = "";
String tmpPass = "";

char *ssid_AP; // Set your own Network Name (SSID)
char *password_AP; // Set your own password

IPAddress server_ip(192, 168, 0, 1);
IPAddress server_gateway(192, 168, 0, 1); // The gateway should be the same as the IP or the AP will be unstable!
IPAddress server_subnet(255, 255, 255, 0);

byte wifiConnected;

/***** SET RECONNECT TIMER *****/
unsigned long rec_timestamp = millis();
unsigned int  rec_track = 0;
unsigned int  rec_timeout = 1 * 60 * 1000; // 1 minute

/***** SET NETWORK SCAN TIMER *****/
unsigned long nwsc_timestamp = millis();
unsigned int  nwsc_timeout_default = 5 * 1000; // 5 seconds
unsigned int  nwsc_timeout = nwsc_timeout_default;
unsigned int  nwsc_track = nwsc_timeout; // execute function then start counting

/***** SCANNED NETWORKS *****/
String       nwsc_result_json  = "[]";
String       nwsc_result_html  = "";
String       nwsc_result_txt   = "";
unsigned int nwsc_result_total = 0;
unsigned int nwsc_result_filtd = 0;

/***** DNS CAPTIVE PORTAL *****/
const byte DNS_PORT = 53;
DNSServer dnsServer;





/////////////////////////////////////////////////
///////////////// CONFIG SERVER /////////////////

void setupWifiConfigServer(ESP8266WebServer &server, int EEPROM_ADDR_FOR_EUNPLUG, int EEPROM_ADDR_FOR_SSID, int EEPROM_ADDR_FOR_PASSWORD, char *ssid_for_AP, char *password_for_AP) {

	configWifiServer               = &server;
	EEPROM_ADDR_EARLY_UNPLUG       = EEPROM_ADDR_FOR_EUNPLUG;
	EEPROM_ADDR_CONNECTED_SSID     = EEPROM_ADDR_FOR_SSID;
	EEPROM_ADDR_CONNECTED_PASSWORD = EEPROM_ADDR_FOR_PASSWORD;
	ssid_AP                        = ssid_for_AP;
	password_AP                    = password_for_AP;

	// ----- COUNT EARLY UNPLUG ----- //

	if ( EEPROM_CELL_IS_EMPTY(EEPROM_ADDR_EARLY_UNPLUG) ) {
		EEPROM.write(EEPROM_ADDR_EARLY_UNPLUG, 0);
		EEPROM.commit();
	}

	unsigned int currVal = EEPROM.read(EEPROM_ADDR_EARLY_UNPLUG);
	unsigned int newVal = currVal + 1;

	if ( newVal > 3 ) {
		EEPROM_CLEAR();
		ESP.restart(); // tells the SDK to reboot, not as abrupt as ESP.reset()
	} else {
		EEPROM.write(EEPROM_ADDR_EARLY_UNPLUG, newVal );
		EEPROM.commit();
	}

	// ------------------------------ //

	WiFi.setAutoReconnect(false); // Establece si el módulo intentará volver a conectarse a un punto de acceso en caso de que esté desconectado.
	WiFi.setAutoConnect(false); // Configura el módulo para conectarse automáticamente tras encenderse al último punto de acceso utilizado.

	Serial.println("Starting DNS Captive Portal...");
	dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
	dnsServer.start(DNS_PORT, "*", server_ip); // if DNSServer is started with "*" for domain name, it will reply with provided IP to all DNS request



	configWifiServer->onNotFound([&]() {

		if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
			return;
		}

		String message = F("File Not Found\n\n");
		message += F("URI: ");
		message += server.uri();
		message += F("\nMethod: ");
		message += (server.method() == HTTP_GET) ? "GET" : "POST";
		message += F("\nArguments: ");
		message += server.args();
		message += F("\n");

		for (uint8_t i = 0; i < server.args(); i++) {
			message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
		}

		server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
		server.sendHeader("Pragma", "no-cache");
		server.sendHeader("Expires", "-1");
		server.send(404, "text/plain", message);
	});

	configWifiServer->on("/", [&]() {

		if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
			return;
		}

		configWifiServer->send(200, "text/html", String(WIFI_ROY_MAIN_1)
			+ ssid_AP
			+ WIFI_ROY_MAIN_2
			+ ssid_AP
			+ WIFI_ROY_MAIN_3
			+ nwsc_result_filtd + " Networks found..." // Before: nwsc_result_total
			+ WIFI_ROY_MAIN_4
			+ nwsc_result_html
			+ WIFI_ROY_MAIN_5
			+ (WiFi.getMode() == WIFI_STA ? "<a href=\"\/start_ap\">Start Access Point<\/a>" : "<a href=\"\/close_ap?stop=false\">Close Access Point<\/a>")
			+ WIFI_ROY_MAIN_6
		);
	});

	configWifiServer->on("/info", [&]() {

		configWifiServer->send(200, "application/json", String("{")
			+ ("\"alias\":\"") + ssid_AP + ("\",")
			+ ("\"type\":\"") + "es-iot-controller" + ("\",")
			+ ("\"firmware\":\"") + "es-firmware" + ("\",")
			+ ("\"mac\":\"") + WiFi.macAddress() + ("\",")
			+ ("\"ipv4_gateway\":\"") + WiFi.gatewayIP().toString() + ("\",")
			+ ("\"ipv4_interface\":\"") + WiFi.localIP().toString() + ("\",")
			+ ("\"ipv4_ap_gateway\":\"") + server_ip.toString() + ("\"")
			// board info?
			// memory size?
			// memory used?
			+ ("}")
		);
	});

	configWifiServer->on("/status/json", [&]() {
		configWifiServer->send(200, "application/json", String("{\"status\":\"") + getNetStatus() + "\"" + (WiFi.isConnected() ? ",\"info\":{\"ssid\":\"" + EEPROM_READ(EEPROM_ADDR_CONNECTED_SSID) + "\"}" : "") + ",\"networks\":" + nwsc_result_json + "}" );
	});

	configWifiServer->on("/status/html", [&]() {
		configWifiServer->send(200, "text/html", String(WIFI_ROY_STATUS_HTML_1)
			+ WIFI_ROY_NO_JS_CSS
			+ WIFI_ROY_STATUS_HTML_2
			+ getNetStatus()
			+ (WiFi.isConnected() ? (" to " + EEPROM_READ(EEPROM_ADDR_CONNECTED_SSID) ) : "" )
			+ WIFI_ROY_STATUS_HTML_3
		);
	});

	configWifiServer->on("/set_wifi", [&]() {
		configWifiServer->send(200, "text/html", String(WIFI_ROY_SET_WIFI_1)
			+ WIFI_ROY_NO_JS_CSS
			+ WIFI_ROY_SET_WIFI_2
		);

		if( !EEPROM_CELL_IS_EMPTY(EEPROM_ADDR_CONNECTED_SSID) ) {
			tmpSSID = EEPROM_READ(EEPROM_ADDR_CONNECTED_SSID);
			tmpPass = EEPROM_READ(EEPROM_ADDR_CONNECTED_PASSWORD);
		}

		// Receive GET values via URL query
		EEPROM_WRITE(EEPROM_ADDR_CONNECTED_SSID, configWifiServer->arg("ssid"));
		EEPROM_WRITE(EEPROM_ADDR_CONNECTED_PASSWORD, configWifiServer->arg("password"));

		if (configWifiServer->arg("ssid") == "") {
			Serial.println("Received empty SSID. Erasing.");
			EEPROM_ERASE(EEPROM_ADDR_CONNECTED_SSID);
		}

		if (configWifiServer->arg("ssid") == "") {
			Serial.println("Received empty PASSWORD. Erasing.");
			EEPROM_ERASE(EEPROM_ADDR_CONNECTED_PASSWORD);
		}

		connectWiFi();
	});

	configWifiServer->on("/forget", [&]() {

		configWifiServer->sendHeader("Location", String("/"), true); // Redirecto to home /
		configWifiServer->send ( 302, "text/plain", "");

		/* Disconnect from network and erase ssid from memory */
		Serial.println("Erasing SSID and password...");
		EEPROM_ERASE(EEPROM_ADDR_CONNECTED_SSID);
		EEPROM_ERASE(EEPROM_ADDR_CONNECTED_PASSWORD);

		Serial.println("Disconnecting...");
		WiFi.disconnect();
	});

	configWifiServer->on("/start_ap", [&]() {

		configWifiServer->sendHeader("Location", String("/"), true); // Redirecto to home /
		configWifiServer->send ( 302, "text/plain", "");

		ESP_AP_STA();
	});

	configWifiServer->on("/close_ap", [&]() {

		configWifiServer->sendHeader("Location", String("/"), true); // Redirecto to home /
		configWifiServer->send ( 302, "text/plain", "");

		if ( configWifiServer->arg("stop") == "true" ) {
			ESP_STATION(false);
		} else {
			ESP_STATION(true);
		}
	});

	configWifiServer->on("/restart", [&]() {

		configWifiServer->sendHeader("Location", String("/"), true); // Redirecto to home /
		configWifiServer->send ( 302, "text/plain", "");

		ESP.restart(); // tells the SDK to reboot, not as abrupt as ESP.reset()
	});


	if ( !EEPROM_CELL_IS_EMPTY(EEPROM_ADDR_CONNECTED_SSID) ) {
		Serial.println("Found SSID in memory... Trying to connect...");
		connectWiFi();
		ESP_STATION(true);
	} else {
		Serial.println("Didn't found SSID in memory... Starting WIFI Access Point...");
		ESP_AP_STA();
	}
}





/////////////////////////////////////////////////
///////////////////// LOOP /////////////////////

void wifiConfigLoop() {

	dnsServer.processNextRequest();
	
	configWifiServer->handleClient();

	// ----- AFTER X SECONDS IS NOT AN EARLY UNPLUG ----- //
	static unsigned long eup_timestamp = millis();
	static unsigned int  eup_times = 0;
	static unsigned int  eup_timeout = 5000; // 5 seconds
	static unsigned int  eup_track = 0; // to_track = to_timeout; TO: execute function and then start counting

	if ( eup_times < 1 ) {

		if ( millis() != eup_timestamp ) { // "!=" intead of ">" tries to void possible bug when millis goes back to 0
			eup_track++;
			eup_timestamp = millis();
		}

		if ( eup_track > eup_timeout ) {
			// DO TIMEOUT!
			eup_times++;
			eup_track = 0;

			// RUN THIS FUNCTION!
			EEPROM.write(EEPROM_ADDR_EARLY_UNPLUG, 0);
			EEPROM.commit();
		}
	}
	// -------------------------------------------------- //

	// LED CONTROLL
	handleLedBlink();

	if ( WiFi.getMode() == WIFI_AP_STA ) {
		setLedBlink(100);
	} else {
		if ( WiFi.isConnected() ) {
			setLedOn();
		} else {
			setLedBlink(1000);
		}
	}

	// NETWORK SCAN CONTROLL (ASYNC)
	if ( WiFi.getMode() == WIFI_AP_STA ) {
		getNetworksAsync(true);
	} else {
		getNetworksAsync(false);
	}




	if (WiFi.status() != WL_CONNECTED) {

		if ( millis() != rec_timestamp ) { // "!=" intead of ">" tries to void possible bug when millis goes back to 0
			rec_track++;
			rec_timestamp = millis();
		}
		
		if ( rec_track > rec_timeout ) {
			// DO TIMEOUT!
			rec_track = 0;

			Serial.println("Reconnecting...");

			// try to reconnect to the previously connected access point:
			WiFi.reconnect();

			// Or, you can call WiFi.disconnect() followed by WiFi.begin(ssid,password).
			/*WiFi.disconnect();
			if ( !EEPROM_CELL_IS_EMPTY(EEPROM_ADDR_CONNECTED_SSID) ) {
				String r_ssid = EEPROM_READ(EEPROM_ADDR_CONNECTED_SSID);
				String r_pass = EEPROM_READ(EEPROM_ADDR_CONNECTED_PASSWORD);
				WiFi.begin(r_ssid, r_pass);
			}*/
		}

	}

	if ( wifiStatChanged(wifiConnected) ) {

		Serial.print("WiFi status changed to: ");
		Serial.print( WiFi.status() );
		Serial.print(" | ");
		Serial.println( getNetStatus() );

		if ( WiFi.isConnected() ) {
			// Se conectó al WIFI
			Serial.println();
			WiFi.printDiag(Serial);
			Serial.println();
			Serial.print("Default Gateway IP: ");
			Serial.print(WiFi.gatewayIP());
			Serial.println(" (esta IP apunta al -modem- al que estás conectado)");
			Serial.print("STA dirección IP: ");
			Serial.print(WiFi.localIP());
			Serial.println(" (esta IP apunta al WeMos cuando estas conectado al modem en red local LAN)");
			Serial.print("STA MAC Address: ");
			Serial.println(WiFi.macAddress());
		}
	}

}



/////////////////////////////////////////////////
/////////// DETECT WIFI STATUS CHANGE ///////////

bool wifiStatChanged(byte &compareVar) {
	byte gotStat = WiFi.status();
	if(gotStat != compareVar){
		compareVar = gotStat;
		return true;
	} else {
		return false;
	}
}





///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// CONNECTION ///////////////////////////////////////////

/////////////////////////////////////////////////
/////////////// WiFi FROM MEMORY ///////////////

void connectWiFi() {
	//int timer = 10; // Seconds
	//int blinkRate = 10; // Bigger = faster

	String r_ssid = EEPROM_READ(EEPROM_ADDR_CONNECTED_SSID);
	String r_pass = EEPROM_READ(EEPROM_ADDR_CONNECTED_PASSWORD);
	Serial.print("SSID: ");
	Serial.println(r_ssid);
	Serial.print("Password: ");
	Serial.println(r_pass);

	if (r_ssid[0] == EEPROM_ADDR_CONNECTED_SSID) { // Is this the best way?
		Serial.println("No SSID. Can't connect.");
		return;
	}

	Serial.println("Connecting...");

	nwsc_timeout = nwsc_timeout_default + 20000; // 20 segundos aprox. de respiro al network scan

	if (r_pass[0] == EEPROM_ADDR_CONNECTED_PASSWORD) { // Is this the best way?
		Serial.println("Connecting without password");
		WiFi.begin(r_ssid);
		return;
	}

	WiFi.begin(r_ssid, r_pass);

	//timer = timer * blinkRate;
	//while (timer > 0) {
	//	yield(); // To don't stop the "EventLoop" (keep running background tasks) ***SHOULD BE INSIDE A LOOP (do, while, for...)? (or loop() ??) ***

	//	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN) );
	//	Serial.print(".");

		//if (WiFi.status() == WL_CONNECTED) {
			//timer = 0;

			

			/* LED ON */
		//} else {
		//	if (timer == 1) {
		//
		//		/* LED OFF */
		//
		//		if (tmpSSID != "") {
		//			EEPROM_WRITE(EEPROM_ADDR_CONNECTED_SSID, tmpSSID);
		//			EEPROM_WRITE(EEPROM_ADDR_CONNECTED_PASSWORD, tmpPass);
		//			tmpSSID = "";
		//			tmpPass = "";
		//		}
		//
		//		Serial.print("WiFi no conectado. Status: ");
		//		Serial.println( getNetStatus() );
		//	}
		//
		//}

		//delay(1000 / blinkRate);
		//timer--;
	//}
}

/////////////////////////////////////////////////
////////////// SCAN NETWORKS ASYNC //////////////

void getNetworksAsync(bool shouldScan) {

	if (!shouldScan) {
		nwsc_track = nwsc_timeout;
		return;
	}


	if ( millis() != nwsc_timestamp ) { // "!=" intead of ">" tries to void possible bug when millis goes back to 0
		nwsc_track++;
		nwsc_timestamp = millis();
	}
	
	if ( nwsc_track > nwsc_timeout ) {
		// DO TIMEOUT!
		nwsc_timeout = nwsc_timeout_default;
		nwsc_track = 0;

		/* RUN FUNCTION (trigger Wi-Fi network scan) */
		WiFi.scanNetworks(true, true);
		Serial.println("Scan start ... ");
	}


	/* print out Wi-Fi network scan result upon completion */

	char *txt_buf;
	char *json_buf;
	char *html_buf;
	size_t txt_sz;
	size_t json_sz;
	size_t html_sz;

	int n = WiFi.scanComplete();

	if(n >= 0) { // -1 = Scanning still in progress, -2 = Scanning has not been triggered

		// CREATE BUFFERS TO KEEP ONLY ONE SSID WITH THE STRONGEST SIGNAL

		String       arrSSID[n];
		int32_t      arrEncryp[n];
		int32_t      arrRSSI[n];
		unsigned int arrIndex[n];

		Serial.print("Original net array: [");

		for (unsigned int i = 0; i < n; i++) { // for each network found

			Serial.print("(");
			Serial.print(WiFi.SSID(i));
			Serial.print(":");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");

			for (unsigned int x = 0; x < n; ++x) { // for each network saved

				bool netDup = false;

				if ( (WiFi.SSID(i) == arrSSID[x]) && (WiFi.encryptionType(i) == arrEncryp[x]) ) {
					// network already added
					netDup = true;
					if ( WiFi.RSSI(i) > arrRSSI[x] ) {
						arrRSSI[x]  = WiFi.RSSI(i);
						arrIndex[x] = i;
					}
					break;
				}

				if ( x == n-1 ) { // in the last loop
					if (!netDup) {
						arrSSID[i]   = WiFi.SSID(i);
						arrRSSI[i]   = WiFi.RSSI(i);
						arrEncryp[i] = WiFi.encryptionType(i);
						arrIndex[i]  = i;
					}
				}
			}
		}

		Serial.println("]");

		nwsc_result_json  = "[";
		nwsc_result_html  = "";
		nwsc_result_txt   = "";
		nwsc_result_total = n;
		nwsc_result_filtd = 0;

		Serial.print("Filtered net array: [");

		for (unsigned int s = 0; s < n; s++) { // for each network saved

			Serial.print("(");
			if (arrSSID[s] == "") {
				Serial.print("=EMPTY=");
			} else {
				Serial.print(arrSSID[s]);
				Serial.print(":");
				Serial.print(arrRSSI[s]);
			}
			Serial.print(")");

			if (arrSSID[s] == "") {
				continue;
			}

			nwsc_result_filtd++;

			unsigned int i = arrIndex[s];
			char *nwEnc = "";

			/* Encryption modes */
			switch ( WiFi.encryptionType(i) ) {
				case ENC_TYPE_WEP : 
					nwEnc = "ENC_TYPE_WEP"; // WEP
					break;
				case ENC_TYPE_TKIP : 
					nwEnc = "ENC_TYPE_TKIP"; // WPA / PSK
					break;
				case ENC_TYPE_CCMP : 
					nwEnc = "ENC_TYPE_CCMP"; // WPA2 / PSK
					break;
				case ENC_TYPE_NONE : 
					nwEnc = "ENC_TYPE_NONE"; // open network
					break;
				case ENC_TYPE_AUTO : 
					nwEnc = "ENC_TYPE_AUTO"; // WPA / WPA2 / PSK
					break;

				default:
					nwEnc = "???"; // ?
					break;
			}

			// text
			nwsc_result_txt += (i + 1);
			nwsc_result_txt += ": ";
			nwsc_result_txt += WiFi.SSID(i).c_str();
			nwsc_result_txt += "[";
			nwsc_result_txt += WiFi.BSSIDstr(i); // WiFi.BSSID(i);
			nwsc_result_txt += "], Ch:";
			nwsc_result_txt += WiFi.channel(i);
			nwsc_result_txt += ", ";
			nwsc_result_txt += WiFi.RSSI(i);
			nwsc_result_txt += "dBm, Enc[";
			nwsc_result_txt += WiFi.encryptionType(i);
			nwsc_result_txt += "]: ";
			nwsc_result_txt += nwEnc;
			nwsc_result_txt += WiFi.isHidden(i) ? " [hidden]" : "";
			nwsc_result_txt += "\n";

//			txt_sz = snprintf(NULL, 0, "%d: %s [%d], Ch:%d, %ddBm, Enc[%d]: %s%s%s", i + 1, WiFi.SSID(i).c_str(), WiFi.BSSID(i), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i), nwEnc, WiFi.isHidden(i) ? " [hidden]" : "", i == n-1 ? "" : "\n");
//			txt_buf = (char *)malloc(txt_sz + 1); /* make sure you check for != NULL in real code */
//crash			snprintf(txt_buf, txt_sz+1, "%d: %s [%d], Ch:%d, %ddBm, Enc[%d]: %s%s%s", i + 1, WiFi.SSID(i).c_str(), WiFi.BSSID(i), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i), nwEnc, WiFi.isHidden(i) ? " [hidden]" : "", i == n-1 ? "" : "\n");
//crash			nwsc_result_txt += txt_buf;

			// json
			nwsc_result_json += "{\"ssid\":\"";
			nwsc_result_json += WiFi.SSID(i).c_str();
			nwsc_result_json += "\",\"bssid\":\"";
			nwsc_result_json += WiFi.BSSIDstr(i); // WiFi.BSSID(i);
			nwsc_result_json += "\",\"channel\":";
			nwsc_result_json += WiFi.channel(i);
			nwsc_result_json += ",\"rssi\":";
			nwsc_result_json += WiFi.RSSI(i);
			nwsc_result_json += ",\"enc_type\":";
			nwsc_result_json += WiFi.encryptionType(i);
			nwsc_result_json += ",\"enc_str\":\"";
			nwsc_result_json += nwEnc;
			nwsc_result_json += "\",\"hidden\":";
			nwsc_result_json += WiFi.isHidden(i) ? "true" : "false";
			nwsc_result_json += "}";
			nwsc_result_json += ",";

//			json_sz = snprintf(NULL, 0, "{\"ssid\":\"%s\",\"bssid\":%d,\"channel\":%d,\"rssi\":%d,\"enc_type\":%d,\"enc_str\":\"%s\",\"hidden\":%s}%s", WiFi.SSID(i).c_str(), WiFi.BSSID(i), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i), nwEnc, WiFi.isHidden(i) ? "true" : "false", i == n-1 ? "" : ",");
//			json_buf = (char *)malloc(json_sz + 1); /* make sure you check for != NULL in real code */
//crash			snprintf(json_buf, json_sz+1, "{\"ssid\":\"%s\",\"bssid\":%d,\"channel\":%d,\"rssi\":%d,\"enc_type\":%d,\"enc_str\":\"%s\",\"hidden\":%s}%s", WiFi.SSID(i).c_str(), WiFi.BSSID(i), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i), nwEnc, WiFi.isHidden(i) ? "true" : "false", i == n-1 ? "" : ",");
//crash			nwsc_result_json += json_buf;

			// html
			nwsc_result_html += "<option value=\"";
			nwsc_result_html += WiFi.SSID(i).c_str();
			nwsc_result_html += "\">";
			nwsc_result_html += WiFi.SSID(i).c_str();
			nwsc_result_html += nwEnc == "ENC_TYPE_NONE" ? "" : " 🔒";
			nwsc_result_html += " [";
			nwsc_result_html += WiFi.RSSI(i);
			nwsc_result_html += "]</option>";

//			html_sz = snprintf(NULL, 0, "<option value=\"%s\">%s [%d]</option>", WiFi.SSID(i).c_str(), WiFi.SSID(i).c_str(), WiFi.RSSI(i));
//			html_buf = (char *)malloc(html_sz + 1); /* make sure you check for != NULL in real code */
//crash			snprintf(html_buf, html_sz+1, "<option value=\"%s\">%s [%d]</option>", WiFi.SSID(i).c_str(), WiFi.SSID(i).c_str(), WiFi.RSSI(i));
//crash			nwsc_result_html += html_buf;

		}

		Serial.println("]");


		if (nwsc_result_filtd > 0) {
			nwsc_result_json[nwsc_result_json.length()-1] = ' '; // Get rid of the last "," in the array
		}

		nwsc_result_json += "]";

		// Serial output
		Serial.printf("%d network(s) found\n", nwsc_result_filtd);
		Serial.println(nwsc_result_txt);
		// Serial.println(nwsc_result_json);
		// Serial.println(nwsc_result_html);
	}

	WiFi.scanDelete();
	
	
}

/////////////////////////////////////////////////
////////////// GET NETWORK STATUS //////////////

String getNetStatus() {

	switch( WiFi.status() ) {
		case WL_CONNECTED:
			return "Connected";

		case WL_NO_SHIELD:
			return "No WiFi shield present";

		case WL_IDLE_STATUS:
			return "Connecting...";

		case WL_NO_SSID_AVAIL:
			return "SSID unreachable";

		case WL_SCAN_COMPLETED:
			return "Scan completed";

		case WL_CONNECT_FAILED:
			return "Wrong password";

		case WL_CONNECTION_LOST:
			return "Connection lost";

		case WL_DISCONNECTED:
			return "Disconnected";

		// case WL_CONNECT_WRONG_PASSWORD:
			// return "Wrong password";

		default:
			return "Getting status...";
	}

}





///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// WiFi MODES ///////////////////////////////////////////

/////////////////////////////////////////////////
///////////////// STATION MODE /////////////////

void ESP_STATION(bool keepServerOpenInLAN) {

	if ( WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA ) {
		Serial.println("Closing server...");
	}

	Serial.print("Initializing as Station... ");

	if (keepServerOpenInLAN) {
		Serial.println("Keep server listening on LAN");
		configWifiServer->begin();
	} else {
		Serial.println("Don't keep server listening on LAN");
		configWifiServer->stop(); ///// DEBUGGING... ???
		configWifiServer->close(); ///// DEBUGGING... ???
	}

	WiFi.mode(WIFI_STA); ///// DEBUGGING... uncomment if using one of the above

	WiFi.setHostname(ssid_AP); // (works in esp8266) Establece nombre de host (se muestra en tabla DHCP en router) // Do after changing WiFi mode
	WiFi.hostname(ssid_AP);    // (works in esp8266) Establece nombre de host (se muestra en tabla DHCP en router) // Do after changing WiFi mode

	// WiFi.softAPdisconnect(true) ? Serial.println("Server closed.") : Serial.println("Fail closing server."); ///// DEBUGGING... ('true' to remove the AP mode ??? BUT SETS MODE TO NULL ???)

}

/////////////////////////////////////////////////
////////// STATION + ACCESS POINT MODE //////////

void ESP_AP_STA() {

	WiFi.mode(WIFI_AP_STA);

	WiFi.setHostname(ssid_AP); // (works in esp8266) Establece nombre de host (se muestra en tabla DHCP en router) // Do after changing WiFi mode
	WiFi.hostname(ssid_AP);    // (works in esp8266) Establece nombre de host (se muestra en tabla DHCP en router) // Do after changing WiFi mode

	Serial.println("Starting server...");
	
	WiFi.softAPConfig(server_ip, server_gateway, server_subnet) || Serial.println("Error configuring Access Point."); // On ESP32 you have to wait until event SYSTEM_EVENT_AP_START has fired, before you can set its configuration. // es necesario llamar a WiFi.softAPConfig() antes de WiFi.softAP()
	WiFi.softAP(ssid_AP, password_AP) || Serial.println("Error setting up Access Point.");

	Serial.print("AP dirección IP: ");
	Serial.print(WiFi.softAPIP());
	Serial.println(" (esta IP apunta al WeMos cuando te conectas a su propio WiFi)");
	Serial.print("AP MAC Address: ");
	Serial.println(WiFi.softAPmacAddress());

	configWifiServer->begin();
	Serial.print("Server started. Access Point SSID: ");
	Serial.println(ssid_AP);
	// configWifiServer->setNoDelay(true);
}

/////////////////////////////////////////////////
////////////////// TOGGLE MODE //////////////////

void ESP_AP_TOGGLE(bool keepServerOpenInLAN) {

	Serial.println("Toggling Access Point...");
	//Serial.println(WiFi.getMode());
	switch (WiFi.getMode()) {
		case WIFI_OFF: // case 0:
			//Serial.println("WiFi off");
			break;
		case WIFI_STA: // case 1:
			//Serial.println("Station (STA)");
			ESP_AP_STA();
			break;
		case WIFI_AP: // case 2:
			//Serial.println("Access Point (AP)");
			break;
		case WIFI_AP_STA: // case 3:
			//Serial.println("Station and Access Point (STA+AP)");
			ESP_STATION(keepServerOpenInLAN);
			break;
		default:
			Serial.print("WiFi mode NPI xD: ");
			Serial.println(WiFi.getMode());
			break;
	}
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
bool captivePortal() {
  if (
  	  !isIp(configWifiServer->hostHeader())
  	  // && server.hostHeader() != (String(myHostname) + ".local") // Only for mDNS
  	) {
    Serial.println("Request redirected to captive portal");
    configWifiServer->sendHeader("Location", String("http://") + toStringIp(configWifiServer->client().localIP()), true);
    configWifiServer->send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    configWifiServer->client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

/** Is this an IP? */
bool isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
