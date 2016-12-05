// This library controls the functions of ControLeo
//  - LCD display and backlight
//  - Buzzer
//  - Thermocouple

// Written by Peter Easton
// Released under WTFPL license
//
// Change History:
// 14 August 2014        Initial Version

#ifndef CONTROLEO2_H
#define CONTROLEO2_H

#include "ControLeo2_LiquidCrystal.h"

// Defines for the 2 buttons
#define CONTROLEO_BUTTON_PIN     A5  // Top button is on D11
#define CONTROLEO_BUTTON_NONE        0
#define CONTROLEO_BUTTON_TOP         1  // S1
#define CONTROLEO_BUTTON_BOTTOM      2  // S2

// The Buzzer is on D13
#define CONTROLEO_BUZZER_PIN         A4

#define MISO_PIN    12
#define CS_PIN      11
#define CLK_PIN     13

#define SERIAL_BUFFER_SIZE 8

#endif // CONTROLEO2_H
