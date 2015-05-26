asm(" .length 10000");
asm(" .width 132");
/*
 *  Created on: Mar 21, 2015
 *      Author: Jesse Belanger
 */

#include "msp430g2553.h"
#include "notes.h"

#define RED 0x01	// P1.0
#define TA0_BIT 0x02	// P1.1
#define RESTART 0x04	// P1.2
#define PLAY_PAUSE 0x08	// P1.3
#define SLOWER 0x10	// P1.4
#define FASTER 0x20	// P1.5
#define GREEN 0x40	// P1.6
#define SELECT 0x80	// P1.7

#define halfSecond 61

void init_timer(void); // routine to setup the timer
void init_buttons(void); // routine to setup the buttons

int playMode;	// 0 off, 1 on
int duration, count, selection;
double tempo;
int last_button;

void main(){
	BCSCTL1 = CALBC1_1MHZ;    // 1Mhz calibration for clock
	DCOCTL  = CALDCO_1MHZ;

	WDTCTL =(WDTPW + WDTTMSEL + WDTCNTCL + 0 + 1); // setup the watchdog timer as an interval timer
	IE1 |= WDTIE;		// enable the WDT interrupt (in the system interrupt register IE1)

	init_timer();  // initialize timer
	init_buttons(); // initialize the buttons

	P1DIR |= (RED + GREEN);	// enable LEDs for use
	P1OUT &= ~(RED + GREEN);	// initialize them to be off

	// Get the system ready to play the default song: Joy to the World
	playMode = 0;	// start with playmode off
	selection = 0; // Joy to the World
	tempo = 1; // start off with baseline tempo of 120 bpm
	duration = tempo*quarter*joyTiming[0];
	count = 1;

	_bis_SR_register(GIE+LPM0_bits);// enable general interrupts and power down CPU
}

// Sound Production System
void init_timer(){
	TA0CTL |= TACLR;              // reset clock
	TA0CTL = TASSEL_2+ID_0+MC_1;  // clock source = SMCLK + clock divider=1 + UP mode + timer A interrupt off
	TA0CCTL0=0; // compare mode, output 0, no interrupt enabled
	TA0CCR0 = joyNotes[0]; // in up mode TAR=0... TACCRO-1
	P1SEL|=TA0_BIT; // connect timer output to pin
	P1DIR|=TA0_BIT;	// set pin to output
}

// Sets up the various buttons
void init_buttons(){
	P1OUT |= PLAY_PAUSE; // pullup
	P1REN |= PLAY_PAUSE + RESTART + SLOWER + FASTER + SELECT; // enable resistor
	P1IES |= PLAY_PAUSE; // set for 1->0 transition
	P1IFG &= ~PLAY_PAUSE;// clear interrupt flag
	P1IE  |= PLAY_PAUSE; // enable interrupt
}

void interrupt button_handler(){
	if (P1IFG & PLAY_PAUSE){	// check that this is the correct interrupt (if not, it is an error, but there is no error handler)
		P1IFG &= ~PLAY_PAUSE; // reset the interrupt flag

		if (playMode == 0) {	// No song playing
			playMode = 1;	// Turn play mode on
			P1OUT ^= GREEN;	// Green light on
			TACCTL0 ^= OUTMOD_4; // Turn sounds on
		}

		else if (playMode == 1) {	// Play mode
			playMode = 2;	// Turn play mode to pause
			P1OUT ^= RED + GREEN;	// Red light on, green off
			TACCTL0 ^= OUTMOD_4; // Turn sounds off
		}

		else if (playMode == 2) { // Paused
			playMode = 1;	// Un-pause play mode
			P1OUT ^= RED + GREEN;	// Green back on
			TACCTL0 ^= OUTMOD_4; // Turn sounds on
		}

	}
}

// Controls timing of notes
interrupt void WDT_interval_handler(){
	int button = (P1IN & (RESTART + SLOWER + FASTER + SELECT));	// Read the buttons

	// Play mode is on
	if (playMode == 1) {
		if((P1IN & RESTART) == RESTART) { 	// Restart button pressed
			TA0CTL |= TACLR;	// Reset timer
			count = 1;

			if (selection == 0) {
				TA0CCR0 = C7_72;	// Start from first Joy note
				duration = tempo*quarter*joyTiming[0];
			}
			if (selection == 1) {
				TA0CCR0 = G_67;	// Start from first Sandstorm note
				duration = tempo*quarter*joyTiming[0];
			}
		}

		else {
			// Slow tempo request
			if (((button & SLOWER) == SLOWER) != (last_button & SLOWER))
				tempo = 2;

			// Fast tempo request
			else if (((button & FASTER) == FASTER) != (last_button & FASTER))
				tempo = 0.5;

			else if (((button & SELECT) == SELECT) != (last_button & SELECT))
				tempo = 1;

			duration--;
			if (duration == 0) {
				if (selection == 0) {
					if (count < joyLength) {
						duration = tempo*quarter*joyTiming[count];	// Select length of next note
						TA0CCR0 = joyNotes[count];	// Select tone of next note
						count++;
					}

					if (count == joyLength) {
						playMode = 0;	// back to playmode off
						P1OUT ^= GREEN;	// Green light off
						TACCTL0 ^= OUTMOD_4; // Turn sounds off

						//Reset song
						duration = tempo*quarter*joyTiming[0];
						TA0CCR0 = joyNotes[0];
						count = 1;
						tempo = 1;
					}
				}

				if (selection == 1) {
					if (count < sandstormLength) {
						duration = tempo*quarter*sandstormTiming[count];
						TA0CCR0 = sandstormNotes[count];
						count++;
					}

					if (count == sandstormLength) {
						playMode = 0;	// back to playmode off
						P1OUT ^= GREEN;	// Green light off
						TACCTL0 ^= OUTMOD_4; // Turn sounds off

						//Reset song
						duration = tempo*quarter*sandstormTiming[0];
						TA0CCR0 = sandstormNotes[0];
						count = 1;
						tempo = 1;
					}
				}
			}
		}
	}

	else if (playMode == 0 && ((button & SELECT) == SELECT) != (last_button & SELECT)) {	//Select button pressed
		if (selection == 0 ) {
			selection = 1;	// Switch to Sandstorm
			// Set up song
			duration = tempo*quarter*sandstormTiming[0];
			TA0CCR0 = sandstormNotes[0];
			count = 1;
			tempo = 1;
		}

		else if (selection == 1) {
			selection = 0; 	// Switch to Joy to the World
			// Set up song
			duration = tempo*quarter*joyTiming[0];
			TA0CCR0 = joyNotes[0];
			count = 1;
			tempo = 1;
		}
	}

	last_button = button;
}
ISR_VECTOR(button_handler,".int02") // declare button interrupt vector
ISR_VECTOR(WDT_interval_handler, ".int10") // declare WDT interrupt vector
