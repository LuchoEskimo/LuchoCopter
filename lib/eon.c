# include "eon.h"

// The number of milliseconds from startup
static volatile uint32_t eon_milliseconds;

/*
 * Function eon_init
 * Desc 	Initialize the eon library,
 * 			allowing then the user to
 * 			get information about the time.
 */
void eon_init() {
	// Clear eon_milliseconds
	eon_milliseconds = 0;

	// Disable the disabling of the Timer1
	PRR &= ~(1 << PRTIM1);

	// Disconnect the OC1A/B from the output compare unit
	TCCR1A &= ~(1 << COM1A1 | 1 << COM1A0 | 1 << COM1B1 | 1 << COM1B0);
	// CTC mode, TOP = OCR1A
	TCCR1A &= ~(1 << WGM11 | 1 << WGM10);
	TCCR1B &= ~(1 << WGM13);
	TCCR1B |= 1 << WGM12;

	// Disable input capture
	TCCR1B &= ~(1 << ICNC1 | 1 << ICES1);
	// No prescaling
	TCCR1B &= ~(1 << CS12 | 1 << CS11);
	TCCR1B |= 1 << CS10;

	// No output
	TCCR1C &= ~(1 << FOC1A | 1 << FOC1B);

	// Settings OCR1A to 16000: then, the TIM1_COMPA int will be trigered every
	// millisecond
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		OCR1A = 16000;
	}

	// Enables only the Output compare A match interrupt
	TIMSK1 = 0;
	TIMSK1 |= 1 << OCIE1A;

	// Enable the interruptions
	sei();
}

/*
 * Function eon_init
 * Desc 	Return the number of milliseconds
 * 			since init of eon library.
 * Output 	The number of ms, an unsigned,
 * 			32bits number.
 */
uint32_t eon_millis(void) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		return (uint32_t)eon_milliseconds;
	}
	return 0;
}

ISR(TIMER1_COMPA_vect) {
	++ eon_milliseconds;
}
