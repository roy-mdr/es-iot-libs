
/* ============================================================================ USO ============================================================================ */
/*                                                                                                                                                               */
/* #include <NoPollSubscriber.h>                                                                                                                                 */
/*                                                                                                                                                               */
/* #define SUB_HOST "notify.estudiosustenta.myds.me"                                                                                                             */
/* #define SUB_PORT 80                                                                                                                                           */
/* #define SUB_PATH "/"                                                                                                                                          */
/*                                                                                                                                                               */
/* WiFiClient sub_WiFiclient;                                                                                                                                    */
/*                                                                                                                                                               */
/* loop() {                                                                                                                                                      */
/* 	// This function will hog the whole loop() function. So move your loop code to doInLoopFunction()...                                                         */
/* 	handleNoPollSubscription(sub_WiFiclient, SUB_HOST, SUB_PORT, SUB_PATH, subConfigJSON, userAgent, doInLoopFunction, onConnectedFunction, onParsedFunction);   */
/* }                                                                                                                                                             */
/*                                                                                                                                                               */
/* ============================================================================================================================================================= */




#ifndef NO_POLL_SUBSCRIBER_H
#define NO_POLL_SUBSCRIBER_H



#include <Arduino.h>
#include <ESP8266WiFi.h>





void handleNoPollSubscription(WiFiClient &client, String subHost, int subPort, String subPath, String method, String subConfigJSON, String userAgent, void (*NPS_doInLoop)(void), void (*NPS_onConnected)(void), void (*NPS_onParsed)(String));





#endif