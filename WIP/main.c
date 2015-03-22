# include <avr/io.h>
# include <stdio.h>
# include <util/delay.h>
# include <math.h>
# include "../lib/eon.h"
# include "../lib/motors.h"
# include "../lib/usart.h"
# include "i2c.h"

const uint8_t mpu9150_addr = 0x68;

void init(void);
uint8_t start(void);
uint8_t adress(uint8_t slarw);

int main(void) {
	init();

	PORTD ^= 1 << PD5;
	char tmp[10];
	sprintf(tmp, "%#x\r\n", i2c_start(I2C_READ | mpu9150_addr << 1));
	i2c_writeData(0x3B);
	PORTD ^= 1 << PD5;
	usart_sendString(tmp);


	return 0;
}

void init(void) {
	// Lib initializations
	usart_init(9600, USART_DIR_TX, USART_PARITY_NO, USART_STOP_BITS_1, USART_DATA_BITS_8);
	eon_init();
	i2c_masterInit();

	// IO init
	DDRD = 1 << PD5;
	PORTD = 0;

	_delay_ms(200);
}
