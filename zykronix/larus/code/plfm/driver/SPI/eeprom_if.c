/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: eeprom_if.c - interface to eeprom
 *  
 *
 *
 * 
 *
 *Author(s): taifan
 * 
 *
 **********************************************************************/
 /********************************************************************
 * $Id: eeprom_if.c,v 1.2 2006/09/21 07:06:20 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/driver/SPI/eeprom_if.c,v $
 *====================================================================
 * $Log: eeprom_if.c,v $
 * Revision 1.2  2006/09/21 07:06:20  cvsshuming
 * add header
 *
 * $Endlog $
 *********************************************************************/

#include <stdio.h>
#include <taskLib.h>
#include <intLib.h>
#include <sysLib.h>
#include <string.h>
#include "tss_types.h"
#include "config.h"
#include "spi.h"
#include "cpm.h"
#include "m8260Pio.h"
#include "drv/mem/m82xxDpramLib.h"
#include "drv/parallel/m8260IOPort.h"
#include "eeprom_if.h"

STATUS_t eepromRead(int offset, char * pBuf, int size)
{
    EEPROM_read((U8 *) pBuf, (int)offset, (int)size);
	return OK;
}


STATUS_t eepromWrite(int offset, char * pBuf, int size)
{
    EEPROM_write((U32)offset, (U8 *)pBuf, (int)size);
	return OK;
}