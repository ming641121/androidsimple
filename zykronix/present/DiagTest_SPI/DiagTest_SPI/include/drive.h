#ifndef DRIVE_H
#define DRIVE_H

//#define MPXINFO
/*
*******************************************************************************
*        #include
*******************************************************************************
*/

/*
*******************************************************************************
*        #define CONSTANTS
*******************************************************************************
*/
///@defgroup CONF Configuration
///Here is the list of static setting.
///@{

/*
// Constant declarations
*/
#define MB_2    0x00200000
#define MB_4    0x00400000
#define MB_8    0x00800000
#define MB_16   0x01000000
#define MB_32   0x02000000
#define MB_64   0x04000000
#define MB_128  0x08000000
#define MB_256  0x10000000
#define MB_512  0x20000000
#define GB_1    0x40000000
#define GB_2    0x80000000

///Maximum number of drive
#define FS_MAX_DRIVE     0x04

///Fat Cache
#define FS_FATCACHENUM      2
#define FS_FATCACHESIZE     2048 
#define FS_FATCACHESECTOR   4

#define FS_FATCACHESIZE_1   (FS_FATCACHESIZE - 1)
#define FS_FATCACHESECTOR_M (DWORD)(~(DWORD)(FS_FATCACHESECTOR - 1))

///Dir Cache
#define FS_DIRCACHESIZE     512
#define FS_DIRCACHESECTOR   1
#define FS_DIRCACHESIZE_1   (FS_DIRCACHESIZE - 1)
#define FS_DIRCACHESECTOR_M (DWORD)(~(DWORD)(FS_DIRCACHESECTOR - 1))

///Maximum number of file opened concurrently
#define FS_MAX_STREAM    0x10

///@}
/*
*******************************************************************************
*        GLOBAL MACROS
*******************************************************************************
*/

/* none */

/*
*******************************************************************************
*        GLOBAL DATA TYPES 
*******************************************************************************
*/



typedef struct
{
    BYTE      Name[8];
    BYTE      Extension[3];
    BYTE      Attribute;
    WORD      Attribute2;
    WORD      CreateTime;
    WORD      CreateDate;
    WORD      AccessDate;
    WORD      StartHigh;
    WORD      ModifyTime;
    WORD      ModifyDate;
    WORD      StartLow;
    DWORD     Size;
} FDB;

typedef struct
{
    BYTE      Number;
    BYTE      Name0[10];
    BYTE      Attribute1;
    BYTE      Attribute2;
    BYTE      CheckSum;
    BYTE      Name1[12];
    BYTE      res[2];
    BYTE      Name2[4];
} LONG_NAME;

typedef struct
{
    BYTE     *CacheBuf;
    DWORD     CachePoint;
} SCACHE;

typedef struct
{
    //master boot record information
    BYTE      bFatType;
    BYTE      bNumFats;
    WORD      wNumHeads;
    WORD      wTrackSectors;
    WORD      wClusterSize;
    WORD      wRootEntries;
    WORD      wNumFatSectors;
    WORD      wReserveSectors;
    WORD      wSectorSize;
    DWORD     dwHiddenSectors;
    DWORD     dwTotalSectors;

    //partition boot record information
    DWORD     dwLogTotalSectors;
    DWORD     dwLogStartSector;
    DWORD     dwLogEndSector;
} ST_DISK_PARAMETER;

typedef struct
{
    DWORD     Start;            // the start cluster of this chain
    DWORD     Current;          // the current cluster the chain point located
    DWORD     Last;
    DWORD     Point;            // the logical byte location counter of chain point
    DWORD     Size;             // the chain actual size in byte
} CHAIN;

typedef struct
{
    FDB      *Node;
    FDB      *Cache;            // sizeof(FDB) * 16 = 512
    DWORD     CachedLba;
    WORD      Path[128];
    DWORD     CurPos;
    DWORD     MskPos;
    CHAIN     Head;             //if long, point to first LFDB, else point to 8.3
    CHAIN     Tail;             //Point to 8.3
    CHAIN     Chain;
    DWORD     ParentNum;
} DIR;

typedef struct
{
    // function members definition
//    DVIO_DEV *Dev;
    WORD      Id;               //TPL drive index
    WORD      Partition;        //TPL Partition number belong to this device

    DWORD     (*FatRead)(void *, DWORD);
    void      (*FatWrite)(void *, DWORD, DWORD);

    DIR      *Dir;

    // data members definition
    struct
    {
        unsigned  Present:1;
        unsigned  ReadOnly:1;
        unsigned  NotFormated:1;
        unsigned  Fat32:1;
        unsigned  Fat16:1;
        unsigned  Fat12:1;
        unsigned  DirCacheChanged:1;
        unsigned  LongNameHit:1;
        unsigned  DriveAddComplete:1;
        unsigned  FileFoundInLong:1;
        unsigned  FileFoundInShort:1;
        unsigned  FileCacheLock: 1;
        unsigned  FatScanned: 1;
        unsigned  Reserve0:18;
    } Flag;

    BYTE      ClusterSize;      //TPL how many sector per clusters
    BYTE      ClusterExp;       //TPL log2(ClusterSize)
    BYTE      NumberOfFat;      //TPL how many FAT region in the drive, need this attribute?
    BYTE      Reserve1;

    DWORD     FsInfoAddr;       //TPL sector number for the first sector of FsInfo
    DWORD     FatStart;         //TPL sector number of the first sector of FAT region
    DWORD     FatSize;          //TPL how many sector per FATs
    DWORD     FatCacheLast;
    WORD      FatCacheChanged[FS_FATCACHENUM];
    DWORD     FatCachePoint[FS_FATCACHENUM];    //TPL sector number of the current cached FAT entry, which is related to FatStart and 4 multiple(0,4,8,12,16...)
    BYTE     *FatCache[FS_FATCACHENUM];         //[SECTOR_SIZE * 4]; //TPL 4 sector FAT entry, (512 in fat32), (1024 in fat16), (4096/2 in fat12)

    DWORD     RootStart;        //TPL fat32: cluster number of the first cluster of root directory
                                //TPL not fat32: sector number for the first cluster of roots directory
    DWORD     DataStart;        //TPL sector number of the first sector of FAT region

    DWORD     FreeClusters;     //TPL how many clusters avaliable
    DWORD     LastCluster;      //TPL cluster number of the last used cluster
    DWORD     TotalClusters;    //TPL how many clusters of data region
    DWORD     DiskInfoClusStart;
} DRIVE;

typedef struct
{
    DRIVE    *Drv;              // drive pointer, if 0 mean this handle is not used
    DWORD     DirSector;        // the sector lba of DIR that contain this file
    WORD      FdbOffset;        // the byte offset of this file entry in DIR that contain this file 
    WORD      Reserved;
    DWORD     CacheBufSize;
    BYTE     *CacheBuf;
    DWORD     CacheByteIdx;

    struct
    {
        unsigned  SizeChanged:1;
        unsigned  ContentChanged:1;
        unsigned  IsHostFile:1;     //when this flag on, means belong to BaseBand
        unsigned  IsRamFile:1;      //when this flag on, means belong to RamFile
        unsigned  Reserved:28;
    } Flag;

    CHAIN     Chain;
} STREAM;

typedef struct
{
    DWORD     saveclus;
    DWORD     savepos;
    DWORD     saveparnum;
} SAVEPOS;

typedef struct
{
    WORD      CreateTime;
    WORD      CreateDate;
    WORD      AccessDate;
    WORD      Time;
    WORD      Date;
} FILE_TIME;

#define AL_CELL_MAX 25

typedef struct stALCELL
{
    DWORD     staddr;
    struct stALCELL *previous;
    struct stALCELL *next;
    struct
    {
        unsigned  used:1;
        unsigned  allocated:1;
        unsigned  reserved:30;
    } Flag;
} ALCELL;

#endif
