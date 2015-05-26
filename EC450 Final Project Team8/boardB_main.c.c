#include "msp430g2553.h"

// Bitmasks and Constants for ADC
#define ADC_INPUT_BIT_MASK 0x10
#define ADC_INCH INCH_4

// Bitmasks and Constants for SPI
#define ACTION_INTERVAL 1
#define BIT_RATE_DIVISOR 32
#define BRLO (BIT_RATE_DIVISOR &  0xFF)
#define BRHI (BIT_RATE_DIVISOR / 0x100)
#define SPI_CLK 0x20
#define SPI_SOMI 0x04
#define SPI_SIMO 0x80

// Function Definitions
 void init_spi(void);
 void init_wdt(void);
 void init_adc(void);
 void start_conversion(void);

// Global variables
volatile unsigned int latest_result;
volatile unsigned int action_counter=ACTION_INTERVAL;


// Orchestrates analog to digital conversion
void start_conversion(){
	if ((ADC10CTL1&ADC10BUSY) == 0){ // if not already converting...
		ADC10CTL0 |= ADC10SC;
		ADC10SA=(unsigned) &latest_result;

	}
}

interrupt void WDT_interval_handler(){
	start_conversion();

	if (--action_counter==0){
		UCB0TXBUF=latest_result/4; // init sending current byte
		action_counter=ACTION_INTERVAL;
	}
}
ISR_VECTOR(WDT_interval_handler, ".int10")


// Doesn't actively do much, but needs to be here
// to clear the flag and prevent the code from
// hitting a trap
void interrupt spi_rx_handler(){
	IFG2 &= ~UCB0RXIFG;		 // clear UCB0 RX flag
}
ISR_VECTOR(spi_rx_handler, ".int07")

//----------------------------------------------------------------//
//              Initialization functions						  //
//----------------------------------------------------------------//
void init_wdt(){
	// setup the watchdog timer as an interval timer
	// INTERRUPT NOT YET ENABLED!
  	WDTCTL =(WDTPW +		// (bits 15-8) password
     	                   	// bit 7=0 => watchdog timer on
       	                 	// bit 6=0 => NMI on rising edge (not used here)
                        	// bit 5=0 => RST/NMI pin does a reset (not used here)
           	 WDTTMSEL +     // (bit 4) select interval timer mode
  		     WDTCNTCL  		// (bit 3) clear watchdog timer counter
  		                	// bit 2=0 => SMCLK is the source
  		                	// bits 1-0 = 10=> source/512
 			 );
  	IE1 |= WDTIE; // enable WDT interrupt
 }

void init_spi(){
	UCB0CTL1 = UCSSEL_2+UCSWRST;  		// Reset state machine; SMCLK source;
	UCB0CTL0 = UCCKPH					// Data capture on rising edge
			   							// read data while clock high
										// lsb first, 8 bit mode,
			   +UCMST					// master
			   +UCMODE_0				// 3-pin SPI mode
			   +UCSYNC;					// sync mode (needed for SPI or I2C)
	UCB0BR0=BRLO;						// set divisor for bit rate
	UCB0BR1=BRHI;
	UCB0CTL1 &= ~UCSWRST;				// enable UCB0 (must do this before setting
										//              interrupt enable and flags)
	IFG2 &= ~UCB0RXIFG;					// clear UCB0 RX flag
	IE2 |= UCB0RXIE;					// enable UCB0 RX interrupt

	// Connect I/O pins to UCB0 SPI
	P1SEL  = SPI_CLK+SPI_SOMI+SPI_SIMO;
	P1SEL2 = SPI_CLK+SPI_SOMI+SPI_SIMO;
}

void init_adc(){
  ADC10CTL1= ADC_INCH	//input channel 4
 			  +SHS_0 //use ADC10SC bit to trigger sampling
 			  +ADC10DIV_4 // ADC10 clock/5
 			  +ADC10SSEL_0 // Clock Source=ADC10OSC
 			  +CONSEQ_0; // single channel, single conversion
 			  ;
  ADC10AE0=ADC_INPUT_BIT_MASK; // enable A4 analog input
  ADC10DTC1=1;   // one block per transfer
  ADC10CTL0= SREF_0	//reference voltages are Vss and Vcc
 	          +ADC10SHT_3 //64 ADC10 Clocks for sample and hold time (slowest)
 	          +ADC10ON	//turn on ADC10
 	          +ENC		//enable (but not yet start) conversions
 	          ;
}


// Main function just sets up the peripherals
// and then gets out of the way
void main(){
	volatile int latest_value; // latest value read

	WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
	BCSCTL1 = CALBC1_8MHZ;			// 8Mhz calibration for clock
  	DCOCTL  = CALDCO_8MHZ;

  	init_adc();
  	init_spi();
  	init_wdt();
 	_bis_SR_register(GIE+LPM0_bits);

}
