/*********************************************************************
 *Copyright(C) 2006, All Rights Reserved, by
 *AHOYA Networks, Taiwan
 *
 *Abstract:
 *
 *File Name: eeprom_if.h - interface to eeprom
 *  
 *
 *
 * 
 *
 *Author(s): taifan
 *
 **********************************************************************/
 /********************************************************************
 * $Id: eeprom_if.h,v 1.2 2006/09/21 07:06:20 cvsshuming Exp $
 * $Source: /cvsroot/larus5820/code/plfm/driver/SPI/eeprom_if.h,v $
 *====================================================================
 * $Log: eeprom_if.h,v $
 * Revision 1.2  2006/09/21 07:06:20  cvsshuming
 * add header
 *
 * $Endlog $
 *********************************************************************/

#ifndef _EEPROM_IF_H_
#define _EEPROM_IF_H_

enum EEPROM_DEFS
{
   EE_CURRENT_VERSION          = 0x00,

   EE_MIN_ADDR                 = 0x00,
   EE_MAX_ADDR                 = 0xFF,
   EE_SIZE                     = ((EE_MAX_ADDR - EE_MIN_ADDR) + 1),

   /* Locations of fields in EEPROM */
/* 0x00 ~ 0x0F */
   EE_SIGNATURE_ADDR           = 0x00,
   EE_DEF_VERSION_ADDR         = 0x04,

   EE_CARD_TYPE_ADDR		   = 0x08,
   EE_CARD_SUB_TYPE_ADDR	   = 0x09,
   EE_CARD_APPL_TYPE_ADDR	   = 0x0A,

   EE_CARD_VERSION_ADDR        = 0x0C,

/* 0x10 ~ 0x1F */
   EE_SERIAL_NUMBER_ADDR       = 0x10,

/* 0x20 ~ 0x2F */
   EE_CLEI_ADDR                = 0x20,

/* 0x80 ~ 0x9F */
   EE_CRAFT_MODE_ADDR          = 0x80,
   EE_MGMT_IP_MODE_ADDR        = 0x81,
   EE_MGMT_IP_MAC_ADDR         = 0x82,
   EE_MGMT_IP_ADDR_ADDR        = 0x88,
   EE_MGMT_IP_SUBNET_ADDR      = 0x8c,
   EE_MGMT_IP_GATEWAY_ADDR     = 0x90,
   EE_TIME_ZONE_ADDR           = 0x94,
   EE_TERM_BAUD_ADDR           = 0x98,
   EE_TERM_OPTION_ADDR         = 0x9c,



   /* Size of EEPROM fields */
/* 0x00 ~ 0x0F */
   EE_SIGNATURE_SIZE           = 4,
   EE_DEF_VERSION_SIZE         = 4,

   EE_CARD_TYPE_SIZE		   = 1,
   EE_CARD_SUB_TYPE_SIZE	   = 1,
   EE_CARD_APPL_TYPE_SIZE	   = 2,

   EE_CARD_VERSION_SIZE        = 4,
/* 0x10 ~ 0x1F */
   EE_SERIAL_NUMBER_SIZE       = 16,
/* 0x20 ~ 0x2F */
   EE_CLEI_SIZE                = 16,



/* 0x80 ~ 0x9F */
   EE_CRAFT_MODE_SIZE          = 1,
   EE_MGMT_IP_MODE_SIZE        = 1,
   EE_MGMT_IP_MAC_SIZE         = 6,
   EE_MGMT_IP_ADDR_SIZE        = 4,
   EE_MGMT_IP_SUBNET_SIZE      = 4,
   EE_MGMT_IP_GATEWAY_SIZE     = 4,
   EE_TIME_ZONE_SIZE           = 4,
   EE_TERM_BAUD_SIZE           = 4,
   EE_TERM_OPTION_SIZE         = 4,



   /* Default values for EEPROM */
   EE_DEFAULT_VALUE            = 0x00,
   EE_SIGNATURE_VALUE          = 0xBEEF5A5A,
   EE_BOOT_TIMEOUT_DEFAULT     = 3,
   EE_CARD_TYPE_UNKNOWN        = '?',

   EE_CARD_TYPE_IMU            = 'A',
   EE_CARD_TYPE_SMU            = 'B',
   EE_CARD_TYPE_RUB            = 'C',
   EE_CARD_TYPE_CRY_ST2        = 'D',
   EE_CARD_TYPE_CRY_ST3        = 'E',
   EE_CARD_TYPE_CRY            = 'F',

   EE_CARD_TYPE_OUT_FREQ       = 'G',
   EE_CARD_TYPE_OUT_1PPS       = 'H',
   EE_CARD_TYPE_OUT_IRIQB      = 'I',
   EE_CARD_TYPE_OUT_TIU        = 'J',
   EE_CARD_TYPE_OUT_F5F10      = 'K',

   EE_CARD_TYPE_GPS_SA         = 'L',
   EE_CARD_TYPE_GPS_HA         = 'M',
   EE_CARD_TYPE_GPS_LA         = 'N',
   EE_CARD_TYPE_CDMA           = 'O',
   EE_CARD_TYPE_GMTS           = 'P'

};

extern STATUS_t eepromRead(int offset, char * pBuf, int size);
extern STATUS_t eepromWrite(int offset, char * pBuf, int size);

#endif /* _EEPROM_IF_H_ */		 