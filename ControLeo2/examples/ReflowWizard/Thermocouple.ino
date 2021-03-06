// Thermocouple
// Instead of getting instantaneous readings from the thermocouple, get an average
// Also, some convection ovens have noisy fans that generate spurious short-to-ground and 
// short-to-vcc errors.  This will help to eliminate those.
// takeCurrentThermocoupleReading() is called from the Timer 1 interrupt (see "Servo" tab).  It is
// called 5 times per second.

#define NUM_READINGS           3   // Number of readings to average the temperature over (5 readings = 1 second)
#define ERROR_THRESHOLD        15  // Number of consecutive faults before a fault is returned


// Store the temperatures as they are read
volatile float recentTemperatures[NUM_READINGS];
volatile int temperatureErrorCount = 0;
volatile float temperatureError;
const MAX6675 thermocouple = {CLK_PIN,CS_PIN,MISO_PIN};


// This function is called every 200ms from the Timer 1 (servo) interrupt
void takeCurrentThermocoupleReading()
{
  noInterrupts();
  //Serial.println(F("Timer called"));
  volatile static int readingNum = 0;
  
  // The timer has fired.  It has been 0.2 seconds since the previous reading was taken
  // Take a thermocouple reading
  float temperature = thermocouple.readCelsius();
  //Serial.print(F("CurrentTemp: "));
  //Serial.println(temperature);
  // Is there an error?
  if (temperature == NAN) {
    // Noise can cause spurious short faults.  These are typically caused by the convection fan
    if (temperatureErrorCount < ERROR_THRESHOLD)
      temperatureErrorCount++;
    temperatureError = temperature;
    Serial.println(F("Temperature error"));
  }
  else {
    // There is no error.  Save the temperature
    recentTemperatures[readingNum] = temperature;
    readingNum = (readingNum + 1) % NUM_READINGS;
    // Clear any previous error
    temperatureErrorCount = 0;
  }
  interrupts();
}



// Routine used by the main app to get temperatures
// This routine disables and then re-enables interrupts so that data corruption isn't caused
// by the ISR writing data at the same time it is read here.
float getCurrentTemperature() {
  float temperature = 0;
  
  // Disable interrupts while reading values
  noInterrupts();

  // Is there an error?
  if (temperatureErrorCount >= ERROR_THRESHOLD)
    temperature = temperatureError;
  else {
    // Return the average of the last NUM_READINGS readings
    for (int i=0; i< NUM_READINGS; i++)
      temperature += recentTemperatures[i];
    temperature = temperature / NUM_READINGS;
  }
  
  // Reenable interrupts
  interrupts();
  
  // Return the temperature
  return temperature;
}

