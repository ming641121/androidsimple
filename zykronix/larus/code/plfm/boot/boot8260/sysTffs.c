/* sysTffs.c - f2xFlash TrueFFS socket driver template */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,17oct02,dee  replace hardcoded numbers with macros
01b,29apr02,tjf modified for PPMC824x
01a,29jan01,tjf writted by Ted Friedl, Wind River PS, Madison WI
*/

/*
DESCRIPTION
This library provides board-independent TrueFFS socket-level routines
necessary to create one or more TrueFFS devices from an f2xFlashMem
flash group (represented by a F2X_GID).  The description of this library
is devoid of the typical "boiler plate" found in the socket driver
examples.  Instead, the focus is on quickly getting TrueFFS running on
flash employing the f2xFlash code suite.

The following is a description of the recommended steps to make one
or more f2xFlash/TrueFFS devices operational:

STEP 0: INSTALL TFFS

STEP 1: GET THE f2xFlash FILES
Copy the files f2xFlashMem.c, f2xFlashMem.h, f2xFlashMtd.c and
f2xFlash-sysTffs.c into the project directory if using the
Tornado Project facility, or the target directory if using make.

STEP 2: INCLUDE f2xFlashMem.o IN THE KERNEL
Add f2xFlashMem.o to the VxWorks kernel either through the Tornado
Project facility or by adding f2xFlashMem.o to the MACH_EXTRA line
of the BSP Makefile.

STEP 3: CREATE VALID FLASH GROUP(S)
The next step is to create f2xFlashMem flash groups that will
later correspond to TrueFFS devices.  At this point, it is often
easiest to experiment by using a script executed from the shell
instead of putting the group creation code into the kernel
initialization sequence.  When finished, be sure the
f2xGroupBlockSize() of each group is not zero (see documentation
for f2xGroupBlockSize() in f2xFlashMem.c).  This is almost always
natural for flash.

STEP 4: TEST THE FLASH GROUPS(S)
TrueFFS needs a solid foundation to build on. Before creating a
TrueFFS device from a flash group, be sure to test the group at the
f2xFlashMem-level to be sure it was created properly.

STEP 5: INCLUDE TFFS IN THE KERNEL
In the routine sysTffsInit() (below), add the group creation code
and call f2xRegister() for each group.  Add the definitions of
INCLUDE_TFFS and INCLUDE_DOSFS to config.h, and add f2xFlashMtd.o
and f2xFlash-sysTffs.o to the kernel (as in step 2).

Note that the first group registered with f2xRegister() is
considered TrueFFS device number 0, the second is device number 1
and so on.

STEP 6: ERASE AND FORMAT THE TFFS DEVICE(S)
Before mounting a TFFS device, it must be erased and formatted.
Do this with the following commands:
.CS
    -> tffsRawio <device number>, 3, 0, <number of blocks>
.CE
and
.CS
    -> tffsDevFormat <device number>, 0
.CE
The number of blocks can be determined by dividing f2xGroupSize()
by f2xGroupBlockSize() for the correspondinng group.

STEP 7: MOUNT THE TFFS DEVICE(S)
To use a TrueFFS device, it must be mounted.  Mounting is 
accomplished with the following command:
.CS
    -> usrTffsConfig <device number>, 0, <mount point>
.CE
here's a real-life example:
.CS
    -> usrTffsConfig 0, 0, "/tffs0"
.CE

SPECIAL CONSIDERATIONS
The f2xFlash philosophy differs from TrueFFS in that it assumes
the flash memory is embedded (it is not removable), always writable,
and the type/geometry of the flash is determined at compile-time or
during kernel initialization (before groups are created). The
f2xFlash philosophy also differs in that an f2xFlash group, and
therefore the f2xFlash MTD, may control only a fraction of a flash
chip or array.

SPECIAL CONSIDERATIONS: MIXING MTDs
Other MTDs may be used with the f2xFlash MTD.  Because 
f2xFlashMtdIdentify() claims all sockets as its own, the identify
routines of any added MTDs must be respresented in the mtdTable[]
(see target/src/drv/tffs/tffsConfig.c) *before* the element
representing f2xFlashMtdIdentify().  Of course, additional socket
driver code is required to support other MTDs.

SPECIAL CONSIDERATIONS: CREATING A BOOT AREA
This code was written to support TrueFFS on 100% of a f2xFlash
group.  If it is desirable to create a "fallow area" at the
beginning of the group (e.g., for boot code), the user is invited
to write a sysTffsFormat() routine similar to those found in the
TrueFFS socket driver examples.
*/

/* includes */

#include "vxWorks.h"
#include "tffs/flsocket.h"
#include "tffs/flflash.h"
#include "tffs/tffsDrv.h"
#include "tffs/backgrnd.h"

#include "config.h"


/* defines */
#define  TFFS_FLASH_BASE_ADRS         SYS_FLASH_BASE_ADRS			/* 0xB0000000 */
#define  TFFS_FLASH_SIZE              SYS_FLASH_SIZE				/* 0x1000000  16M Byte */
#define  TFFS_FLASH_SECTOR_SIZE       (0x00020000)					/* 0x20000    128K Byte */
		 
#define  TFFS_FLASH_CHIP_NUM          (1)
#define  TFFS_FLASH_INTERLEAVING      (1)

#define  INCLUDE_MTD_CFISCS                   /* Common Flash Interface */
#define	 INCLUDE_TL_FTL                       /* FTL translation layer */
#define  INCLUDE_TFFS_BOOT_IMAGE	          /* include tffsBootImagePut() */

#undef   INCLUDE_MTD_I28F008                  /* Intel: 28f008 */
#undef   INCLUDE_MTD_I28F016                  /* Intel: 28f016 */
#undef   INCLUDE_MTD_AMD                      /* AMD, Fujitsu: 29f0{40,80,16} 8bit */
#undef   INCLUDE_MTD_CDSN                     /* Toshiba, Samsung: NAND, CDSN */
#undef   INCLUDE_MTD_DOC2                     /* Toshiba, Samsung: NAND, DOC */
#undef   INCLUDE_MTD_WAMD                     /* AMD, Fujitsu: 29f0{40,80,16} 16bit */
#undef   INCLUDE_TL_NFTL                      /* NFTL translation layer */
#undef   INCLUDE_TL_SSFDC                     /* SSFDC translation layer */

 /* included source */
#include "tffs/tffsConfig.c"
#define FLASH_DATA_PTR(offset)	(TFFS_FLASH_BASE_ADRS+offset)


/* import */


/* locals */

char    *f2xDevName  = "C:";
FLFlash *tVol        = NULL;

int f2xDbg = 0;


STATUS sysFlashEraseSector(unsigned long SectorOffset);
extern int S29GL128N_Programming(int SourceAddress,int TargetAddress,int FileSize);


/*******************************************************************************
*
* f2xGetDevName - Get f2x device name
*
* RETURNS: Always return "C:".
*
*******************************************************************************/

char *f2xGetDevName(void)
{
    return (f2xDevName);
}

/*******************************************************************************
*
* f2xCardDetected - stub
*
* RETURNS: TRUE
*/

LOCAL FLBoolean f2xCardDetected
    (
    FLSocket vol
    )
    {
    return (TRUE);
    }

/*******************************************************************************
*
* f2xVccOn - stub
*
* RETURNS: N/A
*/

LOCAL void f2xVccOn
    (
    FLSocket vol
    )
    {
    }

/*******************************************************************************
*
* f2xVccOff - stub
*
* RETURNS: N/A
*/

LOCAL void f2xVccOff
    (
    FLSocket vol
    )
    {
    }

#ifdef SOCKET_12_VOLTS

/*******************************************************************************
*
* f2xVppOn - stub
*
* RETURNS: flOK.
*/

LOCAL FLStatus f2xVppOn
    (
    FLSocket vol
    )
    {
    return (flOK);
    }

/*******************************************************************************
*
* f2xVppOff - stub
*
* RETURNS: N/A
*/

LOCAL void f2xVppOff
    (
    FLSocket vol
    )
    {
    }

#endif	/* SOCKET_12_VOLTS */

/*******************************************************************************
*
* f2xInitSocket - stub
*
* RETURNS: flOK.
*/

LOCAL FLStatus f2xInitSocket
    (
    FLSocket vol
    )
    {
    vol.cardChanged = FALSE;

    return (flOK);
    }

/*******************************************************************************
*
* f2xSetWindow - stub
*
* RETURNS: N/A
*/

LOCAL void f2xSetWindow
    (
    FLSocket vol
    )
    {
    /* Physical base as a 4K page */
    vol.window.baseAddress = SYS_FLASH_BASE_ADRS >> 12;

    flSetWindowSize (&vol, SYS_FLASH_SIZE >> 12);
    }

/*******************************************************************************
*
* f2xSetMappingContext - stub
*
* RETURNS: N/A
*/

LOCAL void f2xSetMappingContext
    (
    FLSocket vol,
    unsigned page
    )
    {
    }

/*******************************************************************************
*
* f2xGetAndClearCardChangeIndicator - stub
*
* RETURNS: FALSE.
*/

LOCAL FLBoolean f2xGetAndClearCardChangeIndicator
    (
    FLSocket vol
    )
    {
    return (FALSE);
    }

/*******************************************************************************
*
* f2xWriteProtected - stub
*
* RETURNS: FALSE.
*/

LOCAL FLBoolean f2xWriteProtected
    (
    FLSocket vol
    )
    {
    return (FALSE);
    }

/*******************************************************************************
*
* flFitInSocketWindow - stub
*
* RETURNS: chipSize
*
* NOMANUAL
*/

long int flFitInSocketWindow
    (
    long int chipSize,
    int      interleaving,
    long int windowSize
    )
    {
    if (chipSize*interleaving > windowSize) /* doesn't fit in socket window */
    {
        int  roundedSizeBits;

        /* fit chip in the socket window */
        chipSize = windowSize / interleaving;

        /* round chip size at powers of 2 */
        for (roundedSizeBits = 0; (0x1L << roundedSizeBits) <= chipSize;
             roundedSizeBits++)
	    ;

        chipSize = (0x1L << (roundedSizeBits - 1));
    }

    return (chipSize);
    }

/*******************************************************************************
*
* flDelayLoop - stub
*
* RETURNS: N/A
*
* NOMANUAL
*/

void flDelayLoop
    (
    int  cycles
    )
    {
    while (--cycles);
    }

/*******************************************************************************
*
* f2xRegister - install routines for f2xFlash socket
*
* This routine installs necessary functions for the f2xFlash socket.
*
* RETURNS: N/A
*/

void f2xRegister
    (
    )
    {
    FLSocket vol                       = flSocketOf (noOfDrives);

    tffsSocket[noOfDrives]             = "F2X";

    vol.serialNo                       = 0;
    vol.window.baseAddress             = SYS_FLASH_BASE_ADRS >> 12;

    vol.cardDetected                   = f2xCardDetected;
    vol.VccOn                          = f2xVccOn;
    vol.VccOff                         = f2xVccOff;
#ifdef SOCKET_12_VOLTS
    vol.VppOn                          = f2xVppOn;
    vol.VppOff                         = f2xVppOff;
#endif
    vol.initSocket                     = f2xInitSocket;
    vol.setWindow                      = f2xSetWindow;
    vol.setMappingContext              = f2xSetMappingContext;
    vol.getAndClearCardChangeIndicator = f2xGetAndClearCardChangeIndicator;
    vol.writeProtected                 = f2xWriteProtected;

    noOfDrives++;
    }

/*******************************************************************************
*
* sysTffsInit - board level initialization for TFFS
*
* This routine calls the socket registration routines for the socket component
* drivers that will be used with this BSP. The order of registration signifies
* the logical drive number given to the drive associated with the socket.
*
* RETURNS: N/A
*/

void sysTffsInit (void)
    {

    /* --- calls to f2xRegister() go here --- */

    f2xRegister();
    }


/***************************************************************************
*
* tffsFlashLongWordWrite - Write a block of bytes to flash in word-mode
*
* This routine will be registered as the MTD flash.write routine
*
* RETURNS: flOK if the write is successful, flVppFailure or flWriteFault
* if the write fails.
*/
FLStatus tffsFlashLongWordWrite
    (
    FLFlash vol,              /* pointer identifying device */
    CardAddress address,      /* card address to write to */
    const void FAR1 *buffer,  /* address of data to write */
    int length,               /* number of bytes to write */
    int mode                  /* write mode (overwrite yes/no) (not used) */
    )
{
/*  printf("In tffsFlashLongWordWrite\n"); */

    if (flWriteProtected (vol.socket))
    {
        return (flWriteProtect);
    }

    if (S29GL128N_Programming((int)buffer, (int)(FLASH_DATA_PTR(address)), length)!=length)
    {
        printf ("Debug: flashWriteBlock failed, src:%p, tgt:0x%x, len:%d\n", buffer, FLASH_DATA_PTR(address), length);
        return (flWriteFault);
    }

    return (flOK);
}

/***************************************************************************
*
* tffsFlashLongWordRead - Read a block of bytes to flash in word-mode
*
* This routine will be registered as the MTD flash.write routine
*
* RETURNS: flOK if the write is successful, flVppFailure or flWriteFault
* if the write fails.
*/
FLStatus tffsFlashLongWordRead
    (
    FLFlash vol,              /* pointer identifying device */
    CardAddress address,      /* card address to write to */
    const void FAR1 *buffer,  /* address of data to write */
    int length,               /* number of bytes to write */
    int mode                  /* write mode (overwrite yes/no) (not used) */
    )
{
	int i;
	unsigned char ptr[2];
	unsigned char * dataPtr = (unsigned char *) buffer;

    for(i=0; i < length; i++)
	{
	    ptr[0] = *(char *)FLASH_DATA_PTR(address+i);
#if  (CPU_FAMILY==PPC)
		EIEIO_SYNC;
#endif

	    ptr[1] = *(char *)FLASH_DATA_PTR(address+i);
#if  (CPU_FAMILY==PPC)
		EIEIO_SYNC;
#endif

        if(ptr[0] == ptr[1])
		{
		    *dataPtr++ = ptr[0];
#if 0
			printf("Flash Read OK: addr: 0x%x, ptr[0]: 0x%x, ptr[1]: 0x%x\n",
			        (address+i), ptr[0], ptr[1]);
#endif

        }
		else
		{
		    *dataPtr++ = ptr[1];
			if(f2xDbg)
			{
		        printf("Flash Read Error: addr: 0x%x, ptr[0]: 0x%x, ptr[1]: 0x%x\n",
			        (unsigned int)(address+i), (unsigned int)ptr[0], (unsigned int)ptr[1]);
			}
        }

	}

    return (flOK);
}

/***************************************************************************
*
* tffsLongWordErase - erase flash erasable blocks in word-mode
*
* This routine will be registered as the MTD flash.erase routine
*
* RETURNS: 0 on success, failed otherwise
*/

FLStatus tffsLongWordErase
    (
    FLFlash vol,              /* Pointer identifying drive */
    int firstErasableBlock,   /* Number of first block to erase */
    int numOfErasableBlocks   /* Number of blocks to erase */
    )
    {
    FLStatus status = flOK;       /* unless proven otherwise */
    int iBlock;

/*  printf("In tffsLongWordErase\n"); */

    if (flWriteProtected (vol.socket))
        return flWriteProtect;

    for (iBlock = 0; iBlock < numOfErasableBlocks && status == flOK; iBlock++)
    {
        if (sysFlashEraseSector((firstErasableBlock+iBlock)*SYS_BLOCK_SIZE))
        {
            printf ("Debug: flashEraseSector failed.\n");
            status = flGeneralFailure;
        }
    }

    return status;
    }

/***************************************************************************
*
* cfiscsIdentify - Identifies media based on SCS/CFI and registers as an MTD
*
* This routine will be placed on the MTD list in custom.h. It must be
* an extern routine.
*
* On successful identification, the Flash structure is filled out and
* the write and erase routines registered.
*
* RETURNS: flOK on positive identificaion, flUnknownMedia otherwise
*/

FLStatus cfiscsIdentify
    (
    FLFlash vol         /* Pointer identifying drive */
    )
{
    FlashWPTR flashPtr = (FlashWPTR)flMap(vol.socket, 0);
    tVol = &vol;

    printf("In cfiscsIdentify\n");
    flSetWindowBusWidth (vol.socket, 16);   /* use 16-bits */
    flSetWindowSpeed (vol.socket, 120);     /* 120 nsec. */
    flSetWindowSize (vol.socket, 2);        /* 8 KBytes */

    vol.type              = (flashPtr[0] << 8) | flashPtr[1];
    vol.noOfChips         = TFFS_FLASH_CHIP_NUM;
    vol.chipSize          = TFFS_FLASH_SIZE;
    vol.interleaving      = TFFS_FLASH_INTERLEAVING;
    vol.erasableBlockSize = TFFS_FLASH_SECTOR_SIZE;
	vol.read			  = tffsFlashLongWordRead;
    vol.write             = tffsFlashLongWordWrite;
    vol.erase             = tffsLongWordErase;

    return flOK;
}

/*******************************************************************************
*
* sysTffsEraseAll - Erase the entire flash
*
* RETURNS: OK, or ERROR if it fails.
*
* SEE ALSO:
*
*******************************************************************************/
STATUS sysFlashEraseSector(unsigned long SectorOffset)
{
    S29GL128N_Erase_Sector(SectorOffset);
	return OK;
}


/*******************************************************************************
*
* sysTffsEraseAll - Erase the entire flash
*
* RETURNS: OK, or ERROR if it fails.
*
* SEE ALSO:
*
*******************************************************************************/
STATUS sysTffsEraseAll(void)
{
    int sectorOffset;

#if 0
    for (sectorOffset = 0; sectorOffset < TFFS_FLASH_SIZE;
         sectorOffset+=TFFS_FLASH_SECTOR_SIZE)
    {
        if (sysFlashEraseSector(sectorOffset))
        {
            printf ("sysFlashEraseSector failed at %x\n", sectorOffset);
            return (ERROR);
        }
    }
#endif
	S29GL128N_Chip_Erase();
    return (OK);
}

/*******************************************************************************
*
* sysTffsFormat - format the flash memory above an offset
*
* RETURNS: OK, or ERROR if it fails.
*
* SEE ALSO:
* tffsDevFormat(),
* tffsRawio()
*
*******************************************************************************/

STATUS sysTffsFormat ()
{
    tffsDevFormatParams params =
    {
        {0, 99, 1, 0x40000l, NULL, {0,0,0,0}, NULL, 2, 0, NULL},
        FTL_FORMAT_IF_NEEDED
    };

    DEV_HDR * tffsDevHdr = NULL;

    /* vxWorks tffs API is not re-entrenable to support tffs re-format */
    /*
    if (tffsRawio(0, TFFS_PHYSICAL_ERASE, 0,
                  TFFS_FLASH_SIZE/TFFS_FLASH_SECTOR_SIZE, 0) != OK)
    {
        printf("tffsRawio Erase failed!\n");
        return (ERROR);
    }
    */

    printf("Erasing the Soldered Flash for TFFS...\n");
    if (sysTffsEraseAll() != OK)
    {
        printf("sysTffsEraseAll() failed!\n");
        return (ERROR);
    }

    /* remove TFFS from device list if it's presented */
    if((tffsDevHdr = iosDevFind(f2xGetDevName(), NULL)) != NULL)
    {
        printf("Deleting TFFS %s from Device List...\n", f2xGetDevName());
        iosDevDelete(tffsDevHdr);
    }

    printf("Formating the Soldered Flash for TFFS...\n");
	/* flMinClusterSize = 4; */
    if (tffsDevFormat (0, (int)&params) != OK)
    {
        printf("tffsDevFormat failed!\n");
        return (ERROR);
    }

    return (OK);
}

STATUS tffsAppInit(void)
{

#ifdef SYS_RTC_TIME
    sysTimeSet(sysTimeGet());                   /* setup the system time first */
#endif

#if 0
    if (strncmp (sysBootParams.bootDev, "tffs", 4))
#endif
    {
        if (usrTffsConfig(0,0,f2xGetDevName()) != OK)
        {
            printf("TFFS Configuration Failed\n");
            return (ERROR);
        }
    }

    if (cd(f2xGetDevName()))
    {
       printf("TFFS Access Failed\n");
       return (ERROR);
    }

    taskDelay(200);

    return (OK);

}
