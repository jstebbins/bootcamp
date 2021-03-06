#ifndef LIS331DLH_H
#define LIS331DLH_H

#define LIS331DLH_I2C_ADDR       0x18
#define LIS331DLH_I2C_READ_ADDR  (LIS331DLH_I2C_ADDR << 1 | 1)
#define LIS331DLH_I2C_WRITE_ADDR (LIS331DLH_I2C_ADDR << 1)

#define LIS331DLH_WHO_AM_I_REG     0x0F
#define LIS331DLH_CTRL1_REG        0x20
#define LIS331DLH_CTRL2_REG        0x21
#define LIS331DLH_CTRL3_REG        0x22
#define LIS331DLH_CTRL4_REG        0x23
#define LIS331DLH_CTRL5_REG        0x24
#define LIS331DLH_FILTER_RESET_REG 0x25
#define LIS331DLH_STATUS_REG       0x27
#define LIS331DLH_OUT_X_L_REG      0x28
#define LIS331DLH_OUT_X_H_REG      0x29
#define LIS331DLH_OUT_Y_L_REG      0x2A
#define LIS331DLH_OUT_Y_H_REG      0x2B
#define LIS331DLH_OUT_Z_L_REG      0x2C
#define LIS331DLH_OUT_Z_H_REG      0x2D

#define LIS331DLH_DEF_PM           0x27
#define LIS331DLH_REBOOT           0x80

#endif
