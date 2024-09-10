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
static uint8_t spi_SwapByte(uint8_t TxData)
{
	uint8_t Rxdata;
	HAL_SPI_TransmitReceive(&hspi5, &TxData, &Rxdata, 1, 1000);   
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
static void spi_send_some_bytes(uint8_t *pbdata, uint16_t send_length)
{
	uint16_t i = 0;
	for (i = 0; i < send_length; i++)
	{
		spi_SwapByte(pbdata[i]);
	}
	
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
static void spi_recv_some_bytes(uint8_t *pbdata, uint16_t recv_length)
{
	uint8_t *temp_data = pbdata;
	while (recv_length--)
	{
		*temp_data++ = spi_SwapByte(0xFF);	//发送 0xff 为从设备提供时钟
	}
	
}

/*******************************************************************************
 * @Author: wanghao
 * @Date: 2024-09-05 16:11:57
 * @description: w25qxx写使能
 * @return {*}
 * @notes: 
********************************************************************************/
static void w25qxx_WriteEnable(void)
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
static void w25qxx_WriteDisable(void)
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
static uint8_t w25qxx_ReadSR(void)
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
static void w25qxx_WaitNobusy(void)
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
	//printf("!!ad:%X,nb:%d\r\n",_ulWriteAddr,_usNByte);//测试用
	uint8_t command = 0x02;
	uint8_t temp_buff[3] = {0};

	temp_buff[0] = (uint8_t)(_ulWriteAddr >> 16);
	temp_buff[1] = (uint8_t)(_ulWriteAddr >> 8);
	temp_buff[2] = (uint8_t)(_ulWriteAddr >> 0);
	w25qxx_WriteEnable();	//写使能
	w25qxx_WaitNobusy();
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
 * @description: 扇区擦除，4kb
 * @param Addr: 扇区地址，传入地址时最好地址/4096
 * @return {*}
 * @notes: 
********************************************************************************/
void w25qxx_SectorErase(unsigned long Addr)
{
	uint8_t temp_buff[3] = {0};
	uint8_t command = FLASH_ERASE_SECTOR;
	Addr *= 4096;
	temp_buff[0] = (uint8_t)(Addr >> 16);
	temp_buff[1] = (uint8_t)(Addr >> 8);
	temp_buff[2] = (uint8_t)(Addr >> 0);
	w25qxx_WriteEnable();
	w25qxx_WaitNobusy();
	SPI_CS_LOW;
	spi_SwapByte(command);
	spi_send_some_bytes(temp_buff,3);
	SPI_CS_HIGH;
	w25qxx_WaitNobusy();
}
/*******************************************************************************
 * @Author: wiwhh
 * @Date: 2024-09-10 21:10:17
 * @description: 擦除整个芯片
 * @return {*}
 * @notes: 
********************************************************************************/
void w25Qxx_Erase_Chip(void)   
{                                   
    w25qxx_WriteEnable();
	w25qxx_WaitNobusy();  
  	SPI_CS_LOW;                           //使能器件   
    spi_SwapByte(FLASH_ERASE_CHIP);        //发送片擦除命令  
	SPI_CS_HIGH;                           //取消片选     	      
	w25qxx_WaitNobusy();   				   //等待芯片擦除结束
}  
/*******************************************************************************
 * @Author: wiwhh
 * @Date: 2024-09-10 21:50:13
 * @description: 
 * @param pBuffer:
 * @param WriteAddr:
 * @param NumByteToWrite:
 * @return {*}
 * @notes: 
********************************************************************************/
void w25Qxx_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 	
	//printf("ad:%X,nb:%d\r\n",WriteAddr,NumByteToWrite);//测试用		 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		w25qxx_WritePage(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 

uint8_t W25QXX_BUFFER[4096];		 
void w25Qxx_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF = W25QXX_BUFFER;	     
 	secpos = WriteAddr/4096;//扇区地址  
	secoff = WriteAddr%4096;//在扇区内的偏移
	secremain = 4096-secoff;//扇区剩余空间大小
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		w25qxx_ReadSomeBytes(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			w25qxx_SectorErase(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i]; 
			}
			w25Qxx_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}
		else 
		{
			w25Qxx_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 
		}

		if(NumByteToWrite==secremain) break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	}	 
}









