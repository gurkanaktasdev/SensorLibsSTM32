/*
 * G_MPU6050.h
 *
 *  Created on: Jan 2, 2026
 *      Author: aktas
 */

#ifndef INC_G_MPU6050_H_
#define INC_G_MPU6050_H_

#include "stm32f4xx_hal.h"



typedef enum{
	LOW_FS_SEL = 0,
	MEDIUM1_FS_SEL,
	MEDIUM2_FS_SEL,
	HIGH_FS_SEL
}FS_SELConf;

typedef enum{
	DLPF_0 = 0,
	DLPF_1,
	DLPF_2,
	DLPF_3,
	DLPF_4,
	DLPF_5,
	DLPF_6,
}DLPF_Conf;

typedef struct {
	double x_value;
	double y_value;
	double z_value;
}AccelValues;

typedef struct {
	double x_value;
	double y_value;
	double z_value;
}GyroValues;


/*
 *  dlpf digital low pass filter.
 *
 * 	accel_fs_sel:selects the full scale range of the accelerometer outputs according to the folllowing table.
 * 						 accel_fs_sel        Full Scale Range
 * 						 	 0						+- 2g
 * 						 	 1						+- 4g
 * 						 	 2						+- 8g
 * 						 	 3						+- 16g
 *
 * 	gyro_fs_sel:selects the full scale range of the gyroscope outputs according to the following table.
 * 						gyro_fs_sel			Full Scale Range
 * 							 0					+- 250  degree/s
 * 							 1					+- 500  degree/s
 * 							 2					+- 1000 degree/s
 * 							 3					+- 2000 degree/s
 */
void configMPU(I2C_HandleTypeDef *hi2c,DLPF_Conf dlpf, FS_SELConf accel_fs_sel, FS_SELConf gyro_fs_sel);

AccelValues read_MPU6050_Accels();
GyroValues read_MPU6050_Gyros();
double read_MPU6050_Temp();


#endif /* INC_G_MPU6050_H_ */
