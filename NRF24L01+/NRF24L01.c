/*
 * NRF24L01.c
 *
 *  Created on: Dec 21, 2025
 *      Author: aktas
 */


#include "stm32f1xx_hal.h"
#include "NRF24L01.h"

extern SPI_HandleTypeDef hspi1;
#define NRF24_SPI &hspi1

#define NRF24_CE_PORT GPIOB
#define NRF24_CE_PIN  GPIO_PIN_8

#define NRF24_CSN_PORT GPIOB
#define NRF24_CSN_PIN  GPIO_PIN_9


void CS_Select(void){

	HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_RESET);
}
void CS_Unselect(void){

	HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);
}

void CE_Enable(void){

	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_SET);
}
void CE_Disable(void){

	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}




uint8_t nrf24_WriteReg(uint8_t Reg, uint8_t Data){

	uint8_t cmd = 0;
	uint8_t status = 0;
	cmd = (Reg & 0x1F) | W_REGISTER;


	CS_Select();
	HAL_SPI_TransmitReceive(&hspi1, &cmd, &status, 1, 100);
	HAL_SPI_Transmit(&hspi1, &Data, 1, 100);
	CS_Unselect();
	return status;
}

/*
 * NRF Registerları max 5 byte olduğu için size <= 5 olmalı
 */

uint8_t nrf24_WriteReg_Multi(uint8_t Reg, uint8_t *Data, int size){

	uint8_t cmd = (Reg & 0x1F) | W_REGISTER;
	uint8_t status = 0;
	CS_Select();

	HAL_SPI_TransmitReceive(&hspi1, &cmd, &status, 1, 100);
	HAL_SPI_Transmit(NRF24_SPI, Data, size, 1000);

	CS_Unselect();
	return status;
}

uint8_t nrf24_ReadReg(uint8_t Reg){

	uint8_t cmd = (0x1F & Reg) | R_REGISTER;
	uint8_t data = 0;
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &cmd, 1,100);
	HAL_SPI_Receive(&hspi1, &data, 1, 100);

	CS_Unselect();
	return data;

}

void nrf24_ReadReg_Multi(uint8_t Reg, uint8_t* data, int size){

	uint8_t cmd = (0x1F & Reg) | R_REGISTER;
	uint8_t status;
	uint8_t dummy = 0xFF;
	CS_Select();
	HAL_SPI_TransmitReceive(NRF24_SPI, &cmd, &status, 1, 100);

    for(uint8_t i = 0; i < size; i++)
    {
    	HAL_SPI_TransmitReceive(NRF24_SPI, &dummy, &data[i], 1, 100);
    }
	CS_Unselect();

}

void nrf_sendCmd(uint8_t cmd){

	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);

	CS_Unselect();
}


void NRF24_Init(void){

	CE_Disable();


	nrf24_WriteReg(CONFIG, 0);
	nrf24_WriteReg(EN_AA, 0);           // No auto ACK
	nrf24_WriteReg(EN_RXADDR, 0);       // Not enable any pipe
	nrf24_WriteReg(SETUP_AW, 0x03);     // 5 bytes TX/RX
	nrf24_WriteReg(SETUP_RETR,0);       // No retransmission
	nrf24_WriteReg(RF_CH, 0);   		// will be setup during TX or RX
	//nrf24_WriteReg(RF_SETUP,0x22);		// power = -12db, data rate -> 250kps
	nrf24_WriteReg(RF_SETUP,0x26);         // power = 0dbm , data rate -> 250kbps
	//nrf24_WriteReg(RF_SETUP, 0x20);		 // power = -18dbm, data rate -> 250kbps
	//nrf24_WriteReg(RF_SETUP,0x24);		     // power = -6dbm , data rate -> 250kbps

	HAL_Delay(5);
	//CE_Enable();

}

void NRF24_Init2(nRF_PWR_s pwr, nRF_D_RATE_s rate){

	CE_Disable();


	nrf24_WriteReg(CONFIG, 0);
	nrf24_WriteReg(EN_AA, 0);           // No auto ACK
	nrf24_WriteReg(EN_RXADDR, 0);       // Not enable any pipe
	nrf24_WriteReg(SETUP_AW, 0x03);     // 5 bytes TX/RX
	nrf24_WriteReg(SETUP_RETR,0);       // No retransmission
	nrf24_WriteReg(RF_CH, 0);   		// will be setup during TX or RX

	uint8_t rf_setup = pwr | rate;
	nrf24_WriteReg(RF_SETUP,rf_setup);

	HAL_Delay(5);
}





void NRF24_TxMode(uint8_t* Address, uint8_t channel, nRF_CRC_t crc){


	uint8_t config_t = 0;

	CE_Disable();

	nrf24_WriteReg(RF_CH,channel);

	nrf24_WriteReg_Multi(TX_ADDR, Address, 5);

	// 1 byte lık crc işlemi
	if(crc == crc_1){
		config_t = 1<<3;
	}

	// 2 byte lık crc işlemi
	if(crc == crc_2){
		config_t = (1<<3) | (1<<2);

	}

	// Tx modu olduğu için PWR_UP bitini set ediyoruz.
	config_t |= 0x02;

	nrf24_WriteReg(CONFIG,config_t);

	HAL_Delay(5);
	//CE_Enable();

}

uint8_t NRF24_Transmit(uint8_t* data, uint8_t len)
{
	uint8_t status = 0;
    uint8_t cmd = W_TX_PAYLOAD;
    CE_Disable();  // Güvenlik amaçlı

    CS_Select();
    HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);
    HAL_SPI_Transmit(NRF24_SPI, data, len, 100);  // Tek seferde tüm payload
    CS_Unselect();

    // 2. CE pulse ile TX başlat
    CE_Enable();
    //HAL_Delay(1);  // 1ms pulse yeterli
    HAL_Delay(5);
    CE_Disable();

    status = nrf24_ReadReg(STATUS);

    if(status & (1<<5))     // Status Registerındaki 5. bit(TX_DS) i kontrol ediyoruz. Eğer 1 ise başarılı bir gönderim olmuştur.
    {
    	nrf24_WriteReg(STATUS, (1 << 5));
    	return 1;
    }

    nrf24_WriteReg(STATUS, (1 << 5));    // Status deki 5. biti (TX_DS) i temizliyoruz , "Write 1 to clear bit"-Datasheet de olan bir bilgi

    return 0;
}


uint8_t NRF24_Transmit_2(uint8_t* data, uint8_t len)
{
	uint8_t status = 0;
    uint8_t cmd = W_TX_PAYLOAD;
    CE_Disable();  // Güvenlik amaçlı

    CS_Select();
    HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);
    HAL_SPI_Transmit(NRF24_SPI, data, len, 100);  // Tek seferde tüm payload
    CS_Unselect();

    // 2. CE pulse ile TX başlat
    CE_Enable();
    uint32_t timeout = HAL_GetTick();
        while(1)
        {
            status = nrf24_ReadReg(STATUS);

            if(status & (1<<5))
            {
                break; // Paket havaya tamamen çıktı!
            }

            if(HAL_GetTick() - timeout > 10) // 10ms Timeout
            {
                break;
            }
        }

    CE_Disable();

    status = nrf24_ReadReg(STATUS);

    if(status & (1<<5))     // Status Registerındaki 5. bit(TX_DS) i kontrol ediyoruz. Eğer 1 ise başarılı bir gönderim olmuştur.
    {
    	nrf24_WriteReg(STATUS, (1 << 5));
    	return 1;
    }

    nrf24_WriteReg(STATUS, (1 << 5));    // Status deki 5. biti (TX_DS) i temizliyoruz , "Write 1 to clear bit"-Datasheet de olan bir bilgi

    return 0;
}



void NRF24_RxMode(uint8_t* Address, uint8_t channel)
{
	CE_Disable();


	nrf24_WriteReg(RF_CH,channel);  // select the channel

	// select the data pip 1
	uint8_t enrxaddr = nrf24_ReadReg(EN_RXADDR);
	enrxaddr = enrxaddr | (1<<1);
	nrf24_WriteReg(EN_RXADDR, enrxaddr);

	nrf24_WriteReg_Multi(RX_ADDR_P1, Address, 5);

	nrf24_WriteReg(RX_PW_P1, 32);

	//power up the device in RX mode
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1<<1) | (1<<0);
	nrf24_WriteReg(CONFIG,config);
	HAL_Delay(2);
	CE_Enable();

}

void NRF24_RxModeDAI(uint8_t *adress, uint8_t channel, nRF_CRC_t crc){


	uint8_t config_t = 0;
	CE_Disable();
	nrf24_WriteReg(RF_CH, channel); // Kanal
	nrf24_WriteReg(EN_RXADDR, (1 << 1));       // Pipe1 aktif
	nrf24_WriteReg_Multi(RX_ADDR_P1,adress, 5);
	nrf24_WriteReg(RX_PW_P1, 10);


	// 1 byte lık crc işlemi
	if(crc == crc_1){
		config_t = 1<<3;
	}

	// 2 byte lık crc işlemi
	if(crc == crc_2){
		config_t = (1<<3) | (1<<2);

	}

	// Rx modu olduğu için PWR_UP ve PRIM_RX bitlerini set ediyoruz.
	config_t |= 0x03;

	nrf24_WriteReg(CONFIG, config_t);
	HAL_Delay(2); // Tpd2stby

	CE_Enable();


}

uint8_t isDataAvailable(int pipenum)
{
	uint8_t status = nrf24_ReadReg(STATUS);

	if (status & (1 << 6))
	{
		uint8_t pipe = (status >> 1) & 0x07;

		if (pipe == pipenum)
		{
			// RX_DR flag clear (1 yazarak)
			nrf24_WriteReg(STATUS, (1 << 6));
			return 1;
		}
	}
	return 0;
}


void NRF24_Receive(uint8_t *data, uint8_t size)
{
	uint8_t cmdtosend = R_RX_PAYLOAD;
	uint8_t dummy = 0xFF;
	CS_Select();
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);
	for(int i=0; i<size; i++)
	        HAL_SPI_TransmitReceive(NRF24_SPI, &dummy, &data[i], 1, 100);

	CS_Unselect();

	HAL_Delay(1);
	nrf_sendCmd(FLUSH_RX);

}


