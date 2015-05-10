# include "mpu9150.h"

/*
 * Function mpu9150_init
 * Desc     Initialize the i2c connection, using
 *          the i2c lib, and then set the mpu9150
 *          up.
 */
void mpu9150_init() {
    i2c_masterInit();

    mpu9150_setSleep(0);
    mpu9150_setClockSource(MPU9150_gyro_x_clk);
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
    if( tmp & MPU9150_clksels != clk_src ){
        i2c_writeByte(MPU9150_address, MPU9150_pwr_mgmt_1, (tmp & ~MPU9150_clksels) | clk_src);
    }
}
