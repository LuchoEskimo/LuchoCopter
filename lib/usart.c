# include "usart.h"

// Function used to set the serial port
void usart_init(uint32_t baudRate, uint8_t direction, uint8_t parity, uint8_t stopBits, uint8_t dataBits) {
	// Asynchronuous mode
	UCSR0C &= ~( (1 << UMSEL01) | (1 << UMSEL00) | (1 << UCPOL0) );

	// Double speed disabled
	UCSR0A &= ~(1 << U2X0);

	// Direction: tx, rx or rx&tx
	switch(direction) {
		case USART_DIR_TX:
			UCSR0B = 1 << TXEN0;
			break;
		case USART_DIR_RX:
			UCSR0B = 1 << RXEN0;
			break;
		case USART_DIR_RX | USART_DIR_TX:
			UCSR0B = (1 << RXEN0) | (1 << TXEN0);
			break;
	}

	// Parity
	switch(parity) {
		case USART_PARITY_NO:
			UCSR0C &= ~( (1 << UPM00) | (1 << UPM01) );
			break;
		case USART_PARITY_ODD:
			UCSR0C |= (1 << UPM01) | (1 << UPM00);
			break;
		case USART_PARITY_EVEN:
			UCSR0C |= 1 < UPM01;
			UCSR0C &= ~(1 << UPM00);
			break;
	}

	//Stop bits
	switch (stopBits) {
		case USART_STOP_BITS_1:
			UCSR0C &= ~(1 << USBS0);
			break;
		case USART_STOP_BITS_2:
			UCSR0C |= 1 << USBS0;
			break;
	}

	// Data bits
	UCSR0C &= 0b11111000; // Clear the 3 LSB
	UCSR0C |= (dataBits - 5) << UCSZ00; // Set the data bits

	// Baud rate
	// Compute the ubrr value
	uint16_t ubrr = F_CPU / 16 / baudRate - 1;
	// Set the register
	UBRR0H = (uint8_t) ubrr >> 8;
	UBRR0L = (uint8_t) ubrr;
}

// Sends a single ascii character, 8bits. It's a blocking task
void usart_send(uint8_t character) {
	// Wait for the data register to be empty
	while ( !(UCSR0A & (1 << UDRE0)) );

	// Put the character in the data register, asking the µC to send it asap
	UDR0 = character;
}

// Sends an array of char. Uses usart_send, so is also a blocking task
void usart_sendString(char str[]) {
	do {
		usart_send(*str);
	} while( *str ++ != '\0');
}

// Check for data in the receive buffer, if not returns 0 and set error to USART_NO_WAITING_DATA. If
// an error occurred, set error to USART_FRAME_ERROR if it's a Frame error (error in the stop
// bits), USART_PARITY_ERROR for a parity error, and USART_DATA_OVERRUN if there is a data overrun. Then, it
// returns 0. If there isn't any error, return the next character of the receive
// buffer and set error to USART_OK. If error is set to USART_NO_ERROR, doesn't 
uint8_t usart_read(int8_t* error) {
	if ( !(UCSR0A & (1 << RXC0)) ) {
		usart_handleError(error, USART_NO_WAITING_DATA);
		return 0;
	}

	else {
		if (UCSR0A & (1 << FE0)) {
			usart_handleError(error, USART_FRAME_ERROR);
			return 0;
		}

		else if (UCSR0A & (1 << UPE0)) {
			usart_handleError(error, USART_PARITY_ERROR);
			return 0;
		}

		else if (UCSR0A & (1 << DOR0)) {
			usart_handleError(error, USART_DATA_OVERRUN);
			return UDR0;
		}

		else {
			usart_handleError(error, USART_OK);
			return UDR0;
		}
	}
}

// Returns 1 if there is waiting data in UDR0
uint8_t usart_dataAvailable(void) {
	return (UCSR0A & (1 << RXC0)) >> RXC0;
}

// Enable the Reception Complete Interrupt, emitted when there is a new byte in
// the UDR buffer. The UART0_RX_vect interrupt routine must be implemented
void usart_enableInterrupt(void) {
	sei();
	UCSR0B |= 1 << RXCIE0;
}

void usart_handleError(int8_t* error, int8_t code) {
	if (error != USART_NO_ERROR) {
		*error = code;
	}
}
