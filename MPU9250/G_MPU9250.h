/*
 * G_MPU9250.h
 *
 *  Created on: Feb 25, 2026
 *      Author: aktas
 */

#ifndef INC_G_MPU9250_H_
#define INC_G_MPU9250_H_

#include "stm32f4xx_hal.h"
#include <math.h>


typedef enum{
	InitValOutOfRange = 0,
	InitSuccessful,
	InitUnSuccessful,
	MagInit_OK,
	MagInit_NOK,
	I2cProblem,
	AccelRead_OK,
	GyroRead_OK,
	MagRead_OK,
	ResetSuccessful

}G_MPU9250_Status_t;



typedef struct {
	float accelX;
	float accelY;
	float accelZ;
}RawAccel;

typedef struct {
	float gyroX;
	float gyroY;
	float gyroZ;
}RawGyro;

typedef struct{
	float magX;
	float magY;
	float magZ;
}RawMag;

typedef struct{
	float magX;
	float magY;
	float magZ;
}MagValues;


/*
 *  Aşşağıdaki structure lar sensörün kalibrasyonu için
 *  İlerleyen aşamlarda bunları bir buton aracılığı ile otomatik yapacağız.
 */
typedef struct{
	float magXmin;
	float magXmax;
	float magYmin;
	float magYmax;
	float magZmin;
	float magZmax;
}MagOffset;

typedef struct {
	float gyroXOff;
	float gyroYOff;
	float gyroZoff;
}GyroOffset_c;         // _c calibration için kullnadıldığı belli olsun diye.

typedef struct{
	float ASAX_Val;
	float ASAY_Val;
	float ASAZ_Val;
}ASAxResult;

#define G_MPU92xx_ADDR      	 0x68 << 1
#define G_MPU92xx_CONFIG    	 0x1A
#define G_MPU92xx_GYRO_CFG 		 0x1B
#define G_MPU92xx_ACCEL_CFG1     0x1C
#define G_MPU92xx_ACCEL_CFG2	 0x1D
#define G_MPU92xx_ACCEL_XOUT_H   0x3B
#define G_MPU92xx_GYRO_XOUT_H    0x43
#define G_MPU92xx_PWR_MGMT_1     0x6B
#define G_MPU92xx_MAGCTRL        0x37           //  define INT_PIN_CFG
#define G_MPU92xx_MAG_ADDR		 0x0C << 1
#define G_MPU92xx_MAG_WIA        0x00
#define G_MPU92xx_MAG_ST1        0x02
#define G_MPU92xx_MAG_HXL        0x03
#define G_MPU92xx_MAG_ST2      	 0x09
#define G_MPU92xx_MAG_CNTL1      0x0A
#define G_MPU92xx_MAG_ASAX		 0x10
#define G_MPU92xx_MAG_ASAY		 0x11
#define G_MPU92xx_MAG_ASAZ		 0x12



/*
 *  Register mape göre ilgili registerlardan bu değerler ölçülmüştür
 *  @ref : G_MPU92xx_MAG_ASAX
 */
#define ASAX  173.0f
#define ASAY  175.0f
#define ASAZ  164.0f


/*
 *  @brief: void MagDoOFFSet() fonksiyonu çalıştırılarak
 *  		sensör 3 eksende de çeşitli formlarda çevrilmiştir.
 *  		bunun sonucunda ilgili eksenler ile ilgili Max ve Min
 *  		değerleri bulunup ilgili formüller ile değerler anlamlı
 *  		hale getirilmiştir.
 */
#define MAG_XMIN 	-178.0f
#define MAG_XMAX 	 318.0f
#define MAG_YMIN	-20.0f
#define MAG_YMAX  	 465.0f
#define MAG_ZMIN	-472.0f
#define MAG_ZMAX 	 24.0f


#define MAG_XOFF	 (MAG_XMAX + MAG_XMIN) / 2.0
#define MAG_YOFF	 (MAG_YMAX + MAG_YMIN) / 2.0
#define MAG_ZOFF 	 (MAG_ZMAX + MAG_ZMIN) / 2.0


/*
 *  @brief: void  G_MPU9250_GyroOffsetCalibration(GyroOffset_c *data,RawGyro *rawData) fonksiyonu çalışır vaziyette iken
 *  		sensör masada düz bir şekilde sabit bırakılmış ve ilgili değerlere ulaşılmıştır.
 */
#define GYRO_XOFF	-1.957f
#define GYRO_YOFF	 1.098f
#define GYRO_ZOFF	-1.032f

/*
 *   @param YMD:  Your Magnetic Declination
 */
#define YMD          6.0




G_MPU9250_Status_t G_MPU9250_Init(I2C_HandleTypeDef *hi2c);

/*
 *
 *    @param : GyroDLPF = [0,7] aralığında,
 *    			readme sayfasında GYRO_DLPF
 *    			başlığı altında detaylar.
 *
 *    @param : GyroFsSel [0,3]  arası değerler girilir.
 *
 *    				0      +250  dps (degree per second)
 *    				1      +500  dps
 *    				2      +1000 dps
 *    				3      +2000 dps
 *
 *    @param : AccelFsSel [0,3] arası değerler girilir.
 *
 *    				0      +-2g
 *    				1      +-4g
 *    				3      +-8g
 *    				4      +-16g
 *
 *    @param : AccelDLPF[0,7] aralığında,
 *    			readme sayfasında ACCEL_DLPF
 *    			başlığı altında detaylar.
 */

G_MPU9250_Status_t G_MPU9250_Config(uint8_t GyroDLPF, uint8_t GyroFsSel, uint8_t AccelFsSel, uint8_t AccelDLPF);

G_MPU9250_Status_t G_MPU9250_ReadAccel(RawAccel *rawData);

G_MPU9250_Status_t G_MPU9250_ReadGyro(RawGyro *rawData);

G_MPU9250_Status_t G_MPU9250_Reset();

G_MPU9250_Status_t G_MPU9250_MagInit(ASAxResult *res);   // Magnetometer Init.

int8_t G_MPU9250_MagIsReady();

G_MPU9250_Status_t G_MPU9250_MagRead(RawMag *rawData,ASAxResult *asaxResult);

void G_MPU9250_MagCalc(MagValues *magValues, RawMag *rawData); // Calculate Magnetometer Values

float G_MPU9250_GetHeading(MagValues *magValues);


/*
 * @brief:
 * 			Magnotometre için offset ayarlama
 * 			Burada pusula farklı yüzeylerde farklı manyetik alanlara maruz kalabileceği için
 * 			mahnotometre nin yerini her değiştidiğimizde offset ini tekrar ayarlamamız gerkir.
 *
 */

void G_MPU9250_MagDoOffSet(MagOffset *magOffset, RawMag *rawMag);

void G_ASAxCalc(ASAxResult *res);


/*
 * 	@brief: Sensör düz bir zeminde hareketsiz durıyorken
 * 			fonksiyonu çalıştırırsak , bize 1000 örneklem
 * 			sonrasındaki 3 eksenin ort sapma değerlerini
 * 			parametre olarak verdiğimiz struct ile okuyabiliriz.
 */
void G_MPU9250_GyroOffsetCalibration(GyroOffset_c *data,RawGyro *rawData);


#endif /* INC_G_MPU9250_H_ */
