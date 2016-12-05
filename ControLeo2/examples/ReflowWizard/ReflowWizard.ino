/*******************************************************************************
* ControLeo Reflow Oven Controller
* Author: Peter Easton
* Website: whizoo.com
* Software written to work best on ControLeo2 (www.whizoo.com)
*
* This is an example of a reflow oven controller. The reflow curve below is for a
* lead-free profile.  The Reflow Wizard is capable of self-calibrating a reflow oven
* to the J-STD-20 reflow specification.
*
* Temperature (Degree Celcius)                 Magic Happens Here!
* 245-|                                               x  x  
*     |                                            x        x
*     |                                         x              x
*     |                                      x                    x
* 200-|                                   x                          x
*     |                              x    |                          |   x   
*     |                         x         |                          |       x
*     |                    x              |                          |
* 150-|               x                   |                          |
*     |             x |                   |                          |
*     |           x   |                   |                          | 
*     |         x     |                   |                          | 
*     |       x       |                   |                          | 
*     |     x         |                   |                          |
*     |   x           |                   |                          |
* 30 -| x             |                   |                          |
*     |<  60 - 90 s  >|<    60 - 120 s   >|<       60 - 150 s       >|
*     | Preheat Stage |   Soaking Stage   |       Reflow Stage       | Cool
*  0  |_ _ _ _ _ _ _ _|_ _ _ _ _ _ _ _ _ _|_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 
*                                                                Time (Seconds)
*
* This firmware builds on the work of other talented individuals:
* ==========================================
* Rocketscream (www.rocketscream.com)
* Produced the Arduino reflow oven shield and code that inspired this project.
*
* ==========================================
* Limor Fried of Adafruit (www.adafruit.com)
* Author of Arduino MAX6675 library. Adafruit has been the source of tonnes of
* tutorials, examples, and libraries for everyone to learn.
*
* Disclaimer
* ==========
* Dealing with high voltage is a very dangerous act! Please make sure you know
* what you are dealing with and have proper knowledge before hand. Your use of 
* any information or materials on this reflow oven controller is entirely at 
* your own risk, for which we shall not be liable. 
*
* Released under WTFPL license.
*
* Revision  Description
* ========  ===========
* 1.0       Initial public release. (21 October 2014)
* 1.1       Bug fixes (30 December 2014)
*           - Oven temperature might not reach configured maximum temperature
*           - Adjusted values so learning happens faster
*           - Other minor improvements
* 1.2       Improvements (6 January 2014)
*           - Take temperature readings every 0.125s and average them.  Fixes errors
*             generated by noise from convection fans
*           - Refined learning mode so learning should happen in fewer runs
*           - Improved loop duration timer to enhance timing
*           - Moved some strings from RAM to Flash (there is only 2.5Kb of RAM)
* 1.3       No user-facing changes (19 January 2015)
*           - Fixed compiler warnings for Arduino 1.5.8
* 1.4       Added support for servos (6 February 2015)
*           - When using a servo, please connect a large capacitor (e.g. 220uF, 16V)
*             between +5V and GND to avoid a microcontroller reset when the servo
*             stall current causes the voltage to drop.  Small servos may not need
*             a capacitor
*           - Connect the servo to +5V, GND and D3
*           - The open and close positions are configured in the Settings menu
*           - The oven door will open when the reflow is done, and close at 50C.
*             The door is also closed when ControLeo2 is turned on.
* 1.5       Minor improvements (15 July 2015)
*           - Made code easier to read by using “F” macro for strings stored in flash
*           - Minor adjustments to reflow values 
*           - Restrict the maximum duty cycle for the boost element to 60%.  It
*             should never need more than this!
*           - Make thermocouple more tolerant of transient errors, including
*             FAULT_OPEN
* 1.6       Added ability to bake (29 March 2016)
*           - Ability to bake was requested by a number of users, and was implemented
*             by Bernhard Kraft and Mark Foster.  Bernhard's implementation was added
*             to the code base.  This is a simple algorithm that only implements the
*             "P" (proportional) of the PID algorithm.  Temperatures may overshoot
*             initially, but for the rest of the baking time (up to 18 hours) the
*             temperature will be a few degrees below the target temperature.
*           - Added option to Setup menu to restart learning mode.
*           - Added option to Setup menu to reset to factory mode (erase everything)
* 1.7       Added support for cooling fan (7 June 2106)
*           - Any of the 4 outputs can be configured to control a cooling fan
*           - When bake finishes, the servo will open the oven door (thanks jcwren)
* 1.8       Major changes to bake functionality (22 June 2016)
*           - Complete rewrite of bake functionality.  Users reported that ControLeo2
*             would sometimes freeze during bake, leaving one or more elements on.
*           - Added Integral (the "I" of PID) to bake, so temperatures come closer
*             to desired bake temperature
*           - Added bake countdown timer to LCD screen
*           - Fixed bug where oven door would not close once bake completed.
* 1.9       Minor change to bake (25 June 2016)
*           - Initial bake duty cycle is based on desired bake temperature
*******************************************************************************/


// ***** INCLUDES *****
#include "ControLeo2.h"
#include "ReflowWizard.h"
#include <TimerOne.h>

// ***** TYPE DEFINITIONS *****

ControLeo2_LiquidCrystal lcd = {};

int mode = 0;

void setup() {
  // *********** Start of ControLeo2 initialization ***********
  // Set up the buzzer and buttons
  pinMode(CONTROLEO_BUZZER_PIN, OUTPUT);
  // Set the relays as outputs and turn them off
  // The relay outputs are on D5 to D8 (4 outputs)
  for (int i=5; i<=8; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  // Set up the LCD's number of rows and columns 
  lcd.begin(16, 2);
  // Create the degree symbol for the LCD - you can display this with lcd.print("\1") or lcd.write(1)
  unsigned char degree[8]  = {12,18,18,12,0,0,0,0};
  lcd.createChar(1, degree);
  // *********** End of ControLeo2 initialization ***********
  
  // Log data to the computer using USB
  Serial.begin(57600);

  // Setup Timer used to take Thermocouple readings
  Timer1.initialize(200000);
  Timer1.attachInterrupt(takeCurrentThermocoupleReading);

  // Write the initial message on the LCD screen
  lcdPrintLine(0, "   ControLeo2");
  lcdPrintLine(1, "Reflow Oven v1.9");
  delay(1000);  
  
  // Initialize the EEPROM, after flashing bootloader
  InitializeSettingsIfNeccessary();
  lcd.clear();

  Serial.println(F("ControLeo2 Reflow Oven controller v1.9"));
}


// The main menu has 4 options
boolean (*action[NO_OF_MODES])() = {Testing, Config, Reflow, Bake};
const char testing[] PROGMEM = "Test Outputs?";
const char config[] PROGMEM = "Setup?";
const char reflow[] PROGMEM = "Start Reflow?";
const char baking[] PROGMEM = "Start Baking?";
const char* const modes[NO_OF_MODES] PROGMEM = {testing, config, reflow, baking};
char modeBuffer[14];

// This loop is executed 20 times per second
void loop()
{
  static boolean drawMenu = true;
  static boolean showMainMenu = true;
  static int counter = 0;
  static unsigned long nextLoopTime = 50; // Should be 3000 + 100 + fudge factor + 50 - but no harm making it 50!
  
  if (showMainMenu) {
    if (drawMenu) {
      drawMenu = false;
      strcpy_P(modeBuffer, (char*)pgm_read_word(&(modes[mode])));
      lcdPrintLine(0, modeBuffer);
      lcdPrintLine(1, "          Yes ->");
    }
    
    // Update the temperature roughtly once per second
    if (counter++ % 20 == 0)
      displayTemperature(getCurrentTemperature());
    
    // Get the button press to select the mode or move to the next mode
    switch (getButton()) {
    case CONTROLEO_BUTTON_TOP:
      // Move to the next mode
      mode = (mode + 1) % NO_OF_MODES;
      drawMenu = true;
      break;
    case CONTROLEO_BUTTON_BOTTOM:
      // Move to the selected mode
      showMainMenu = false;
      drawMenu = true;
      break;
    }
  }
  else {
    // Go to the mode's menu system
    if ((*action[mode])() == NEXT_MODE)
      showMainMenu = true;
  }
  
  // Execute this loop 20 times per second (every 50ms). 
  if (millis() < nextLoopTime)
    delay(nextLoopTime - millis());
  nextLoopTime += 50;
}


// Determine if a button was pressed (with debounce)
// A button can only be pressed once every 200ms. If a button is
// pressed and held, a button press will be generated every 200ms.
// Returns:
//   CONTROLEO_BUTTON_NONE if no button are pressed
//   CONTROLEO_BUTTON_TOP if the top button was pressed
//   CONTROLEO_BUTTON_BOTTOM if the bottom button was pressed
// Note: If both buttons are pressed simultaneously, CONTROLEO_BUTTON_TOP will be returned
#define DEBOUNCE_INTERVAL  200

int getButton()
{
  static long lastChangeMillis = 0;
  long nowMillis = millis();
  int buttonValue;
  int val = analogRead(CONTROLEO_BUTTON_PIN);
  
  // If insufficient time has passed, just return none pressed
  if (lastChangeMillis + DEBOUNCE_INTERVAL > nowMillis)
    return CONTROLEO_BUTTON_NONE;
  
  // Read the current button status
  buttonValue = CONTROLEO_BUTTON_NONE;
  if (val < 195) {
    buttonValue = CONTROLEO_BUTTON_TOP;
    playTones(50,0,1);
  }
  else if (val < 380) {
    buttonValue = CONTROLEO_BUTTON_BOTTOM;
    playTones(50,0,1);
  }
    
  // Note the time the button was pressed
  if (buttonValue != CONTROLEO_BUTTON_NONE)
   lastChangeMillis = nowMillis;
  
  return buttonValue;
}


// Display a line on the LCD screen
// The provided string is padded to take up the whole line
// There is less flicker when overwriting characters on the screen, compared
// to clearing the screen and writing new information
void lcdPrintLine(int line, const char* str) {
  char buffer[17] = "                ";
  // Sanity check on the parameters
  if (line < 0 || line > 1 || !str || strlen(str) > 16)
    return;
  lcd.setCursor(0, line);
  strncpy(buffer, str, strlen(str));
  lcd.print(buffer);
}


// Displays the temperature in the bottom left corner of the LCD display
void displayTemperature(float temperature) {
  lcd.setCursor(0, 1);
  if (temperature == NAN) {
    lcd.print("        ");
    return;
  }
  lcd.print(temperature);
  // Print degree Celsius symbol
  lcd.print(F("\1C "));  
}

