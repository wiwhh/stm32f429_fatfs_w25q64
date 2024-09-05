#ifndef __W25QXX_H__
#define __W25QXX_H__

#include <stdint.h>
#include "spi.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

#define  FLASH_WRITE_ENABLE_CMD 		0x06
#define  FLASH_WRITE_DISABLE_CMD		0x04
#define  FLASH_READ_SR_CMD				0x05
#define  FLASH_WRITE_SR_CMD				0x01
#define  FLASH_READ_DATA				0x03
#define  FLASH_FASTREAD_DATA			0x0b
#define  FLASH_WRITE_PAGE				0x02
#define  FLASH_ERASE_PAGE      			0x81
#define  FLASH_ERASE_SECTOR       		0x20
#define	 FLASH_ERASE_BLOCK				0xd8
#define	 FLASH_ERASE_CHIP				0xc7
#define  FLASH_POWER_DOWN				0xb9
#define  FLASH_RELEASE_POWER_DOWN       0xab
#define  FLASH_READ_DEVICE_ID      		0x90
#define  FLASH_READ_JEDEC_ID      		0x9f


#define SPI_CS_LOW HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, 0)
#define SPI_CS_HIGH HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, 1)

uint8_t spi_SwapByte(uint8_t TxData);
void spi_send_some_bytes(uint8_t *pbdata, uint16_t send_length);
void spi_recv_some_bytes(uint8_t *pbdata, uint16_t recv_length);
void w25qxx_WriteEnable(void);
void w25qxx_WriteDisable(void);
uint8_t w25qxx_ReadSR(void);
void w25qxx_WaitNobusy(void);
void w25qxx_ReadSomeBytes(uint8_t *ucpBuffer, uint32_t _ulReadAddr, uint16_t _usNByte);
void w25qxx_FastReadByte(uint8_t *ucpBuffer, uint32_t _ulReadAddr, uint16_t _usNByte);
void w25qxx_WritePage(uint8_t *ucpBuffer, uint32_t _ulWriteAddr, uint16_t _usNByte);
void w25qxx_SectorErase(unsigned long Addr);















#endif /*__W25QXX_H__*/
