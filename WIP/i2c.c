# include "i2c.h"



/*
 * Function i2c_masterInit
 * Desc 	initialize the TWI clock, in order to have
 * 			an SCL frequency equal to I2C_FREQUENCY, and 
 * 			then enable the TWI	module, and the 
 * 			acknoledgments.
 */
void i2c_masterInit(void) {
	// Set the PC4 and PC5 (SCL & SDA) ios as input, pull-ups enabled
	DDRC &= ~(1 << PC4 | 1 << PC5);
	PORTC |= 1 << PC4 | 1 << PC5;

	// Set the TWI clock
	TWSR = I2C_PRESCALER;
	TWBR = I2C_TWBR;

	// Enable the TWI module
	TWCR = 1 << TWEN ;
}

/*
 * Function i2c_start
 * Desc 	Send a start condition, wait for it to be
 * 			transmitted, and then send the address, with
 * 			the read / write bit set. Finally, wait for
 * 			an acknoledgment to be received.
 * Input 	address: 7bits i2c device address plus 1bit
 * 					 read / write bit
 * Output 	I2C_OK if there all is good
 * 			I2C_ERROR if something was not correctly sent
 */
uint8_t i2c_start(uint8_t address) {
	// Send the start condition
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;

	// Wait for the start condition to be transmitted
	while ( !(TWCR & (1 << TWINT)));

	// Check if the start condition has been succefully transmitted
	if ((TWSR & 0xF8) != 0x08)
		return I2C_ERROR;

	// Send the sla+r/w byte
	TWDR = address;
	TWCR = 1 << TWINT | 1 << TWEN;

	// Wait for the sla+r/w byte to be transmitted
	while( !(TWCR & (1 << TWINT)));
	
	// Check if the sla+r/w byte has been transmitted, and ack received
	if((TWSR & 0xF8) != 0x18 + (address & 0x01) * 0x28)
		return I2C_ERROR;

	return I2C_OK;
}

/*
 * Function i2c_repeatedStart
 * Desc 	Send a repeated start over the 
 * 			i2c line.
 * Input 	address, the 7bits left shifted address
 * 			of the slave to read/write to, and the
 * 			read/write bit
 * Output 	I2C_ERROR if an error occured,
 * 			I2C_OK else
 */
uint8_t i2c_repeatedStart(uint8_t address) {
	// Enable twi, and send a start condition
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	
	// Wait for the start to be sent
	while( !(TWCR & (1 << TWINT)));

	// Check the status register
	if ( (TWSR & 0xF8) != 0x10)
		return I2C_ERROR;

	// Send the address byte
	TWDR = address;
	TWCR = 1 << TWINT | 1 << TWEN;

	// Wait for the sla+r/w byte to be transmitted
	while( !(TWCR & (1 << TWINT)));

	// Check if the sla+r/w byte has been transmitted, and ack received
	if((TWSR & 0xF8) != 0x18 + (address & 0x01) * 0x28)
		return I2C_ERROR;

	return I2C_OK;
}


/*
 * Function i2c_writeData
 * Desc 	Try to send a single byte to the I2C
 * 			line. The start and sla+R/W condtions
 * 			must have been sent
 * Input 	data: the byte of data to be sent
 * Output 	I2C_ERROR if an error happened, else
 * 			O2C_OK
 */
uint8_t i2c_writeData(uint8_t data) {
	// Put the data in the data register, and send it
	TWDR = data;
	TWCR = 1 << TWINT | 1 << TWEN;

	// Wait for the data to be sent
	while ( !(TWCR & (1 << TWINT)));

	// Check if there was an error
	if ((TWSR & 0xF8) != 0x28)
		return I2C_ERROR;

	return I2C_OK;
}

/*
 * Function i2c_stop
 * Desc 	Send a stop condition
 */
void i2c_stop(void) {
	TWCR = 1 << TWINT | 1 << TWEN | 1 << TWSTO;
	// Wait for the stop condition to be sent
	while ( (TWCR & (1 << TWSTO)));
}

/*
 * Function i2c_writeBytes
 * Desc 	Send an array of bytes, through the i2c
 * 			line, to the slave with the given 7bits right
 * 			shifted address. By default, stop the transmittion
 * 			after the array has been transmitted.
 * Input 	address: the address of the slave, right shifted
 * 			reg: the registersto write to
 * 			bytes: an array of bytes to send
 * 			length: the number of bytes to send
 * Ouptut: 	The number of bytes sent
 */
uint8_t i2c_writeBytes(uint8_t address, uint8_t* reg, uint8_t* bytes, uint8_t length) {
	// Start the communication as a master transmitter
	if(i2c_start(I2C_WRITE | address << 1) == I2C_ERROR)
		return 0;

	// Loop in the bytes array, to send all the byte
	uint8_t i = 0;
	uint8_t sent = 0;

	while (i < length) {
		// doesnt't send a repeated start if it's the first reg to write
		if(i != 0) {
			if (i2c_repeatedStart(I2C_WRITE | address << 1) == I2C_ERROR)
				return sent;
		}

		// load the reg in the data reg
		if (i2c_writeData( *(reg + i)) == I2C_ERROR)
			return sent;

		// load the data in the data reg
		if (i2c_writeData( *(bytes + i)))
			return sent;
		
		++i;
		++sent;
	}

	// Ends correctly the transmittion
	i2c_stop();

	return sent;
}

/*
 * Function i2c_writeByte
 * Desc 	Send a single byte to the given
 * 			address, with the given register
 * 			to write to.
 * Input 	address: the 7bits right shifted
 * 			address of the slave
 * 			reg: the 8bits register address
 * 			byte: the byte to write
 * Output  	1 if all is ok, 0 else
 */
uint8_t i2c_writeByte(uint8_t address, uint8_t reg, uint8_t byte) {
	return i2c_writeBytes(address, &reg, &byte, 1);
}

/*
 * Function i2c_readByte
 * Desc 	Read a single bit from an i2c slave
 * 			from a single register
 * Input 	address: the 7bits right shifted
 * 			slave address
 * 			reg: the 8bits address of the register
 * Ouput 	The value readen, and 0 if an error occured
 */
uint8_t i2c_readByte(uint8_t address, uint8_t reg) {
	// Initiate a transimission, first must write the reg address
	if(i2c_start(I2C_WRITE | address << 1) == I2C_ERROR)
		return 0;

	// Write the reg address
	if (i2c_writeData(reg) == I2C_ERROR)
		return 0;

	// Initiate a new transmission, as master receiver
	if (i2c_repeatedStart(I2C_READ | address << 1) == I2C_ERROR)
		return 0;

	// Ready to receive data
	TWCR = 1 << TWINT | 1 << TWEN;
	
	// Wait for the TWDR to be filled
	while ( !(TWCR & (1 << TWINT)) );

	if ((TWSR & 0xF8) != 0x58)
		return 0;

	// Read the received data and stop transmission
	uint8_t data = TWDR;
	i2c_stop();
	
	return data;
}

/* 
 * Function i2c_readBytes
 * Desc 	Read multiple register of
 * 			an i²c slave, and write them to
 * 			a given array
 * Input 	address: address of the slave, 7bits
 * 			reg: array of the registers address
 * 			out: array which will contain the 
 * 			value of the registers
 * 			length: the number of registers to read
 * Output 	The number of register read
 */
uint8_t i2c_readBytes(uint8_t address, uint8_t* reg, uint8_t* out, uint8_t length) {
	// Initiate a new transmission, first as MT
	if(i2c_start(I2C_WRITE | address << 1) == I2C_ERROR)
		return 0;

	// Loop
	uint8_t i = 0;
	uint8_t read = 0;

	while (i < length) {
		if (i != 0) {
			if(i2c_repeatedStart(I2C_WRITE | address << 1) == I2C_ERROR) {
				return read;
			}
		}
		
		// Write the register to read
		if(i2c_writeData(*reg + i) == I2C_ERROR)
			return read;

		// New transmissio, MR
		if(i2c_repeatedStart(I2C_READ | address << 1) == I2C_ERROR)
			return read;

		// Ready to receive daata in the TWDR
		TWCR = 1 << TWINT | 1 << TWEN;

		// Wait for the TWDR to be filled
		while ( !(TWCR & (1 << TWINT)) );

		if ((TWSR & 0xF8) != 0x58)
			return read;

		*(out + i) = TWDR;
		++i;
		++read;
	}

	i2c_stop();
	return read;
}

/*
 * Function i2c_burstWrite
 * Desc 	Write in a row many bytes
 * Input 	address: 7bits right-shifted
 * 			slave address
 * 			start_reg: The register to start
 * 			writing from
 * 			values: the values (8bits) to write
 * 			to the given registers
 * 			length: the amount of reg to write to
 * Output: 	The number of registers written
 */
uint8_t i2c_burstWrite(uint8_t address, uint8_t start_reg, uint8_t* values, uint8_t length) {
	// Start as MT
	if (i2c_start(I2C_WRITE | address << 1) == I2C_ERROR)
		return 0;

	// Set the first register
	if (i2c_writeData(start_reg) == I2C_ERROR)
		return 0;

	// Write all the registers
	uint8_t i = 0;
	uint8_t sent = 0;

	while( i < length) {
		i2c_writeData(*(values) + i);
		
		++i;
		++sent;
	}

	i2c_stop();

	return sent;
}

/*
 * Function i2c_burstRead
 * Desc 	Burst read a number of registers
 * 			that are next to each other
 * Input 	address: 7bits slave address, right-shifted
 * 			start_reg: 8bits register address
 * 			out: an array of bytes, to write to
 * 			length: the number of registers to read,
 * 			length must be less or equal to the length
 * 			of the array out.
 * Output 	The number of bytes read
 */
uint8_t i2c_burstRead(uint8_t address, uint8_t start_reg, uint8_t* out, uint8_t length) {
	// Initiate the transmission
	if (i2c_start(I2C_WRITE | address << 1) == I2C_ERROR)
		return 0;

	// Write the first register to read from
	if (i2c_writeData(start_reg) == I2C_ERROR)
		return 0;

	// Start MR
	if (i2c_repeatedStart(I2C_READ | address << 1) == I2C_ERROR)
		return 0;

	// Start reading
	uint8_t i = 0;
	uint8_t read = 0;
	uint8_t last = 0x00;

	while ( i < length ) {
		// If it's the last byte to read, NACK
		if (i == length - 1) {
			TWCR = 1 << TWINT | 1 << TWEN;
			last = 0x08;
		}
		// Else, ACK
		else {
			TWCR = 1 << TWINT | 1 << TWEN | 1 << TWEA;
		}
	
		// Wait for the TWDR to be filled
		while ( !(TWCR & (1 << TWINT)) );

		if ((TWSR & 0xF8) != (0x50 | last))
			return read;

		// Read the data in the receive register
		*(out + i) = TWDR;
		++i;
		++read;
	}

	i2c_stop();
	return read;
}
