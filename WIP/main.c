# include <avr/io.h>
# include <util/delay.h>
# include <stdio.h>

# include "../lib/usart.h"
# include "../lib/eon.h"
# include "../lib/motors.h"
# include "../lib/i2c.h"

# define mpu 0x68

void init(void);
void mpu9150_init(void);

int main(void) {
    init();
    mpu9150_init();
    DDRD |= 1 << PD5;
    _delay_ms(100);

    char ret[90];

    uint16_t temps;
    uint8_t buffer[100];
    for( uint8_t i = 0; i < 100; ++i ){ buffer[i] = 0; }
    uint8_t reg[100];
    for( uint8_t i = 0; i < 100; ++i ){ reg[i] = i + 0x23;}
    uint16_t debut = eon_millis();
    i2c_burstWrite(mpu, reg[0], buffer, 18);
    temps = eon_millis() - debut;
    sprintf(ret, "%d\n", temps);
    usart_sendString(ret);

    uint8_t out[6];
    int16_t ax, ay, az;

    for(;;) {
        /*
        i2c_burstRead(mpu, 0x43, out, 6);

        ax = (int16_t)(out[0] << 8 | out[1]);
        ay = (int16_t)(out[2] << 8 | out[3]);
        az = (int16_t)(out[4] << 8 | out[5]);

        sprintf(ret, "X: %d\tY: %d\tZ: %d\n", ax, ay, az);
        usart_sendString(ret);
        _delay_ms(1000);
        //*/
    }

    return 0;
}

void init() {
    // Usart init
    usart_init(9600, USART_DIR_TX | USART_DIR_RX, USART_PARITY_NO,
               USART_STOP_BITS_1, USART_DATA_BITS_8);
    _delay_ms(20);

    // Eon init
    eon_init();

    // Motors init
    //motors_init();

    // I2C init
    i2c_masterInit();

    _delay_ms(50);
}

void mpu9150_init(void) {
    uint8_t reg[]   = {0x6B, 0x37, 0x6A, 0x1B, 0x1C, 0x19};
    uint8_t bytes[] = {0x01, 0X00, 0x00, 0x00, 0x00, 0X00};

    uint8_t resultat = i2c_writeBytes(mpu, reg, bytes, 6);

    if( resultat != 6 ) {
        usart_sendString("Et meeerde...\n");
    }
}
