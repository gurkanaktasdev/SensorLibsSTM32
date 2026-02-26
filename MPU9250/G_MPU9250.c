/*
 * G_MPU9250.c
 *
 *  Created on: Feb 25, 2026
 *      Author: aktas
 */


#include "G_MPU9250.h"


static I2C_HandleTypeDef *MPU9250;

/*
 *  @param  gyroRecieved  : G_MPU9250_Config() fonksiyonundaki giriş olarak verilen GyroFsSel değerini tutar
 *  @param  accelRecieved : G_MPU9250_Config() fonksiyonundaki giriş olarak verilen AccelFsSel değerini tutar
 */
static uint8_t gyroRecieved;
static uint8_t accelRecieved;






G_MPU9250_Status_t G_MPU9250_Init(I2C_HandleTypeDef *hi2c){

	HAL_StatusTypeDef stat = HAL_ERROR;
	MPU9250 = hi2c;
	uint8_t pwr_mgmt = 0x01;
	stat = HAL_I2C_Mem_Write(hi2c, G_MPU92xx_ADDR, G_MPU92xx_PWR_MGMT_1, 1, &pwr_mgmt, 1, 100);

	if(HAL_OK != stat){
		return InitUnSuccessful;
	}
	return InitSuccessful;
}

G_MPU9250_Status_t G_MPU9250_Config(uint8_t GyroDLPF, uint8_t GyroFsSel, uint8_t AccelFsSel, uint8_t AccelDLPF){

	HAL_StatusTypeDef stat = HAL_ERROR;

	// Default Değerler
	uint8_t gyroDLPF   = 3;
	uint8_t gyroFsSel  = 2;
	uint8_t accelFsSel = 2;
	uint8_t accelDLPF   = 2;

	if(GyroDLPF <  0 || GyroDLPF > 7) return InitValOutOfRange;
	if(AccelDLPF < 0 || AccelDLPF > 7) return InitValOutOfRange;
	if(GyroFsSel < 0 || GyroFsSel > 3) return InitValOutOfRange;
	if(AccelFsSel < 0 || AccelFsSel > 3) return InitValOutOfRange;

	gyroDLPF = GyroDLPF;
	stat = HAL_I2C_Mem_Write(MPU9250, G_MPU92xx_ADDR, G_MPU92xx_CONFIG, 1, &gyroDLPF, 1, 100);
	if(HAL_OK != stat) return I2cProblem;

	gyroRecieved = GyroFsSel;
	gyroFsSel = (GyroFsSel<<3);
	stat = HAL_I2C_Mem_Write(MPU9250, G_MPU92xx_ADDR, G_MPU92xx_GYRO_CFG, 1,&gyroFsSel, 1, 100);
	if(HAL_OK != stat) return I2cProblem;

	accelRecieved = AccelFsSel;
	accelFsSel = (AccelFsSel<<3);
	stat = HAL_I2C_Mem_Write(MPU9250, G_MPU92xx_ADDR, G_MPU92xx_ACCEL_CFG1, 1, &accelFsSel, 1, 100);
	if(HAL_OK != stat) return I2cProblem;

	accelDLPF = AccelDLPF;
	stat = HAL_I2C_Mem_Write(MPU9250, G_MPU92xx_ADDR, G_MPU92xx_ACCEL_CFG2, 1, &accelDLPF, 1, 100);
	if(HAL_OK != stat) return I2cProblem;

	return InitSuccessful;

}

G_MPU9250_Status_t G_MPU9250_ReadAccel(RawAccel *rawData){

	HAL_StatusTypeDef stat = HAL_ERROR;

	uint8_t t[6] =  {0};
	int16_t x[3] =  {0};

	stat = HAL_I2C_Mem_Read(MPU9250, G_MPU92xx_ADDR, G_MPU92xx_ACCEL_XOUT_H, 1, t, 6, 100);
	if(HAL_OK != stat) return I2cProblem;

	x[0] = (int16_t)((t[0]<<8) | t[1]);
	x[1] = (int16_t)((t[2]<<8) | t[3]);
	x[2] = (int16_t)((t[4]<<8) | t[5]);

	switch(accelRecieved){

	case 0:
		rawData->accelX = x[0] / 16384.0;
		rawData->accelY = x[1] / 16384.0;
		rawData->accelZ = x[2] / 16384.0;
		break;

	case 1:
		rawData->accelX = x[0] / 8192.0;
		rawData->accelY = x[1] / 8192.0;
		rawData->accelZ = x[2] / 8192.0;
		break;

	case 2:
		rawData->accelX = x[0] / 4096.0;
		rawData->accelY = x[1] / 4096.0;
		rawData->accelZ = x[2] / 4096.0;
		break;

	case 3:
		rawData->accelX = x[0] / 2048.0;
		rawData->accelY = x[1] / 2048.0;
		rawData->accelZ = x[2] / 2048.0;
		break;

	default:
		return InitUnSuccessful;
	}

	return AccelRead_OK;
}

G_MPU9250_Status_t G_MPU9250_ReadGyro(RawGyro *rawData){

	HAL_StatusTypeDef stat = HAL_ERROR;

	uint8_t t[6] =  {0};
	int16_t x[3] =  {0};
	stat = HAL_I2C_Mem_Read(MPU9250, G_MPU92xx_ADDR, G_MPU92xx_GYRO_XOUT_H, 1, t, 6, 100);
	if(HAL_OK != stat) return I2cProblem;

	x[0] = (int16_t)((t[0]<<8) | t[1]);
	x[1] = (int16_t)((t[2]<<8) | t[3]);
	x[2] = (int16_t)((t[4]<<8) | t[5]);

	switch(gyroRecieved){
	case 0:
		rawData->gyroX = (x[0] / 131.0) - GYRO_XOFF;
		rawData->gyroY = (x[1] / 131.0) - GYRO_YOFF;
		rawData->gyroZ = (x[2] / 131.0) - GYRO_ZOFF;
		break;

	case 1:
		rawData->gyroX = (x[0] / 65.5) - GYRO_XOFF;
		rawData->gyroY = (x[1] / 65.5) - GYRO_YOFF;
		rawData->gyroZ = (x[2] / 65.5) - GYRO_ZOFF;
		break;

	case 2:
		rawData->gyroX = (x[0] / 32.8) - GYRO_XOFF;
		rawData->gyroY = (x[1] / 32.8) - GYRO_YOFF;
		rawData->gyroZ = (x[2] / 32.8) - GYRO_ZOFF;
		break;

	case 3:
		rawData->gyroX = (x[0] / 16.4) - GYRO_XOFF;
		rawData->gyroY = (x[1] / 16.4) - GYRO_YOFF;
		rawData->gyroZ = (x[2] / 16.4) - GYRO_ZOFF;
		break;

	default:
		return InitUnSuccessful;
	}
	return GyroRead_OK;
}

G_MPU9250_Status_t G_MPU9250_Reset(){

	HAL_StatusTypeDef stat = HAL_ERROR;

	uint8_t x = 1<<7;
	stat = HAL_I2C_Mem_Write(MPU9250, G_MPU92xx_ADDR, G_MPU92xx_PWR_MGMT_1, 1, &x, 1, 100);
	if(HAL_OK != stat) return I2cProblem;

	return ResetSuccessful;
}

G_MPU9250_Status_t G_MPU9250_MagInit(ASAxResult *res)   // Magnetometer Init.
{
	HAL_StatusTypeDef stat = HAL_ERROR;

	uint8_t result= 0x02;
	stat = HAL_I2C_Mem_Write(MPU9250, G_MPU92xx_ADDR, G_MPU92xx_MAGCTRL, 1,&result, 1, 100);
	if(HAL_OK != stat) return I2cProblem;
	HAL_Delay(10);

	//ontinuous measurement mode 1 and BIT:16
	uint8_t data = 0x16;
	stat = HAL_I2C_Mem_Write(MPU9250, G_MPU92xx_MAG_ADDR, G_MPU92xx_MAG_CNTL1, 1,&data, 1, 100);
	if(HAL_OK != stat) return I2cProblem;
	HAL_Delay(100);

	G_ASAxCalc(res);

	return MagInit_OK;
}

int8_t G_MPU9250_MagIsReady(){

	uint8_t data = 0;
	HAL_I2C_Mem_Read(MPU9250, G_MPU92xx_MAG_ADDR, G_MPU92xx_MAG_ST1, 1, &data, 1, 100);
	if(1 == (data & 0x01)){
		return 1;
	}
	return 0;
}

G_MPU9250_Status_t G_MPU9250_MagRead(RawMag *rawData,ASAxResult *asaxResult){

	HAL_StatusTypeDef stat = HAL_ERROR;

	uint8_t check;
	uint8_t t[6] =  {0};
	int16_t x[3] =  {0};
	stat = HAL_I2C_Mem_Read(MPU9250, G_MPU92xx_MAG_ADDR, G_MPU92xx_MAG_HXL, 1, t, 6, 100);
	if(HAL_OK != stat) return I2cProblem;
	HAL_I2C_Mem_Read(MPU9250, G_MPU92xx_MAG_ADDR, G_MPU92xx_MAG_ST2, 1, &check, 1, 100);   // Kilitlenme olmaması için ST2 registerını okuyoruz

	x[0] = (int16_t)((t[1]<<8) | t[0]);
	x[1] = (int16_t)((t[3]<<8) | t[2]);
	x[2] = (int16_t)((t[5]<<8) | t[4]);


	rawData->magX = ((float)x[0] * asaxResult->ASAX_Val);
	rawData->magY = ((float)x[1] * asaxResult->ASAY_Val);
	rawData->magZ = ((float)x[2] * asaxResult->ASAZ_Val);

	return MagRead_OK;
}

void G_MPU9250_MagCalc(MagValues *magValues, RawMag *rawData){

	float x_c = rawData->magX - MAG_XOFF;
	float y_c = rawData->magY - MAG_YOFF;
	float z_c = rawData->magZ - MAG_ZOFF;

	magValues->magX = x_c * 0.15;
	magValues->magY = y_c * 0.15;
	magValues->magZ = z_c * 0.15;
}

float G_MPU9250_GetHeading(MagValues *magValues){
	float heading = 0;
	heading = atan2(magValues->magX, magValues->magY) * (180.0 / 3.14159265);
	heading += YMD;
	if (heading < 0) {
		heading += 360.0;
	}

	return heading;
}

void G_MPU9250_MagDoOffSet(MagOffset *magOffset, RawMag *rawMag){
	static uint16_t counter = 0;
	counter++;

	// Default olarak magnetometre değerlerini alıyoruz.
	if(counter == 20)
	{
		magOffset->magXmax = rawMag->magX;
		magOffset->magXmin = rawMag->magX;
		magOffset->magYmax = rawMag->magY;
		magOffset->magYmin = rawMag->magY;
		magOffset->magZmax = rawMag->magZ;
		magOffset->magZmin = rawMag->magZ;

	}

	if(rawMag->magX > magOffset->magXmax){
		magOffset->magXmax = rawMag->magX;
	}
	if(rawMag->magX < magOffset->magXmin)
	{
		magOffset->magXmin = rawMag->magX;
	}
	if(rawMag->magY > magOffset->magYmax){
		magOffset->magYmax = rawMag->magY;
	}
	if(rawMag->magY < magOffset->magYmin)
	{
		magOffset->magYmin = rawMag->magY;
	}
	if(rawMag->magZ > magOffset->magZmax){
		magOffset->magZmax = rawMag->magZ;
	}
	if(rawMag->magZ < magOffset->magZmin)
	{
		magOffset->magZmin = rawMag->magZ;
	}
}

void G_ASAxCalc(ASAxResult *res){
	res->ASAX_Val = ((ASAX-128)*0.5)/128.0 + 1.0;
	res->ASAY_Val = ((ASAY-128)*0.5)/128.0 + 1.0;
	res->ASAZ_Val = ((ASAY-128)*0.5)/128.0 + 1.0;
}

void G_MPU9250_GyroOffsetCalibration(GyroOffset_c *data,RawGyro *rawData){
	float gyroX = 0;
	float gyroY = 0;
	float gyroZ = 0;

	for(uint16_t t = 0; t < 1000; t++ ){
		G_MPU9250_ReadGyro(rawData);
		gyroX += rawData->gyroX;
		gyroY += rawData->gyroY;
		gyroZ += rawData->gyroZ;
		HAL_Delay(1);
	}

	data->gyroXOff = gyroX / 1000.0;
	data->gyroYOff = gyroY / 1000.0;
	data->gyroZoff = gyroZ / 1000.0;
}
