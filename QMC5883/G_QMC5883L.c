/*
 * G_QMC5883L.c
 *
 *  Created on: Feb 5, 2026
 *      Author: aktas
 */



#include "G_QMC5883L.h"



static I2C_HandleTypeDef *QMCI2Cx;
static int8_t scale;


void QMC5883L_Init(I2C_HandleTypeDef *hi2cx)
{
	QMCI2Cx = hi2cx;

	HAL_I2C_Mem_Write(QMCI2Cx, QMC_ADD_R,CTRL_R2,1,(uint8_t *)0x80, 1, 100);
	HAL_Delay(50);
	HAL_I2C_Mem_Write(QMCI2Cx, QMC_ADD_R,S_RESET_R , 1,(uint8_t *) 0x01, 1, 100);
	HAL_Delay(20);
}


QMC5883_Status QMC5883_Config(uint8_t osr,uint8_t rng, uint8_t odr){

	HAL_StatusTypeDef status;
	uint8_t controlRegValue = 0;

	if((osr > 3) || (rng > 1) || (odr > 3)) return QMC5883L_NOK;

	controlRegValue = (osr << 6) | (rng << 4) | (odr << 2) | 1;
	status = HAL_I2C_Mem_Write(QMCI2Cx, QMC_ADD_R, CTRL_R1, 1, &controlRegValue, 1, 100);

	if(HAL_OK == status){

		scale = rng;
		return QMC5883L_OK;
	}

	return QMC5883L_NOK;

}


QMC5883_Status QMC5883L_getRaw(Magnometre_raw *values){

	HAL_StatusTypeDef status;
	uint8_t manV[6] = {0};
	status = HAL_I2C_Mem_Read(QMCI2Cx, QMC_ADD_R, D_OP_X_LSB_R, 1, manV, 6, 100);
	if(HAL_OK != status) return QMC5883L_NOK;
	values->x_raw = (int16_t)((manV[1]<<8) | manV[0]);
	values->y_raw = (int16_t)((manV[3]<<8) | manV[2]);
	values->z_raw = (int16_t)((manV[5]<<8) | manV[4]);

	return QMC5883L_OK;
}


void QMC5883L_Scale(Magnometre_raw *values,Magnometre_Scaled *scaled)
{
	float x,y,z;

	if(scale == 0)
	{
		x = (float)((values->x_raw) / 12000.0f);
		y = (float)((values->y_raw) / 12000.0f);
		z = (float)((values->z_raw) / 12000.0f);
	}else{
		x = (float)((values->x_raw) / 3000.0f);
		y = (float)((values->y_raw) / 3000.0f);
		z = (float)((values->z_raw) / 3000.0f);
	}
	scaled->x_scaled = x;
	scaled->y_scaled = y;
	scaled->z_scaled = z;
}


QMC5883_Status QMC5883L_IsReady(void)
{
	uint8_t status = 0;
	HAL_StatusTypeDef check;
	check = HAL_I2C_Mem_Read(QMCI2Cx, QMC_ADD_R, STATUS_R, 1, &status, 1, 100);

	if(HAL_OK == check)
	{
		status = (0x01) & status;
		if( 1 == status) return QMC5883L_OK;

	}

	return QMC5883L_NOK;

}

