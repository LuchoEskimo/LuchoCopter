# include "mpu9150.h"

// The current scales
static uint8_t gyroScale = 0xFF;
static uint8_t accelScale = 0xFF;


/*
 * Function mpu9150_init
 * Desc     Initialize the i2c connection, using
 *          the i2c lib, and then set the mpu9150
 *          up.
 */
void mpu9150_init(uint8_t gyroFullScale, uint8_t accelFullScale) {
    i2c_masterInit();

    mpu9150_setSleep(0);
    mpu9150_setClockSource(MPU9150_gyro_x_clk);

    // Set to gyroScale
    mpu9150_setGyroFullScale(gyroFullScale);
    mpu9150_setAccelFullScale(accelFullScale);
}

/*
 * Function mpu9150_setSleep
 * Desc     Disable or enable sleep mode
 * Input    1 (yes) or 0 (no)
 */
void mpu9150_setSleep(uint8_t sleep) {
    uint8_t tmp = i2c_readByte(MPU9150_address, MPU9150_pwr_mgmt_1);
    
    // Check if there's nothin to do
    if( (tmp & (1 << MPU9150_sleep)) != sleep << MPU9150_sleep ) {
        i2c_writeByte(MPU9150_address, MPU9150_pwr_mgmt_1, tmp ^ 1 << MPU9150_sleep);
    }
}

/* 
 * Function mpu9150_setClockSource
 * Desc     Set the clock source of the MPU9150,
 *          either the internal oscillator, PLL
 *          with gyroscope as reference, or an
 *          external oscillator
 * Input    MPU9150_internal_osc
 *          MPU9150_gyro_x_clk
 *          MPU9150_gyro_y_clk
 *          MPU9150_gyro_z_clk
 *          MPU9150_ext_32_osc
 *          MPU9150_ext_19_osc
 */
void mpu9150_setClockSource(uint8_t clk_src) {
    uint8_t tmp = i2c_readByte(MPU9150_address, MPU9150_pwr_mgmt_1);

    // Check if there is nothing to do
    if( (tmp & MPU9150_clksels) != clk_src ){
        i2c_writeByte(MPU9150_address, MPU9150_pwr_mgmt_1, (tmp & ~MPU9150_clksels) | clk_src);
    }
}

/*
 * Function mpu9150_setGyroFullScale
 * Dest     Set the full scale range of the 
 *          gyroscope to the given range
 * Input    MPU9150_gyro_250
 *          MPU9150_gyro_500
 *          MPU9150_gyro_1000
 *          MPU9150_gyro_2000
 */
void mpu9150_setGyroFullScale(uint8_t scale) {
    if( scale != gyroScale ) {
        uint8_t tmp = i2c_readByte(MPU9150_address, MPU9150_gyro_config);
        i2c_writeByte(MPU9150_address, MPU9150_gyro_config, (tmp & ~MPU9150_fs_sels) | (scale << MPU9150_fs_sel0));
        gyroScale = scale;
    }
}

void mpu9150_setAccelFullScale(uint8_t scale) {
    if( scale != accelScale ) {
        uint8_t tmp = i2c_readByte(MPU9150_address, MPU9150_accel_config);
        i2c_writeByte(MPU9150_address, MPU9150_accel_config, (tmp & ~MPU9150_afs_sels) | (scale << MPU9150_afs_sel0));
        accelScale = scale;
    }
}

float mpu9150_getAccelX(void) {
    // The value of the high and low register
    uint8_t registers[2];
    i2c_burstRead(MPU9150_address, MPU9150_accel_xout_h, registers, 2);

    // The whole acceleration, in bits not converted
    int16_t value = (int16_t)(registers[0] << 8 | registers[1]);
    
    // The precision, 2g, 4g, 8g, or 16g
    int8_t precision = 2 << accelScale;
    float coef = (float)precision * 0.000030518f;
    float ret = coef * value;

    return ret;
}
