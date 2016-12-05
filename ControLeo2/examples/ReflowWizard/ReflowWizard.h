// Include file for the Reflow Wizard
#ifndef REFLOW_WIZARD_H
#define REFLOW_WIZARD_H

#include <avr/pgmspace.h>

// Main menu modes
#define MODE_TESTING                         0
#define MODE_CONFIG                          1
#define MODE_REFLOW                          2
#define MODE_BAKE                            3
#define NO_OF_MODES                          4

#define NEXT_MODE                            false

// Output type
#define TYPE_UNUSED                          0
#define TYPE_TOP_ELEMENT                     1
#define TYPE_BOTTOM_ELEMENT                  2
#define TYPE_BOOST_ELEMENT                   3
#define TYPE_CONVECTION_FAN                  4
#define TYPE_COOLING_FAN                     5
#define NO_OF_TYPES                          6
#define isHeatingElement(x)                  (x == TYPE_TOP_ELEMENT || x == TYPE_BOTTOM_ELEMENT || x == TYPE_BOOST_ELEMENT)

const char c_outDesc_unused[] PROGMEM = "Unused";
const char c_outDesc_top[] PROGMEM = "Top";
const char c_outDesc_bottom[] PROGMEM = "Bottom";
const char c_outDesc_boost[] PROGMEM = "Boost";
const char c_outDesc_convection[] PROGMEM = "Convection Fan";
const char c_outDesc_cooling[] PROGMEM = "Cooling Fan";

const char* const outputDescription[NO_OF_TYPES] PROGMEM = {c_outDesc_unused, c_outDesc_top, c_outDesc_bottom, c_outDesc_boost, c_outDesc_convection, c_outDesc_cooling};
char descBuffer[15];

// Phases of reflow
#define PHASE_INIT                           0    // Variable initialization
#define PHASE_PRESOAK                        1    // Pre-soak rapidly gets the oven to around 150C
#define PHASE_SOAK                           2    // Soak brings the PCB and components to the same (high) temperature
#define PHASE_REFLOW                         3    // Reflow melts the solder
#define PHASE_WAITING                        4    // After reaching max temperature, wait a bit for heat to permeate and start cooling
#define PHASE_COOLING_BOARDS_IN              5    // The oven door should be open and the boards remain in until components definitely won't move
#define PHASE_COOLING_BOARDS_OUT             6    // Boards can be removed. Remain in this state until another reflow can be started at 50C
#define PHASE_ABORT_REFLOW                   7    // The reflow was aborted or completed.

#define BAKING_PHASE_INIT                    0    // Initialize baking, check oven temperature
#define BAKING_PHASE_HEATUP                  1    // Heat up the oven rapidly to just under the desired temperature
#define BAKING_PHASE_BAKE                    2    // The main baking phase. Just keep the oven temperature constant
#define BAKING_PHASE_START_COOLING           3    // Start the cooling process
#define BAKING_PHASE_COOLING                 4    // Wait till the oven has cooled down to 50°C
#define BAKING_PHASE_ABORT                   5    // Baking was aborted or completed

const char empty[] PROGMEM = "";

const char c_phaseDesc_presoak[] PROGMEM = "Presoak";
const char c_phaseDesc_soak[] PROGMEM = "Soak";
const char c_phaseDesc_reflow[] PROGMEM = "Reflow";
const char c_phaseDesc_waiting[] PROGMEM = "Waiting";
const char c_phaseDesc_cooling[] PROGMEM = "Cooling";
const char c_phaseDesc_cooldoor[] PROGMEM = "Cool - open door";
const char c_phaseDesc_abort[] PROGMEM = "Abort";

const char* const phaseDescription[] PROGMEM = {empty, c_phaseDesc_presoak, c_phaseDesc_soak, c_phaseDesc_reflow, c_phaseDesc_waiting, c_phaseDesc_cooling, c_phaseDesc_cooldoor, c_phaseDesc_abort};
char phaseBuffer[17];

const char c_bakingPDesc_heating[] PROGMEM = "Heating";
const char c_bakingPDesc_baking[] PROGMEM = "Baking";
const char c_bakingPDesc_cooling[] PROGMEM = "Cooling";

const char* const bakingPhaseDescription[] PROGMEM = {empty, c_bakingPDesc_heating, c_bakingPDesc_baking, empty, c_bakingPDesc_cooling, empty};
char bakingBuffer[8];

// EEPROM settings
// Remember that EEPROM initializes to 0xFF after flashing the bootloader
#define SETTING_EEPROM_NEEDS_INIT             0    // EEPROM will be initialized to 0 at first run
#define SETTING_D5_TYPE                       1    // Element type controlled by D5 (or fan, unused)
#define SETTING_D6_TYPE                       2    // Element type controlled by D6 (or fan, unused)
#define SETTING_D7_TYPE                       3    // Element type controlled by D7 (or fan, unused)
#define SETTING_D8_TYPE                       4    // Element type controlled by D8 (or fan, unused)
#define SETTING_MAX_TEMPERATURE               5    // Maximum oven temperature.  Relow curve will be based on this (stored temp is offset by 150 degrees)
#define SETTING_SETTINGS_CHANGED              6    // Settings have changed.  Relearn duty cycles
#define SETTING_BAKE_TEMPERATURE              7    // The baking temperature (divided by 5)
#define SETTING_BAKE_DURATION                 8    // The baking duration (see getBakeSeconds)

// Learned settings
#define SETTING_LEARNING_MODE                 10   // ControLeo is learning oven response and will make adjustments
#define SETTING_PRESOAK_D5_DUTY_CYCLE         11   // Duty cycle (0-100) that D4 must be used during presoak
#define SETTING_PRESOAK_D6_DUTY_CYCLE         12   // Duty cycle (0-100) that D5 must be used during presoak
#define SETTING_PRESOAK_D7_DUTY_CYCLE         13   // Duty cycle (0-100) that D6 must be used during presoak
#define SETTING_PRESOAK_D8_DUTY_CYCLE         14   // Duty cycle (0-100) that D7 must be used during presoak
#define SETTING_SOAK_D5_DUTY_CYCLE            15   // Duty cycle (0-100) that D4 must be used during soak
#define SETTING_SOAK_D6_DUTY_CYCLE            16   // Duty cycle (0-100) that D5 must be used during soak
#define SETTING_SOAK_D7_DUTY_CYCLE            17   // Duty cycle (0-100) that D6 must be used during soak
#define SETTING_SOAK_D8_DUTY_CYCLE            18   // Duty cycle (0-100) that D7 must be used during soak
#define SETTING_REFLOW_D5_DUTY_CYCLE          19   // Duty cycle (0-100) that D4 must be used during reflow
#define SETTING_REFLOW_D6_DUTY_CYCLE          20   // Duty cycle (0-100) that D4 must be used during reflow
#define SETTING_REFLOW_D7_DUTY_CYCLE          21   // Duty cycle (0-100) that D4 must be used during reflow
#define SETTING_REFLOW_D8_DUTY_CYCLE          22   // Duty cycle (0-100) that D4 must be used during reflow

#define TEMPERATURE_OFFSET                    150  // To allow temperature to be saved in 8-bits (0-255)
#define BAKE_TEMPERATURE_STEP                 5    // Allows the storing of the temperature range in one byte
#define BAKE_MAX_DURATION                     176  // 176 = 18 hours (see getBakeSeconds)
#define BAKE_MIN_TEMPERATURE                  40   // Minimum temperature for baking
#define BAKE_MAX_TEMPERATURE                  200  // Maximum temperature for baking

#endif // REFLOW_WIZARD_H
