#ifndef __S29GL256N__
#define __S29GL256N__
/* extern volatile unsigned long long rstdat=	0x00f000f000f000f0; */
/* extern volatile unsigned long rstdat=	0x00f000f0; */
volatile unsigned long FLASH_ADDR = 0xB0000000;

int S29GL256N_CheckId(void);
int S29GL256N_Chip_Erase(void);
int BlankCheck(int targetAddr,int targetSize);
int S29GL256N_Programming(int SourceAddress,int TargrtAddress,int FileSize);
int Verify(int SourceAddress,int targetAddr,int targetSize);
#endif /*S29GL256N*/
