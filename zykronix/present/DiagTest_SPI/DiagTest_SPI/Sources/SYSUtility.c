/*
****************************************************************
*                      Magic Pixel Inc.
*
*    Copyright 2004, Magic Pixel Inc., HsinChu, Taiwan
*                    All rights reserved.
*
*
*
* Filename:    SYSUtility.c
*
* Programmer:    Alex Tsai
*                MPX E120 division
*
* Created: 10/28/2004
*
* Description: Source file of system utilities
*
*        
* Change History (most recent first):
*     <1>     10/28/2004    Alex Tsai    first file
****************************************************************
*/
#include "Global.h"

const DWORD OneMaskArray[33] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
		0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
		0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
		0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
		0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
		0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
		0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
		0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
		0xffffffff
};

BYTE *pbMainWindowBuffer, *pbSubWindowBuffer;
BYTE *pbJpgDecodeImageBuffer, *pbJpgEncodeImageBuffer, *pbJpgStreamBuffer;
BYTE *pbMp4RefFrameBuffer, *pbMp4RawImageBuffer, *pbMp4StreamBuffer;
BYTE *pbVideoTrackBuffer, *pbAudioTrackBuffer;
BYTE *pbAudioCodecBuffer, *pbFileSystemBuffer;

void SetRegBitsUtil(volatile DWORD *RegAddr, WORD offset, DWORD RegVal)
{
    BYTE x, len;

    x = (BYTE)offset;
    len = (BYTE)(offset>>8) - x + 1;

    *(RegAddr) = (*(RegAddr) & (~(OneMaskArray[len] << x))) | (RegVal<<x);
}

DWORD GetRegBitsUtil(volatile DWORD *RegAddr, WORD offset)
{
    BYTE x, len;
    DWORD value;

    x = (BYTE)offset;
    len = (BYTE)(offset>>8) - x + 1;

    value = (*(RegAddr)>>x) & (OneMaskArray[len]);

    return(value);
}

BYTE *GetBufferAddressUtil(BYTE bBufferId)
{
    switch (bBufferId) {
        case MAIN_WINDOW_BUFFER_ID:    return pbMainWindowBuffer;
        case SUB_WINDOW_BUFFER_ID:    return pbSubWindowBuffer;
        case JPG_DEC_IMAGE_BUFFER_ID:    return pbJpgDecodeImageBuffer;
        case JPG_ENC_IMAGE_BUFFER_ID:    return pbJpgEncodeImageBuffer;
        case JPG_STREAM_BUFFER_ID:    return pbJpgStreamBuffer;
        case MP4_RAW_IMAGE_BUFFER_ID:    return pbMp4RawImageBuffer;
        case MP4_REF_FRAME_BUFFER_ID:    return pbMp4StreamBuffer;//Timothy modified pbMp4RefFrameBuffer;
        case MP4_STREAM_BUFFER_ID:    return pbJpgStreamBuffer;	//Timothy modify0516/2006
        case VIDEO_TRACK_BUFFER_ID:    return pbVideoTrackBuffer;
        case AUDIO_TRACK_BUFFER_ID:    return pbAudioTrackBuffer;
        case AUDIO_CODEC_BUFFER_ID:    return pbAudioCodecBuffer;
        case FILE_SYSTEM_BUFFER_ID:    return pbFileSystemBuffer;
    }
	return pbMainWindowBuffer;	// It's should not go to here
}

WORD AddressToStartRowUtil(BYTE *pbAddress, BYTE bTargetBits)
{
    DWORD dwRowSize;
    WORD wRowPerBank, wBankNumber, wRowNumber;
    
    dwRowSize = 0x400;    // 1KB
//    wRowPerBank = 1024 << GETREGBITS(&(pstDrai->SDRAM_SETTING), ROWPERBANK);
    wRowPerBank = 4096;
    wRowNumber = ((DWORD)pbAddress & 0x07FFFFFF) / dwRowSize;
    wBankNumber = wRowNumber / wRowPerBank;
    wRowNumber = wRowNumber % wRowPerBank;

    if (bTargetBits == 7)
        return(((wBankNumber >> 1) << 7) | (wRowNumber >> 6));
    else if (bTargetBits == 8)
        return((wBankNumber << 6) | (wRowNumber >> 7));
    else if (bTargetBits == 10)
        return((wBankNumber << 8) | (wRowNumber >> 5));

	return((wBankNumber << 8) | (wRowNumber >> 5));	// it should not go to here 
}

DWORD AddressToStartColUtil(BYTE *pbAddress, BYTE bTargetBits)
{
    DWORD dwRowSize;
    WORD wColPerRow, wRowNumber, wColNumber;

    dwRowSize = 0x400;    // 1KB
    wColPerRow = 256;
    if (bTargetBits == 8)
        wRowNumber = (((DWORD)pbAddress & 0x07FFFFFF) / dwRowSize) % 128;
    else if (bTargetBits == 10)
        wRowNumber = (((DWORD)pbAddress & 0x07FFFFFF) / dwRowSize) % 32;
    wColNumber = (((DWORD)pbAddress & 0x07FFFFFF) % dwRowSize) / (dwRowSize/wColPerRow);

    return((wRowNumber << 16) | (wColNumber & 0xFF));
}

void MemoryAddressInit(void)//ok
{
    BYTE bBankCount, *pbBankBStart;
    DWORD dwMemorySize, dwBankSize, dwRowSize;

    bBankCount = 4;
    pbBankBStart = (BYTE *)0xA0800000;
    dwMemorySize = 0x1000000;    // 16MB
    dwBankSize = 0x400000;    // 4MB
    dwRowSize = 0x400;    // 1KB
// 0XA0B0 0000
    pbJpgDecodeImageBuffer = (BYTE *)0xA0000000 + dwMemorySize - dwBankSize - 0x100000;
    pbJpgEncodeImageBuffer = pbJpgDecodeImageBuffer;
    pbMp4RawImageBuffer = pbJpgDecodeImageBuffer;
    pbMp4RefFrameBuffer = pbMp4RawImageBuffer + ((dwRowSize << 7) << 2);
    if (dwRowSize >= 0x400) {
        pbMp4StreamBuffer = pbMp4RawImageBuffer + (dwRowSize << 6);
        pbAudioTrackBuffer = pbMp4StreamBuffer + (dwRowSize << 7);
    } else {
        pbMp4StreamBuffer = pbMp4RefFrameBuffer + ((dwRowSize << 7) << 2);
        pbAudioTrackBuffer = pbMp4StreamBuffer + 0x10000;
    }
    pbAudioCodecBuffer = pbBankBStart;
    pbFileSystemBuffer = pbAudioCodecBuffer + 0x10000;
    pbVideoTrackBuffer = pbFileSystemBuffer + (1376 * dwRowSize);

    if (bBankCount == 2) {
        pbSubWindowBuffer = pbJpgDecodeImageBuffer - (dwRowSize << 7);	
		// x128 is real Start row address
        pbMainWindowBuffer = pbSubWindowBuffer - (dwRowSize << 7);
        pbJpgStreamBuffer = pbFileSystemBuffer + 0x30000;    // to be test
    } else if (bBankCount == 4) {
    //0xa0800000
        pbMainWindowBuffer = (BYTE *)0xA0000000 + (dwBankSize << 1);
		//0xa082 0000
        pbSubWindowBuffer = pbMainWindowBuffer + (dwRowSize << 7);
		//0xa0900000
        pbJpgStreamBuffer = (BYTE *)0xA0000000 + (dwBankSize << 1) + (dwRowSize << 10);
    }
}

void wait(DWORD TimeVal)
{
    register DWORD xi, xj;

    for (xi = 0; xi < TimeVal; xi++)
        for (xj = 0; xj < 3600; xj++)    __asm("nop");
}

#define BAUD_RATE    115200
#define UART_COUNTER(x) (x / 16 / BAUD_RATE - 1)

DWORD PllParmTab[6][2] = {
//  {PLL_CLOCK, UART_CLOCK},
    {0x00003F08, (UART_COUNTER(48000000)<<2) + 3},    // 48 MHz,
    {0x00003F07, (UART_COUNTER(54000000)<<2) + 3},    // 54 MHz
    {0x00004F08, (UART_COUNTER(60000000)<<2) + 3},    // 60 MHz
    {0x00005708, (UART_COUNTER(66000000)<<2) + 3},    // 66 MHz
    {0x00003F05, (UART_COUNTER(72000000)<<2) + 3},    // 72 MHz
    {0x00007F08, (UART_COUNTER(96000000)<<2) + 3},    // 96 MHz
};

void DRAIClockSelect(BYTE bSelect)//ok
{
    WORD i;

    pstCkg->DRAICK_SELECT = bSelect;
    SETREGBITS(&(pstCkg->CFG_DRAI_CLK), DRAI_TSTOP, 0);
    SETREGBITS(&(pstDrai->SDRAM_SETTING), SFRFR_PERIOD, 0);
    for (i = 0; i < 1024;)    i++;
    SETREGBITS(&(pstDrai->SDRAM_SETTING), SFRFR_PERIOD, 128);
}

BYTE GetDRAIClockSelect(void)
{
    return pstCkg->DRAICK_SELECT;
}

void CPUClockSelect(BYTE bSelect)//ok
{
    WORD i;

    pstCkg->CPUCK_SELECT = bSelect;
    SETREGBITS(&(pstCkg->CFG_CPU_CLK), CPU_TSTOP, 0);
    for (i = 0; i < 1024;)    i++;
}

void PLL1Set(DWORD freq)
{
    pstCkg->CFG_UART_CLK = PllParmTab[freq][1];    // Set UART baud rate to 115200
    pstCkg->PLL1_FREQUENCY = PllParmTab[freq][0];    
    pstCkg->PLL1_CONFIG = 0x00000018;

    return;
}

void PLL2Set(DWORD freq)
{
    pstCkg->PLL2_FREQUENCY = PllParmTab[freq][0];
    pstCkg->PLL2_CONFIG = 0x00000018;

    return;
}

void PLLInit(void)//ok
{
    // Config PLL1
//    PLL1Set(PLL_CLOCK_48M);
    DRAIClockSelect(0);    // PLL1

    // Config PLL2
//    PLL2Set(PLL_CLOCK_48M);
    CPUClockSelect(0);    // PLL1

    return;
}
