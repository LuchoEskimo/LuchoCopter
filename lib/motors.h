# ifndef MOTORS_H
# define MOTORS_H

# include <avr/io.h>
# include <inttypes.h>
# include <util/delay.h>


// The number of motors
# define MOTORS_NUMBER 4

// Pin's macros, for setting IO's
# define MOTORS_PIN_FR PD5
# define MOTORS_PIN_BR PD3
# define MOTORS_PIN_BL PD6
# define MOTORS_PIN_FL PB3

// Motors' timer's macros, for setting them
# define MOTORS_TIMER_FR OCR0B
# define MOTORS_TIMER_BR OCR2B
# define MOTORS_TIMER_BL OCR0A
# define MOTORS_TIMER_FL OCR2A

// Motors' macros, for end-user's use
# define MOTORS_FR 1
# define MOTORS_BR 2
# define MOTORS_BL 3
# define MOTORS_FL 4


// Macros used in order to specify the speed

// Values used in order to calibrate the motors. They correspond to the extrem
// timers' values caused by the extrem position of the joystick / pot
# define MOTORS_LOWEST_SPEED 110
# define MOTORS_HIGHEST_SPEED 245

// Value for extrem speed: below, the motors won't spin, and above theyr speed
// won't change
# define MOTORS_MIN_SPEED 125
# define MOTORS_MAX_SPEED 225

// Coefficient used to compute the OCR value from the speed in percent, or make
// the inverse process
# define MOTORS_SPEED_COEF ((MOTORS_MAX_SPEED - MOTORS_MIN_SPEED) / 100)


void motors_init(void);
void motors_calibrate(void);
void motors_setSpeed(uint8_t motor, uint8_t speedPercent);
uint8_t motors_getSpeed(uint8_t motor);
void motors_stop(uint8_t motor);
void motors_stopAll(void);

# endif
