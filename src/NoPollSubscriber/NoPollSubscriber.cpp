#include "NoPollSubscriber.h"


/***** SET CONNECTION TIMEOUT TIMER *****/
unsigned long NPS_to_timestamp = millis();
unsigned int  NPS_to_track = 0;
unsigned int  NPS_to_times = 0;
unsigned int  NPS_to_timeout = 60 * 1000; // 60 seconds





///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// LETS TAKE THIS TO IT'S OWN LIBRARY! ///////////////////////////////

void handleNoPollSubscription(WiFiClient &client, String subHost, int subPort, String subPath, String method, String subConfigJSON, String userAgent, void (*NPS_doInLoop)(void), void (*NPS_onConnected)(void), void (*NPS_onParsed)(String)) {

  /***** RUN LOGIC IN LOOP *****/
  NPS_doInLoop();
  /***** ----------------- *****/
  //Serial.println("loop_doInLoop");

  if (WiFi.status() == WL_CONNECTED) {
    // Do while WiFi is connected
    
    Serial.printf("\n[Connecting to %s ... ", subHost);
    // Intentamos conectarnos
    if (client.connect(subHost, subPort)) {
      Serial.println("connected]");

      /*** DO ON-CONNECTED CALLBACK ***/
      NPS_onConnected();

      String dataPOST = subConfigJSON;
      Serial.println("[Sending POST request]");
      
      ///// ---------- BEGIN CONNECTION
      client.print(method);
      client.print(" ");
      client.print(subPath);
      client.println(" HTTP/1.1");
      
      client.print("Host: ");
      client.println(subHost);
      
      client.print("User-Agent: ");
      client.println(userAgent);
      
      client.println("Accept: */*");
      
      client.println("Content-Type: application/json");

      client.print("Content-Length: ");
      client.println(dataPOST.length());
      client.println();
      client.print(dataPOST);

      Serial.println("[Response:]");

      bool headersDone = false;
      bool readStr = false;

      // Mientras la conexion perdure
      while (client.connected()) {

        if ( millis() != NPS_to_timestamp ) { // "!=" intead of ">" tries to void possible bug when millis goes back to 0
          NPS_to_track++;
          NPS_to_timestamp = millis();
        }

        if ( NPS_to_track > NPS_to_timeout ) {
          // DO TIMEOUT!
          NPS_to_times++;
          NPS_to_track = 0;
          Serial.print("[timeout #");
          Serial.print(NPS_to_times);
          Serial.println("]");
          client.stop();
          
          if (NPS_to_times > 2) {
            
            NPS_to_times = 0;
            
            if (WiFi.getMode() != WIFI_AP_STA) {
              Serial.println("3 timeouts: restarting...");
              // ESP.restart(); // tells the SDK to reboot, not as abrupt as ESP.reset()
              WiFi.reconnect(); // just restart the wifi connection... why reboot the whole thing?
            }
            
          }
        }

        /***** SAME FUNCTIONS IN LOOP *****/
//yield(); // = delay(0);
//Serial.println("loop_client_connected");
        NPS_doInLoop();
        /***** because this while loop hogs the loop *****/

        // Si existen datos disponibles
        if (client.available()) {
          
//Serial.println("loop_client_available");

          NPS_to_track = 0;
          //NPS_to_times = 0;

          String line = client.readStringUntil('\n');
          Serial.println(line);
          
          if ( (line.length() == 1) && (line[0] == '\r') ) {
            if (headersDone) {
              client.stop();
            } else {
              Serial.println("---[HEADERS END]---");
              headersDone = true;
              readStr = false;
            }
          } else {
            if (headersDone) {
              if (readStr) {
                ///// START LOGIC
                Serial.println("^^^^^");

                /***** PLEASE IMPLEMENT THE ACTUAL LOGIC HERE XD *****/
                int quotes = 0;
                while (quotes < 4) {
                	if (line[0] == '\'') {
                		quotes++;
                	}
                	line.remove(0, 1);
                }
                line.remove(line.length() - 1, 1);
                /*****************************************************/

                if (line.length() > 0) {
                  /*** DO ON-PARSED CALLBACK ***/
                  NPS_onParsed(line);
                }
                
              }
              readStr = !readStr;
            }


          }
        }
      }
      ///// ---------- END
      
      // Una vez el servidor envia todos los datos requeridos se desconecta y el programa continua
      Serial.println("\n[Disconnecting...]");
      client.stop();
      Serial.println("\n[Disconnected]");
    } else {
      Serial.println("connection failed!]");
      client.stop();

      //***** IS THIS TOO ABRUPT? 🤔🤔🤔
      if (WiFi.getMode() != WIFI_AP_STA) {
        Serial.println("Can't connect: restarting...");
        // ESP.restart(); // tells the SDK to reboot, not as abrupt as ESP.reset()
        WiFi.reconnect(); // just restart the wifi connection... why reboot the whole thing?
      }
      //*****
    }

  } // else {
    // Do while WiFi is NOT connected
    //Serial.println("disconnected");
  // }
}
