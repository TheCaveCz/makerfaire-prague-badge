//////////////////////////////
//
// MakerFaire Prague 2018 badge
// (c) The Cave, 2018
// https://thecave.cz
//
// Licensed under MIT License.
//
// (written in a very short time and without a lot of sleep)
// 
// There are surely lot of ways how this code can be made better, but it works for the specific case we had.
// Lot of assumptions about sizes/commands/behaiours is hardcoded. If you change something, don't be
// surprised that it can break something else.
//
// Use "Arduino Pro or Pro Mini" board with "ATMega328P (3V3, 8MHz)" setting
//
// But if you change something and it works, feel free to send a pull request!
//
// Happy hacking!
//

#include <EEPROM.h>
#include <TaskScheduler.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>

// enable serial line debug info output
#define LOG_ENABLED 1


// Pin Assignments

// PB2 (dip 16) output - data ouput line for WS2818 LEDS
#define PIN_LEDS_WS 10
// PD7 (dip 13) output - gate for P-MOSFET controlling WS2818 VCC line
#define PIN_LEDS_WS_ENABLE 7
// PD4 (dip 6) output - VCC for IR receiver
#define PIN_IRRX_EN 4
// PD2 (dip 4) input - data input line from IR receiver
#define PIN_IRRX 2
// PD3 (dip 5) output - IR led 
#define PIN_IRTX 3
// PD6 (dip 12) output - left eye LED
#define PIN_LED1 6
// PD5 (dip 11) output - right eye LED
#define PIN_LED2 5
// PB5 builtin led - LED on the back of the robot
#define PIN_LED3 13

// both buttons need to be on the same port
// if you change pins you need to change the pin change interrupt regs too!
// PB1 (dip 15) input pullup - PAIR button
#define PIN_SW1 9
// PB0 (dip 14) input pullup - SHOW button
#define PIN_SW2 8


Scheduler scheduler;

// NOTE : you may see function prototypes declarations in the source
// for some reason Arduino IDE fails to generate function prototypes in this source




