/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "w25qxx.h"
/* Definitions of physical drive number for each drive */

#define DEV_FLASH	0	/* Example: Map USB MSD to physical drive 2 */

#define FLASH_SECTOR_SIZE	4096
#define FLASH_BLOCK_SIZE 	16
#define FLASH_SECTOR_COUNT	128*64

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{

	if(pdrv == DEV_FLASH) 
	{
		w25qxx_ReadSomeBytes(buff, sector*FLASH_SECTOR_SIZE, count*FLASH_SECTOR_SIZE);
		return RES_OK;
	
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if(pdrv == DEV_FLASH) 
	{
		w25Qxx_Write((uint8_t*)buff, sector*FLASH_SECTOR_SIZE,count*FLASH_SECTOR_SIZE);
		return RES_OK;
	}
	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	if(pdrv == DEV_FLASH) 
	{
		switch (cmd)
		{
			case CTRL_SYNC: return RES_OK;
			case GET_SECTOR_COUNT: *(uint32_t *)buff = FLASH_SECTOR_COUNT;break;
			case GET_SECTOR_SIZE: *(uint32_t *)buff = FLASH_SECTOR_SIZE;break;
			case GET_BLOCK_SIZE: *(uint32_t *)buff = FLASH_BLOCK_SIZE;break;
		}
		return RES_OK;
	}
	return RES_PARERR;
}

