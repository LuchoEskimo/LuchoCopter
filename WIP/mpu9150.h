# ifndef MPU9150_H
# define MPU9150_H

# include "../lib/i2c.h"
# include "fixed.h"

// Definition of all the intenal registers of the mpu9150
# define    MPU9150_self_test_x         0x0D
# define    MPU9150_self_test_y         0x0E
# define    MPU9150_self_test_z         0x0F
# define    MPU9150_self_test_a         0x10
# define    MPU9150_smplrt_div          0x19
# define    MPU9150_config              0x1A
# define    MPU9150_gyro_config         0x1B
# define    MPU9150_accel_config        0x1C
# define    MPU9150_fifo_en             0x23
# define    MPU9150_i2c_mst_ctrl        0x24
# define    MPU9150_i2c_slv0_addr       0x25
# define    MPU9150_i2c_slv0_reg        0x26
# define    MPU9150_i2c_slv0_ctrl       0x27
# define    MPU9150_i2c_slv1_addr       0x28
# define    MPU9150_i2c_slv1_reg        0x29
# define    MPU9150_i2c_slv1_ctrl       0x2A
# define    MPU9150_i2c_slv2_addr       0x2B
# define    MPU9150_i2c_slv2_reg        0x2C
# define    MPU9150_i2c_slv2_ctrl       0x2D
# define    MPU9150_i2c_slv3_addr       0x2E
# define    MPU9150_i2c_slv3_reg        0x2F
# define    MPU9150_i2c_slv3_ctrl       0x30
# define    MPU9150_i2c_slv4_addr       0x31
# define    MPU9150_i2c_slv4_reg        0x32
# define    MPU9150_i2c_slv4_do         0x33
# define    MPU9150_i2c_slv4_ctrl       0x34
# define    MPU9150_i2c_slv4_di         0x35
# define    MPU9150_i2c_mst_status      0x36
# define    MPU9150_int_pin_cfg         0x37
# define    MPU9150_int_enable          0x38
# define    MPU9150_int_status          0x3A
# define    MPU9150_accel_xout_h        0x3B
# define    MPU9150_accel_xout_l        0x3C
# define    MPU9150_accel_yout_h        0x3D
# define    MPU9150_accel_yout_l        0x3E
# define    MPU9150_accel_zout_h        0x3F
# define    MPU9150_accel_zout_l        0x40
# define    MPU9150_temp_out_h          0x41
# define    MPU9150_temp_out_l          0x42
# define    MPU9150_gyro_xout_h         0x43
# define    MPU9150_gyro_xout_l         0x44
# define    MPU9150_gyro_yout_h         0x45
# define    MPU9150_gyro_yout_l         0x46
# define    MPU9150_gyro_zout_h         0x47
# define    MPU9150_gyro_zout_l         0x48
# define    MPU9150_ext_sens_data_00    0x49
# define    MPU9150_ext_sens_data_01    0x4A
# define    MPU9150_ext_sens_data_02    0x4B
# define    MPU9150_ext_sens_data_03    0x4C
# define    MPU9150_ext_sens_data_04    0x4D
# define    MPU9150_ext_sens_data_05    0x4E
# define    MPU9150_ext_sens_data_06    0x4F
# define    MPU9150_ext_sens_data_07    0x50
# define    MPU9150_ext_sens_data_08    0x51
# define    MPU9150_ext_sens_data_09    0x52
# define    MPU9150_ext_sens_data_10    0x53
# define    MPU9150_ext_sens_data_11    0x54
# define    MPU9150_ext_sens_data_12    0x55
# define    MPU9150_ext_sens_data_13    0x56
# define    MPU9150_ext_sens_data_14    0x57
# define    MPU9150_ext_sens_data_15    0x58
# define    MPU9150_ext_sens_data_16    0x59
# define    MPU9150_ext_sens_data_17    0x5A
# define    MPU9150_ext_sens_data_18    0x5B
# define    MPU9150_ext_sens_data_19    0x5C
# define    MPU9150_ext_sens_data_20    0x5D
# define    MPU9150_ext_sens_data_21    0x5E
# define    MPU9150_ext_sens_data_22    0x5F
# define    MPU9150_ext_sens_data_23    0x60
# define    MPU9150_i2c_slv0_do         0x63
# define    MPU9150_i2c_slv1_do         0x64
# define    MPU9150_i2c_slv2_do         0x65
# define    MPU9150_i2c_slv3_do         0x66
# define    MPU9150_i2c_mst_delay_ctrl  0x67
# define    MPU9150_signal_path_reset   0x68
# define    MPU9150_user_ctrl           0x6A
# define    MPU9150_pwr_mgmt_1          0x6B
# define    MPU9150_pwr_mgmt_2          0x6C
# define    MPU9150_fifo_counth         0x72
# define    MPU9150_fifo_countl         0x73
# define    MPU9150_fifo_r_w            0x74
# define    MPU9150_who_am_i            0x75

// Position of different bits in their registers
// CONFIG
# define    MPU9150_ext_sync_set2       5
# define    MPU9150_ext_sync_set1       4
# define    MPU9150_ext_sync_set0       3
# define    MPU9150_ext_sync_sets       0b00111000
# define    MPU9150_dlpf_cfg2           2
# define    MPU9150_dlpf_cfg1           1
# define    MPU9150_dlpf_cfg0           0
# define    MPU9150_dlpf_cfgs           0b00000111

// GYRO_CONFIG
# define    MPU9150_xg_st               7
# define    MPU9150_yg_st               6
# define    MPU9150_zg_st               5
# define    MPU9150_fs_sel1             4
# define    MPU9150_fs_sel0             3
# define    MPU9150_fs_sels             0b00011000

// ACCEL_CONFIG
# define    MPU9150_xa_st               7
# define    MPU9150_ya_st               6
# define    MPU9150_za_st               5
# define    MPU9150_afs_sel1            4
# define    MPU9150_afs_sel0            3
# define    MPU9150_afs_sels            0b00011000

// PWR_MGMT_1
# define    MPU9150_deivce_reset        7
# define    MPU9150_sleep               6
# define    MPU9150_cycle               5
# define    MPU9150_temp_dis            3
# define    MPU9150_clksel2             2
# define    MPU9150_clksel1             1
# define    MPU9150_clksel0             0
# define    MPU9150_clksels             0b00000111

// Digital Low-Pass Filter configs
# define    MPU9150_dlpf_null           0
# define    MPU9150_dlpf_very_low       1
# define    MPU9150_dlpf_low            2
# define    MPU9150_dlpf_middle_low     3
# define    MPU9150_dlpf_middle_high    4
# define    MPU9150_dlpf_high           5
# define    MPU9150_dlpf_very_high      6

// Different clock sources
# define    MPU9150_internal_osc        0
# define    MPU9150_gyro_x_clk          1
# define    MPU9150_gyro_y_clk          2
# define    MPU9150_gyro_z_clk          3
# define    MPU9150_ext_32_osc          4
# define    MPU9150_ext_19_osc          5

// The different scales
# define    MPU9150_gyro_250            0
# define    MPU9150_gyro_500            1
# define    MPU9150_gyro_1000           2
# define    MPU9150_gyro_2000           3
# define    MPU9150_accel_2g            0
# define    MPU9150_accel_4g            1
# define    MPU9150_accel_8g            2
# define    MPU9150_accel_16g           3

// The normal address ofthe MPU9150, uncomment the following line of the AD0 is
// set
// # define    MPU9150_address             0x69
# define MPU9150_address                0x68

// TODO
// calibrate(Gyro|Accel)

void mpu9150_init(uint8_t gyroFullScale, uint8_t accelFullScale);

void mpu9150_setSleep(uint8_t sleep);
void mpu9150_setClockSource(uint8_t clk_src);
void mpu9150_setDLPF(uint8_t cfg);
void mpu9150_setGyroFullScale(uint8_t scale);
void mpu9150_setAccelFullScale(uint8_t scale);

float  mpu9150_getAccelX(void);
float  mpu9150_getAccelY(void);
float  mpu9150_getAccelZ(void);
void mpu9150_getAccelXYZ(float  *out);

float  mpu9150_getGyroX(void);
float  mpu9150_getGyroY(void);
float  mpu9150_getGyroZ(void);
void mpu9150_getGyroXYZ(float  *out);


# endif
