/*
****************************************************************
*                      Magic Pixel Inc.
*
*    Copyright 2004, Magic Pixel Inc., HsinChu, Taiwan
*                    All rights reserved.
*
*
*
* Filename:    SYSUTILITY.H
*
* Programmer:    Alex Tsai
*                MPX E120 division
*
* Created: 10/28/2004
*
* Description: Header file of system utilities
*
*        
* Change History (most recent first):
*     <1>     10/28/2004    Alex Tsai    first file
****************************************************************
*/
#ifndef __SYSUTILITY_H
#define __SYSUTILITY_H

// PLL Frequency Definition
#define PLL_CLOCK_48M    0
#define PLL_CLOCK_54M    1
#define PLL_CLOCK_60M    2
#define PLL_CLOCK_66M    3
#define PLL_CLOCK_72M    4
#define PLL_CLOCK_84M    5
#define PLL_CLOCK_96M    6

void DRAIClockSelect(BYTE);
BYTE GetDRAIClockSelect(void);
void CPUClockSelect(BYTE);
void PLL1Set(DWORD);
void PLL2Set(DWORD);
void PLLInit(void);

// File Format Definition
#define FILE_FORMAT_JPG 0
#define FILE_FORMAT_BMP 1
#define FILE_FORMAT_3GP 2
#define FILE_FORMAT_AVI 3
#define FILE_FORMAT_MP3 4
#define FILE_FORMAT_MID 5
#define FILE_FORMAT_AMR 6
#define FILE_FORMAT_WAV 7
#define FILE_FORMAT_BIN 8

// Buffer Index Definition
#define MAIN_WINDOW_BUFFER_ID   0
#define SUB_WINDOW_BUFFER_ID    1
#define SUB_WINDOW_BUFFER2_ID   2
#define JPG_DEC_IMAGE_BUFFER_ID 3
#define JPG_ENC_IMAGE_BUFFER_ID 4
#define JPG_STREAM_BUFFER_ID    5
#define MP4_RAW_IMAGE_BUFFER_ID 6
#define MP4_REF_FRAME_BUFFER_ID 7
#define MP4_STREAM_BUFFER_ID    8
#define VIDEO_TRACK_BUFFER_ID   9
#define AUDIO_TRACK_BUFFER_ID   10
#define AUDIO_CODEC_BUFFER_ID   11
#define FILE_SYSTEM_BUFFER_ID   12

void SetRegBitsUtil(volatile DWORD *, WORD, DWORD);
DWORD GetRegBitsUtil(volatile DWORD *, WORD);
BYTE *GetBufferAddressUtil(BYTE);
WORD AddressToStartRowUtil(BYTE *, BYTE);
void MemoryAddressInit(void);
void wait(DWORD);

typedef struct {
    BYTE  bColor;
    BYTE  bReserved[3];
    DWORD dwWhiteBalanceGain[2];
} STWHITEBALANCESETTING;

typedef struct {
    DWORD dwColorCorrection[7];
    DWORD dwDigitalGain;
    DWORD dwGammaTable[8];
} STIMAGEFILTERSETTING;

typedef struct {
    // Sensor related parameters
    BYTE bFirstSensorType;
    BYTE bSecondSensorType;
    BYTE bActiveSensor;    // 0: First sensor, 1: Second sensor
    BYTE bReserved1[5];
    // Panel related parameters
    BYTE bMainPanelType;
    BYTE bSubPanelType;
    BYTE bActivePanel;    // 0: Main-panel, 1: Sub-panel
    BYTE bPanelOrientation;
    BYTE bReserved2[4];
    // Memory card related parameters
    BYTE bMemoryCardType;    // 0: SD/MMC, 1: MS Duo
    BYTE bReserved3[3];
    // Color tuning parameters
    BYTE bWhiteBalanceMode;
    BYTE bReserved4[2];
    BYTE bCustomizedWBMode;
    STWHITEBALANCESETTING stCustomizedWBMode[8];
    BYTE bMeteringMode;
    BYTE bAEWindowWeighting[5][5];
    BOOL boolAELock;
    BYTE bFlashMode;
    BYTE bExposureValue;
    BYTE bBrightness;
    BYTE bHue;
    BYTE bSaturation;
    BYTE bContrast;
    BYTE bSharpness;
    BYTE bReserved5[2];
    DWORD dwGammaTable[8];
    WORD wImageWidth;
    WORD wImageHeight;
    BYTE bImageQuality;
    BYTE bReserved6;
    BYTE bImageFilter;
    BYTE bCustomizedImageFilter;
    STIMAGEFILTERSETTING stCustomizedImageFilter[16];
    BYTE bPhotoFrame;
    BYTE bFilterEffect;
    BYTE bReserved7[2];
    // Audio related parameters
    BYTE bMicVolume;
    BYTE bSpeakerVolume;
    BYTE bHeadphoeVolume;
    BYTE bReserved8[5];
} STSYSTEMPARAMETER;

#endif    // __SYSUTILITY_H
