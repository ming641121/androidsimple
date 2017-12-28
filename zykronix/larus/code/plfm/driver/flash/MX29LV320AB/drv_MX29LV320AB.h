#ifndef __MX29LV320_h__
#define __MX29LV320_h__

volatile unsigned long FLASH_ADDR = 0xB0000000;

int MX29LV320_CheckId(void);
int MX29LV320_Chip_Erase(void);
int BlankCheck(int targetAddr,int targetSize);
int MX29LV320_Programming(int SourceAddress,int TargrtAddress,int FileSize);
int Verify(int SourceAddress,int targetAddr,int targetSize);
#endif /*MX29LV320*/
