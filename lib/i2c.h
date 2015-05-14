# ifndef I2C_H
# define I2C_H

# include <avr/io.h>
# include <util/twi.h>
# include <inttypes.h>

// Used to compute the TWBR value: set I2C_FREQUENCY to the desired frequency of
// the SCL line, in Hertz, and the I2C_PRESCALER to the prescaler applied to
// F_CPU, the preprocessor will find the good TWBR value
# define I2C_FREQUENCY 400000UL
# define I2C_PRESCALER 2
# define I2C_TWBR ((((F_CPU / I2C_FREQUENCY) / I2C_PRESCALER) - 16 ) / 2)

// Error codes
# define I2C_OK 1
# define I2C_ERROR 0

# define I2C_WRITE 0x00
# define I2C_READ 0x01

void i2c_masterInit(void);

uint8_t i2c_start(uint8_t address);
uint8_t i2c_repeatedStart(uint8_t address);
uint8_t i2c_writeData(uint8_t data);
void i2c_stop(void);

uint8_t i2c_writeByte(uint8_t address, uint8_t reg, uint8_t byte);
uint8_t i2c_writeBytes(uint8_t address, uint8_t* reg, uint8_t* bytes, uint8_t number);
uint8_t i2c_burstWrite(uint8_t address, uint8_t start_reg, uint8_t* values, uint8_t length);

uint8_t i2c_readByte(uint8_t address, uint8_t reg);
uint8_t i2c_readBytes(uint8_t address, uint8_t* reg, uint8_t* out, uint8_t length);
uint8_t i2c_burstRead(uint8_t address, uint8_t start_reg, uint8_t* out, uint8_t length);

# endif
