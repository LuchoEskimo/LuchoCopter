# include "usart.h"

// Function used to set the serial port
/*
 * Function usart_init
 * Desc 	Set the serial port
 * Input 	baudRate: the baudRate, the speed
 * 			of the transmission in bauds
 * 			direction: either USART_DIR_[TX|RX],
 * 			or USART_DIR_TX | USART_DIR_RX
 * 			parity: USART_PARITY_[NO|EVEN|ODD]
 * 			stopBits: USART_STOP_BITS_[1|2]
 * 			dataBits: The number of bits par 
 * 			transmitted packet, USART_DATA_[5...8]
 */
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

/*
 * Function usart_send
 * Desc 	Send a single ascii character, but
 * 			be cautious: it's a blocking function
 * Input 	character: an 8bits ascii character
 * 			to send
 */
void usart_send(uint8_t character) {
	// Wait for the data register to be empty
	while ( !(UCSR0A & (1 << UDRE0)) );

	// Put the character in the data register, asking the µC to send it asap
	UDR0 = character;
}

/*
 * Function usart_sendString
 * Desc 	Send an array of ascii character (a string).
 * 			It's based on usart_send, so it's also
 * 			a blocking function
 * Input 	str: a C-style (ended with '\0') array
 * 			of char, encodedd in ascii.
 */
void usart_sendString(char str[]) {
	do {
		usart_send(*str);
	} while( *str ++ != '\0');
}

/*
 * Function usart_read
 * Desc 	Check for ready to be read character
 * 			in the receive buffer: if so, returns
 * 			them
 * Input 	error: a pointer, used to hold the
 * 			error status of the function.
 * 			-no waiting data: USART_NO_WAITING_DATA
 * 			-error in the stop bits: USART_FRAME_ERROR
 * 			-error the parity bit: USART_PARITY_ERROR
 * 			-to much data in the buffer: USART_DATA_OVERRUN
 * 			-USART_OK else
 * Output 	The read character
 */
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

/*
 * Function usart_dataAvailable
 * Desc 	Inform if there is either data available
 * 			or not
 * Return 	1 if the receive buffer isn't empty, 0 else
 */
uint8_t usart_dataAvailable(void) {
	return (UCSR0A & (1 << RXC0)) >> RXC0;
}

/* 
 * Function usart_enableInterrupt
 * Desc 	Enables the Reception Complete Interrupt:
 * 			it's triggered when there is a new byte
 * 			in the receive buffer. Be cautious, the
 * 			UART0_RX_vect ISR must be implemented.
 */
void usart_enableInterrupt(void) {
	UCSR0B |= 1 << RXCIE0;
	sei();
}

void usart_handleError(int8_t* error, int8_t code) {
	if (error != USART_NO_ERROR) {
		*error = code;
	}
}
