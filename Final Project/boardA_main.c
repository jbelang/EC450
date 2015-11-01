asm(" .length 10000");
asm(" .width 132");

#include "msp430g2553.h"
#include "definitions.h"

// Function Declarations
void init_spi(void);
void init_wdt(void);
void init_timer(void);
void init_adc(void);
void init_button(void);
void light_conversion(void);

// Global variables
volatile int light_lvl;					// Input from sensor
volatile unsigned int moisture_lvl= 0; 	// Received over SPI
volatile char plant_state;	// Keeps plant's current state
volatile char last_plant_state;	// Tracks previous state
volatile char mode; // Keeps track of whether device is in day or night mode
volatile int alarm_time;
volatile char alarm_mode;
volatile char motor_mode;
volatile int motor_time;
volatile int motor_delay;
volatile char moisture_flag;
volatile char running;


/*
 * Converts light input to usable data
 */
void light_conversion(){
	if ((ADC10CTL1 & ADC10BUSY) == 0){
		ADC10CTL0 |= ADC10SC;
		ADC10SA =(unsigned) &light_lvl;
	}
}

/*
 * Function to change the plant's state based on sensor input.
 * Plant can assume 1 of six states based on three moisture levels
 * and two light levels
 */
void state_monitor() {

	// Comfortable moisture for plant
	if (THRESHOLD_DRY < moisture_lvl && moisture_lvl < THRESHOLD_WET) {
		if (light_lvl < THRESHOLD_DARK)
			plant_state = GOOD_DARK;
		else
			plant_state = GOOD_GOOD;
	}

	// Too dry for plant, needs water
	else if (moisture_lvl < THRESHOLD_DRY) {
		if (light_lvl < THRESHOLD_DARK)
			plant_state = DRY_DARK;
		else
			plant_state = DRY_GOOD;
	}

	// Too wet for the plant, overwatered
	else if (moisture_lvl > THRESHOLD_WET) {
		if(light_lvl < THRESHOLD_DARK)
			plant_state = WET_DARK;
		else
			plant_state = WET_GOOD;
	}
}

/*
 * WDT Handler
 * Runs the state machine which controls all the actions of the
 * plant monitoring device
 */
interrupt void WDT_interval_handler(){
	last_plant_state = plant_state;	// Capture previous state
	light_conversion();	// Convert raw light data into light_lvl
	state_monitor();	// Determine what the plant's state is

	// Check to see if it's time to shut off an alarm that may be on
	if (alarm_mode == ALARM_ON) {
		if (--alarm_time == 0) {
			TACCTL0 &= ~(OUTMOD_4);	// Turn the sound off
			alarm_time = TONE_LEN;	// Reset the time counter
			alarm_mode = ALARM_OFF;
		}
	}

	// Turn on motor after delay
	if (motor_delay == 0 && motor_mode == MOTOR_ON) {
		P2OUT |= MOTOR;
		motor_delay = DELAY;
		running = 'R';	// Motor is running
	}

	// Check to see if it's time to shut off the motor
	if (running == 'R' && motor_mode == MOTOR_ON) {
		if (--motor_time == 0) {
			P2OUT &= ~(MOTOR);		// Turn the motor off
			motor_time = RUN_LEN; // Reset the time counter
			motor_mode = MOTOR_OFF;
			running = 'N';	// Motor not running
		}
	}

	// The different states will be dealt with if the system is in day mode
	if (mode == DAY) {

		if (plant_state == GOOD_GOOD) {
			P2OUT &= ~(WET_LED+DARK_LED+DRY_LED+PI);	// All warning lights off
			moisture_flag = GOOD_FLAG;
			motor_mode = MOTOR_OFF;
		}

		else if (plant_state == GOOD_DARK) {
			P2OUT |= DARK_LED+PI;	// Dark warning light on
			P2OUT &= ~(WET_LED+DRY_LED);	// Wet and dry warning lights off
			moisture_flag = GOOD_FLAG;
			motor_delay = DELAY;

			if (plant_state != last_plant_state) {
				TACCTL0 |= OUTMOD_4;	// Turn on sound
				alarm_mode = ALARM_ON;
			}
		}

		else if (plant_state == DRY_GOOD) {
			P2OUT |= DRY_LED+PI;	// Dry warning light on
			P2OUT &= ~(WET_LED+DARK_LED);	// Wet and Dark warning light off


			if (plant_state != last_plant_state) {
				TACCTL0 |= OUTMOD_4;	// Turn on sound
				alarm_mode = ALARM_ON;

				if (moisture_flag != DRY_FLAG) {	// Checks for transition between two dry states
					motor_mode = MOTOR_ON;
				}
			}

			if (motor_mode == MOTOR_ON) {
				--motor_delay;	// Decrement delay counter
			}
			moisture_flag = DRY_FLAG;
		}

		else if (plant_state == DRY_DARK) {
			P2OUT |= DARK_LED+DRY_LED+PI;	// Dark and Dry warning light on
			P2OUT &= ~(WET_LED);	// Wet warning light off

			if (plant_state != last_plant_state) {
				TACCTL0 |= OUTMOD_4;	// Turn on sound
				alarm_mode = ALARM_ON;

				if (moisture_flag != DRY_FLAG) {
					motor_mode = MOTOR_ON;
					motor_delay = DELAY;	// Set delay for start of motor
				}
			}

			if (motor_mode == MOTOR_ON) {
				--motor_delay;	// Decrement delay counter
			}
			moisture_flag = DRY_FLAG;
		}

		else if (plant_state == WET_GOOD) {
			P2OUT |= WET_LED;	// Wet warning light on
			P2OUT &= ~(DARK_LED+DRY_LED);	// Dark and Dry warning light off
			motor_delay = DELAY;

			if (plant_state != last_plant_state) {
				TACCTL0 |= OUTMOD_4;	// Turn on sound
				alarm_mode = ALARM_ON;
			}
			moisture_flag = WET_FLAG;
		}

		else if (plant_state == WET_DARK) {
			P2OUT |= WET_LED+DARK_LED+PI;	// Wet and Dark warning lights on
			P2OUT &= ~(DRY_LED);  // Dry warning light off
			motor_delay = DELAY;

			if (plant_state != last_plant_state) {
				TACCTL0 |= OUTMOD_4;	// Turn on sound
				alarm_mode = ALARM_ON;
			}
			moisture_flag = WET_FLAG;
		}
	}

	// Night mode
	else {
		P2OUT &= ~(WET_LED+DARK_LED+DRY_LED); // Turn off all warning lights
		plant_state = SLEEPING;
	}
}
ISR_VECTOR(WDT_interval_handler, ".int10")

// Receives the moisture sensor data
// which is communicated from auxiliary board
// via SPI communication protocol
void interrupt spi_rx_handler(){
	moisture_lvl=UCB0RXBUF; // copy data to global variable
	IFG2 &= ~UCB0RXIFG;

}
ISR_VECTOR(spi_rx_handler, ".int07")

/*
 * Handles button interrrupts, which
 * change the state of the device from day
 * mode to night mode
 */
void interrupt button_handler(){
	if (P1IFG & BUTTON_BIT){
		P1IFG &= ~BUTTON_BIT; // reset the interrupt flag

		if (mode == DAY) {
			mode = NIGHT;	// Switch mode
			P1OUT ^= (DAY_LED + NIGHT_LED);	// Switch which light is on

		}

		else {
			mode = DAY;
			P1OUT ^= DAY_LED + NIGHT_LED;
		}
	}
}
ISR_VECTOR(button_handler,".int02") // declare interrupt vector

///////////////////////////////////////////
// Initializers                          //
// for WDT, SPI, and Timer, ADC, button  //
///////////////////////////////////////////

void init_wdt() {
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
			   							// SLAVE
			   +UCMODE_0				// 3-pin SPI mode
			   +UCSYNC;					// sync mode (needed for SPI or I2C)
	UCB0BR0=BRLO;						// set divisor for bit rate
	UCB0BR1=BRHI;
	UCB0CTL1 &= ~UCSWRST;				// enable UCB0 (must do this before setting
										//              interrupt enable and flags)
	IFG2 &= ~UCB0RXIFG;					// clear UCB0 RX flag
	IE2 |= UCB0RXIE;					// enable UCB0 RX interrupt

	// Connect I/O pins to UCB0 SPI
	P1SEL =SPI_CLK+SPI_SOMI+SPI_SIMO;
	P1SEL2=SPI_CLK+SPI_SOMI+SPI_SIMO;
}

void init_timer(){
	TA0CTL |= TACLR;
	TA0CTL = TASSEL_2+ID_0+MC_1;
	TA0CCTL0=0;
	TA0CCR0 = 2000;
	P1SEL|=TA0_BIT;
	P1DIR|=TA0_BIT;

	alarm_time = TONE_LEN;
}


void init_adc(){
  ADC10CTL1= ADC_INCH	//input channel 4
 			  +SHS_0 //use ADC10SC bit to trigger sampling
 			  +ADC10DIV_4 // ADC10 clock/5
 			  +ADC10SSEL_0 // Clock Source=ADC10OSC
 			  +CONSEQ_0; // single channel, single conversion
 			  ;
  ADC10AE0=ADC_INPUT_BIT_MASK;
  ADC10DTC1=1;
  ADC10CTL0= SREF_0 + ADC10SHT_3 + ADC10ON + ENC;
}

void init_button(){
	P1OUT |= BUTTON_BIT; // pullup
	P1REN |= BUTTON_BIT; // enable resistor
	P1IES |= BUTTON_BIT; // set for 1->0 transition
	P1IFG &= ~BUTTON_BIT;// clear interrupt flag
	P1IE  |= BUTTON_BIT; // enable interrupt
}

/////////////////////////////////////////////////////
// Main Program                                    //
// Initializes things and then gets out of the way //
/////////////////////////////////////////////////////
void main(){

	WDTCTL = WDTPW + WDTHOLD;
	BCSCTL1 = CALBC1_8MHZ;			// 8Mhz calibration for clock
  	DCOCTL  = CALDCO_8MHZ;

  	init_adc();
  	init_spi();
  	init_wdt();
  	init_timer();
  	init_button();

  	// LED Set up
  	mode = DAY;
  	P1DIR |= NIGHT_LED+DAY_LED;	// Set LEDs as outputs
  	P1OUT |= DAY_LED;	//Day mode on, day mode is always on when the device starts up
  	P1OUT &= ~(NIGHT_LED);  // Night mode and Dry warning light off

  	P2DIR |= WET_LED+DARK_LED+DRY_LED;	// Set P2 LEDs as outputs
  	P2OUT &= ~(WET_LED+DARK_LED+DRY_LED); // Set them both to off to start

  	// Initialize alarm to off
  	alarm_mode = ALARM_OFF;

  	// Set up motor
  	P2DIR |= MOTOR;		// Set motor pin as output
  	motor_mode = MOTOR_OFF;
  	motor_time = RUN_LEN;

  	// Set up out-pin to Pi
  	P2DIR |= PI;
  	P2OUT &= ~PI;

 	_bis_SR_register(GIE+LPM0_bits);
}
