/*
 * G_MPU6050.c
 *
 *  Created on: Jan 2, 2026
 *      Author: aktas
 */

#include "G_MPU6050.h"
#include "math.h"

#define mpuDevAdr		 0x68
#define CONFIG 			 0x1A
#define GYRO_CONFIG 	 0x1B
#define ACCEL_CONFIG 	 0x1C
#define ACCEL_XOUT_H 	 0x3B		// X Accel values [15:8]
#define ACCEL_XOUT_L 	 0x3C       // X Accel values [7:0]
#define ACCEL_YOUT_H 	 0x3D		// Y Accel values [15:8]
#define ACCEL_YOUT_L 	 0x3E		// Y Accel values [7:0]
#define ACCEL_ZOUT_H 	 0x3F		// Z Accel values [15:8]
#define ACCEL_ZOUT_L 	 0x40		// Z Accel values [7:0]
#define TEMP_OUT_H  	 0x41       // Temperature value [15:8]
#define TEMP_OUT_L  	 0x42		// Temperature value [7:0]
#define GYRO_XOUT_H 	 0x43		// X Gyro values [15:8]
#define GYRO_XOUT_L 	 0x44       // X Gyro values [7:0]
#define GYRO_YOUT_H 	 0x45		// Y Gyro values [15:8]
#define GYRO_YOUT_L 	 0x46		// Y Gyro values [7:0]
#define GYRO_ZOUT_H 	 0x47		// Z Gyro values [15:8]
#define GYRO_ZOUT_L 	 0x48		// Z Gyro values [7:0]
#define PWR_MGMT_1  	 0x6B




FS_SELConf gyro = 3; //default
FS_SELConf accel =3; //default
static I2C_HandleTypeDef *MPU_I2C;




/*
 *  @brief: Alttaki değerlere imu sensörü sabit bir halde dururken @ref:gyroOffsetCheck fonksiyonun
 *  		çalıştırılması üzerinden ulaşırız. Kendi cihazınız için bu değerleri kontrol etmeyi unutmayınız.
 */
float gyro_x_off = -4.963f;
float gyro_y_off = 0.460f;
float gyro_z_off = -0.040f;


void sleepOn()
{
	 uint8_t val = 0x01;
	 HAL_I2C_Mem_Write(MPU_I2C, mpuDevAdr<<1, PWR_MGMT_1, 1, &val, 1, 100);
	 HAL_Delay(20);
}


void configMPU(I2C_HandleTypeDef *hi2c,DLPF_Conf dlpf, FS_SELConf accel_fs_sel, FS_SELConf gyro_fs_sel)
{

	gyro  = gyro_fs_sel;
	accel = accel_fs_sel;

	MPU_I2C = hi2c;
	HAL_Delay(30);

	sleepOn();
	HAL_Delay(20);

	HAL_I2C_Mem_Write(MPU_I2C, mpuDevAdr<<1, CONFIG, 1, &dlpf, 1, 100);
	HAL_Delay(20);

	gyro_fs_sel = gyro_fs_sel << 3;
	HAL_I2C_Mem_Write(MPU_I2C, mpuDevAdr<<1, GYRO_CONFIG, 1, &gyro_fs_sel, 1, 100);
	HAL_Delay(20);

	accel_fs_sel = accel_fs_sel << 3;
	HAL_I2C_Mem_Write(MPU_I2C, mpuDevAdr<<1, ACCEL_CONFIG, 1, &accel_fs_sel, 1, 100);
	HAL_Delay(20);
}

AccelValues read_MPU6050_Accels()
{
	AccelValues values;
	uint8_t buffer [6];
	int16_t accelValues[3] = {0};
	HAL_I2C_Mem_Read(MPU_I2C, mpuDevAdr<<1 , ACCEL_XOUT_H, 1, buffer, 6, 100);
	accelValues[0] = (int16_t)((buffer[0]<<8) | buffer[1]);  // Accel value for X axis
	accelValues[1] = (int16_t)((buffer[2]<<8) | buffer[3]);  // Accel value for y axis
	accelValues[2] = (int16_t)((buffer[4]<<8) | buffer[5]);  // Accel value for z axis
	HAL_Delay(10);

	switch (accel) {

		case LOW_FS_SEL:
			values.x_value = (accelValues[0]/16384.0);
			values.y_value = (accelValues[1]/16384.0);
			values.z_value = (accelValues[2]/16384.0);
			break;

		case MEDIUM1_FS_SEL:
			values.x_value = (accelValues[0]/8192.0);
			values.y_value = (accelValues[1]/8192.0);
			values.z_value = (accelValues[2]/8192.0);
			break;

		case MEDIUM2_FS_SEL:
			values.x_value = (accelValues[0]/4096.0);
			values.y_value = (accelValues[1]/4096.0);
			values.z_value = (accelValues[2]/4096.0);
			break;

		case HIGH_FS_SEL:
			values.x_value = (accelValues[0]/2048.0);
			values.y_value = (accelValues[1]/2048.0);
			values.z_value = (accelValues[2]/2048.0);
			break;

	}


	return values;
}

GyroValues read_MPU6050_Gyros()
{
	GyroValues values;
	uint8_t buffer [6];
	int16_t gyroValues[3] = {0};
	HAL_I2C_Mem_Read(MPU_I2C, mpuDevAdr<<1 , GYRO_XOUT_H, 1, buffer, 6, 100);
	gyroValues[0] = (int16_t)((buffer[0]<<8) | buffer[1]);  // Gyro value for X axis
	gyroValues[1] = (int16_t)((buffer[2]<<8) | buffer[3]);  // Gyro value for Y axis
	gyroValues[2] = (int16_t)((buffer[4]<<8) | buffer[5]);  // Gyro value for Z axis
	HAL_Delay(10);

	switch (gyro) {

		case LOW_FS_SEL:
			values.x_value = (gyroValues[0]/131.0);
			values.y_value = (gyroValues[1]/131.0);
			values.z_value = (gyroValues[2]/131.0);
			break;

		case MEDIUM1_FS_SEL:
			values.x_value = (gyroValues[0]/65.5);
			values.y_value = (gyroValues[1]/65.5);
			values.z_value = (gyroValues[2]/65.5);
			break;

		case MEDIUM2_FS_SEL:
			values.x_value = (gyroValues[0]/32.8);
			values.y_value = (gyroValues[1]/32.8);
			values.z_value = (gyroValues[2]/32.8);
			break;

		case HIGH_FS_SEL:
			values.x_value = (gyroValues[0]/16.4);
			values.y_value = (gyroValues[1]/16.4);
			values.z_value = (gyroValues[2]/16.4);
			break;

	}

	values.x_value -= gyro_x_off;
	values.y_value -= gyro_y_off;
	values.z_value -= gyro_z_off;

	return values;

}

void compute_MPU6050_atitude(AttitudeVal *attitudeVal, AccelValues accelVal, GyroValues gyroVal){

	float a = 0.98;
	float roll_gyro = 0.0;
	float pitch_gyro = 0.0;
	float roll_acc = 0.0;
	float pitch_acc = 0.0;
	float dt = 0;
	uint32_t current_t = 0;

	// @brief: dt(Delta t) ayarlamasının yapıldığı kısım.
	current_t = HAL_GetTick();
	dt = (current_t - attitudeVal->prev_time) / 1000.0f;
	attitudeVal->prev_time = current_t;


	// @brief:   gyro(Açısal Hız) tarafından hesaplanan roll değeri için y ekseni referans
	roll_gyro = attitudeVal->prev_gyro_roll + gyroVal.y_value * dt;
	pitch_gyro = attitudeVal->prev_gyro_pitch + gyroVal.x_value * dt;

	attitudeVal->prev_gyro_roll = roll_gyro;
	attitudeVal->prev_gyro_pitch = pitch_gyro;

	// @brief : accel(ivme) tarafından yapılan hesaplamalar.

	roll_acc = atan2f(accelVal.y_value,accelVal.z_value);
	pitch_acc = atan2f(-accelVal.x_value,sqrtf(((accelVal.y_value * accelVal.y_value) + (accelVal.z_value * accelVal.z_value))));

	// @brief: Complementary Filter son aşamsı

	attitudeVal->roll_angle = a * roll_gyro + (1 - a) * roll_acc;
	attitudeVal->pitch_angle = a * pitch_gyro + (1 - a) * pitch_acc;
}



void gyroOffsetCheck(GyroOffset *gyroOffset){
	float gyroX = 0;
	float gyroY = 0;
	float gyroZ = 0;
	GyroValues gyVal;

	for(uint16_t t = 0; t < 1000; t++ ){
		gyVal = read_MPU6050_Gyros();
		gyroX += gyVal.x_value;
		gyroY += gyVal.y_value;
		gyroZ += gyVal.z_value;
		HAL_Delay(5);
	}

	gyroOffset->gyro_x_offset = gyroX / 1000.0;
	gyroOffset->gyro_y_offset = gyroY / 1000.0;
	gyroOffset->gyro_z_offset = gyroZ / 1000.0;
}

double read_MPU6050_Temp()
{
	double temp = 0.0;
	uint8_t buffer [2];
	HAL_I2C_Mem_Read(MPU_I2C, mpuDevAdr<<1 , TEMP_OUT_H, 1, buffer, 2, 100);

	temp = ((buffer[0] << 8) | buffer[1]);
	if(temp == 0) return 0.0;

	temp = (temp / 340.0) + 36.53;
	return temp;
}






