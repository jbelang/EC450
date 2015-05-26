/*
 * definitions.h
 *
 *  Created on: Apr 25, 2015
 *      Author: Jesse Belanger and Dimitrios Tsitsirigkos
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// Bitmasks and Constants for SPI
#define SPI_CLK 0x20
#define SPI_SOMI 0x04
#define SPI_SIMO 0x80
#define BIT_RATE_DIVISOR 32
#define BRLO (BIT_RATE_DIVISOR &  0xFF)
#define BRHI (BIT_RATE_DIVISOR / 0x100)

// Bitmasks and Constants for ADC
#define ADC_INPUT_BIT_MASK 0x10
#define ADC_INCH INCH_4

// Buttons, Pins and LEDs
#define BUTTON_BIT 0x08
#define NIGHT_LED 0x01
#define DAY_LED 0x40
#define WET_LED 0x02
#define DRY_LED 0x10
#define DARK_LED 0x01
#define PI 0x08

// Motor Bitmasks
#define MOTOR 0x04
#define MOTOR_ON '1'
#define MOTOR_OFF '0'
#define RUN_LEN 30
#define DELAY 1250

// Timer/Alarm bitmasks and constants
#define TONE_LEN 500
#define TA0_BIT 0x02
#define ALARM_ON '1'
#define	ALARM_OFF '0'

// Plant Monitoring Thresholds
#define THRESHOLD_DARK 100
#define THRESHOLD_DRY 20
#define THRESHOLD_WET 140
#define DRY_FLAG	'd'
#define WET_FLAG	'w'
#define GOOD_FLAG	'g'

// Plant states
#define GOOD_GOOD	'a'
#define GOOD_DARK	'b'
#define DRY_GOOD	'c'
#define DRY_DARK	'd'
#define WET_GOOD	'e'
#define WET_DARK	'f'
#define SLEEPING	'g'

// Modes
#define DAY	'D'
#define NIGHT 'N'

#endif /* DEFINITIONS_H_ */
