# include <avr/io.h>
# include <util/delay.h>
# include <stdio.h>
# include <math.h>

# include "../lib/usart.h"
# include "../lib/eon.h"
# include "../lib/motors.h"
# include "../lib/i2c.h"

# include "mpu9150.h"
# include "fixed.h"

# define mpu 0x68

# define dt 10
# define Kc 0.90f
# define Kp 10
# define Ki 0
# define Kd 0
# define iKp 3
# define iKi 0
# define iKd 0

# define MIN_THRUST 5
# define MAX_THRUST 80

# define MAX_ANGLE  30
# define MAX_YAW    60

# define TURNED_OFF 0
# define SLOW       1
# define ON         2

# define CMD        0
# define DATA       1

# define BUF_LENGTH 20

// # define MDEBUG

void init(void);
void demarrer();

volatile uint8_t uart_state = CMD;
volatile uint8_t uart_addr;
volatile uint8_t receivedChar;

// Pid
volatile float roll, pitch, yaw, thrust;

volatile uint8_t state = TURNED_OFF;

int main(void) {
    init();

# ifdef MDEBUG
    char ret[100];
# endif

    // IMU
    float ax, ay, az; // Acceleration
    float wx, wy, wz; // Vitesse angulaire
    float atx, aty; // Estimation de l'angle d'après l'accéléromètre
    float tx, ty; // Estimation de l'angle

    // PID
    float roll_cmd, pitch_cmd, yaw_cmd; // Les commande de sortie du PID
    float coef;
    uint8_t front_left, front_right, back_right, back_left; // Vitesse des moteurs

    float roll_error, roll_last_error, roll_total_error;
    float pitch_error, pitch_last_error, pitch_total_error;
    float yaw_error, yaw_last_error, yaw_total_error;

    roll_error = roll_last_error = roll_total_error = 0;
    pitch_error = pitch_last_error = pitch_total_error = 0;
    yaw_error = yaw_last_error = yaw_total_error = 0;
    
    thrust = MIN_THRUST;
    coef =  MAX_THRUST - MIN_THRUST;
    coef = coef / 600;

    yaw = pitch = roll = 0;

    int16_t start; // Date du début de chaque itération

# ifdef MDEBUG
    uint8_t iter = 0; // Nombre d'itération depuis la dernière MàJ
#endif

    // Estimation initiale
    tx = ty = 0;

    for(;;) {
        start = eon_millis();

        if( state == TURNED_OFF ) {
            motors_stopAll();
        }

        else if( state == SLOW ) {
            if( (motors_getSpeed(MOTORS_FL) == 0 ) &&
                (motors_getSpeed(MOTORS_FR) == 0 ) &&
                (motors_getSpeed(MOTORS_BL) == 0 ) &&
                (motors_getSpeed(MOTORS_BR) == 0 ) ) {
                demarrer();
            }

            motors_setSpeed(MOTORS_FL, MIN_THRUST);
            motors_setSpeed(MOTORS_BL, MIN_THRUST);
            motors_setSpeed(MOTORS_BR, MIN_THRUST);
            motors_setSpeed(MOTORS_FR, MIN_THRUST);
        }

        else if( state == ON ) {
# ifdef MDEBUG
            ++ iter;
# endif

            if( (motors_getSpeed(MOTORS_FL) == 0 ) &&
                (motors_getSpeed(MOTORS_FR) == 0 ) &&
                (motors_getSpeed(MOTORS_BL) == 0 ) &&
                (motors_getSpeed(MOTORS_BR) == 0 ) ) {
                demarrer();
            }

            // Acquisitions des valeurs brutes
            ax = mpu9150_getAccelX();
            ay = mpu9150_getAccelY();
            az = mpu9150_getAccelZ();

            wx = mpu9150_getGyroX();
            wy = mpu9150_getGyroY();
            wz = mpu9150_getGyroZ();

            // Calcul des angles
            aty = atan2(ax, sqrt(pow(ay,2) + pow(az,2))) * 180 / M_PI;
            atx = atan2(-ay, sqrt(pow(ax, 2) + pow(az,2))) * 180 / M_PI;
            tx = Kc * (tx + wx * (dt * 0.001)) + (1 - Kc) * atx;
            ty = Kc * (ty + wy * (dt * 0.001)) + (1 - Kc) * aty;

            // Calcul des erreurs
            cli();
            roll_last_error = roll_error;
            roll_error = roll - tx;
            roll_total_error += roll_error * (dt * 0.001);

            pitch_last_error = pitch_error;
            pitch_error = pitch - ty;
            pitch_total_error += pitch_error * (dt * 0.001);

            yaw_last_error = yaw_error;
            yaw_error = yaw - wz;
            yaw_total_error += yaw_error * (dt * 0.001);

            roll_cmd = Kp * roll_error + Ki * roll_total_error + Kd * wx;
            pitch_cmd = Kp * pitch_error + Ki * pitch_total_error + Kd * wy;
            yaw_cmd = iKp * yaw_error + iKi * yaw_total_error + iKd * wz;

            // Normalise les commandes
            if ( roll_cmd < -300.0 )
                roll_cmd = -300.0;
            else if ( roll_cmd > 300.0 )
                roll_cmd = 300.0;
            if ( pitch_cmd < -300.0 )
                pitch_cmd = -300.0;
            else if ( pitch_cmd > 300.0 )
                pitch_cmd = 300.0;
            if ( yaw_cmd < -300 )
                yaw_cmd = -300;
            else if ( yaw_cmd > 300 )
                yaw_cmd = 300;

            // Commande des moteurs
            roll_cmd *= coef;
            pitch_cmd *= coef;
            yaw_cmd *= coef;

            // Action !
            front_left = thrust + (roll_cmd - pitch_cmd + yaw) / 3;
            front_right = thrust + (-roll_cmd - pitch_cmd - yaw) / 3;
            back_right = thrust + (-roll_cmd + pitch_cmd + yaw) / 3;
            back_left = thrust + (roll_cmd + pitch_cmd - yaw) / 3;

            sei();

            ///*
            if( front_left < MIN_THRUST)
                front_left = MIN_THRUST;
            if( front_right < MIN_THRUST)
                front_right = MIN_THRUST;
            if( back_right < MIN_THRUST)
                back_right = MIN_THRUST;
            if( back_left < MIN_THRUST)
                back_left = MIN_THRUST;

            if( front_left > MAX_THRUST)
                front_left = MAX_THRUST;
            if( front_right > MAX_THRUST)
                front_right = MAX_THRUST;
            if( back_right > MAX_THRUST)
                back_right = MAX_THRUST;
            if( back_left > MAX_THRUST)
                back_left = MAX_THRUST;
            //*/

            motors_setSpeed(MOTORS_FL, front_left);
            motors_setSpeed(MOTORS_FR, front_right);
            motors_setSpeed(MOTORS_BR, back_right);
            motors_setSpeed(MOTORS_BL, back_left);

            // Affichage des résultats
            # ifdef MDEBUG
            if( iter % 100 == 0 ) {
                sprintf(ret, "FL: %d FR: %d BR: %d BL: %d\n",
                        (int8_t)front_left, (int8_t)front_right, (int8_t)back_right, (int8_t)back_left);
                usart_sendString(ret);
                iter = 0;
            }

            # endif
        }

        while(eon_millis() - start < dt);
    }

    return 0;
}

void init() {
    // Usart init
    usart_init(9600, USART_DIR_TX | USART_DIR_RX, USART_PARITY_NO,
               USART_STOP_BITS_1, USART_DATA_BITS_8);
    usart_enableInterrupt();
    _delay_ms(20);

    // Eon init
    eon_init();

    // Motors init
    motors_init();
    motors_calibrate();
    motors_stopAll();

    // I2C init
    i2c_masterInit();

    // MPU9150 init
    mpu9150_init(MPU9150_gyro_250, MPU9150_accel_2g);
    mpu9150_setDLPF(MPU9150_dlpf_very_high);


    _delay_ms(1000);
}

ISR(USART_RX_vect) {
    receivedChar = usart_read(0);

    if( uart_state == CMD ) {
        // Check if it's a real CMD byte
        if( (receivedChar & 0b11100000) == 0b10100000 ) {
            uart_state = DATA;
            uart_addr = receivedChar & 0b00011111;
        }
    }

    else if( uart_state == DATA ) {
        uart_state = CMD;

        switch( uart_addr ) {
            case 0x10:
                thrust = (float)(uint8_t)receivedChar;
                thrust = MIN_THRUST + thrust * ( (MAX_THRUST - MIN_THRUST) / 100.0f);
                break;
            case 0x11:
                yaw = (float)(int8_t)receivedChar;
                yaw = yaw * (MAX_YAW / 50.0f);
                break;
            case 0x12:
                pitch = (float)(int8_t)receivedChar;
                pitch = pitch * (MAX_ANGLE / 50.0f);
                break;
            case 0x13:
                roll = (float)(int8_t)receivedChar;
                roll = roll * (MAX_ANGLE / 50.0f);
                break;
            case 0x14:
                if( receivedChar == 1 ) {
                    state = TURNED_OFF;
                }
                break;
            case 0x15:
                if( receivedChar == 1 ) {
                    state = ON;
                }
                break;
            case 0x16:
                if( receivedChar == 1 ) {
                    state = SLOW;
                }
                break;
        }
    }
}

void demarrer(void) {
    for( uint8_t i = 0; i < MIN_THRUST; ++i ) {
        motors_setSpeed(MOTORS_FL, i);
        motors_setSpeed(MOTORS_BL, i);
        motors_setSpeed(MOTORS_BR, i);
        motors_setSpeed(MOTORS_FR, i);
        _delay_ms(100);
    }
}
