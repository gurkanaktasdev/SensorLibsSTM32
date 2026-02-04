/*
 * G_QMC5883L.h
 *
 *  Created on: Feb 5, 2026
 *      Author: aktas
 */

#ifndef INC_G_QMC5883L_H_
#define INC_G_QMC5883L_H_

#include "stm32f4xx_hal.h"

typedef struct{
	int16_t x_raw;
	int16_t y_raw;
	int16_t z_raw;
}Magnometre_raw;

typedef struct{
	float x_scaled;
	float y_scaled;
	float z_scaled;
}Magnometre_Scaled;

typedef enum{
	QMC5883L_OK = 0,
	QMC5883L_NOK
}QMC5883_Status;



void QMC5883L_Init(I2C_HandleTypeDef *hi2cx);

/*
 *  @info:
 * 	OSR : Over Sample Ratio 0->512 Sample, 1->256 sample, 2->128 sample, 3->64 sample
 * 	RNG : Full Scale        0 -> 2g, 1 -> 8g
 * 	ODR : Output Data Rate  0 -> 10Hz, 1 -> 50Hz, 2 -> 100Hz, 3 -> 200Hz
 */

QMC5883_Status QMC5883_Config(uint8_t osr,uint8_t rng, uint8_t odr);

/*
 * 	@info:
 *  @parameters
 *  values :should be Magnometre_yaw type pointer
 */
QMC5883_Status QMC5883L_getRaw(Magnometre_raw *values);



/*
 *  @info:
 */
void QMC5883L_Scale(Magnometre_raw *values,Magnometre_Scaled *scaled);

/*
 *  @info:
 */
QMC5883_Status QMC5883L_IsReady(void);


#define QMC_ADD_R      0x2C<<1
#define D_OP_X_LSB_R   0x00
#define D_OP_X_MSB_R   0x01
#define D_OP_Y_LSB_R   0x02
#define D_OP_Y_MSB_R   0x03
#define D_OP_Z_LSB_R   0x04
#define D_OP_Z_MSB_R   0x05
#define STATUS_R       0x06
#define TEMP_LSB_R     0x07
#define TEMP_MSB_R     0x08
#define CTRL_R1        0x09
#define CTRL_R2    	   0x0A
#define S_RESET_R      0x0B



#endif /* INC_G_QMC5883L_H_ */
