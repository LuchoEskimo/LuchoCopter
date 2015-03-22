# ifndef USART_H
# define USART_H

# include <avr/io.h>
# include <avr/interrupt.h>
# include <stdint.h>

// Macros used to set the serial port
// The direction of the stream
# define USART_DIR_TX 1
# define USART_DIR_RX 2

// The parity
# define USART_PARITY_NO 0
# define USART_PARITY_ODD 1
# define USART_PARITY_EVEN 2

// The number of stop bits
# define USART_STOP_BITS_1 1
# define USART_STOP_BITS_2 2

// The amount of bits of data sent in a single packet
# define USART_DATA_BITS_5 5
# define USART_DATA_BITS_6 6
# define USART_DATA_BITS_7 7
# define USART_DATA_BITS_8 8

// Used in usart_read, in order to not to stock the error
# define USART_NO_ERROR (int8_t*)0

// Error codes
# define USART_NO_WAITING_DATA 1
# define USART_FRAME_ERROR -1
# define USART_PARITY_ERROR -2
# define USART_DATA_OVERRUN -3
# define USART_OK 0

void usart_init(uint32_t baudRate, uint8_t direction, uint8_t parity, uint8_t stopBits, uint8_t dataBits);
void usart_send(uint8_t character);
void usart_sendString(char str[]);
uint8_t usart_read(int8_t* error);
uint8_t usart_dataAvailable(void);
void usart_enableInterrupt(void);
void usart_handleError(int8_t* error, int8_t code);

# endif
