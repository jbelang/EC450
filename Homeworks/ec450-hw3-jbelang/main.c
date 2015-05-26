#include <msp430g2553.h>

// Bit masks for port 1
#define RED 0x01
#define GREEN 0x40
#define BUTTON 0x08

// Constants
#define TEN_SEC 1351	// 10s/7.4ms = 1351
#define ONE_SEC 135	// 1s/7.4ms = 135
#define LIMIT 100

// Global state variables
volatile unsigned char last_button; // the state of the button bit at the last interrupt
int record;	// Tracks number of button pushes needed to start record mode
int mode;	// 0 for play mode, 1 for record mode
int idle;	// Tracks how many idle seconds while in play mode to control switching back to record mode
int limit;
int intervals[LIMIT];	// To hold the intervals that the LED is on and off
int interval;
int index;
int ii;



void main(void) {
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

	  P1DIR |= RED+GREEN;					// Set RED and GREEN to output direction
	  P1DIR &= ~BUTTON;						// make sure BUTTON bit is an input

	  P1OUT &= ~(RED+GREEN);	// Turn off both LEDs

	  mode = 0;	// Sets mode to play mode initially
	  record = 2; // Two button pushes start recording mode
	  idle = TEN_SEC;	// Initialize length of expected idle period to be 10 seconds
	  interval = 0;
	  index = 0;
	  ii = 0;

	  P1OUT |= BUTTON;
	  P1REN |= BUTTON;						// Activate pullup resistors on Button Pin

	  _bis_SR_register(GIE+LPM0_bits);  // enable interrupts and also turn the CPU off!
}

// ===== Watchdog Timer Interrupt Handler =====
// This event handler is called to handle the watchdog timer interrupt,
//    which is occurring regularly at intervals of about 8K/1.1MHz ~= 7.4ms.

interrupt void WDT_interval_handler(){
  	unsigned char b;
  	b = (P1IN & BUTTON);  // read the BUTTON bit

  	if (last_button && (b==0) && mode==0){ // Checks if the button has been pushed, and if record mode is off
  		record--;	// Decrement button pushes needed to start record mode

  		if (record==0){	// Signal to start record mode detected
  			P1OUT ^= RED; // Turn on red LED
  			P1OUT &= ~GREEN; // Make sure green LED is off
  			mode = 1;	// Switch to record mode
  			record=2;	// Reset record
  			index = 0;	// Set starting index into array for storing intervals
  			ii = 0; // Reset playback iterator
  			intervals[index] = 0; // Make sure previous last element is non-negative
  			interval = 0;
  		}
  	}

  	if (last_button && (b==0) && mode==1){ // Detects button pushes in record mode
  		P1OUT ^= GREEN; // Toggle the green LED
  		intervals[index] = interval;	// Store time between toggles into an array
  		interval = 0;	// Reset the interval counter
  		index++;	// Increment index into storage array
  		idle = TEN_SEC;	// Establish an idle period, if button is not pressed again in this time, record mode switches off

  		if (index == LIMIT) { // If the array is full
  			mode = 0;	// Switch to playback mode
  		}
  	}

  	else if (mode == 1){	// No button pushes detected while in record mode
  		idle--;	// Decrements time left in waiting period
  		interval++;	// Increments time since last button push

  		if (idle == 0){	// Waiting period expired
  			mode = 0;	// Switch to play mode
  			intervals[index] = interval;
  			//index++;	// Increment index into storage array
  			P1OUT &= ~(RED+GREEN);	// Turn off both LEDs
  			idle = TEN_SEC;	// Resets idle
  		}
  	}

  	else if (mode == 0) {	// In play mode

  		if (index != 0) {	// Makes sure that some data has been stored (e.g. button pushed)
  			if (ii != 0 && ii <= LIMIT && ii <= index) { // Skips first element because it will always be zero
  				intervals[ii] -= 1;	// Decrement time left in interval
  			}

  			if (intervals[ii] == 0 && ii < index) {	// If the stored interval has been reached
  				P1OUT ^= GREEN; // Toggle the green LED
  				ii++;	// Go to next stored interval
  			}

  			if (intervals[ii] == 0 && ii == index) {
  				if (index % 2 != 0){	//Only if the LED was left on do we toggle one more time to turn it off
  					P1OUT ^= GREEN; // Toggle the green LED
  					ii++;	// Go to next stored interval
  				}
  			}

  		}
  	}

  	last_button=b;    // remember button reading for next time.
}
// DECLARE function WDT_interval_handler as handler for interrupt 10
// using a macro defined in the msp430g2553.h include file
ISR_VECTOR(WDT_interval_handler, ".int10")
