# include <avr/io.h>
# include <util/delay.h>
//# include <stdio.h>

# include "../lib/usart.h"
# include "../lib/eon.h"
# include "../lib/motors.h"
# include "../lib/i2c.h"

# include "mpu9150.h"
# include "fixed.h"

# define mpu 0x68

void init(void);

int main(void) {
    init();
    mpu9150_init(MPU9150_gyro_250, MPU9150_accel_2g);


    //char ret[50];
    fixed16_10 a[3];
    fixed16_6 angleX, angleY;

    for(;;) {
        mpu9150_getAccelXYZ(a);
        angleX = fx_atan2(a[2], a[0]);
        angleY = fx_atan2(a[2], a[1]);

        usart_send((usartX / 100) % 10); usart_send((angleX / 10) % 10); usart_send(angleX % 10); usart_send('\n');

        //sprintf(ret, "X: %d\tY: %d\n", angleX >> 6, angleY >> 6);
        //usart_sendString(ret);

        _delay_ms(1000);
    }//*/

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
