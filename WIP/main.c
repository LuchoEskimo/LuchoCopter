# include <avr/io.h>
# include <util/delay.h>
# include <stdio.h>

# include "../lib/usart.h"
# include "../lib/eon.h"
# include "../lib/motors.h"
# include "../lib/i2c.h"

# include "mpu9150.h"

# define mpu 0x68

void init(void);
//void mpu9150_init(void);

void displayFixed10(fixed16_10 f) {
    float a = fixed16_10toFloat(f);
    int16_t integer = a;
    float fractionnal = a - integer;
    int16_t frac = (int16_t)(fractionnal * 1000.0f);
    char ret[10];
    sprintf(ret, "%d,%03d\n", integer, frac);
    usart_sendString(ret);
}

int main(void) {
    init();
    mpu9150_init(MPU9150_gyro_250, MPU9150_accel_2g);


    char ret[30];
    float ax;
    int16_t fax;
    uint16_t debut, temps;

    displayFixed10(floatToFixed16_10(10.159f));

    /*
    for(int16_t i = -20; i <= 20; i += 1) {
        fixed16_2 value = (int16_t) i << 1;
        debut = eon_millis();
        float fp = fixed16_2toFloat(value);
        temps = eon_millis() - debut;
        sprintf(ret, "%d.%d\t%d\n", (int16_t)fp, ~(int16_t)((fp - (int16_t)fp) * 4.0f) + 1, temps);
        usart_sendString(ret);
    } //*/

    for(;;) {
        /*
        debut = eon_millis();
        ax = mpu9150_getAccelX();
        temps = eon_millis() - debut;
        fax = (int16_t)(ax * 1000.0f);

        sprintf(ret, "X: %d\t(%d)\n", fax, temps);
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

/*
void mpu9150_init(void) {
    uint8_t reg[]   = {0x6B, 0x37, 0x6A, 0x1B, 0x1C, 0x19};
    uint8_t bytes[] = {0x01, 0X00, 0x00, 0x00, 0x00, 0X00};

    uint8_t resultat = i2c_writeBytes(mpu, reg, bytes, 6);

    if( resultat != 6 ) {
        usart_sendString("Et meeerde...\n");
    }
}//*/
