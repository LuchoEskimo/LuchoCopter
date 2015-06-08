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

// # define MDEBUG

void init(void);

int main(void) {
    init();
    mpu9150_init(MPU9150_gyro_250, MPU9150_accel_2g);
    mpu9150_setDLPF(MPU9150_dlpf_very_high);

# ifdef MDEBUG
    char ret[100];
# endif

    // IMU
    float ax, ay, az; // Acceleration
    float wx, wy, wz; // Vitesse angulaire
    float atx, aty; // Estimation de l'angle d'après l'accéléromètre
    float tx, ty; // Estimation de l'angle

    // PID
    float roll, pitch; // Consignes en angle
    float yaw; // Consigne en vitesse angulaire
    float roll_cmd, pitch_cmd, yaw_cmd; // Les commande de sortie du PID
    float thrust; // Poussée globale des moteurs, minimale
    float max; // Poussée maximale des moteurs
    float coef;
    uint8_t front_left, front_right, back_right, back_left; // Vitesse des moteurs

    float roll_error, roll_last_error, roll_total_error;
    float pitch_error, pitch_last_error, pitch_total_error;
    float yaw_error, yaw_last_error, yaw_total_error;

    roll_error = roll_last_error = roll_total_error = 0;
    pitch_error = pitch_last_error = pitch_total_error = 0;
    yaw_error = yaw_last_error = yaw_total_error = 0;
    
    thrust = 20;
    max = 40;
    coef = max - thrust;
    coef = coef / 600;

    yaw = pitch = roll = 0;

    int16_t start; // Date du début de chaque itération

    uint8_t iter = 0; // Nombre d'itération depuis la dernière MàJ

    // Estimation initiale
    tx = ty = 0;

    // Mise en route des moteurs
    for(uint8_t i = 0; i < 10; ++i) {
        motors_setSpeed(MOTORS_FL, i);
        motors_setSpeed(MOTORS_FR, i);
        motors_setSpeed(MOTORS_BR, i);
        motors_setSpeed(MOTORS_BL, i);
        _delay_ms(50);
    }

    for(;;) {
        start = eon_millis();
        ++ iter;

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
        pitch_cmd = Kp * pitch_error + Ki * pitch_total_error + Kd * wx;
        yaw_cmd = iKp * yaw_error + iKi * yaw_total_error + iKd * wx;

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
        front_left = thrust + (roll_cmd - pitch_cmd) / 2;
        front_right = thrust + (-roll_cmd - pitch_cmd) / 2;
        back_right = thrust + (-roll_cmd + pitch_cmd) / 2;
        back_left = thrust + (roll_cmd + pitch_cmd) / 2;

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

        while(eon_millis() - start < dt);
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
    motors_calibrate();
    motors_stopAll();

    // I2C init
    i2c_masterInit();

    _delay_ms(1000);
}
