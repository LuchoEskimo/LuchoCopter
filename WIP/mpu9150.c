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
    const uint8_t tmp = i2c_readByte(MPU9150_address, MPU9150_pwr_mgmt_1);
    
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
    const uint8_t tmp = i2c_readByte(MPU9150_address, MPU9150_pwr_mgmt_1);

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
        const uint8_t tmp = i2c_readByte(MPU9150_address, MPU9150_gyro_config);
        i2c_writeByte(MPU9150_address, MPU9150_gyro_config, (tmp & ~MPU9150_fs_sels) | (scale << MPU9150_fs_sel0));
        gyroScale = scale;
    }
}

/*
 * Function mpu9150_setAccelFullScale
 * Desc     Set the accelerometer full scale
 *          range.
 * Input    MPU9150_accel_2g
 *          MPU9150_accel_4g
 *          MPU9158_accel_8g
 *          MPU9150_accel_16g
 */
void mpu9150_setAccelFullScale(uint8_t scale) {
    if( scale != accelScale ) {
        const uint8_t tmp = i2c_readByte(MPU9150_address, MPU9150_accel_config);
        i2c_writeByte(MPU9150_address, MPU9150_accel_config, (tmp & ~MPU9150_afs_sels) | (scale << MPU9150_afs_sel0));
        accelScale = scale;
    }
}

/*
 * Function mpu9150_getAccelX
 * Desc     Return the acceleration of
 *          the X axis, in g and in format
 *          fixed<16_10>
 * Ouptut   fixed16_10: the value of the acceleration
 */
fixed16_10 mpu9150_getAccelX(void) {
    // The value of the high and low register
    uint8_t registers[2];
    i2c_burstRead(MPU9150_address, MPU9150_accel_xout_h, registers, 2);

    // The whole acceleration, in bits not converted
    int16_t accelBits = (int16_t)(registers[0] << 8) | registers[1];
    
    return (fixed16_10)(((int32_t)(accelBits) << (6 + accelScale)) >> 10);
}

/* 
 * Function mpu9150_getAccelY
 * Desc     Returns the accelration following
 *          the Y axis, in fixed<16-10>
 * Output   fixed16_10: the Y acceleration
 */
fixed16_10 mpu9150_getAccelY(void) {
    uint8_t regs[2];
    i2c_burstRead(MPU9150_address, MPU9150_accel_yout_h, regs, 2);

    int16_t accelBits = (int16_t)(regs[0] << 8) | regs[1];

    return (fixed16_10)(((int32_t)(accelBits) << (6 + accelScale)) >> 10);
}

/*
 * Function mpu9150_getAccelZ
 * Desc     Returns the acceleration following
 *          the Z axis, in fixed<16-10>
 * Output   fixed16_10: the Z acceleration
 */
fixed16_10 mpu9150_getAccelZ(void) {
    uint8_t regs[2];
    i2c_burstRead(MPU9150_address, MPU9150_accel_zout_h, regs, 2);

    int16_t accelBits = (int16_t)(regs[0] << 8) | regs[1];

    return (fixed16_10)(((int32_t)(accelBits) << (6 + accelScale)) >> 10);
}

/*
 * Function mpu9150_getAccelXYZ
 * Desc     Returns the measured acceleration
 *          following the 3 axis, in the given
 *          input array
 * Input    fixed16_10*: the array to write to
 * Output   Nothin
 */
void mpu9150_getAccelXYZ(fixed16_10 *out) {
    uint8_t regs[6];
    i2c_burstRead(MPU9150_address, MPU9150_accel_xout_h, regs, 6);

    int16_t accelBits;
    for( uint8_t i = 0; i < 3; ++i) {
        accelBits = (int16_t)(regs[i<<1] << 8) | regs[(i<<1)+1];
        out[i] = (fixed16_10)(((int32_t)(accelBits) << (6+accelScale)) >> 10);
    }
}
