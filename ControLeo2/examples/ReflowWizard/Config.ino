// Setup menu
// Called from the main loop
// Allows the user to configure the outputs and maximum temperature

// Called when in setup mode
// Return false to exit this mode
boolean Config() {
  static int setupPhase = 0;
  static int output = 5;    // Start with output D4
  static int type = TYPE_TOP_ELEMENT;
  static boolean drawMenu = true;
  static int maxTemperature;
  static int servoDegrees;
  static int servoDegreesIncrement = 5;
  static int selectedServo = SETTING_SERVO_OPEN_DEGREES;
  static int bakeTemperature;
  static int bakeDuration;
  int oldSetupPhase = setupPhase;
  
  switch (setupPhase) {
    case 0:  // Set up the output types
      if (drawMenu) {
        drawMenu = false;
        lcdPrintLine(0, "Dx is");
        lcd.setCursor(1, 0);
        lcd.print(output);
        type = getSetting(SETTING_D5_TYPE - 5 + output);
        strcpy_P(descBuffer, (char*)pgm_read_word(&(outputDescription[type])));
        lcdPrintLine(1, descBuffer);
      }
  
      // Was a button pressed?
      switch (getButton()) {
        case CONTROLEO_BUTTON_TOP:
          // Move to the next type
          type = (type+1) % NO_OF_TYPES;
          strcpy_P(descBuffer, (char*)pgm_read_word(&(outputDescription[type])));
          lcdPrintLine(1, descBuffer);
          break;
        case CONTROLEO_BUTTON_BOTTOM:
          // Save the type for this output
          setSetting(SETTING_D5_TYPE - 5 + output, type);
          // Go to the next output
          output++;
          if (output != 9) {
            lcd.setCursor(1, 0);
            lcd.print(output);
            type = getSetting(SETTING_D5_TYPE - 5 + output);
            strcpy_P(descBuffer, (char*)pgm_read_word(&(outputDescription[type])));
            lcdPrintLine(1, descBuffer);
            break;
          }
          
          // Go to the next phase.  Reset variables used in this phase
          setupPhase++;
          output = 5;
          break;
      }
      break;
      
    case 1:  // Get the maximum temperature
      if (drawMenu) {
        drawMenu = false;
        lcdPrintLine(0, "Max temperature");
        lcdPrintLine(1, "xxx\1C");
        maxTemperature = getSetting(SETTING_MAX_TEMPERATURE);
        displayMaxTemperature(maxTemperature);
      }
      
      // Was a button pressed?
      switch (getButton()) {
        case CONTROLEO_BUTTON_TOP:
          // Increase the temperature
          maxTemperature++;
          if (maxTemperature > 280)
            maxTemperature = 175;
          displayMaxTemperature(maxTemperature);
          break;
        case CONTROLEO_BUTTON_BOTTOM:
          // Save the temperature
          setSetting(SETTING_MAX_TEMPERATURE, maxTemperature);
          // Go to the next phase
          setupPhase++;
      }
      break;
    
    case 2:     
      setupPhase++;
      break;

    case 3:  // Get bake temperature
      if (drawMenu) {
        drawMenu = false;
        lcdPrintLine(0, "Bake temperature");
        lcdPrintLine(1, "");
        bakeTemperature = getSetting(SETTING_BAKE_TEMPERATURE);
        lcd.setCursor(0, 1);
        lcd.print(bakeTemperature);
        lcd.print("\1C ");
      }

      // Was a button pressed?
      switch (getButton()) {
        case CONTROLEO_BUTTON_TOP:
          // Increase the temperature
          bakeTemperature += BAKE_TEMPERATURE_STEP;
          if (bakeTemperature > BAKE_MAX_TEMPERATURE)
            bakeTemperature = BAKE_MIN_TEMPERATURE;
          lcd.setCursor(0, 1);
          lcd.print(bakeTemperature);
          lcd.print("\1C ");
          break;
        case CONTROLEO_BUTTON_BOTTOM:
          // Save the temperature
          setSetting(SETTING_BAKE_TEMPERATURE, bakeTemperature);
          // Go to the next phase
          setupPhase++;
      }
      break;

    case 4:  // Get bake duration
      if (drawMenu) {
        drawMenu = false;
        lcdPrintLine(0, "Bake duration");
        lcdPrintLine(1, "");
        bakeDuration = getSetting(SETTING_BAKE_DURATION);
        displayDuration(0, getBakeSeconds(bakeDuration));
      }

      // Was a button pressed?
      switch (getButton()) {
        case CONTROLEO_BUTTON_TOP:
          // Increase the duration
          bakeDuration = ++bakeDuration % BAKE_MAX_DURATION;
          displayDuration(0, getBakeSeconds(bakeDuration));
          break;
        case CONTROLEO_BUTTON_BOTTOM:
          // Save the temperature
          setSetting(SETTING_BAKE_DURATION, bakeDuration);
          // Go to the next phase
          setupPhase++;
      }
      break;      

    case 5: // Restart learning mode
      if (drawMenu) {
        drawMenu = false;
        if (getSetting(SETTING_LEARNING_MODE) == false) {
          lcdPrintLine(0, "Restart learning");
          lcdPrintLine(1, "mode?      No ->");
        }
        else
        {
          lcdPrintLine(0, "Oven is in");
          lcdPrintLine(1, "learning mode");
        }
      }
      
      // Was a button pressed?
      switch (getButton()) {
        case CONTROLEO_BUTTON_TOP:
          // Turn learning mode on
          setSetting(SETTING_LEARNING_MODE, true);
          drawMenu = true;
          break;
        case CONTROLEO_BUTTON_BOTTOM:
            // Go to the next phase
            setupPhase++;
       }
      break;

     case 6: // Restore to factory settings
      if (drawMenu) {
        drawMenu = false;
        lcdPrintLine(0, "Restore factory");
        lcdPrintLine(1, "settings?  No ->");
      }
      
      // Was a button pressed?
      switch (getButton()) {
        case CONTROLEO_BUTTON_TOP:
          // Reset EEPROM to factory
          lcdPrintLine(0, "Please wait ...");
          lcdPrintLine(1, "");
          setSetting(SETTING_EEPROM_NEEDS_INIT, true);
          InitializeSettingsIfNeccessary();

          // Intentional fall-through
        case CONTROLEO_BUTTON_BOTTOM:
            // Go to the next phase
            setupPhase++;
       }
      break;

   
  }
  
  // Does the menu option need to be redrawn?
  if (oldSetupPhase != setupPhase)
    drawMenu = true;
  if (setupPhase > 6) {
    setupPhase = 0;
    return false;
  }
  return true;
}


void displayMaxTemperature(int maxTemperature) {
  lcd.setCursor(0, 1);
  lcd.print(maxTemperature);
}

void displayDuration(int offset, uint16_t duration) {
  lcd.setCursor(offset, 1);
  if (duration >= 3600) {
    lcd.print((duration / 3600));
    lcd.print("h ");
  }
  lcd.print((duration % 3600) / 60);
  lcd.print("m ");

  if (duration < 3600) {
    lcd.print(duration % 60);
    lcd.print("s ");
  }

  lcd.print("   ");
}


