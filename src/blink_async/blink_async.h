
/* ============================================================== USO ============================================================== */
/*                                                                                                                                   */
/* #include <blink_async.h>                                                                                                          */
/*                                                                                                                                   */
/* setup() {                                                                                                                         */
/*   pinMode(LED_BUILTIN, OUTPUT);                                                                                                   */
/*   setLedModeInverted(true);                                                                                                       */
/* }                                                                                                                                 */
/*                                                                                                                                   */
/* loop() {                                                                                                                          */
/*   handleLedBlink();                                                                                                               */
/* }                                                                                                                                 */
/*                                                                                                                                   */
/* funcionX() {                                                                                                                      */
/*                                                                                                                                   */
/*   setLedBlink(500); // Blink speed in ms                                                                                          */
/*   setLedBlink(500, true); // Cancel blink timer and force change (DON'T USE THIS INSIDE LOOP WHERE handleLedBlink() IS CALLED)    */
/*   setLedBlink(500, false, 1); // Don't cancel blink timer and Set initial state to 1                                              */
/*                                                                                                                                   */
/*   setLedOff();                                                                                                                    */
/*                                                                                                                                   */
/*   setLedOn();                                                                                                                     */
/*                                                                                                                                   */
/*   escapeLedBlink();                                                                                                               */
/*                                                                                                                                   */
/* }                                                                                                                                 */
/*                                                                                                                                   */
/* ================================================================================================================================= */




#ifndef BLINK_ASYNC_H
#define BLINK_ASYNC_H



#include <Arduino.h>





void setLedModeInverted(bool mode);

void handleLedBlink();

void setLedBlink(unsigned int speed);

void setLedBlink(unsigned int speed, bool reset);

void setLedBlink(unsigned int speed, bool reset, bool init);

void setLedOff();

void setLedOn();

void escapeLedBlink();





#endif