/*
 *  SOS.c
 *	Course: EC 450
 *  Created on: Feb 8, 2015
 *  Author: Jesse Belanger
 */

#include <msp430g2553.h>

#define S 1
#define PAUSE 2
#define O 3
#define LONG_PAUSE 4

volatile unsigned int blink_interval;  // number of WDT interrupts per blink of LED
volatile unsigned int blink_counter;   // down counter for interrupt handler
volatile unsigned int state, prev;
volatile unsigned int flash, stall;

int main(void) {
	  // setup the watchdog timer as an interval timer
	  WDTCTL =(WDTPW + // (bits 15-8) password
	                   // bit 7=0 => watchdog timer on
	                   // bit 6=0 => NMI on rising edge (not used here)
	                   // bit 5=0 => RST/NMI pin does a reset (not used here)
	           WDTTMSEL + // (bit 4) select interval timer mode
	           WDTCNTCL +  // (bit 3) clear watchdog timer counter
	  		          0 // bit 2=0 => SMCLK is the source
	  		          +1 // bits 1-0 = 01 => source/8K
	  		   );
	  IE1 |= WDTIE;		// enable the WDT interrupt (in the system interrupt register IE1)

	  P1DIR |= 0x01;					// Set P1.0 to output direction
	  P1OUT = 0x00;						// Set P1.0 to off initially

	  // initialize the state variables
	  blink_interval=67;                // the number of WDT interrupts per toggle of P1.0
	  blink_counter=blink_interval;     // initialize the counter
	  state = S;						// initial state: flash S
	  prev = LONG_PAUSE;				// lets state machine know this is the first S, rather than last
	  flash = 6;						// toggles P1 six times to go on, off, on, off, on, off
	  stall = 2;						// normal stall = 2, since flashes have a built in 1 unit off

	  _bis_SR_register(GIE+LPM0_bits);  // enable interrupts and also turn the CPU off!
}

// ===== Watchdog Timer Interrupt Handler =====
// This event handler is called to handle the watchdog timer interrupt,
//    which is occurring regularly at intervals of about 8K/1.1MHz ~= 7.4ms.

interrupt void WDT_interval_handler(){
  if (--blink_counter==0){          // decrement the counter and act only if it has reached 0

	  if (state==S && flash>0) {
		  P1OUT ^= 1;                   // toggle LED on P1.0
		  blink_counter=blink_interval; // reset the down counter
		  flash--;

		  //If last pause was long, we know we're at the first S. Otherwise, we're at the last
		  if (flash==0) {
			  flash = 6;	// reset flash
			  if (prev==LONG_PAUSE) {
				  state = PAUSE;
				  prev = S;		// current state becomes previous state
			  }
			  else {
				  state = LONG_PAUSE;
				  prev = S;
			  }
		  }
	  }

	  else if (state==PAUSE && stall>0) {
		  blink_counter=blink_interval; // reset the down counter
		  stall--;

		  //If last letter was S, next letter is O (and vice versa)
		  if (stall==0 && prev==S) {
			  stall = 2;	// reset stall
			  state = O;
			  prev = PAUSE;	// current state becomes previous state
		  }
		  else if (stall==0 && prev==O) {
			  stall = 6;	// set stall for upcoming long pause
			  state = S;
			  prev = PAUSE;	// current state becomes previous state
		  }
	  }

	  else if (state==O && flash>0) {
		  P1OUT ^= 1;                   // toggle LED on P1.0

		  if (flash == 6 || flash == 4 || flash == 2)
			  blink_counter=3*blink_interval; // reset the down counter
		  else if (flash == 5 || flash == 3 || flash == 1)
			  blink_counter=blink_interval; // reset the down counter

		  flash--;

		  //Always transition from O to inter-letter pause
		  if (flash==0) {
			  flash = 6;	//reset flash
			  state = PAUSE;
			  prev = O;	// current state becomes previous state
		  }
	  }

	  else if (state==LONG_PAUSE && stall) {
		  blink_counter=blink_interval; // reset the down counter
	  	  stall--;

	  	  //Always transition to S
	  	  if (stall==0) {
	  		  stall = 2;	// reset stall for short pause
	  		  state = S;
	  		  prev = LONG_PAUSE; // current state becomes previous state
	  	  }
	  }
  }
}
// DECLARE function WDT_interval_handler as handler for interrupt 10
// using a macro defined in the msp430g2553.h include file
ISR_VECTOR(WDT_interval_handler, ".int10")
