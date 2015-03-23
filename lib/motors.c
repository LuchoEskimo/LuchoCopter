# include "motors.h"

/*
 * Function motors_init
 * Desc 	Init the timer0 and timer1,
 * 			in order to control the four 
 * 			motors.
 */
void motors_init(void) {
	// Set the motors' pin as outputs
	DDRD |= (1 << MOTORS_PIN_FR) | (1 << MOTORS_PIN_BR) | (1 << MOTORS_PIN_BL);
	DDRB |= (1 << MOTORS_PIN_FL);

	// Setting of the timers
	
	// Disable the disabling of the timers
	PRR &= ~((1 << PRTIM0) | (1 << PRTIM2));
	// PWM, phase correct, top = 0xFF
	TCCR0A = 1 << WGM00;
	TCCR2A = 1 << WGM20;
	// Cleared on compare match when up counting, and set when down counting
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
	TCCR2A |= (1 << COM2A1) | (1 << COM2B1);
	// Set the clock to F_CPU / 64, thus the pwm frequency will be ~488Hz
	TCCR0B = (1 << CS01) | (1 << CS00);
	TCCR2B = 1 << CS22;
	// Set the OCR registers to lowest speed
	MOTORS_TIMER_FR = MOTORS_LOWEST_SPEED;
	MOTORS_TIMER_BR = MOTORS_LOWEST_SPEED;
	MOTORS_TIMER_BL = MOTORS_LOWEST_SPEED;
	MOTORS_TIMER_FL = MOTORS_LOWEST_SPEED;
}

/*
 * Function motors_calibrate
 * Desc 	This function calibrate the ESCs: it
 * 			set them, giving them the MOTORS_LOWEST_SPEED
 * 			and MOTORS_HIGHEST_SPEED
 */
void motors_calibrate(void) {
	// The function takes 500ms to go from MOTORS_HIGHEST_SPEED to
	// MOTORS_LOWEST_SPEED.

	// Compute the gap between each decrementation
	uint8_t dt = (500) / (MOTORS_HIGHEST_SPEED - MOTORS_LOWEST_SPEED);
	// Value which change during the function
	uint8_t ocr = MOTORS_HIGHEST_SPEED;

	// Start with all the motors to the maximum value
	MOTORS_TIMER_FR = ocr;
	MOTORS_TIMER_BR = ocr;
	MOTORS_TIMER_BL = ocr;
	MOTORS_TIMER_FL = ocr;
	
	// Waits 5s, in order to the ESCs to be ready
	_delay_ms(1000);

	for(; ocr >= MOTORS_LOWEST_SPEED; --ocr) {
		MOTORS_TIMER_FR = ocr;
		MOTORS_TIMER_BR = ocr;
		MOTORS_TIMER_BL = ocr;
		MOTORS_TIMER_FL = ocr;
		_delay_ms(dt);
	}
}

/* 
 * Function motors_setSpeed
 * Desc 	This function set the speed of one motor.
 * Input 	motor: the macro MOTORS_XX representing
 * 			the motor
 * 			speed: the speed in percents
 */
void motors_setSpeed(uint8_t motor, uint8_t speedPercent) {
	// If the speed, or the motor, is not good, stop right now
	if (speedPercent > 100 || speedPercent < 0 || motor < 1 || motor > 4) {
		return;
	} 
	
	else {
		// The value of OCRnx corresponding to the wanted speed
		uint8_t ocrSpeed;

		if (speedPercent == 0) {
			ocrSpeed = MOTORS_LOWEST_SPEED;
		} else {
			// Compute f(speedPercent) = OCRnx
			ocrSpeed = MOTORS_MIN_SPEED + (speedPercent * MOTORS_SPEED_COEF);
		}

		switch (motor) {
			case MOTORS_FR:
				MOTORS_TIMER_FR = ocrSpeed;
				break;
			case MOTORS_BR:
				MOTORS_TIMER_BR = ocrSpeed;
				break;
			case MOTORS_BL:
				MOTORS_TIMER_BL = ocrSpeed;
				break;
			case MOTORS_FL:
				MOTORS_TIMER_FL = ocrSpeed;
				break;
		}
	}
}

/*
 * Function motors_getSpeed
 * Desc 	Returns the speed, in percent, of the
 * 			given motor
 * Input 	motor: the macro MOTORS_XX
 * Output 	If the speed is in [0; 100]: the speed
 * 			If the speed is above MOTORS_MAX_SPEED:
 * 			above 100
 * 			If the value is out of [highest; lowest]
 * 			range: 201
 * 			If the id of the motors doesn't match any
 * 			existing motor, 202
 */
uint8_t motors_getSpeed(uint8_t motor) {
	uint8_t ocrSpeed;
	switch (motor) {
		case MOTORS_FR:
			ocrSpeed = MOTORS_TIMER_FR;
			break;
		case MOTORS_BR:
			ocrSpeed = MOTORS_TIMER_BR;
			break;
		case MOTORS_BL:
			ocrSpeed = MOTORS_TIMER_BL;
			break;
		case MOTORS_FL:
			ocrSpeed = MOTORS_TIMER_FL;
			break;
		default:
			return 202;
	}

	// If out of main range
	if (ocrSpeed > MOTORS_HIGHEST_SPEED || ocrSpeed < MOTORS_LOWEST_SPEED) {
		return 201;
	}

	// If the motor doesn't spin
	else if (ocrSpeed >= MOTORS_LOWEST_SPEED && ocrSpeed < MOTORS_MIN_SPEED) {
		return 0;
	}

	else {
		return (ocrSpeed - MOTORS_MIN_SPEED) / MOTORS_SPEED_COEF;
	}
}

/*
 * Function motors_stop
 * Desc 	Stop a given motor
 * Input 	motor: the id of the motor
 */
void motors_stop(uint8_t motor) {
	motors_setSpeed(motor, 0);
}

/*
 * Function motors_stopAll
 * Desc 	Stop all the motors of the 
 * 			quadcopter
 */
void motors_stopAll(void) {
	motors_stop(MOTORS_FR);
	motors_stop(MOTORS_BR);
	motors_stop(MOTORS_BL);
	motors_stop(MOTORS_FL);
}
