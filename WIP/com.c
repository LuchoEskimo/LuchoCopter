# include <avr/io.h>
# include <util/delay.h>
# include <stdio.h>

# include "../lib/usart.h"
# include "../lib/eon.h"
# include "../lib/motors.h"

# include "i2c.h"

void init(void);

int main(void) {
    init();

    uint8_t state = 0;
    uint8_t tmp;
    uint8_t centaines, dixaines, unites;
    centaines = dixaines = unites = 0;
    uint8_t vitesse = 0;
    char out[10];

    usart_sendString("C'est parti mon kiki :)\n");

    for(;;) {
        if( usart_dataAvailable() ) {
            tmp = usart_read(0);
            if( tmp == '$' && state == 0 ){
                state = 1;
                usart_sendString("s1\n");
            }
            else if( state == 1 ){
                centaines = tmp - '0';
                state = 2;
                usart_sendString("s2\n");
            }
            else if( state == 2 ){
                dixaines = tmp - '0';
                state = 3;
                usart_sendString("s3\n");
            }
            else if( state == 3) {
                unites = tmp - '0';
                state = 4;
                usart_sendString("s4\n");
            }
            else if( tmp == '$' && state == 4) {
                vitesse = 100 * centaines + 10 * dixaines + unites;
                sprintf(out, "%u\n", vitesse);
                usart_sendString(out);
                motors_setSpeed(MOTORS_BL, vitesse);
                state = 0;
            }
        }
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
    motors_init();

    // I2C init
    i2c_masterInit();

    _delay_ms(50);
}
