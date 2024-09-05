/*******************************************************************************
 * @FilePath     : \STM32F429_SPIFLASH_FATFS\Drivers\W25Qxx\W25Qxx.c
 * @Author       : wanghao
 * @version      : V1.0.0
 * @Date         : 2024-09-05 13:31:42
 * @Description  : 
 * @
 * @Copyright (c) 2024 by ${git_name}, All Rights Reserved.
********************************************************************************/

#include "w25qxx.h"
#include <stdint.h>
#include <stdio.h>
/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:08:33
 * @description: SPI交换一个字节，中间层重新封装spi收发函数
 * @param TxData:发送的字节
 * @return {*}接收的字节
 * @notes: 移植该项目，这个函数要重新写
********************************************************************************/
uint8_t spi_SwapByte(uint8_t TxData)
{
	uint8_t Rxdata;
	SPI_CS_LOW;
	HAL_SPI_TransmitReceive(&hspi5, &TxData, &Rxdata, 1, 1000);   
	SPI_CS_HIGH; 
 	return Rxdata;          		    
}

/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:10:32
 * @description: spi发送多个字节
 * @param *pbdata:发送的buf
 * @param send_length:发送的字节数量
 * @return {*}
 * @notes: 
********************************************************************************/
void spi_send_some_bytes(uint8_t *pbdata, uint16_t send_length)
{
	uint16_t i = 0;
	SPI_CS_LOW;
	for (i = 0; i < send_length; i++)
	{
		spi_SwapByte(pbdata[i]);
	}
	SPI_CS_HIGH;
	
}
/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:11:14
 * @description: 接收多个字节
 * @param *pbdata:接收数据的地址
 * @param recv_length:接收的数量
 * @return {*}
 * @notes: 
********************************************************************************/
void spi_recv_some_bytes(uint8_t *pbdata, uint16_t recv_length)
{
	uint8_t *temp_data = pbdata;
	SPI_CS_LOW;
	while (recv_length--)
	{
		*temp_data++ = spi_SwapByte(0xFF);	//发送 0xff 为从设备提供时钟
	}
	SPI_CS_HIGH;
	
}

/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:11:57
 * @description: w25qxx写使能
 * @return {*}
 * @notes: 
********************************************************************************/
void w25qxx_WriteEnable(void)
{
	uint8_t command = FLASH_WRITE_ENABLE_CMD;
	SPI_CS_LOW;
	spi_SwapByte(command);//开启写使能
	SPI_CS_HIGH;
}

/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:13:35
 * @description: w25qxx写失能
 * @return {*}
 * @notes: 
********************************************************************************/
void w25qxx_WriteDisable(void)
{
	uint8_t command = FLASH_WRITE_DISABLE_CMD;
	SPI_CS_LOW;
	spi_SwapByte(command);
	SPI_CS_HIGH;
}

/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:06:51
 * @description: 读取w25qxx状态寄存器
 * @return {*}
 * @notes: 
********************************************************************************/
uint8_t w25qxx_ReadSR(void)
{
	uint8_t ucTmpVal = 0xff;
	uint8_t command = FLASH_READ_SR_CMD;

	SPI_CS_LOW;
	
	spi_SwapByte(command);	//05h
	ucTmpVal = spi_SwapByte(0xff);
	
	SPI_CS_HIGH;
	return ucTmpVal;
}

/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:17:40
 * @description: 等待BUSY位清空
 * @return {*}
 * @notes: 
********************************************************************************/
void w25qxx_WaitNobusy(void)
{
	//FLASH_READ_SR_CMD 指令的发送,有的FLASH仅需发送一次,FLASH自动回复,有的FLASH无法自动回复,需要循环一直发送等待
	while(((w25qxx_ReadSR()) & 0x01)==0x01);	//等待BUSY位清空
}


/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:27:28
 * @description: 读取多个字节
 * @param *ucpBuffer:数据存储区首地址
 * @param _ulReadAddr:要读出Flash的首地址
 * @param _usNByte:要读出的字节数(最大65535B)
 * @return {*}
 * @notes: 
********************************************************************************/
void w25qxx_ReadSomeBytes(uint8_t *ucpBuffer, uint32_t _ulReadAddr, uint16_t _usNByte)
{
	uint8_t command = FLASH_READ_DATA;
	uint8_t temp_buff[3] = {0};

	temp_buff[0] = (uint8_t)(_ulReadAddr >> 16);
	temp_buff[1] = (uint8_t)(_ulReadAddr >> 8);
	temp_buff[2] = (uint8_t)(_ulReadAddr >> 0);

	SPI_CS_LOW;
	spi_SwapByte(command);
	spi_send_some_bytes(temp_buff,3);
	spi_recv_some_bytes(ucpBuffer, _usNByte);
	SPI_CS_HIGH;
}


/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:31:40
 * @description: 
 * @param *ucpBuffer:数据存储区首地址
 * @param _ulReadAddr:要读出Flash的首地址
 * @param _usNByte:要读出的字节数(最大65535B)
 * @return {*}
 * @notes: 
********************************************************************************/
void w25qxx_FastReadByte(uint8_t *ucpBuffer, uint32_t _ulReadAddr, uint16_t _usNByte)
{
	uint8_t command = FLASH_FASTREAD_DATA;
	uint8_t temp_buff[3] = {0};

	temp_buff[0] = (uint8_t)(_ulReadAddr >> 16);
	temp_buff[1] = (uint8_t)(_ulReadAddr >> 8);
	temp_buff[2] = (uint8_t)(_ulReadAddr >> 0);

	SPI_CS_LOW;

	spi_SwapByte(command);
	spi_send_some_bytes(temp_buff,3);
	spi_recv_some_bytes(ucpBuffer, _usNByte);
	
	SPI_CS_HIGH;
}
/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:38:10
 * @description: flash 写数据(按页写入,一页256字节,写入之前FLASH地址上必须为0xFF)
 * @param *ucpBuffer:数据存储区首地址
 * @param _ulWriteAddr:要读写入Flash的首地址
 * @param _usNByte:要写入的字节数(最大65535B = 64K 块)
 * @return {*}
 * @notes: 
********************************************************************************/
void w25qxx_WritePage(uint8_t *ucpBuffer, uint32_t _ulWriteAddr, uint16_t _usNByte)
{
	uint8_t command = FLASH_WRITE_PAGE;
	uint8_t temp_buff[3] = {0};

	temp_buff[0] = (uint8_t)(_ulWriteAddr >> 16);
	temp_buff[1] = (uint8_t)(_ulWriteAddr >> 8);
	temp_buff[2] = (uint8_t)(_ulWriteAddr >> 0);
	
	w25qxx_WriteEnable();	//写使能
	w25qxx_WaitNobusy();	//等待写入结束
	
	SPI_CS_LOW;
	
	spi_SwapByte(command);
	spi_send_some_bytes(temp_buff,3);
	spi_send_some_bytes(ucpBuffer, _usNByte);

	SPI_CS_HIGH;
	
	w25qxx_WaitNobusy();	//等待写入结束
}

/*******************************************************************************
 * @Author: wiwhh
 * @Date: 2024-09-06 00:08:17
 * @description: 
 * @param Addr:
 * @return {*}
 * @notes: 
********************************************************************************/
void w25qxx_SectorErase(unsigned long Addr)
{
	uint8_t temp_buff[4] = {0};
	temp_buff[0] = FLASH_ERASE_SECTOR;
	temp_buff[1] = (uint8_t)(Addr >> 16);
	temp_buff[2] = (uint8_t)(Addr >> 8);
	temp_buff[3] = (uint8_t)(Addr >> 0);
	w25qxx_WriteEnable();
	SPI_CS_LOW;
	w25qxx_WaitNobusy();
	spi_send_some_bytes(temp_buff,4);
	SPI_CS_HIGH;
	w25qxx_WaitNobusy();
}












