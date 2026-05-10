/*
 * MFRC.c
 *
 *  Created on: Apr 24, 2026
 *      Author: Yashraj
 */


#include "MFRC522.h"
#include "main.h"

uint8_t atqa[2];

void MFRC522_Init(MFRC522_t *dev)
{
	USER_LOG("MFRC522 Minimum Init Started");

	//HARDWARE RESET

	HAL_GPIO_WritePin(dev->rstPort, dev->rstPin, GPIO_PIN_RESET);
	HAL_Delay(50);

	HAL_GPIO_WritePin(dev->rstPort, dev->rstPin, GPIO_PIN_SET);
	HAL_Delay(50);

	//SOFT RESET
	MFRC522_WriteReg(dev, PCD_CommandReg, PCD_SoftReset);
	HAL_Delay(50);

	//CLEAR INTERRUPTS
	MFRC522_WriteReg(dev, PCD_ComIrqReg, 0x7F);
	HAL_Delay(50);

	//FIFO FLUSH
	MFRC522_WriteReg(dev, PCD_FIFOLevelReg, 0x80);
	HAL_Delay(50);

	//TIMER : ~25ms TIMEROUT

	MFRC522_WriteReg(dev, PCD_TModeReg, 0x80);
	MFRC522_WriteReg(dev, PCD_TPrescalarReg, 0xA9);
	MFRC522_WriteReg(dev, PCD_TReloadRegH, 0x03);
	MFRC522_WriteReg(dev, PCD_TReloadRegL, 0xE8);

	//RF SETTINGS
	MFRC522_WriteReg(dev,PCD_TxAutoReg,0x40);
	MFRC522_WriteReg(dev,PCD_RFCfgReg,0x7F);
	MFRC522_WriteReg(dev,PCD_DemoReg,0x4D);

	MFRC522_AntennaOn(dev);
	HAL_Delay(10);

	uint8_t version = MFRC522_ReadReg(dev,PCD_VersionReg);

	if((version != 0x91) || (version != 0x92))
	{
		USER_LOG("Version : 0x%02X(counterfeit OK for UID)", version);
	}

	else
	{
		USER_LOG("Version : 0x%02X", version);
	}


	uint8_t txCtrl = MFRC522_ReadReg(dev,PCD_TxControlReg);

	DEBUG_LOG("TxControlReg: 0x%02X (expect >= 0x03)",txCtrl);

	USER_LOG("MFRC522 MIN INIT COMPLETE");
}

void MFRC522_AntennaOn(MFRC522_t *dev)
{
	MFRC522_SetBitMask(dev,PCD_TxControlReg,0x03);
	DEBUG_LOG("ANTENNA ON");
}


void MFRC522_AntennaOff(MFRC522_t *dev)
{
	MFRC522_ClearBitMask(dev,PCD_TxControlReg, 0x03);
	DEBUG_LOG("ANTENNA OFF");
}

void MFRC522_WriteReg(MFRC522_t *dev, uint8_t reg, uint8_t value)
{
	uint8_t addr = (reg << 1) & 0x7E;

	HAL_GPIO_WritePin(dev->csPort, dev->csPin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(dev->hspi,&addr, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(dev->hspi, &value, 1,HAL_MAX_DELAY);

	HAL_GPIO_WritePin(dev->csPort, dev->csPin, GPIO_PIN_SET);

	HAL_Delay(1);

	DEBUG_LOG("WriteReg : 0x%02X = 0x%02X",reg,value);

}


uint8_t MFRC522_ReadReg(MFRC522_t *dev, uint8_t reg)
{
	uint8_t addr = ((reg << 1) & 0x7E) | 0x80;
	uint8_t val = 0;

	uint8_t addr_byte = addr;
	HAL_GPIO_WritePin(dev->csPort, dev->csPin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(dev->hspi, &addr_byte,1,HAL_MAX_DELAY);
	HAL_SPI_Receive(dev->hspi, &val, 1, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(dev->csPort, dev->csPin, GPIO_PIN_SET);
	HAL_Delay(1);

	DEBUG_LOG("ReadRed : 0x%02X -> 0x%02X",reg, val);

	return val;
}

void MFRC522_SetBitMask(MFRC522_t *dev, uint8_t reg, uint8_t mask)
{
	uint8_t tmp = MFRC522_ReadReg(dev,reg);

	MFRC522_WriteReg(dev,reg,tmp | mask);

	DEBUG_LOG("SetBitMask: 0x%02X |= 0x%02X", reg, mask);
}


void MFRC522_ClearBitMask(MFRC522_t *dev, uint8_t reg, uint8_t mask)
{
	uint8_t tmp = MFRC522_ReadReg(dev,reg);

	MFRC522_WriteReg(dev, reg, tmp &(~mask));

	DEBUG_LOG("ClearBitMask: 0x%02X &= ~0x%02X", reg, mask);
}

uint8_t MFRC522_RequestA(MFRC522_t *dev, uint8_t *atqa)
{
	DEBUG_LOG("RequestA");

	MFRC522_AntennaOff(dev);
	HAL_Delay(10);

	MFRC522_AntennaOn(dev);
	HAL_Delay(10);

	MFRC522_WriteReg(dev, PCD_ComIrqReg, 0x7F);
	MFRC522_WriteReg(dev, PCD_FIFOLevelReg, 0x80);
	MFRC522_WriteReg(dev, PCD_BitFramingReg, 0x07);
	MFRC522_WriteReg(dev, PCD_FIFODataReg, PICC_REQA);
	HAL_Delay(5);
	MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Transceive);
	MFRC522_SetBitMask(dev, PCD_BitFramingReg, 0x80);


	uint32_t timeout = HAL_GetTick() + 25;

	while(HAL_GetTick()< timeout)
	{
		uint8_t status2 = MFRC522_ReadReg(dev, PCD_Status2Reg);

		if(status2 & 0x01)
		{
			uint8_t err = MFRC522_ReadReg(dev, PCD_ErrorReg);
			if(err & 0x1D)
			{
				DEBUG_LOG("RequestA error: 0x%02X", err);
			  MFRC522_AntennaOff(dev);
			  HAL_Delay(5);
			  MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
			  return STATUS_ERROR;
        	}

			uint8_t fifoLvl = MFRC522_ReadReg(dev, PCD_FIFOLevelReg);

			if(fifoLvl >= 2)
			{
			     atqa[0] = MFRC522_ReadReg(dev, PCD_FIFODataReg);
				 atqa[1] = MFRC522_ReadReg(dev, PCD_FIFODataReg);

				 DEBUG_LOG("RequestA ATQA: 0x%02X 0x%02X", atqa[0], atqa[1]);

				 MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
				 HAL_Delay(2);
				return STATUS_OK;
			}

			DEBUG_LOG("RequestA bad FIFO LEVEL : %d",fifoLvl);

			MFRC522_AntennaOff(dev);
			HAL_Delay(5);
			MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
			return STATUS_ERROR;
		}
       HAL_Delay(1);
	}
	DEBUG_LOG("REQUESTA TimeOut");
     MFRC522_AntennaOff(dev);
     HAL_Delay(5);
     MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
     return STATUS_TIMEOUT;
}


uint8_t MFRC522_Anticoll(MFRC522_t *dev, uint8_t *uid_buffer)
{

	DEBUG_LOG("AntoCOLL");
	MFRC522_WriteReg(dev, PCD_ComIrqReg, 0x7F);
	MFRC522_WriteReg(dev, PCD_FIFOLevelReg, 0x80);
	MFRC522_WriteReg(dev, PCD_BitFramingReg, 0x00);
	MFRC522_WriteReg(dev, PCD_FIFODataReg, PICC_SEL_CL1);
	MFRC522_WriteReg(dev, PCD_FIFODataReg, 0x20);
	HAL_Delay(2);
	MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Transceive);
	MFRC522_SetBitMask(dev, PCD_BitFramingReg, 0x80);


	uint32_t timeout = HAL_GetTick() + 25;

	while(HAL_GetTick() < timeout)
	{
		uint8_t status2 =  MFRC522_ReadReg(dev, PCD_Status2Reg);

		if(status2 & 0x01)
		{
			uint8_t err = MFRC522_ReadReg(dev, PCD_ErrorReg);

			if(err & 0x1D)
			{
				DEBUG_LOG("Anticoll error: 0x%02X", err);
				MFRC522_AntennaOff(dev);
				HAL_Delay(5);

				MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
				return STATUS_ERROR;
			}

			uint8_t fifoLvl = MFRC522_ReadReg(dev, PCD_FIFOLevelReg);

			if(fifoLvl == 5)
			{
				for(int i = 0; i < 5; i++)
				{
					uid_buffer[i] = MFRC522_ReadReg(dev, PCD_FIFODataReg);
				}

				uint8_t calcBcc = uid_buffer[0] ^ uid_buffer[1] ^ uid_buffer[2] ^ uid_buffer[3];

				if(uid_buffer[4] != calcBcc)
				{
					DEBUG_LOG("Anticoll bad BCC: calc = 0x%02X, got = 0x%02X", calcBcc, uid_buffer[4]);
					MFRC522_AntennaOff(dev);
					HAL_Delay(10);
					MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
					return STATUS_ERROR;
				}

				DEBUG_LOG("Anticoll UID: %02X %02X %02X %02X %02X", uid_buffer[0], uid_buffer[1], uid_buffer[2], uid_buffer[3], uid_buffer[4]);
				MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
				HAL_Delay(2);
				return STATUS_OK;
			}
			DEBUG_LOG("Anticoll bad FIFO level: %d", fifoLvl);
			MFRC522_AntennaOff(dev);
		    HAL_Delay(5);
	        MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
	        return STATUS_ERROR;
		}
		 HAL_Delay(1);
	}
	DEBUG_LOG("Anticoll timeout");
	 MFRC522_AntennaOff(dev);
	 HAL_Delay(5);
	 MFRC522_WriteReg(dev, PCD_CommandReg, PCD_Idel);
	 return STATUS_TIMEOUT;
}

uint8_t MFRC522_Read_Uid(MFRC522_t *dev, uint8_t *uid_buffer)
{
	USER_LOG("Reading UID...");

		uint8_t rawUID[5];

		if(MFRC522_Anticoll(dev,rawUID) != STATUS_OK)
		{
			DEBUG_LOG("Anticollision Failed");

			return STATUS_ERROR;
		}

		for(int i = 0; i < 4; i++)
		{
			uid_buffer[i] = rawUID[i];
		}

		USER_LOG("CARD UID: %02X %02X %02X %02X", uid_buffer[0], uid_buffer[1], uid_buffer[2], uid_buffer[3]);
	return STATUS_OK;
}

uint8_t WaitCardRemoval(MFRC522_t *dev)
{
	USER_LOG("Waiting for Card Removing");

	while(1)
	{
		if(MFRC522_RequestA(dev,atqa) != STATUS_OK)
		{
			USER_LOG("Card_Removed");
			return STATUS_OK;
		}

		HAL_Delay(100);
	}
}


uint8_t WaitCardDetect(MFRC522_t *dev)
{
	atqa[0] = atqa[1] = 0;

	USER_LOG("Waiting for the Card..");

	while(1)
	{
		if(MFRC522_RequestA(dev,atqa) == STATUS_OK)
		{
			USER_LOG("Card Detected");
			return STATUS_OK;
		}

		HAL_Delay(100);
	}
}
