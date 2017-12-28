/*
 * Copyright (C) 2005, All Rights ecserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    This file contains all routines associated with file Management  
 *    services offered by AXSS-1600 infrastructure.
 *
 * Author(s):
 *
 *  
 */

/*
 *====================================================================
 *=                            include                               =
 *====================================================================
 */
#include "axss_types.h"
#include "axss_if.h"
#include "sys_if.h"
#include "siw_if.h"
#include "msgQLib.h"
#include "sysLib.h"
#include "string.h" /* bzero(), etc */
#include "stdio.h"  /* for printf */
#include "lstLib.h" /* for linked List processing */
#include "dirent.h" /* for reading directory structure */
#include "stat.h"   /* for stat on a file */
#include "ioLib.h"
#include "ctype.h"
#include "mrpc_if.h"
#include "sm_if.h"
#include "cm_if.h"
#include "crm_if.h"
#include "fm_if.h"
#include "fm.h"
#include "fm_events.h"
#include "pathLib.h"
/*
 *====================================================================
 *=                            constants                             =
 *====================================================================
 */
#define BASE_NAME_START      0
#define DASH_LOC             3
#define FIRST_DOT_LOC        13 
#define SUBSR_START          14 
#define BASE_NAME_SIZE       3
#define SUBSR_SIZE           3

/*
 *====================================================================
 *====================================================================
 */

FT_t   fileTable[MAX_FILES];

LIST   freeFdList;
LIST   imageFtList;
SIW_SEMID_t imageFileTableSem;
U32_t  imageFileIdxCnt = 0; 
S32_t  crmFmSyncFlag = 0;
extern  SIW_SEMID_t  fmHDImageTbSem;
extern  SIW_SEMID_t  fmCrmSyncSem;
extern char* typeKeyword[];
#define FILE_SYNC_DEBUG 0


/*
 *====================================================================
 *=                         external variables                       =
 *====================================================================
 */
extern RET_t FlashWrite(char *src, char *dest, S32_t len);
extern STATUS_t AreYouSure(U8_t *prompt, U8_t *sureStr);
extern STATUS copy(char *in, char *out);
extern RET_t moveFile2Target(S32_t target, char *src, char *dst,
							 FILE_TYPE_t ft);

/*
 *====================================================================
 *=                         function prototypes                      =
 *====================================================================
 */
RET_t fmRemoveImageFile(char *imageFileName);
BOOL_t fmBuildAbsFilename(U8_t *filename, U8_t* absFilename);

/*
 *====================================================================
 *=                              routines                            =
 *====================================================================
 */


/****************************************************************************
* Function:    writeEntry()
* Descripion:  Writes a record to a file at a specified offset
*
* In Parms:    S32_t      idx        - The Index of DB being updated
* Out Parms:   None 
* Return Values:
*              DBM_OK
*              DBM_ERROR
*/
RET_t
writeEntry(S32_t fd, S32_t offset, char *buf, S32_t len, char *name)
{
    S32_t stat;

    SIW_FUNC_TRACE(FM, writeEntry, fd, offset, buf, len , name);
    stat = lseek(fd, offset, SEEK_SET);
    if (offset != stat) {
            FM_TRACE5(writeEntry, 0xF000001, fd, offset, buf, len, name);
            /*
            DBM_LOG5(WRITE_LSEEK_FAIL, name, stat,  fd, len, offset);
            */
            PERR("writeEntry(): write Fail File(%s) ret(%d) fd(%d) len(%d) off(%d)\n",
                             name, stat,  fd, len, offset);
            return (FM_ERROR);
    }
    stat = write (fd, buf, len);
    if (stat != len) {
           FM_TRACE5(writeEntry,0xF000001, fd, offset, buf, len, name);
          /* DBM_LOG5(FILE_WRITE_FAIL, name, stat,  fd, len, offset);
          */
           PERR("writeEntry(): write Fail File(%s) ret(%d) fd(%d) len(%d) off(%d)\n",
                  name, stat,  fd, len, offset);
           return (FM_ERROR);
    }

    return(FM_OK);
}

                                    



/****************************************************************************
 * Function:    fmUpdateImageFileTable()
 * Descripion:  This routine  is used to register an entry to the image file
 *              table or compare/update the check sum if the file has been created. 
 *
 * In Parms:    IMAGE_FT_t      a image File Entry 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_EQUAL_CHKSUM 
 *              FM_ERROR
 *              FM_ILLEGAL_FILE_NAME
 */
RET_t
fmUpdateImageFileTable( char *newImageFileName, U32_t newCheckSum )
{
  IMAGE_FT_t *newNode;
  IMAGE_FT_t *l;
  NODE *tmp;
  U32_t found = 0;
  STATUS_t stat;

  SIW_FUNC_TRACE( FM, fmUpdateImageFileTable, newImageFileName, newCheckSum, 0, 0, 0);

  if (!VALID_FILE_NAME(newImageFileName)) 
      return (FM_ILLEGAL_FILE_NAME);

  if (lstFirst(&imageFtList) == NULL) {
      /* Allocate memory for the node */
      newNode =(IMAGE_FT_t*)SiwAllocMem(SIW_DYNAMIC, sizeof(IMAGE_FT_t), WAIT_FOREVER);
      if (newNode == NULL)
          return FM_ERROR;
      strcpy ( newNode->fName, newImageFileName);
      newNode->fCkSum = newCheckSum;
      newNode->idx = imageFileIdxCnt++;

      /* take semaphore */
      if ((stat = SiwSemTake (imageFileTableSem, WAIT_FOREVER)) != OK) {
	    /* Error from semTake() */
	    FM_TRACE5(fmUpdateImageFileTable, 0xF0000001, newImageFileName, newCheckSum,stat, 0,0);
	    FM_DEBUG("Could not take semaphore %x, %x", stat, errno);
	    return(FM_ERROR);
	}
      lstAdd (&imageFtList, (NODE *) newNode);
      /* give the semaphore away */
      SiwSemGive(imageFileTableSem);

  } 
  else {
      if (strcmp(((IMAGE_FT_t *)lstFirst(&imageFtList))->fName, newImageFileName) > 0) {
           /* insert new node as the head node. */
           newNode =(IMAGE_FT_t *)SiwAllocMem(SIW_DYNAMIC, sizeof(IMAGE_FT_t), WAIT_FOREVER);
           if (newNode == NULL)
               return FM_ERROR;
           strcpy ( newNode->fName, newImageFileName);
           newNode->fCkSum = newCheckSum;
           newNode->idx = imageFileIdxCnt++;
           /* insert new node as a second node, them move the first to the secode node. */
           tmp = lstFirst(&imageFtList);

           /* take semaphore */
           if ((stat = SiwSemTake (imageFileTableSem, WAIT_FOREVER)) != OK) {
                    /* Error from semTake() */
                    FM_TRACE5(fmUpdateImageFileTable, 0xF0000002, newImageFileName, newCheckSum,stat, 0,0);
                    FM_DEBUG("Could not take semaphore %x, %x", stat, errno);
                    return(FM_ERROR);
           }

           lstInsert(&imageFtList, tmp, (NODE *) newNode);
           tmp = lstGet(&imageFtList);
           lstInsert(&imageFtList, (NODE*)newNode, tmp);

           /* give the semaphore away */
           SiwSemGive(imageFileTableSem);

      } 

      else {
          for ( l = (IMAGE_FT_t *)lstFirst(&imageFtList); l != NULL;
                                   l = (IMAGE_FT_t *)l->node.next ) {
              /* found the existing node and update the checksum if not same,
               * otherwise return FM_EQUAL_CHKSUM
               */
              if (strcmp ( newImageFileName,  l->fName) == 0) {
                  if ( l->fCkSum == newCheckSum) return FM_EQUAL_CHKSUM;
                  else {
                       l->fCkSum = newCheckSum;
                       found = 1;
                       break;    
                  }    
              }    

              /* insert the new node into the list in a sorted order */
              else if (strcmp ( l->fName, newImageFileName ) > 0) {
           /*       printf(" new node is less than found node:\n");
                  printf(" l->fName = %s\n", l->fName);
            */
                  newNode =(IMAGE_FT_t *)SiwAllocMem(SIW_DYNAMIC, sizeof(IMAGE_FT_t), WAIT_FOREVER);
                  if (newNode == NULL)
                      return FM_ERROR;
                  strcpy ( newNode->fName, newImageFileName);
                  newNode->fCkSum = newCheckSum;
                  newNode->idx = imageFileIdxCnt++;
                  tmp = lstPrevious((NODE *)l);
                  /* take semaphore */
                  if ((stat = SiwSemTake (imageFileTableSem, WAIT_FOREVER)) != OK) {
                        /* Error from semTake() */
                        FM_TRACE5(fmUpdateImageFileTable, 0xF0000003, newImageFileName, newCheckSum,stat, 0,0);
                        FM_DEBUG("Could not take semaphore %x, %x", stat, errno);
                        return(FM_ERROR);
                  }
                  lstInsert ( &imageFtList, tmp, (NODE *) newNode);

                  /* give the semaphore away */
                  SiwSemGive(imageFileTableSem);

                  found = 1;
                  break;
              } 
          }
          if(!found) {
                 /* insert the new node as the last node */
                newNode =(IMAGE_FT_t *)SiwAllocMem(SIW_DYNAMIC, sizeof(IMAGE_FT_t), WAIT_FOREVER);
                if (newNode == NULL)
                         return FM_ERROR;
                strcpy ( newNode->fName, newImageFileName);
                newNode->fCkSum = newCheckSum;
                newNode->idx = imageFileIdxCnt++;
                /* take semaphore */
                if ((stat = SiwSemTake (imageFileTableSem, WAIT_FOREVER)) != OK) {
                    /* Error from semTake() */
                    FM_TRACE5(fmUpdateImageFileTable, 0xF0000004, newImageFileName, newCheckSum,stat, 0,0);
                    FM_DEBUG("Could not take semaphore %x, %x", stat, errno);
                    return(FM_ERROR);
                }
                lstAdd(&imageFtList, (NODE *) newNode);

                /* give the semaphore away */
                SiwSemGive(imageFileTableSem);
          }
     }
   }
   return FM_OK;
}



/****************************************************************************
 * Function:    fmRenameImageFile()
 * Descripion:  This routine is used to change the image file name.
 *
 * In Parms:    char *       old Image File name 
 *              char *       new Image File name 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 *              FM_ILLEGAL_FILE_NAME
 */
RET_t
fmRenameImageFile(char *oldImageFileName, char *newImageFileName)
{
    S32_t oldIdx, newIdx;
    U32_t checksum;

    SIW_FUNC_TRACE( FM, fmRenameImageFile, oldImageFileName, newImageFileName, 0, 0, 0);

    /* verify the file name */
    if (!VALID_FILE_NAME(oldImageFileName)) 
        return (FM_ILLEGAL_FILE_NAME);

    if (!VALID_FILE_NAME(newImageFileName)) 
        return (FM_ILLEGAL_FILE_NAME);
    /* hold  the checksum for the old image file */
    if (fmGetImageFileCheckSum(oldImageFileName, &checksum) < 0) {
        PERR("fmRenameImageFile(): image file(%s) is not existed \n", oldImageFileName);
        return FM_ERROR;
    }
    /* hold  the index for the old image file */
    oldIdx = fmGetImageFileIndex(oldImageFileName);
    if (oldIdx < 0) {
        PERR("fmRenameImageFile(): get index fails,image file(%s) is not existed \n", oldImageFileName);
        return FM_ERROR;
    }
    /* remove the old image file from the RAM table */ 
    fmRemoveImageFile(oldImageFileName);
    
    /* check if the new image file is existed in the RAM table */
    newIdx =  fmGetImageFileIndex(newImageFileName);
    /* if the new ImageFile is existed in the RAM table, just delete old entry */ 
    if (newIdx >= 0) {
        /* delete the old image file from the HD table */
        fmUpdateHDImageFileTable( HD_IMAGE_FILE_TABLE, oldIdx, UNUSED_ENTRY, 0);

        /* update the new image file on the HD Table */
        fmUpdateHDImageFileTable( HD_IMAGE_FILE_TABLE, newIdx, newImageFileName, checksum);
        
        /* update the RAM Image File Table by newImageFileName */
        fmUpdateImageFileTable(newImageFileName, checksum);
    } 
    /* new image file is not existed in the RAM Table yet */
    else {
            /* update the RAM Image File Table by newImageFileName */
            fmUpdateImageFileTable(newImageFileName, checksum);
            newIdx = fmGetImageFileIndex(newImageFileName);
            /* delete the old entry in the HD table  */
            fmUpdateHDImageFileTable( HD_IMAGE_FILE_TABLE, oldIdx, UNUSED_ENTRY,0);
            /* update the HD table by newImageFileName and checksum. */
            fmUpdateHDImageFileTable( HD_IMAGE_FILE_TABLE, newIdx, newImageFileName, checksum);
    }
    return FM_OK;
}

/****************************************************************************
 * Function:    fmGetImageFileCheckSum()
 * Descripion:  This routine is used to get the checksum of the image file.
 *
 * In Parms:    char *      a Image File name 
 * Out Parms:   U32_t       checksum 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
U32_t
fmGetImageFileCheckSum(char *imageFileName, U32_t *checkSum)
{
  IMAGE_FT_t *l;

  SIW_FUNC_TRACE( FM, fmGetImageFileCheckSum, imageFileName, *checkSum, 0, 0, 0);

  /* verify the file name */
  if (!VALID_FILE_NAME(imageFileName)) 
      return (FM_ILLEGAL_FILE_NAME);

  for (l = (IMAGE_FT_t *)lstFirst(&imageFtList); l != NULL; l = (IMAGE_FT_t *)l->node.next ) {
    if (strcmp(imageFileName, l->fName) == 0) {
        *checkSum = l->fCkSum;
        return FM_OK;
    }
  }
  return FM_ERROR;
}


/****************************************************************************
 * Function:    fmGetImageFileIndex()
 * Descripion:  This routine is used to get the table index of the image file.
 *
 * In Parms:    char *      a Image File name 
 * Out Parms:   none 
 * Return Values:
 *              table index 
 *              FM_ERROR
 */
RET_t
fmGetImageFileIndex(char *imageFileName )
{
  IMAGE_FT_t *l;

  SIW_FUNC_TRACE( FM, fmGetImageFileIndex, imageFileName, 0, 0, 0, 0);

  /* verify the file name */
  if (!VALID_FILE_NAME(imageFileName)) 
      return (FM_ILLEGAL_FILE_NAME);

  for (l = (IMAGE_FT_t *)lstFirst(&imageFtList); l != NULL; 
                                    l = (IMAGE_FT_t *)l->node.next ) {
    if (strcmp(imageFileName, l->fName) == 0) {
        return l->idx;
    }
  }
  return FM_ERROR;
}


/****************************************************************************
 * Function:    fmRemoveImageFile()
 * Descripion:  This routine is used to remove an entry from image file table 
 *
 * In Parms:    char *      a Image File name 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR - no entry found
 */
RET_t
fmRemoveImageFile(char *imageFileName)
{
  IMAGE_FT_t *l;
  STATUS_t stat;
  SIW_FUNC_TRACE( FM, fmRemoveImageFile, imageFileName, 0, 0, 0, 0);

  /* verify the file name */
  if (!VALID_FILE_NAME(imageFileName)) 
      return (FM_ILLEGAL_FILE_NAME);

  for (l = (IMAGE_FT_t *)lstFirst(&imageFtList); l != NULL; l = (IMAGE_FT_t *)l->node.next ) {
    if (strcmp(imageFileName, l->fName) == 0) {
        /* take semaphore */
        if ((stat = SiwSemTake (imageFileTableSem, WAIT_FOREVER)) != OK) {
            /* Error from semTake() */
            FM_TRACE5(fmRemoveImageFile, 0xF0000001, imageFileName, stat, 0,0,0);
            FM_DEBUG("Could not take semaphore %x, %x", stat, errno);
            return(FM_ERROR);
        }
        lstDelete(&imageFtList, &l->node);
        SiwFreeMem((IMAGE_FT_t *)l);
        /* give the semaphore away */
        SiwSemGive(imageFileTableSem);
        return FM_OK;
    }
  }
  return FM_ERROR;
}

/****************************************************************************
 * Function:    FmDispImageFileTable()
 * Descripion:  This routine  is used to display the image file
 *              table. 
 *
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
void
FmDispImageFileTable()
{
        S32_t j; 
        IMAGE_FT_t *l;
        printf("\nFile Manager's Image File Table: \n\n");
        printf("Num     IMAGE File Name              CheckSum     Index\n");
        printf("======================================================== \n");

        for ( l = (IMAGE_FT_t *) lstFirst(&imageFtList), j=0; l != NULL; l = (IMAGE_FT_t * )l->node.next ) {
            if (strlen(l->fName) != 0) {
                j++;
                printf ("%4d  %-60s  0x%08x  %d\n", j, l->fName, l->fCkSum, l->idx);
            }
        } 
        if (!j) printf ("No entries found in Image File Table\n\n");

}


/****************************************************************************
 *
 * Function:    fmUpdateHDImageFileTable()
 * Descripion:  This routine takes a Image File Table record and copies into 
 *              the file for safekeeping purposes.
 *
 * In Parms:    S32_t      idx        - The Index of File Entry 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
RET_t
fmUpdateHDImageFileTable( char* tbName, U32_t idx, char *fName,  U32_t checksum)
{
    S32_t   fd;
    S32_t   offset;
    RET_t   ret;
    struct  IMAGE_FILE_REC_t {
            char   fName[MAX_FILE_NAME_LENGTH];
            U32_t  checksum;
    } imageFileRec;
    STATUS_t stat; 

    SIW_FUNC_TRACE(FM, fmUpdateHDImageFileTable, idx, fName, checksum,0,0);

    /* verify the file name */
    if (!VALID_FILE_NAME(fName)) 
        return (FM_ILLEGAL_FILE_NAME);

    /* varify both idx and checksum */
    if (( idx < 0 ) || ( checksum < 0 ))
        return (FM_ERROR);

    if ((stat = SiwSemTake (fmHDImageTbSem, WAIT_FOREVER)) != OK) {
       /* Error from semTake() */
       FM_TRACE5(fmUpdateHDImageFileTable, 0xF0000001, fName, checksum,stat, 0,0);
       FM_DEBUG("Could not take semaphore %x, %x", stat, errno);
       return(FM_ERROR);
    }
 
    strcpy(imageFileRec.fName, fName);
    imageFileRec.checksum = checksum;

    fd = open(tbName, O_RDWR, 0644);
    if (fd < 0) {
            /* Opss... we have a problem here */
            FM_TRACE5(fmUpdateHDImageFileTable, 0xF000002, fd, idx, 0, 0, 0);
            /*FM_LOG3(FM_FILE_OPEN_FAIL, HD_IMAGE_FILE_TABLE, fd, idx);*/
            PERR("fmUpdateHDImageFileTable: File Open Fail fd(%d) idx(%d)\n", fd, idx);
            SiwSemGive(fmHDImageTbSem);
            return (FM_CB_FILE_OPEN_FAIL);
    }

    offset = sizeof(struct IMAGE_FILE_REC_t) * idx; 
    /* 
     * now we need to get to the record we are interested in
     */
    ret = writeEntry(fd,offset,(char *)&imageFileRec, sizeof(struct IMAGE_FILE_REC_t), 
                                                          tbName);
    if (ret != FM_OK) {
        /* Oppss... could not write this record back into the file */

        FM_TRACE5(fmUpdateHDImageFileTable,0xF000001, fd, ret, idx, 
                                          sizeof(struct IMAGE_FILE_REC_t),offset);
        PERR("fmUpdateHDImageFileTable:write Fail ret(%d) len(%d) fd(%d) idx(%d)\n" ,
                    ret, sizeof(struct IMAGE_FILE_REC_t), fd, idx);
        close (fd);
        SiwSemGive(fmHDImageTbSem);
        return(FM_CB_FILE_WR_FAIL);
    }

    close (fd);
    SiwSemGive(fmHDImageTbSem);
    return (FM_OK);
}


RET_t
fmAddMiscFiles2ImageFileTb(); 

/****************************************************************************
 *
 * Function:    fmBuildNewImageFileTable()
 * Descripion:  This routine takes the Image File Table record from RAM Image
 *              File Table and rebuild a new HD Image File Table.
 *
 * In Parms:    S32_t      idx        - The Index of File Entry 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
RET_t
fmBuildNewImageFileTable()
{

    S32_t rst,  newFd, stat, offset;

    struct  IMAGE_FILE_REC_t {
            char   fName[MAX_FILE_NAME_LENGTH];
            U32_t  checksum;
    } imageFileRec; 

    IMAGE_FT_t *l;

    SiwTaskInitAck(FM_OK);

    /* open the new HD Image File Table */
    newFd = open(NEW_HD_IMAGE_FILE_TABLE, O_RDWR, 0644);
    if (newFd < 0) {
        /* Opss... we have a problem here */
        FM_TRACE5(fmBuildNewImageFileTable, 0xF000001, newFd, 0, 0, 0, 0);
        PERR("fmRebuildImageFileTable: File Open Fail fd(%d) \n", newFd);
        return (FM_CB_FILE_OPEN_FAIL);
    }

    for (l = (IMAGE_FT_t *)lstFirst(&imageFtList); l != NULL;
                                l = (IMAGE_FT_t *)l->node.next ) {

        strcpy ( imageFileRec.fName, l->fName);
        imageFileRec.checksum = l->fCkSum;

        offset = sizeof(struct IMAGE_FILE_REC_t) * l->idx;
        stat = lseek(newFd, offset, SEEK_SET);
        if (offset != stat) {
                PERR("fmBuildNewImageFileTable(): write Fail File(%s) ret(%d) fd(%d)  offset(%d)\n",
                                        imageFileRec.fName, stat,  newFd,  offset);
                close(newFd);
                return (FM_CB_FILE_WR_FAIL);
        }
        /*  now we need to get to the record we are interested in */
        rst = write(newFd, (char *)&imageFileRec, sizeof(struct IMAGE_FILE_REC_t)); 
        if (rst != sizeof (struct IMAGE_FILE_REC_t)) {
        /* Oppss... could not write this record back into the file */
                            
            PERR("fmUpdateHDImageFileTable:write Fail ret(%d) len(%d) fd(%d) newIdx(%d)\n" ,
                    rst, sizeof(struct IMAGE_FILE_REC_t), newFd, l->idx);
            close (newFd);
            return(FM_CB_FILE_WR_FAIL);
        }
    }
    close (newFd);
    /* remove the  HD_IMAGE_FILE_TABLE */
    remove(HD_IMAGE_FILE_TABLE);
    rename(NEW_HD_IMAGE_FILE_TABLE, HD_IMAGE_FILE_TABLE);
    remove(NEW_HD_IMAGE_FILE_TABLE);
    printf("fmBuildNewImageFileTable(): Done!!!\n");


    if ((stat = SiwSemTake (fmCrmSyncSem, WAIT_FOREVER)) != OK) {
       /* Error from semTake() */
       FM_TRACE5(fmRebuildImageFileTable, 0xF0000001, 0, 0,stat, 0,0);
       FM_DEBUG("Could not take semaphore %x, %x", stat, errno);
       return(FM_ERROR);
    }

    crmFmSyncFlag += 1;

    /* give the semaphore away */
    SiwSemGive(fmCrmSyncSem);

    SiwTaskDelete(SIW_TID_SELF);
    exit(0);
	return (FM_OK);
}

/****************************************************************************
 *
 * Function:    fmSpawnBuildNewTableTask()
 * Descripion:  This routine spawns a new task to build the NEW_IMAGE_FILE_TB
 *
 * In Parms:    None 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
RET_t
fmSpawnBuildNewTableTask()
{
    SIW_TID_t tid;
    STATUS_t  taskStatus;
    SIW_PRIORITY_t pri;
    SIW_TASKINFO_t info;

    SIW_FUNC_TRACE(FM, fmSpawnBuildNewTableTask, 0,0,0,0,0);

    pri.applId     = SIW_PLATFORM_SW;
    pri.prioGroup  = SIW_PRI_GRP_NORMAL;
    pri.priority   = LO_SERVER_FN_PRI;

    info.stackSize = SIW_DEFAULT_STACK;
    info.entryPt = (FUNC_PTR_t) fmBuildNewImageFileTable;
    info.arg1 = 0;
    info.arg2 = 0;
    info.arg3 = 0;
    info.arg4 = 0;
    info.waitAck = WAIT_FOREVER;
#if 0
    info.waitAck = 0;
#endif

    DINFO("fmSpawnBuildNewTableTask(): task %s is getting spawned\n", "fmBuildNewImageFileTable");

    tid = SiwTaskSpawn("fmBuildNewImageFileTable", &pri, &info,
		       SIW_RECV_LOG_ERROR,
		       SIW_RECV_LOG_ERROR,
		       NULL,
		       &taskStatus);
    
    DINFO("fmSpawnBuildNewTableTask(): task %s is SPAWNED, tid = %p\n", "fmBuildNewImageFileTable", tid);
    if (tid == SIW_INVALID_TID ) {
        /*
	DBM_LOG2(FAIL_TO_SPAWN_CLEAN_TASK, tid, taskStatus);
        */
        DINFO("spawnCleanupTask: Failed to spawntask %s tid = %p\n", 
                                              "fmBuildNewImageFileTable", tid);
	return (FM_ERROR);
    }
    if (taskStatus < 0) {
	SiwTaskDelete(tid);
	return (FM_ERROR);
    }
    return (FM_OK);

}


/****************************************************************************
 *
 * Function:    fmRebuildImageFileTable()
 * Descripion:  This routine takes the Image File Table record from HD Image
 *              File Table and rebuild the RAM Image File Table.
 *
 * In Parms:    S32_t      idx        - The Index of File Entry 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
RET_t
fmRebuildImageFileTable()
{
    S32_t   fd, offset;
    RET_t   ret;
    struct  IMAGE_FILE_REC_t {
            char   fName[MAX_FILE_NAME_LENGTH];
            U32_t  checksum;
    } imageFileRec; 

    /* open the HD Image File Table */
    fd = open(HD_IMAGE_FILE_TABLE, O_RDWR, 0644);
    if (fd < 0) {
        /* Opss... we have a problem here */
        FM_TRACE5(fmRebuildImageFileTable, 0xF000001, fd, 0, 0, 0, 0);
        PERR("fmRebuildImageFileTable: File Open Fail fd(%d) \n", fd);
        return (FM_CB_FILE_OPEN_FAIL);
    }

    /* rebuild a Ram image file table */
    FM_LOG(START_REBUILD_MSG, SiwTickGet());

    /* read a record from HD Image File Table */
    while((ret = read(fd,  (char *) &imageFileRec, sizeof(struct IMAGE_FILE_REC_t))) > 0) {
#if 0 
       printf("fmRebuildImageFileTable: read returns %d bytes,  fname %s, checksum %u\n",ret
               , imageFileRec.fName, imageFileRec.checksum);
#endif
        /* check if the buffer has the valid stuff */
        if (strncmp (imageFileRec.fName,  IMAGE_FILE_NAME_PREFIX, 
                                sizeof(IMAGE_FILE_NAME_PREFIX)-1) != 0) {
            printf("fmRebuildImageFileTable():  invalid image file name = %s\n", imageFileRec.fName);
            continue;
        }
        /* if the image file has been removed, no rebuild, skip it */ 
        if (strncmp (imageFileRec.fName, UNUSED_ENTRY , strlen(UNUSED_ENTRY)-1)== 0) {
            printf("fmRebuildImageFileTable():  fName = %s\n", imageFileRec.fName);
            continue;
        }
        /* rebuild the RAM Image File Table */
        else {
             S32_t offset;

                if (fmUpdateImageFileTable(imageFileRec.fName, 
                                        imageFileRec.checksum) == FM_ERROR) {
                    FM_TRACE5(fmRebuildImageFileTable,0xF000004, fd, ret,  
                                    sizeof(struct IMAGE_FILE_REC_t),offset, 0);
                    PERR("fmRebuildImageFileTable: Update RAM Image file Table Fail ret(%d) len(%d) fd(%d) \n", 
                            ret, sizeof(struct IMAGE_FILE_REC_t), fd);
                    continue;
                    /*
                    close (fd);
                    return (FM_ERROR);
                    */
                }
        }
    }
    if (ret < 0) {
        /* Oppss... could not read a record from the file */

        FM_TRACE5(fmRebuildImageFileTable,0xF000003, fd, ret,  
                                    sizeof(struct IMAGE_FILE_REC_t),offset, 0);
        PERR("fmRebuildImageFileTable: read Fail ret(%d) len(%d) fd(%d) \n" ,
                    ret, sizeof(struct IMAGE_FILE_REC_t), fd);
        close (fd);
        return(FM_ERROR);
    }
    if ( ret == 0) {
        /* end of file */
        S32_t rst;
        close (fd);

        /* add Misc files to image file TB */
        if ( fmAddMiscFiles2ImageFileTb() < 0) {
            PERR("fmRebuildImageFileTable: fails to add misc files to image file table \n" ); 
            return (FM_ERROR);
        }

        if ((rst =fmSpawnBuildNewTableTask()) <0) {
            PERR("dbmUntarDir: Failed to spawn Cleanup RetCode(%x)\n", rst);
        }
        else 
            printf("fmRebuildImageFileTable(): Spawnd build new HD Table task\n");

    }

    FM_LOG(FINISH_REBUILD_MSG, SiwTickGet());
    printf("fmReBuildImageFileTable(): rebuild is done !!!\n");
    return (FM_OK);
}

/****************************************************************************
 * Function:    FmRemGetImageFileChecksum()
 * Descripion:  This routine is a wrapper around FmCreate(). It takes the
 *              remote message, unpacks the message, calls FmCreate() and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              FM_ERROR
 *              Any error returned by lower level routine
 */
S32_t 
FmRemGetImageFileChecksum(IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    RET_t    ret;
    U8_t  *pHdr;
    FM_HDR_t  *pOutHdr;
    U32_t  checksum;

    SIW_FUNC_TRACE( FM, FmRemGetImageFileChecksum, inMsg, outMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemCreate, 0xF0000001,inMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, inMsg);
	FM_DEBUG("FmRemCreate(): got inMsg as NULL");
	*outMsg = NULL;
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_ADDRESS);
    }

    /* get FM header address; so that we can see what's up */
    pHdr = (U8_t *)(FM_HDR_PTR(inMsg));

    /* first of all get a response buffer */
    *outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(*outMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemGetImageFileChecksum, 0xF0000002,*outMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, *outMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    if (!VALID_FILE_NAME(pHdr)) {
        *outMsg = inMsg;
        return (-1);
    }
    pOutHdr = FM_HDR_PTR(*outMsg);

    /* Now here call the routine */
    ret = fmGetImageFileCheckSum((char *)pHdr, &checksum);
#if 0
    printf("FmRemGetImageFileChecksum(): return code from Local FmGetImageFileCheckSum is %d, checksum is %u\n", ret, checksum);
#endif /* 0 */

    PINFO("FmRemGetImageFileChecksum(): return code from Local FmGetImageFileCheckSum is %d, checksum is %u\n", ret, checksum);
    

    if (ret < 0 ) {
	/*FM_TRACE5(FmRemGetImageFileChecksum, 0xF0000003,ret,fName, checksum);
        */
	PACK_FM_ERR_MSG(pOutHdr,ret)
    } else {
	char * pOutData = FM_DATA_PTR(*outMsg);
	/* Here pack the response and return */
	bcopy((char *)&checksum, pOutData, sizeof(U32_t));
	PACK_FM_RESP_MSG(pOutHdr);
    }
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(*outMsg, sizeof(FM_HDR_t) + sizeof(U32_t) );

    /* Since we are the last one to use the input buffer, we must
     * free it up
     */
    SiwFreeMem(inMsg);
    return (ret);
}

/****************************************************************************
 * Function:    FmGetRemImageFileCheckSum()
 * Descripion:  This routine gets the checksum of Remote image file on
 *              a target card.
 * In Parms:    S32_t        target; 
 *              char         *fName; 
 * Out Parms:   U32_t        *checksum; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
FmGetRemImageFileCheckSum( IN S32_t target, char *fName, U32_t *checksum)
{
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    U8_t *pData;
    U32_t chksum;
    RET_t ret;

    SIW_FUNC_TRACE(FM, FmGetRemImageFileCheckSum, target, fName, checksum, 0,0);

    if (!VALID_SLOT(target)) 
	return (FM_ILLEGAL_SLOT);

    if (!VALID_FILE_NAME(fName)) 
	return (FM_ILLEGAL_FILE_NAME);

    /*  
     * Before we do that we need to allocate a message
     * buffer, fill up the FM Header message for the remote
     * File Manager task so that it knows what to do with it
     */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("FmGetImageFileCheckSum(): ########  WARNING!!!!! ######\n");
	return(FM_ILLEGAL_ADDRESS);
    }
    pData = (U8_t *)(FM_HDR_PTR(outMsg));
    
    bcopy (fName, pData, MAX_FILE_NAME_LENGTH);
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + MAX_FILE_NAME_LENGTH );

    ret = MrpcSync(target, FM_MOD, FM_GET_IMAGE_FILE_CHECKSUM_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
    FM_DEBUG("FmGetRemImageFileCheckSum(): First Time Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
       /* printf("FmGetRemImageFileCheckSum():failed MrpcSync \n");*/
        /* Failed to open second time; so give up */
        if (replyMsg != NULL)
            SiwFreeMem(replyMsg);
        return(ret);
    }
    /* Just make sure that the file got opened OK, if so, get
     * the dest file descriptor poiner, as it is needed for
     * writing to the destination
     */
    if (((FM_HDR_t *)&(replyMsg->data[0]))->err < 0) {
	/* Opsss, we got an error opening the remote file */
	/* Need to take recovery action */
	SiwFreeMem(replyMsg);
        printf("FmGetRemImageFileCheckSum():no file exist on remote side \n");
	return(FM_COULD_NOT_OPEN_DST_FILE);
    } 

    bcopy( (char *)&replyMsg->data[sizeof(FM_HDR_t)], (char *)&chksum, sizeof(U32_t));
    SiwFreeMem(replyMsg);
/*    printf("FmGetRemImageFileCheckSum(): remote checksum = %d\n", chksum);*/
    *checksum = chksum;
    return(FM_OK);
}


/****************************************************************************

 * Function:    fmUpdateCb()
 * Descripion:  This routine takes a FM record and copies into the file for 
 *              safekeeping purposes.
 *
 * In Parms:    S32_t      idx        - The Index of File Entry 
 * Out Parms:   None 
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
RET_t
fmUpdateCb(S32_t idx)
{
#ifdef FM_CB_FILE
    S32_t fd;
    S32_t offset;
    RET_t ret;

    SIW_FUNC_TRACE(FM, saveDbRecToFile, idx, 0,0,0,0);

    fd = open(FM_FILE_NAME, O_RDWR, 0644);
    if (fd < 0) {
        /* Opss... we have a problem here */
        FM_TRACE5(fmUpdateCb,0xF000002, fd, idx, 0, 0, 0);
        FM_LOG3(FM_FILE_OPEN_FAIL, FM_FILE_NAME, fd, idx);
        PERR("fmUpdateCb: File Open Fail fd(%d) idx(%d)\n", fd, idx);
        return (FM_CB_FILE_OPEN_FAIL);
    }
    offset = sizeof(FT_t) * idx; 
    /* 
     * now we need to get to the record we are interested in
     */
    ret = writeRec(fd,offset,(char *)&fileTable[idx],sizeof(FT_t),FM_FILE_NAME);
    if (ret != FM_OK) {
        /* Oppss... could not write this record back into the file */

        FM_TRACE5(saveDbRecTofile,0xF000001, fd, ret, idx, 
                                                sizeof(FT_t),offset);
        FM_LOG4(FM_FILE_WRITE_FAIL, FM_FILE_NAME, sizeof(FT_t),idx,offset);
        PERR("fmUpdateCb:write Fail ret(%d) len(%d) fd(%d)idx(%d)\n" ,
                    ret, sizeof(FT_t), fd, idx);
        close (fd);
        return(FM_CB_FILE_WR_FAIL);
    }
    close (fd);
#endif /* FM_CB_FILE */
    return (FM_OK);
}





/****************************************************************************
 * Function:    FmIndex2Ptr()
 * Descripion:  This routine takes the index tro the FileTable and returns
 *              the corresponding Address of the RAM file.
 *              NOTE: This should be used ONLY for RAM Files.
 *              NOTE2: The input parameter for this routine is the index which
 *                     is returned by the FmCreate() routine. Therefore, the
 *                     use of this routine should be as follows:
 *                     index = FmCreate(.......) - only for RAM files
 *                     ptr = FmIndex2Ptr(index)  - only for RAM files.
 *
 * In Parms:    S32_t index        - File Table Index
 * Out Parms:   None 
 * Return Values:
 *              pointer to file     - If all works out OK
 *              NULL                - if Error
 */
char *
FmIndex2Ptr(S32_t index)
{
    if (!VALID_INDEX(index)) 
	return (NULL);

    return ((char *)fileTable[index].fdp->fStart);
}


 



/****************************************************************************
 * Function:    getNextFD()
 * Descripion:  This routine is used to create a new file in any of the
 *              media such as RAM, Hard Drive, Non-Volatile RAM, and Flash.
 *              All of the data in AXSS-1600 is stored in the form a file 
 *              which can be written to or read from using the routines  
 *              provided in this interface file. A file must be created before
 *              it can be opend and read/written to/from. When a file is 
 *              created, it must specifyt he type of the file.
 * In Parms:    char *fileName     - Name of the file to be created.  
 *              U32_t        flag  - What is this file being opened for.
 *                                   READ_ONLY, WRITE_ONLY, READ_WRITE
 *              FILE_TYPE_t fileType - type of file, i.e.
 *                                   RAM_FILE, HD_FILE, NVRAM_FILE, FLASH_FILE
 *              S32_t fileSize     - Max. Anticipated size of this file.
 * Out Parms:   None 
 * Return Values:
 *              fileDescriptor     - If all works out OK
 *              FM_ERROR
 *              FM_OUT_OF_FDS
 *              FM_OUT_OF_ROOM
 */
S32_t 
getNextFD()
{

    S32_t fd;

    for (fd=0; fd < MAX_FILES; fd++) {
	if (strlen(fileTable[fd].fName) == 0) {
	    break;
	}
    }
    if (fd == MAX_FILES)
	return (-1);

    return (fd);
}



/****************************************************************************
 * Function:    FmCreate()
 * Descripion:  This routine is used to create a new file in any of the
 *              media such as RAM, Hard Drive, Non-Volatile RAM, and Flash.
 *              All of the data in AXSS-1600 is stored in the form a file 
 *              which can be written to or read from using the routines  
 *              provided in this interface file. A file must be created before
 *              it can be opend and read/written to/from. When a file is 
 *              created, it must specifyt he type of the file.
 * In Parms:    char *fileName     - Name of the file to be created.  
 *              U32_t        flag  - What is this file being opened for.
 *                                   READ_ONLY, WRITE_ONLY, READ_WRITE
 *              FILE_TYPE_t fileType - type of file, i.e.
 *                                   RAM_FILE, HD_FILE, NVRAM_FILE, FLASH_FILE
 *              S32_t       fileSize     - Max. Anticipated size of this file.
 *              BOOL_t      replFlag     - TRUE(1) replicate on Standby
 * Out Parms:   None 
 * Return Values:
 *              fileDescriptorIndex     - If all works out OK
 *              FM_ERROR
 *              FM_OUT_OF_FDS
 *              FM_OUT_OF_ROOM
 */
S32_t
FmCreate(IN char *fileName, 
	    U32_t flag, 
	    FILE_TYPE_t fileType, 
#ifndef FM_CB_FILE
	    S32_t fileSize)
#else /* FM_CB_FILE */
	    S32_t fileSize,
	    BOOL_t repl)
#endif /* FM_CB_FILE */
{
    S32_t fdIndex;
    S32_t i;
    FD_t  *fdp;

    SIW_FUNC_TRACE( FM, FmCreate, fileName, flag, fileType, fileSize, 0);

    /* first validate all the input parameters */

    if (!VALID_FILE_TYPE(fileType)) 
	return (FM_ILLEGAL_FILE_TYPE);

    if (!VALID_FILE_SIZE(fileSize)) 
	return (FM_ILLEGAL_FILE_SIZE);

    if (!VALID_FILE_FLAGS(flag & ~CREATE)) 
	return (FM_ILLEGAL_FILE_FLAGS);

    if (!VALID_FILE_NAME(fileName)) 
	return (FM_ILLEGAL_FILE_NAME);

    /* See of this file is aready created */
    for (i=0; i < MAX_FILES; i++) {
	if (strcmp(fileName, fileTable[i].fName) == 0)
#if 0
	    return (FM_FILE_ALREADY_CREATED);
#else
	    return (i);
#endif
    }

    /* Now that all sanity checks are passed, see if we have a room 
     * for next descriptor
     */
    if ((fdIndex = getNextFD()) < 0) 
	return (FM_FILE_SYSTEM_FULL);

    /* Get a file descriptor from free pool */
    if ((fdp = (FD_t *)lstGet (&freeFdList)) == NULL) {
	FM_TRACE5(FmCreate, 0xF0000001,fdp, 0,0,0,0);
	FM_LOG(OUT_OF_FDPS, fdp);
	return (FM_OUT_OF_FDS);
    }

    /* Stamp the FD with a unique signatures so that we can validate it
     * later to ensure that we are working with a valid FD; Any FD which 
     * does not check out against its signatures, will be rejected
     */
    STAMP_FD(fdp);

    fileTable[fdIndex].fdp = fdp;
#ifdef FM_CB_FILE
    fileTable[fdIndex].repl = ((repl == 1) ? 1: 0);
#endif /* FM_CB_FILE */
    strcpy(fileTable[fdIndex].fName, fileName);

    /* Fill in the File Descriptor */
    fdp->fStatus = 0;  /* close */
    fdp->fMode   = flag;
    fdp->fSemId  = SiwSemCreate("plfm:fm",SIW_SEM_BINARY, SIW_SEM_Q_PRIORITY, 
					      SIW_SEM_FULL,SIW_RECV_NO_ACTION);
    fdp->fLen    = fileSize;
    fdp->vxWFd   = NULL;
    fdp->fType   = fileType;
    lstInit(&fdp->tlist);
    fdp->node.next = 0;
    fdp->node.previous = 0;
    strcpy ( fdp->fName, fileName);

    switch (fileType) {
    case RAM:
	if ((strcmp(fileName, FW_IMAGE_FILE_NAME)) == 0) {
	    /* Creating the image file, so do not do the malloc, 
	     * instead, use a fixed address for it.
	     */
	    fdp->fStart = GET_FW_IMAGE_START_ADDR();
	    fdp->fLen = GET_FW_IMAGE_LEN();
	} else {
	    /* if not FW image file, go ahead and allocate  the file size */
	    fdp->fStart =(U32_t)SiwAllocMem(SIW_STATIC, fileSize, WAIT_FOREVER); 
	}
	if (fdp->fStart == NULL)
	    return (FM_ERROR);
	/* Fill in the rest of the File Descriptor */
	break;

    case NVRAM:
	return (FM_NO_SUPPORT_FOR_NVRAM);
	break;
    case HD:
	fdp->fStart =  0;
	fdp->fStatus = 1;  /* open */
	fdp->fLen    = 0;
#if 0
        /* update the Image file table when a image file is created */
        if (strncmp (fileName,  IMAGE_FILE_NAME_PREFIX, sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0){
             ret = fmUpdateImageFileTable(fileName, 0 );
             if (ret < 0 ) return (FM_ERROR);
        }
#endif /* 0 */
	break;
    case FLASH:
	/* Allocate Flash Memory */
	fdp->fStart = (U32_t)allocFlashMem();
	if (fdp->fStart == NULL)
	    return (FM_ERROR);
	break;
    }
    fdp->fNextWr = fdp->fStart;
#ifdef FM_CB_FILE
    fmUpdateCb(fdIndex);
#endif /* FM_CB_FILE */
    return (fdIndex);
}

/****************************************************************************
 * Function:    FmDelete()
 * Descripion:  This routine is used to delete a file in any of the
 *              media such as RAM, Hard Drive, Non-Volatile RAM, and Flash.
 * In Parms:    char *fileName     - Name of the file to be deleted.  
 * Out Parms:   None 
 * Return Values:
 *              FM_OK               - If all works out OK
 *              FM_ERROR
 *              FM_FILE_DOES_NOT_EXIST
 */
RET_t 
FmDelete(IN char *fileName)
{
    S32_t i;
    FD_t  *fdp;
    RET_t ret = FM_OK;
    S32_t c;
    FD_TID_t *n;
    BOOL_t foundFile = FALSE;
    BOOL_t isImageFile = FALSE;
#ifdef SCC
	U8_t *ft;
	U32_t idx;
#endif
#ifdef NSC
	S32_t stat;
#endif

    /* first validate all the input parameters */

    SIW_FUNC_TRACE( FM, FmDelete, fileName, 0, 0, 0, 0);

    if (!VALID_FILE_NAME(fileName)) 
	return (FM_ILLEGAL_FILE_NAME);

#ifdef SCC
    ft = strrchr(fileName, '.');
	
    /* update the Image file table when an image file is deleted */
    if (strncmp (fileName,  IMAGE_FILE_NAME_PREFIX, 
		 sizeof(IMAGE_FILE_NAME_PREFIX)-1) == 0) {
      PINFO("   ==> This is an image file! <==\n");

      isImageFile = TRUE;

      idx = fmGetImageFileIndex(fileName);
      ret =  fmRemoveImageFile(fileName);
      /* return error if the file is not exist */
      if ( ret < 0 ) return (FM_ERROR);

      /* replace the filename using "XXXXX" in HD image file table 
       * after the entry is deleted in RAM table. 
       */
      fmUpdateHDImageFileTable(HD_IMAGE_FILE_TABLE, idx, UNUSED_ENTRY, 0);

      /* remove from vxWorks */
      ret = remove (fileName);
      
    } /* if image file */
#endif  /* SCC */

    PINFO("    Checking to see if %s is in fileTable\n", fileName);
    /* See of this file is in the fileTable */
    for (i=0; i < MAX_FILES; i++) {
      if (strcmp(fileName, fileTable[i].fName) == 0) {
	/* Got the file */
	PINFO("    Found file!\n");
	foundFile = TRUE;

	fdp = fileTable[i].fdp;
	switch (fdp->fType) {
	case HD:
	  PINFO("    HD file\n");
	  
	  /* For Hard Drive pass it down straight to vxWorks */
	  if (!isImageFile)
	    ret = remove (fileName);

	  /* Here see if we have multiple FDs open for this file,
	   * if so, they all need to be deleted except for the first
	   * one, as it gets deleted later on in this routine
	   */
	  if (fdp->node.next) {
	    FD_t  *n, *p; 
	    S32_t cnt = 0;
	    
	    n = fdp;
	    while (n->node.next){
	      n = (FD_t *)n->node.next;
	      cnt++;
	    }
	    p = (FD_t *)n->node.previous;
	    while (cnt > 0) {
	      n = (FD_t *)p->node.next;
	      p->node.next =0;
	      /* Zero out the FD */
	      bzero((char *)n, sizeof(FD_t));
		  
	      /* return the FD back to free list */
	      lstAdd (&freeFdList, &n->node);
	      p = (FD_t *)p->node.previous;
	      cnt--;
	    }
	  }
	  break;
	case RAM:
	  PINFO("    RAM file\n");
	      
	  /* Free up the RAM Memory */
	  SiwFreeMem((char *)fdp->fStart);
	  break;
	case FLASH:
	  PINFO("    FLASH file\n");
	  /* Free up the FLASH Memory */
	  freeFlashMem(fdp->fStart);
	  break;
	case NVRAM:
	  FM_DEBUG("fmDelete(): NVRAM type of file found in FD");
	  break;
	default:
	  FM_TRACE5(FmDelete, 0xF0000001,fdp, i,0,0,0);
	  FM_LOG(ILLEGAL_FILE, fdp);
	  FM_DEBUG("fmDelete(): Illegal type of file found in FD");
	} /* switch */
	
	/* We need to get rid of the FD and return it back to 
	 * Free Pool
	 */
	/* walk thru the tlist and free up all elements
	 * this is necessary to reclaim all of the memory
	 */
	c = lstCount (&fdp->tlist);
	if (c > 0) {
	  n = (FD_TID_t *)lstFirst(&fdp->tlist);
	  i=0;
	  do {
	    lstDelete (&fdp->tlist, (NODE *)n);
	    SiwFreeMem(n);
	    n = (FD_TID_t *)lstNext((NODE *)n);
	    i++;
	  } while (i < c);
	  /* Free up the taskId list and free up the memory */
	  lstFree( &fdp->tlist);
	}
	
	/* This semaphore is not needed anymore, so get rid of it */
	SiwSemDelete (fdp->fSemId);
	
	/* Zero out the FD */
	bzero((char *)fdp, sizeof(FD_t));
	
	/* return the FD back to free list */
	lstAdd (&freeFdList, &fdp->node);
	
	/* Now remove the file from the file Table */
	strcpy(fileTable[i].fName, "");
	fileTable[i].fdp = NULL;
#ifdef FM_CB_FILE
	fmUpdateCb(i);
#endif /* FM_CB_FILE */
	
	break;   /* break from for loop */
      } /* if file found */
    } /* for */
    
    if ((!foundFile) && (!isImageFile)){
      /* We must not get here */
      PERR("FmDelete(): File does not exist\n");
      return (FM_FILE_DOES_NOT_EXIST);
    }
    return(FM_OK);
}

#if 0
/****************************************************************************
 * Function:    FmDelete()
 * Descripion:  This routine is used to delete a file in any of the
 *              media such as RAM, Hard Drive, Non-Volatile RAM, and Flash.
 * In Parms:    char *fileName     - Name of the file to be deleted.  
 * Out Parms:   None 
 * Return Values:
 *              FM_OK               - If all works out OK
 *              FM_ERROR
 *              FM_FILE_DOES_NOT_EXIST
 */
RET_t 
FmDelete(IN char *fileName)
{
    S32_t i;
    FD_t  *fdp;
    U32_t idx;
    RET_t ret = FM_OK;
    U8_t *ft;
    S32_t stat;

    /* first validate all the input parameters */

    SIW_FUNC_TRACE( FM, FmDelete, fileName, 0, 0, 0, 0);

    if (!VALID_FILE_NAME(fileName)) 
	return (FM_ILLEGAL_FILE_NAME);

    /* See of this file is in the fileTable */
    for (i=0; i < MAX_FILES; i++) {
	if (strcmp(fileName, fileTable[i].fName) == 0) {
	    /* Got the file */
	    fdp = fileTable[i].fdp;
	    switch (fdp->fType) {
	    case HD:
		/* For Hard Drive pass it down straight to vxWorks */
		ret = remove (fileName);
		/* Here see if we have multiple FDs open for this file,
		 * if so, they all need to be deleted except for the first
		 * one, as it gets deleted later on in this routine
		 */
		if (fdp->node.next) {
		    FD_t  *n, *p; 
		    S32_t cnt = 0;

		    n = fdp;
		    while (n->node.next){
			n = (FD_t *)n->node.next;
			cnt++;
		    }
		    p = (FD_t *)n->node.previous;
		    while (cnt > 0) {
			n = (FD_t *)p->node.next;
			p->node.next =0;
			/* Zero out the FD */
			bzero((char *)n, sizeof(FD_t));

			/* return the FD back to free list */
			lstAdd (&freeFdList, &n->node);
			p = (FD_t *)p->node.previous;
			cnt--;
		    }
		}
                /* update the Image file table when an image file is deleted */
                if (strncmp (fileName,  IMAGE_FILE_NAME_PREFIX, 
                                                sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0){

                     idx = fmGetImageFileIndex(fileName);
                     ret =  fmRemoveImageFile(fileName);
                     /* return error if the file is not exist */
                     if ( ret < 0 ) return (FM_ERROR);

                     /* replace the filename using "XXXXX" in HD image file table 
                      * after the entry is deleted in RAM table. 
                      */
                     fmUpdateHDImageFileTable(HD_IMAGE_FILE_TABLE, idx, UNUSED_ENTRY, 0);
                }

		break;
	    case RAM:
		/* Free up the RAM Memory */
		SiwFreeMem((char *)fdp->fStart);
		break;
	    case FLASH:
		/* Free up the FLASH Memory */
		freeFlashMem(fdp->fStart);
		break;
	    case NVRAM:
		FM_DEBUG("fmDelete(): NVRAM type of file found in FD");
		break;
	    default:
		FM_TRACE5(FmDelete, 0xF0000001,fdp, i,0,0,0);
		FM_LOG(ILLEGAL_FILE, fdp);
		FM_DEBUG("fmDelete(): Illegal type of file found in FD");
	    }
	    /* We need to get rid of the FD and return it back to 
	     * Free Pool
	     */
#if 0
	    /* Free up the taskId list and free up the memory */
	    lstFree( &fdp->tlist);
#else
	    {
		S32_t c;
		FD_TID_t *n;
		/* walk thru the tlist and free up all elements
		 * this is necessary to reclaim all of the memory
		 */
		c = lstCount (&fdp->tlist);
		if (c > 0) {
		    n = (FD_TID_t *)lstFirst(&fdp->tlist);
		    i=0;
		    do {
			lstDelete (&fdp->tlist, (NODE *)n);
			SiwFreeMem(n);
			n = (FD_TID_t *)lstNext((NODE *)n);
			i++;
		    } while (i < c);
		    /* Free up the taskId list and free up the memory */
		    lstFree( &fdp->tlist);
		}
	    }
#endif
	    /* This semaphore is not needed anymore, so get rid of it */
	    SiwSemDelete (fdp->fSemId);

	    /* Zero out the FD */
	    bzero((char *)fdp, sizeof(FD_t));

	    /* return the FD back to free list */
	    lstAdd (&freeFdList, &fdp->node);

	    /* Now remove the file from the file Table */
	    strcpy(fileTable[i].fName, "");
	    fileTable[i].fdp = NULL;
#ifdef FM_CB_FILE
            fmUpdateCb(i);
#endif /* FM_CB_FILE */
	    return (ret);
	}
    }

    /* We must not get here */
    return (FM_FILE_DOES_NOT_EXIST);
}
#endif

/****************************************************************************
 * Function:    FmOpen()
 * Descripion:  This routine is used to open a file which has already been
 *              created by using FmCreate(). If not done so, this routine
 *              will return an Error. Every file must be opened before it can
 *              read from or written to and every file must be Created before
 *              it can be opened.
 * In Parms:    char *fileName     - Name of the file to be opened.  
 *              S32_t flag         - What is this file being opened for.
 *                                   READ_ONLY, WRITE_ONLY, READ_WRITE
 * Out Parms:   None
 * Return Values:
 *              fileDescriptor     - If file opened successfully
 *              FM_ERROR
 *              FM_FILE_DOES_NOT_EXIST
 */
FD_t * 
FmOpen(IN char *fileName, U32_t flag)
{

    S32_t i;
    FD_t  *fdp;
    STATUS_t stat;
    FD_TID_t *tnode;
    S32_t vxWFd;
    BOOL_t localCreate= FALSE;
    RET_t ret;
    
    SIW_FUNC_TRACE( FM, FmOpen, fileName, flag, 0, 0, 0);
    PINFO("FmOpen(): fileName(%s) flag(%x)\n", fileName, flag);

    if (!VALID_FILE_NAME(fileName)) 
	return (NULL);

    if (!VALID_FILE_FLAGS(flag & ~ CREATE)) 
	return (NULL);

    /* See of this file is in the fileTable */
    for (i=0; i < MAX_FILES; i++) {
	if (strcmp(fileName, fileTable[i].fName) == 0) {
	    localCreate = TRUE;
	    break;
	}
    }
    PINFO("FmOpen():file(%s) fnd @%d in FT outof %d\n", fileName, i, MAX_FILES);
    /* File not found */
    if (i == MAX_FILES) {
	/* We know this is not our file; see if this is for HD.
	 * To do this, pass the request on to the VxWroks HD file
	 * system and see if it can recognize this file
	 */
	vxWFd = open(fileName, flag, 0644);
	if (vxWFd >= 0) { /* Got a valid file in HD */
	    /* So go ahead and create it */
#ifdef FM_CB_FILE
	    S32_t idx = FmCreate(fileName, flag, HD, 1000, 0);
#else /* FM_CB_FILE */
	    S32_t idx = FmCreate(fileName, flag, HD, 1000);
#endif /* FM_CB_FILE */
	    if (idx >= 0) {
		/* Got file created in HD as well as in our DB */
		fdp = fileTable[idx].fdp;
		fdp->vxWFd = vxWFd;
		fdp->fMode = flag & ~CREATE;
                strcpy ( fdp->fName, fileName);

                /* reset the checksum in the image file table */
                if ((strncmp (fileName,  IMAGE_FILE_NAME_PREFIX,
                       sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0) && (flag != READ_ONLY)){
                        ret = fmUpdateImageFileTable(fileName, 0 );
                        if (ret < 0 ) return (NULL);
                }
		return (fdp);
	    } else {
		return (NULL);
	    }
	} else {
            PERR("FmOpen(): open1 failed for file %s; ret %d\n",fileName,vxWFd);
	    return (NULL);
	}
    } else {
	/* file is alreay created, so get the File Descriptor */
	fdp = fileTable[i].fdp;
    }
    PINFO("FmOpen():file(%s) fnd @%d in FT with FD %p localCrt = %d\n", 
                   fileName, i, fdp, localCreate);

    /* Here if the file is already created and all checks out fine */

    /* If it is a HD file, then pass it straight to vxWorks */
    if (fdp->fType == HD) {
        /* See if the file alreay exists, if so then open it */
	vxWFd = open(fileName, flag, 0644);
        if (vxWFd < 0) {
            PINFO("FmOpen(): Open Fail for HD file %s; flags(%x); ret(%d)\n", 
		   fileName, flag, vxWFd);
            /* Oppsss, we could not open the file. This means the file has not
             * been created in the HD yet. It only has beed created in the
             * fileTable, so create it
             */
            /*
            if (localCreate == TRUE)
                */
            flag |= CREATE;
            vxWFd = open(fileName, flag, 0644);
            if (vxWFd < 0) { /* Did not get  a valid file in HD */
#if 0 /* SSSSSS */
                taskSuspend(taskIdSelf());
#endif /* SSSSSS */

                FM_DEBUG("FmOpen():Can't Create file %s; flags(%x);retcode(%d)\n",
                    fileName, flag, vxWFd);
                PINFO("FmOpen():Can't Create file %s; flags(%x);retcode(%d)\n",
                    fileName, flag, vxWFd);
                FM_TRACE5(FmOpen, 0xF0000021,vxWFd, flag,localCreate,fdp,0);
                FM_LOG3(FAIL_OPEN_HD_FILE, fileName,flag,vxWFd);
                return (NULL);
            } else { 
                PINFO("FmOpen(): created a file %s; ret %d\n",fileName,vxWFd);

                /* for image file, add a new entry in the Image File Table 
                 * when a file is created 
                 */
                if ((strncmp (fileName,  IMAGE_FILE_NAME_PREFIX,
                     sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0) && (flag != READ_ONLY)){
                    ret = fmUpdateImageFileTable (fileName, 0);
                    if (ret < 0) {
                       PERR("FmOpen(): %s file is not exist in the Image File Table. \n", fileName);
		       FM_TRACE5(FmOpen, 0xF0000012,fileName, 0,0,0,0);
                       return (NULL);
                    }
                }
           } 
        }
        PINFO("FmOpen():open HD file %s; flags ox%x; ret(%d); fdp->vxwd ox%x\n",
		   fileName, flag, vxWFd, (U32_t)fdp->vxWFd);
	/* 
	 * Here see if the file is already opend. If so, then we need to 
	 * create a new FD for it as it HD file. If not done so, the
	 * fdp->vxWFd will be overwritten and the whole thing will go to 
	 * hell. This is very critical
	 */
	if (fdp->vxWFd == 0) {
	    /* No it was not open before, so we are done with this case */
	    fdp->fStatus = 1;  /* open */
	    fdp->vxWFd = vxWFd;
	    fdp->fMode = flag & ~CREATE;
            strcpy ( fdp->fName, fileName);

            if ((strncmp (fileName,  IMAGE_FILE_NAME_PREFIX,
                         sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0) && (flag != READ_ONLY)){
               /* for image file, reset the checksum in the Image File Table. */
               ret = fmUpdateImageFileTable (fileName, 0);
               if (ret < 0) {
                   PERR("FmOpen(): %s file is not exist in the Image File Table. \n", fileName);
		   FM_TRACE5(FmOpen, 0xF0000012,fileName, 0,0,0,0);
                   return (NULL);
               }
            } 
	    return(fdp);
	}else {
	    FD_t  *newFdp, *n;
	    /* 
	     * This is the case where the file is being opened twice
	     * so we need to take care of it
	     */
	    /* Get a file descriptor from free pool */
	    if ((newFdp = (FD_t *)lstGet (&freeFdList)) == NULL) {
                PERR("FmOpen(): Out of FDs (%p); \n", newFdp);
		FM_TRACE5(FmOpen, 0xF0000011,newFdp, 0,0,0,0);
		FM_LOG(OUT_OF_FDPS, newFdp);
		return (NULL);
	    }

	    /* Stamp the FD with a unique signatures so that we can validate it
	     * later to ensure that we are working with a valid FD;Any FD which 
	     * does not check out against its signatures, will be rejected
	     */
	    STAMP_FD(newFdp);

	    /* Fill in the File Descriptor */
	    newFdp->fStatus = 1;  /* open */
	    newFdp->fMode   = flag;
	    newFdp->vxWFd   = NULL;
	    newFdp->fType   = HD;
	    lstInit(&newFdp->tlist);
	    newFdp->fStart =  0;
	    newFdp->fLen    = 0;
	    newFdp->vxWFd = vxWFd;
	    newFdp->node.next = 0;
            strcpy ( newFdp->fName, fileName);

            if ((strncmp (fileName,  IMAGE_FILE_NAME_PREFIX,
                     sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0) && (flag!=READ_ONLY)){
                /* for image file, reset the checksum in the Image File Table. */
                ret = fmUpdateImageFileTable (fileName, 0);
                if (ret < 0) {
                    PERR("FmOpen(): %s file is not exist in the Image File Table. \n", fileName);
                    FM_TRACE5(FmOpen, 0xF0000012,fileName, 0,0,0,0);
                    return (NULL);
                }
            } 

	    /* Now that this new FDP is ready, add it into the list of
	     * existing FDPs
	     */
	    n=fdp;
	    while (n->node.next){
		n = (FD_t *)n->node.next;
	    }
	    n->node.next = (void *)newFdp;
	    newFdp->node.previous = (void *)n;
	    return (newFdp);
	}
    }
    fdp->fMode |= (flag & ~CREATE);

    /* If the file is being opened for write only or read-write, then 
     * we must take the semaphore so that two applications do not 
     * end up in write mode at the same time
     */
    if (((fdp->fMode & WRITE_ONLY) || (fdp->fMode & READ_WRITE)) && 
        ((flag & WRITE_ONLY) || (flag & READ_WRITE))) {
	if ((stat = SiwSemTake (fdp->fSemId, WAIT_FOREVER)) != OK) {
	    /* Error from semTake() */
	    FM_TRACE5(FmOpen, 0xF0000001,fdp->fMode, flag,fdp->fSemId,0,0);
	    FM_LOG2(SEM_ERROR, fdp->fSemId, fdp);
	    FM_DEBUG("Could not take semaphore %x, %x", stat, errno);
	    return(NULL);
	}
	fdp->fNextWr = fdp->fStart;
	fdp->semStatus = TRUE;
    } else {
	if ((flag & WRITE_ONLY) || (flag & READ_WRITE)) 
	    fdp->fNextWr = fdp->fStart;
    }

    tnode = SiwAllocMem(SIW_STATIC, sizeof(FD_TID_t), WAIT_FOREVER);
    tnode->tid = SiwTaskIdSelf();
    tnode->mode = flag;

    /* Add this node to the list */
    lstAdd(&fdp->tlist, &tnode->node);

    FM_DEBUG("FmOpen(): Task %d is trying to open FD %x\n", 
					       (S32_t)tnode->tid, (U32_t)fdp);

    fdp->fStatus++;
    return (fdp);
}


/****************************************************************************
 * Function:    FmClose()
 * Descripion:  This routine is used to close a file which has already been
 *              opened by using FmOpen(). This routine requires the file
 *              descriptor as an input paramter (the one which was assigned by
 *              the FmOpen() routine.
 * In Parms:    FD_t *fdp           - A pointer to the File Descriptor
 * Out Parms:   None
 * Return Values:
 *              FM_OK               - If all works out OK
 *              FM_ERROR
 *              FM_FILE_NOT_CREATED
 */
RET_t 
FmClose(IN FD_t *fdp) 
{
    SIW_TID_t tid;
    S32_t i;
    S32_t c;
    FD_TID_t *n;
    U32_t finalCheckSum;

    SIW_FUNC_TRACE( FM, FmClose, fdp, 0, 0, 0, 0);
    if (!VALID_FDP(fdp)) 
	return (FM_ILLEGAL_FILE_DESCR);

    if (fdp->fType == HD) {
	/* This is for HD, so send it straight to vxWorks */
	S32_t ret = close(fdp->vxWFd);
	/* If we have a list of FDs, then we need to return the FD back 
	 * to the free list. However, on the other hand, if it the only FD
	 * associated with this file, then simply free it up and mark as 
	 * closed and no need to return until the file is deleted. 
	 * NOTE: the last FD is returned by FmDelete, all others are 
	 *       freed and returned here
	 */
	if (!fdp->node.previous) {
	    /* This is the only FD associated with this file or this is
	     * the first one in the list, so  do not
	     * free up, simply mark it closed and null the vxFdp
	     */
	    fdp->vxWFd = 0;
	    fdp->fStatus = 0; /* close */
	} else {
	    FD_t *n, *p;
	    if ((fdp->node.next) && (fdp->node.previous))  {
		/* This the case when this FD is in the middle of a list */
		p = (FD_t *)fdp->node.previous;
		n = (FD_t *)fdp->node.next;
		p->node.next = (void *)n;
		n->node.previous = (void *)p;
	    } else if ((!fdp->node.next) && (fdp->node.previous))  {
		/* This is the last element in the list */
		p = (FD_t *)fdp->node.previous;
		p->node.next = 0;
	    }
	    /* Zero out the FD */
	    bzero((char *)fdp, sizeof(FD_t));

	    /* return the FD back to free list */
	    lstAdd (&freeFdList, &fdp->node);
	}

        if ((strncmp (fdp->fName,  IMAGE_FILE_NAME_PREFIX,
                     sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0) &&
             (fdp->fMode!=READ_ONLY)) {
            RET_t rst;

            /* get the final checksum from the Image File Table */
            rst = fmGetImageFileCheckSum(fdp->fName, &finalCheckSum);

            if (rst < 0) {
                PERR("FmClose(): image file (%s) is not exist in Image File Table.\n", fdp->fName);
                return (rst);
            }
            else { /* update the checksum in the HD image file table */
                fmUpdateHDImageFileTable(HD_IMAGE_FILE_TABLE, fmGetImageFileIndex(fdp->fName),
                                             fdp->fName, finalCheckSum);
            }
        }

	return (ret);
    }

    tid = SiwTaskIdSelf();

    FM_DEBUG("FmClose():Task %d is trying to close FD %x\n",(S32_t)tid,(U32_t)fdp);

    /* See if this task has the semaphore */
    c = lstCount (&fdp->tlist);
    if (c > 0) {
	n = (FD_TID_t *)lstFirst(&fdp->tlist);
	i=0;
	do {
#if 0
	    if (n->tid == tid) {
#endif
		if ((n->mode & WRITE_ONLY) || (n->mode & READ_WRITE)) {
		    fdp->fMode &= n->mode;
		    n->mode = 0;
		    n->tid = 0;
		    lstDelete (&fdp->tlist, (NODE *)n);
		    SiwFreeMem(n);
		    SiwSemGive(fdp->fSemId);
		    fdp->semStatus = 0;
#if 1
		    /* Do not reset it as it messeses up FmRead
		    fdp->fNextWr = fdp->fStart;
		    */
#else
		    fdp->fNextWr = fdp->fStart;
#endif
		    break;
		}
#if 0
	    }
#endif
	    n = (FD_TID_t *)lstNext((NODE *)n);
	    i++;
	} while (i < c);

	if (i == c) {
	    /* could not find the entry; so see if we have a READ_ONLY entry*/
	    n = (FD_TID_t *)lstFirst(&fdp->tlist);
	    i=0;
	    do {
#if 0
		if (n->tid == tid) {
#endif
		    fdp->fMode &= n->mode;
		    n->mode = 0;
		    n->tid = 0;
		    lstDelete (&fdp->tlist, (NODE *)n);
		    SiwFreeMem(n);
		    break;
#if 0
		}
#endif
		n = (FD_TID_t *)lstNext((NODE *)n);
		i++;
	    } while (i < c);
	}
    }

    fdp->fStatus--;
    if (fdp->fStatus < 0 ) {
	fdp->fStatus = 0;
	SiwSemGive(fdp->fSemId);
    }
    return (FM_OK);
}


/****************************************************************************
 * Function:    FmRead()
 * Descripion:  This routine is used to read a previously opened file. This 
 *              routine will allow the reading of file from any offset within
 *              the file. A multiple applications can read the same file
 *              simultaneously. Also, a file can be read while it is being
 *              written to. However, two applications can not write to the 
 *              same file simultaneously.
 * In Parms:    S32_t fd           - The File Descriptor
 *              char *buf          - pointer to buffer where to read data into
 *              S32_t offset       - The offset from the begining of the file
 *              S32_t maxBytes     - Max. number of bytes to read
 * Out Parms:   None
 * Return Values:
 *              #of bytes read     - Number of actual bytes read
 *              FM_ERROR
 */
S32_t 
FmRead(IN FD_t *fdp, char *buf, S32_t offset, S32_t maxBytes)
{
    SIW_FUNC_TRACE( FM, FmRead, fdp, buf, offset, maxBytes, 0);

    if (!VALID_FDP(fdp)) 
	return (FM_ILLEGAL_FILE_DESCR);

    if (!VALID_ADDRESS(buf)) 
	return (FM_ILLEGAL_ADDRESS);

    if (offset < 0) 
	return (FM_ILLEGAL_FILE_OFFSET);

    if (maxBytes < 0) 
	return (FM_ILLEGAL_LENGTH);

    switch (fdp->fType) {

    case HD:
	/* This is for HD, so sent it straight to vxWorks */
	{
	    S32_t ret = read(fdp->vxWFd, buf, maxBytes);
	    return (ret);
	}
	/* 
	return (read(fdp->vxWFd, buf, maxBytes));
	*/
	break;
    case RAM:
	/* Make sure that the file is open, before it can be read from */
	if (fdp->fStatus == 0) 
	    return(FM_FILE_NOT_OPEN);

	/*  
	 * read the file from specified offset for maxBytes number of bytes
	 * What happens if the file does not have enough data?
	 * Well send only up to the end of the file
	 */
	if ((offset + maxBytes) > fdp->fLen) {
	    /* We are going to go past the end of file. This is not good, 
	     * so only read until the end of file
	     */

	    S32_t len = fdp->fLen - offset;
            if (len > 0)
                bcopy ((char *)(fdp->fStart+offset), buf, len);
	    return (len);
	} else {
	    bcopy ((char *)(fdp->fStart+offset), buf, maxBytes);
	    return (maxBytes);
	}
	break;
    case FLASH:
	    /* FLASH does not suppot reading function */
	default:
	    FM_DEBUG("fmRead(): Illegal type of memory to read %d\n", fdp->fType);
	    return (FM_ILLEGAL_MEM_TYPE_READ);
    }
    return (FM_ERROR);
}

S32_t
FmReadLine(IN FD_t *fdp, OUT char *buf, S32_t offset, S32_t maxBytes)
{
  int ch, i = 0;
  char tmpBuffer;
  S32_t ret;
  int vxOffset = 0;

  SIW_FUNC_TRACE( FM, FmReadLine, fdp, buf, offset, maxBytes, 0);

  if (!VALID_FDP(fdp)) 
	return (FM_ILLEGAL_FILE_DESCR);

  if (!VALID_ADDRESS(buf)) 
	return (FM_ILLEGAL_ADDRESS);

  if (offset < 0) 
	return (FM_ILLEGAL_FILE_OFFSET);

  if (maxBytes < 0) 
	return (FM_ILLEGAL_LENGTH);

  switch (fdp->fType) {

    case HD:
	/* This is for HD, so send it straight to vxWorks */
	{
	  PINFO("FmReadLine(): Case HD\n");

	  /* Read chars one by one from file */
	  /* if FmRead returns 0, it means we've hit the EOF */
	  while ((ret = FmRead(fdp, &tmpBuffer, vxOffset + offset, 1)) > 0) {
		if (i >= maxBytes - 1) {
		  /* ran out of buffer space, so just return what you have */
		  printf("FmReadLine(): Ran out of buffer space!\n");
		  buf[i] = '\0';
#if 0
		  return (maxBytes - 1);
#endif
		  return (FM_ERROR);
		}

		if (tmpBuffer == '\n') {
		  if (i == 0) {
			/* hit EOL before reading anything, so go on to read the next 
			   line */
			continue;
		  } else {
			buf[i] = '\0';
			PINFO("FmReadLine(): Line read = %s\n", buf);
			if (i == 0) 
			  return (i);
			else
			  return (i - 1);
		  }
		}

		buf[i] = tmpBuffer;
		i++;
		vxOffset++;
	  }

	  /* EOF - will return 0*/
	  if (ret == 0) {
		PINFO("FmReadLine(): Reached EOF!\n");
		buf[i] = '\0';
		return (ret);
	  }
	}
	break;
	case RAM:
	  PINFO("FmReadLine(): Case RAM\n");
	  /* Make sure that the file is open, before it can be read from */
	  if (fdp->fStatus == 0) 
	    return(FM_FILE_NOT_OPEN);

	  /* only copy until '\n' or EOF */
	  while (((ch = *((char *)(fdp->fStart+offset))) != '\n') && (ch != EOF)) {
		if (offset >= maxBytes - 1) {
		  printf("FmReadLine(): Ran out of buffer space!\n");
		  buf[offset] = '\0';
#if 0
		  return (maxBytes - 1);
#endif
		  return (FM_ERROR);
		}

		bcopy ((char *)(fdp->fStart+offset), buf+offset, 1);
		offset++;
	  }

	  /* reached EOF and didn't read in anything */
	  if ((offset == 0) && (offset == EOF)) {
		PINFO("FmReadLine(): Nothing to read; end of file reached.\n");
	  } 
	  
	  buf[offset] = '\0';
	  if (offset == 0)
		return (offset);
	  else
		return (offset - 1);
	  break;

    case FLASH:
	  /* FLASH does not support reading function */
    default:
	  FM_DEBUG("FmReadLine(): Illegal type of memory to read %d\n", fdp->fType);
	  return (FM_ILLEGAL_MEM_TYPE_READ);
  }
  return (FM_ERROR);
}

U16_t
computeCkSum(U32_t sum, U32_t *addr, U32_t len)
{
    U32_t  ln = len >> 1;
    U16_t  *p = (U16_t *)addr;
#if 0
        U32_t  sum = 0; 
#endif
    U32_t i;

#if 0
    return 0;
#else
#if 0
SIW_FUNC_TRACE(CM, checkSum, addr,len,0,0,0);
#endif
    for (i = 0; i < ln; i++ ) {
        sum += *p++;
    }
    while (sum & 0xffff0000)
        sum = (sum>>16) + (sum & 0xffff);

    return (~sum );
#endif
}

/****************************************************************************
* Function:    FmWrite()
* Descripion:  This routine is used to Write into a previously opened file.
 *              This routine allows writing into a file at any location. 
 *		Same file can not be written to by two different applications.
 * In Parms:    FD_t *fdp          - The File Descriptor
 *              char *buf          - pointer to buffer where the data is
 *              S32_t offset       - The offset from the begining of the file
 *              S32_t maxBytes     - Max. number of bytes to read
 * Out Parms:   None
 * Return Values:
 *              #of bytes written  - if not equal to maxBytes, an error occured
 *              FM_ERROR
 */
S32_t 
FmWrite(IN FD_t *fdp, char *buf, S32_t offset, S32_t maxBytes, BOOL_t append)
{
    U32_t checksum;
    U32_t *tmpBuf;
    U32_t i;

    SIW_FUNC_TRACE( FM, FmWrite, fdp, buf, offset, maxBytes, append);
    FM_DEBUG("FmWrite(): NxtWr(%x), Offset(%d),maxBytes(%d)\n",
			fdp->fNextWr, offset, maxBytes);

    if (!VALID_FDP(fdp)) 
	return (FM_ILLEGAL_FILE_DESCR);

    if (!VALID_ADDRESS(buf)) 
	return (FM_ILLEGAL_ADDRESS);

    if (offset < 0)
	return (FM_ILLEGAL_FILE_OFFSET);

    if (maxBytes < 0)
	return (FM_ILLEGAL_LENGTH);

    switch (fdp->fType) {

    case HD:
       {
	/* This is for HD, so send it straight to vxWorks */
            U32_t nLoop = 0;
	    S32_t ret = write(fdp->vxWFd, buf, maxBytes);
            tmpBuf = (U32_t *) buf;

            if (ret >0 ) {
                if (strncmp (fdp->fName,  IMAGE_FILE_NAME_PREFIX,
                                        sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0){
                    if (fmGetImageFileCheckSum( fdp->fName, &checksum) < 0)  
                        return (FM_ERROR);
                    if ( (ret %4) != 0) {
                        nLoop = ret/4 +1; 
                        for (i = 0; i < nLoop;  i++, tmpBuf++) 
                        {
                            if ( i == nLoop - 1) {
                                if ( ret % 4 == 1)  {
                                    *tmpBuf = *tmpBuf & 0xff000000;
                                } 
                                else if ( ret % 4 == 2) {
                                    *tmpBuf = *tmpBuf & 0xffff0000;
                                }
                                else if ( ret % 4 == 3) {
                                    *tmpBuf = *tmpBuf & 0xffffff00;
                                } 
                            }
                            checksum += *tmpBuf;
                        }
                    }
                    else {
                        nLoop = ret/4;
                        /* caculate the checksum */
                        for (i = 0; i < nLoop;  i++, tmpBuf++) 
                            checksum += *tmpBuf;
                    }

                    /* update checksum in the Image File Table */
                    if( fmUpdateImageFileTable(fdp->fName, checksum) <0)
                        return (FM_ERROR);
                }
          } 

          /* if the "write" fails due to a disk segment error,
           *  rename the file, then this segment will not be reused. 
           */
          if (ret < 0) {
               char tmpName[MAX_FILE_NAME_LENGTH];
               char *pFname;
               
               close (fdp->vxWFd);
               strcpy (tmpName, "C:/TMP");
               /* get the last occurrence of "/" (0x2f) in fdp->fName */ 
               pFname = strrchr(fdp->fName, 0x2f);
               strcat ( tmpName, pFname); 
               rename(fdp->fName,  tmpName);
          }
          return (ret);
	/* 
	return (write(fdp->vxWFd, buf, maxBytes));
	*/
	break;
       }
    case RAM:
    case FLASH:
	{
	    S32_t len;
	    char  *dest;

	    /* Make sure that the file is open, before it can be written to*/
	    if (fdp->fStatus == 0) 
		return(FM_FILE_NOT_OPEN);

	    /* Make sure that file is open for writing */
	    if (!((fdp->fMode & WRITE_ONLY) || (fdp->fMode & READ_WRITE)))
		return(FM_FILE_NOT_OPEN_FOR_WRITE);
		
	    /*  
	     * write the file from specified offset for maxBytes number of bytes
	     * What happens if we exceed the max Lenght of the file ?
	     * Well, write only up to the end of the file and return error.
	     */
	    
	    if (append) {
		/* see if are going to go past the end of the file */
		if ((fdp->fNextWr + maxBytes) > (fdp->fStart + fdp->fLen)) {
		    len  = fdp->fLen - (fdp->fNextWr - fdp->fStart);
		} else {
		    len  = maxBytes;
		}
		dest = (char *)fdp->fNextWr;

	    } else { /* Write at an arbitrary location */
		if ((offset + maxBytes) > fdp->fLen) {
		    /* 
		     * We are going to go past the end of file. 
		     * This is not good, so only write until the end of file
		     */
		    len = fdp->fLen - offset;
		    if (len <= 0)
			len = 0;
		} else {
		    len  = maxBytes;
		}
		dest = (char *)(fdp->fStart + offset);
	    }
	    if (fdp->fType == RAM) {
		/* for RAM, simply do the bcopy */
		bcopy (buf, dest, len);
	    } else {
		/* For the flash programming, call the API */
		FlashWrite(buf, dest, len); /* SSSS */
	    }
            if (append)
                fdp->fNextWr += len;
            else
                fdp->fNextWr = (U32_t)(dest + len);

	    /* This one for debugging and Sanity checking only */
	    if (fdp->fNextWr > (fdp->fStart + fdp->fLen)) {
		/* Screeem like Hell; Some thing got messed up and we
		 * need to find out what happened
		 */
		FM_TRACE5(FmWrite, 0xF0000001,fdp, fdp->fNextWr,fdp->fStart,
				     fdp->fLen,0);
		FM_LOG3(WR_OVERRUN, fdp->fNextWr, fdp->fStart, fdp->fLen);
		FM_DEBUG("FmWrite(): XXXXXXXXXXXXXXXXX WARNING**********\n");
		FM_DEBUG("FmWrite(): Going to overwrite a file          \n");
		FM_DEBUG("FmWrite(): NxtWr(%x), Offset(%d),maxBytes(%d) len(%d)\n",
			fdp->fNextWr, offset, maxBytes, len);
	    }
	    return (len);
	}
	break;

	default:
	    FM_TRACE5(FmWrite, 0xF0000002,fdp, fdp->fType,0, 0,0);
	    FM_LOG2(WR_ILLEGAL_FT, fdp, fdp->fType);
	    FM_DEBUG("fmWrite(): Illegal type of memory to write %d\n",fdp->fType);
	    return (FM_ILLEGAL_MEM_TYPE_WRITE);
    }
    return (FM_ERROR);
}



/****************************************************************************
 * Function:    FmRemCreate()
 * Descripion:  This routine is a wrapper around FmCreate(). It takes the
 *              remote message, unpacks the message, calls FmCreate() and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              FM_ERROR
 *              Any error returned by lower level routine
 */
S32_t 
FmRemCreate(IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    RET_t    ret;
    FM_HDR_t *pHdr, *pOutHdr;

    SIW_FUNC_TRACE( FM, FmRemCreate, inMsg, outMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemCreate, 0xF0000001,inMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, inMsg);
	FM_DEBUG("FmRemCreate(): got inMsg as NULL");
	*outMsg = NULL;
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_ADDRESS);
    }

    /* get FM header address; so that we can see what's up */
    pHdr = FM_HDR_PTR(inMsg);

    /* first of all get a response buffer */
    *outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(*outMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemCreate, 0xF0000002,*outMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, *outMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    pOutHdr = FM_HDR_PTR(*outMsg);
    bcopy( (char *)pHdr, (char *)pOutHdr, sizeof(FM_HDR_t)); 

    /* Now here call the routine */
    ret =  FmCreate( (char *)&inMsg->data[pHdr->arg1],       /* fileName */
		     (U32_t)pHdr->arg2,        /* flags */
		     (FILE_TYPE_t)pHdr->arg3,  /* fileType*/
#ifndef FM_CB_FILE
		     (S32_t)pHdr->arg4         /* fileSize */
#else /* FM_CB_FILE */
		     (S32_t)pHdr->arg4,        /* fileSize */
		     (S32_t)pHdr->arg5         /* replFlag */
#endif /* FM_CB_FILE */
		   );
    PINFO("FmRemCreate(): return code from Local FmCreate is %d\n", ret);

    if (ret < 0 ) {
	FM_TRACE5(FmRemCreate, 0xF0000003,ret, pHdr->arg1,pHdr->arg2,pHdr->arg3,
		       pHdr->arg4);
	FM_LOG3(FMCREATE_FAIL, ret,pHdr->arg1,pHdr->arg2);
	PACK_FM_ERR_MSG(pOutHdr,ret)
    } else {
	char * pOutData = FM_DATA_PTR(*outMsg);
	/* Here pack the response and return */
	bcopy((char *)&ret, pOutData, sizeof(U32_t));
	PACK_FM_RESP_MSG(pOutHdr);
    }
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(*outMsg, sizeof(FM_HDR_t) + sizeof(U32_t) );

    /* Since we are the last one to use the input buffer, we must
     * free it up
     */
    SiwFreeMem(inMsg);
    return (ret);
}



/****************************************************************************
 * Function:    FmRemOpen()
 * Descripion:  This routine is a wrapper around FmOpen(). It takes the
 *              remote message, unpacks the message, calls FmOpen() and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 *              Any error returned by lower level routine
 */
S32_t 
FmRemOpen(IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    FM_HDR_t *pHdr, *pOutHdr;
    FD_t     *fdp;

    SIW_FUNC_TRACE( FM, FmRemOpen, inMsg, outMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemOpen, 0xF0000001,inMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, inMsg);
	FM_DEBUG("FmRemOpen(): got inMsg as NULL");
	*outMsg = NULL;
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    { U32_t i;
	FM_DEBUG("Header Info for FmRemOpen:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", inMsg->data[i]);
	FM_DEBUG("\n");
    }

    /* get FM header address; so that we can see what's up */
    pHdr = FM_HDR_PTR(inMsg);

    /* first of all get a response buffer */
    *outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(*outMsg))  {
	FM_TRACE5(FmRemOpen, 0xF0000001,*outMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, *outMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    pOutHdr = FM_HDR_PTR(*outMsg);
    bcopy( (char *)pHdr, (char *)pOutHdr, sizeof(FM_HDR_t)); 

    /* Now here call the routine */
    fdp =  FmOpen( (char *)&inMsg->data[pHdr->arg1],       /* fileName */
		   (U32_t) pHdr->arg2        /* flags    */
		   );
    if (fdp == NULL ) {
	FM_TRACE5(FmRemOpen, 0xF0000003,fdp, pHdr->arg1,pHdr->arg2,0, 0);
	FM_LOG3(FMOPEN_FAIL, fdp,pHdr->arg1,pHdr->arg2);
	PACK_FM_ERR_MSG(pOutHdr,FM_FILE_DOES_NOT_EXIST)
    } else {
	char * pOutData = FM_DATA_PTR(*outMsg);
	/* Here pack the response and return */
	bcopy((char *)&fdp, pOutData, sizeof(U32_t));
	PACK_FM_RESP_MSG(pOutHdr);
    }
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(*outMsg, sizeof(FM_HDR_t) + sizeof(U32_t) );
    /* Since we are the last one to use the input buffer, we must
     * free it up
     */
    SiwFreeMem(inMsg);
    return (0);
}


/****************************************************************************
 * Function:    FmRemClose()
 * Descripion:  This routine is a wrapper around FmClose(). It takes the
 *              remote message, unpacks the message, calls FmClose() and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 *              Any error returned by lower level routine
 */
S32_t 
FmRemClose(IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    RET_t    ret;
    FM_HDR_t *pHdr, *pOutHdr;

    SIW_FUNC_TRACE( FM, FmRemClose, inMsg, outMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemClose, 0xF0000001,inMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, inMsg);
	FM_DEBUG("FmRemClose(): got inMsg as NULL");
	*outMsg = NULL;
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_ADDRESS);
    }

    { U32_t i;
	FM_DEBUG("Header Info for FmRemClose:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", inMsg->data[i]);
	FM_DEBUG("\n");
    }

    /* get FM header address; so that we can see what's up */
    pHdr = FM_HDR_PTR(inMsg);

    /* first of all get a response buffer */
    *outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(*outMsg))  {
	FM_TRACE5(FmRemClose, 0xF0000002,*outMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, *outMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    pOutHdr = FM_HDR_PTR(*outMsg);
    bcopy( (char *)pHdr, (char *)pOutHdr, sizeof(FM_HDR_t)); 

    FM_DEBUG("FD being closed is %x\n", (U32_t)pHdr->arg1);

    /* Now here call the routine */
    ret =  FmClose( (FD_t *)pHdr->arg1       /* File Descr. Pointer */
		   );
    if (ret < 0 ) {
	FM_TRACE5(FmRemClose, 0xF0000003,ret, pHdr->arg1,0,0, 0);
	FM_LOG2(FMCLOSE_FAIL, ret,pHdr->arg1);
	PACK_FM_ERR_MSG(pOutHdr,ret)
    } else {
	/* Here pack the response and return */
	((MSG_t *)*outMsg)->data[sizeof(FM_HDR_t)] = ret;
	PACK_FM_RESP_MSG(pOutHdr);
    }
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(*outMsg, sizeof(FM_HDR_t) + sizeof(U32_t) );
    /* Since we are the last one to use the input buffer, we must
     * free it up
     */
    SiwFreeMem(inMsg);
    return (ret);
}




/****************************************************************************
 * Function:    FmRemRead()
 * Descripion:  This routine is a wrapper around FmRead(). It takes the
 *              remote message, unpacks the message, calls FmRead() and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 *              Any error returned by lower level routine
 */
S32_t 
FmRemRead(IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    RET_t    ret;
    FM_HDR_t *pHdr, *pOutHdr;

    SIW_FUNC_TRACE( FM, FmRemRead, inMsg, outMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemRead, 0xF0000001,inMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, inMsg);
	FM_DEBUG("FmRemRead(): got inMsg as NULL");
	*outMsg = NULL;
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_ADDRESS);
    }

    /* get FM header address; so that we can see what's up */
    pHdr = FM_HDR_PTR(inMsg);

    /* first of all get a response buffer */
    *outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(*outMsg))  {
	FM_TRACE5(FmRemRead, 0xF0000002,*outMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, *outMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    pOutHdr = FM_HDR_PTR(*outMsg);
    bcopy( (char *)pHdr, (char *)pOutHdr, sizeof(FM_HDR_t)); 

    /* Now here call the routine */
    ret =  FmRead( (FD_t *)pHdr->arg1,       /* File Descr. Pointer */
		   FM_DATA_PTR(pOutHdr)+sizeof(S32_t),/* Buffer ptr */
		   (S32_t)pHdr->arg3,        /* Offset */
		   (S32_t)pHdr->arg4         /* maxBytes */
		   );
    if (ret < 0 ) {
	FM_TRACE5(FmRemRead, 0xF0000003,ret, pHdr->arg1,pHdr->arg2,pHdr->arg3,
		       pHdr->arg4);
	FM_LOG3(FMREAD_FAIL, ret,pHdr->arg1,pHdr->arg4);
	PACK_FM_ERR_MSG(pOutHdr,ret)
    } else {
	/* Here pack the response and return */
	((MSG_t *)*outMsg)->data[sizeof(FM_HDR_t)] = ret;
	PACK_FM_RESP_MSG(pOutHdr);
    }
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(*outMsg, sizeof(FM_HDR_t) + sizeof(U32_t) + pHdr->arg4);
    /* Since we are the last one to use the input buffer, we must
     * free it up
     */
    SiwFreeMem(inMsg);
    return (ret);
}


/****************************************************************************
 * Function:    FmRemWrite()
 * Descripion:  This routine is a wrapper around FmWrite(). It takes the
 *              remote message, unpacks the message, calls FmWrite() and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 *              NOTE: if the write request is for the FLASH, then FmWrite()
 *                    is not called, istead, the message is enqued for the
 *                    FM task and the task deals with it. If the write request
 *                    is for RAM or HD, then FmWrite is called diretly.
 *                    Another Note to keep in mind is that if flash is being
 *                    written to, the caller must provide the callback 
 *                    module and the service number so that it can be invoked
 *                    once the write is complete.
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              FM_ERROR
 *              Any error returned by lower level routine
 */
S32_t 
FmRemWrite(IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    RET_t    ret;
    FM_HDR_t *pHdr, *pOutHdr;
    FD_t     *fdp;
    MSG_BUF_HDR_t itMsg;

    SIW_FUNC_TRACE( FM, FmRemWrite, inMsg, outMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to 
	 */
	FM_TRACE5(FmRemWrirte, 0xF0000001,inMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, inMsg);
	*outMsg = NULL;
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    { U32_t i;
	FM_DEBUG("Header Info for FmRemwrite:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", inMsg->data[i]);
	FM_DEBUG("\n");

	FM_DEBUG("Data To Be written:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", inMsg->data[sizeof(FM_HDR_t)+i]);
	FM_DEBUG("\n");
    }

    /* get FM header address; so that we can see what's up */
    pHdr = FM_HDR_PTR(inMsg);

    /* Extract file descriptor pointer */
    fdp = (FD_t *)pHdr->arg1;

    /* first of all get a response buffer */
    *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(*outMsg))  {
	FM_TRACE5(FmRemWrite, 0xF0000002,*outMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, *outMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    pOutHdr = FM_HDR_PTR(*outMsg);
    bcopy( (char *)pHdr, (char *)pOutHdr, sizeof(FM_HDR_t)); 

    /* Make sure we are dealing with a valid File Descriptor pointer */
    if (!VALID_FDP(fdp)) {
	FM_TRACE5(FmRemWrite, 0xF0000003,fdp, 0,0,0,0);
	FM_LOG(ILLEGAL_FDP, fdp);
	PACK_FM_ERR_MSG(pOutHdr, FM_ILLEGAL_FILE_DESCR)
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_FILE_DESCR);
    }

    /* see if we are dealing with Flash or any other media type */
    if (fdp->fType == FLASH) {
	/* 
	 * forward the message to FM task. FM task will directly send
	 * the response via MRPC to the sender of this message. This 
	 * routine will simply send the ACK back to the caller of this
	 * routine
	 */
	/* Send pointer to the data */
	itMsg.msgBufPtr = inMsg;
	itMsg.dstTaskId = TASK_FM;
	ret = SiwMsgQSend(MSGQ2CM, &itMsg, sizeof(MSG_BUF_HDR_t), 
					 WAIT_FOREVER, SIW_MSG_PRI_NORMAL);
	if (ret < 0) {
	    FM_TRACE5(FmRemWrite, 0xF0000004,ret, 0,0,0,0);
	    FM_LOG(FAIL_2Q_MSG, ret);
	    PACK_FM_ERR_MSG(pOutHdr, ret);
	    SiwFreeMem(inMsg);
	} else {
	    PACK_FM_ACK_MSG(pOutHdr);
	}
    } else {
	/* Make sure do not go here if file type is FLASH!!!!! */
	ret =  FmWrite(  fdp,                      /* file Desc. Ptr */
			 (char *)&inMsg->data[pHdr->arg2],   /* buf Address */
			 (S32_t)pHdr->arg3,        /* offset */
			 (S32_t)pHdr->arg4,        /* maxBytes */
			 (BOOL_t)pHdr->arg5        /* append */
		       );
	if (ret <= 0 ) {
	    FM_TRACE5(FmRemWrite, 0xF0000005,ret, pHdr->arg1,pHdr->arg2,
			   pHdr->arg3, pHdr->arg4);
	    FM_LOG3(FMWRITE_FAIL, ret,pHdr->arg1,pHdr->arg2);
            ret = -1;	
	    PACK_FM_ERR_MSG(pOutHdr,ret)
	} else {
	    /* Here pack the response and return */
	    ((MSG_t *)*outMsg)->data[sizeof(FM_HDR_t)] = ret;
	    PACK_FM_RESP_MSG(pOutHdr);
	}
        /* Set the Data Length */
        SIW_SARBUFLEN_SET(*outMsg, sizeof(FM_HDR_t) + sizeof(U32_t) );
	/* Since we are the last one to use the input buffer, we must
	 * free it up
	 */
	SiwFreeMem(inMsg);
    }
    return (ret);
}






/****************************************************************************
 * Function:    rmDir()
 * Descripion:  This routine deletes the entire Directory under VxWorks 
 *              File System. 
 * In Parms:    char  *name      - Directory Name
 * Out Parms:   NONE
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 */
RET_t 
rmDir (char	*dirName)
{
    RET_t               ret;
    STATUS		status;		/* return status */
    DIR	         	*pDir;		/* ptr to directory descriptor */
    struct dirent	*pDirEnt;	/* ptr to dirent */
    struct stat		fileStat;	/* file status info    (long listing) */
    STATUS		statResult;     /* return value of stat () call */
    char 		fileName [200];
					/* buffer for building file name */

    if ((pDir = opendir (dirName)) == NULL) {
	PERR ("rmDir(): Can't open \"%s\".\n", dirName);
	return (ERROR);
    }

    status = OK;

    while (TRUE) {
    	pDirEnt = readdir (pDir);
	if (pDirEnt == NULL) {
            PINFO ("rmDir(): Removing Directory %s \n", dirName);
            ret = closedir(pDir);
            if (ret == ERROR) {
                PERR("rmDir(): closedir Failed for dir %s return Code %d\n", 
                                dirName, ret);
                return(ERROR);
            }
            ret = rmdir (dirName);
            if (ret == ERROR) {
                PERR("rmDir(): rmdir Failed for dir %s return Code %d\n", 
                                dirName, ret);
                return(ERROR);
            }
	    return(OK);
        }

        /* Construct path/filename for stat */
        (void) pathCat (dirName, pDirEnt->d_name, fileName);

        if (!strncmp(pDirEnt->d_name, ".",1)) {
            continue;
        }
        if (!strncmp(pDirEnt->d_name, "..", 2)) {
            continue;
        }

        /* Get and print file status info */

        if ((statResult = (stat (fileName, &fileStat))) != OK) {
            /* 
                * Since we got this file from a readdir call, we know
                * something is there.  A failed stat() is probably
                * a symlink that can't be resolved.  Continue with the
                * listing.
                */

            PERR ("IO Error - can't stat file %s \n ", fileName);
            return(ERROR);
        }
        else if (S_ISDIR (fileStat.st_mode)) {
            ret = rmDir(fileName);
            if (ret != OK) {
                PINFO("rmDir(): rmDir failed for file %s\n", fileName);
                return(ERROR);
            }
        } else {
            ret = remove(fileName);
            if (ret != OK) {
                PERR("rmDir(): remvove failed for file %s\n", fileName);
                return(ERROR);
            }
        }
    }

    return (status);
}





/****************************************************************************
 * Function:    FmCreateRemFile()
 * Descripion:  This routine Create a Remote file on a target card.
 * In Parms:    S32_t        target; 
 *              char         *name; 
 *              U32_t        flag; 
 *              FILE_TYPE_t  fileType; 
 *              S32_t        fileSize; 
 * Out Parms:   FD_t     **fdp; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
FmCreateRemFile(IN S32_t target, 
                   char *fileName, 
                   U32_t flag, 
                   FILE_TYPE_t fileType, 
#ifndef FM_CB_FILE
                   S32_t fileSize)
#else /* FM_CB_FILE */
                   S32_t fileSize,
                   BOOL_t repl)
#endif /* FM_CB_FILE */
{
    FM_HDR_t *fmHdrP;
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    RET_t ret;
    S32_t resp;

    SIW_FUNC_TRACE(FM,FmCreateRemFile,target,fileName,flag,fileType,fileSize);

    if (!VALID_SLOT(target)) 
	return (FM_ILLEGAL_SLOT);

    if (!VALID_FILE_TYPE(fileType)) 
	return (FM_ILLEGAL_FILE_TYPE);

    if (!VALID_FILE_SIZE(fileSize)) 
	return (FM_ILLEGAL_FILE_SIZE);

    if (!VALID_FILE_FLAGS(flag & ~CREATE)) 
	return (FM_ILLEGAL_FILE_FLAGS);

    if (!VALID_FILE_NAME(fileName)) 
	return (FM_ILLEGAL_FILE_NAME);

    /*  * Allocate a buffer for Remote communications */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("FmCreateRemFile(): ########  WARNING!!!!! ######\n");
	return(FM_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    strcpy(&outMsg->data[sizeof(FM_HDR_t)], fileName);
    fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset */
    fmHdrP->arg2 = flag;
    fmHdrP->arg3 = fileType;
    fmHdrP->arg4 = fileSize;
#ifdef FM_CB_FILE
    fmHdrP->arg5 = repl;
#endif /* FM_CB_FILE */

    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + strlen(fileName) + 1);

    ret = MrpcSync(target, FM_MOD, FM_CREATE_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
    PINFO("FmCreateRemFile(): Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
        /* Failed to Create remote file */
        if (replyMsg != NULL)
            SiwFreeMem(replyMsg);
        return(ret);
    }
    /* Just make sure that the file got created OK, if so, get
     * the return code over to the user.
     */
    if (((FM_HDR_t *)&(replyMsg->data[0]))->err != FM_OK) {
	/* Opsss, we got an error opening the remote file */
	/* Need to take recovery action */
	SiwFreeMem(replyMsg);
	return(FM_ERROR);
    } 

    bcopy(
       (char *)&replyMsg->data[sizeof(FM_HDR_t)], (char *)&resp, sizeof(S32_t));
    SiwFreeMem(replyMsg);

    PINFO ("FmCreateRemFile(): Ret code from Remote FmCreate is %d\n", resp);
    return(resp);
}






/****************************************************************************
 * Function:    FmOpenRemFile()
 * Descripion:  This routine Create a Remote file on a target card.
 * In Parms:    S32_t        target; 
 *              char         *name; 
 *              U32_t        flag; 
 * Out Parms:   FD_t     **fdp; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
FmOpenRemFile(IN S32_t target, char *dstName, U32_t flag, FD_t **fdp)
{
    FM_HDR_t *fmHdrP;
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    FD_t *fd;
    RET_t ret;

    SIW_FUNC_TRACE(FM, FmOenRemFile, target, dstName, flag, fdp, 0);

    if (!VALID_SLOT(target)) 
	return (FM_ILLEGAL_SLOT);

    if (!VALID_FILE_FLAGS(flag & ~CREATE)) 
	return (FM_ILLEGAL_FILE_FLAGS);

    if (!VALID_FILE_NAME(dstName)) 
	return (FM_ILLEGAL_FILE_NAME);

    /*  
     * At this time need to  open the remote file to write 
     * But, before we do that we need to allocate a message
     * buffer, fill up the FM Header message for the remote
     * File Manager task so that it knows what to do with it
     */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("FmOenRemFile(): ########  WARNING!!!!! ######\n");
	return(FM_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    strcpy(&outMsg->data[sizeof(FM_HDR_t)], dstName);
    fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset */
    fmHdrP->arg2 = flag;

    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + strlen(dstName) + 1);

    ret = MrpcSync(target, FM_MOD, FM_OPEN_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
    FM_DEBUG("FmOenRemFile(): First Time Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
        /* Failed to open second time; so give up */
        *fdp = NULL;
        if (replyMsg != NULL)
            SiwFreeMem(replyMsg);
        return(ret);
    }
    /* Just make sure that the file got opened OK, if so, get
     * the dest file descriptor poiner, as it is needed for
     * writing to the destination
     */
    if (((FM_HDR_t *)&(replyMsg->data[0]))->err != FM_OK) {
	/* Opsss, we got an error opening the remote file */
	/* Need to take recovery action */
	SiwFreeMem(replyMsg);
	*fdp = NULL;
	return(FM_COULD_NOT_OPEN_DST_FILE);
    } 

    bcopy(
       (char *)&replyMsg->data[sizeof(FM_HDR_t)], (char *)&fd, sizeof(U32_t));
    SiwFreeMem(replyMsg);

    if (fd == NULL) {
	/* There must have been an error on the way */
	/* need to handle this case and do not proceed from
	 * here on, as we are not working with a valid File
	 * descriptor. The user needs to be informed that 
	 * things did not go OK and therefore a recoverty 
	 * mechanism needs to kick in at this time
	 */
	*fdp = NULL;
	return(FM_COULD_NOT_OPEN_DST_FILE);
    }
    *fdp = fd;
    return(FM_OK);
}

/****************************************************************************
 * Function:    FmRename()
 * Descripion:  This routine is used to delete a file in any of the
 *              media such as RAM, Hard Drive, Non-Volatile RAM, and Flash.
 * In Parms:    char *file1     - Name of the source file
 *              char *file2     - Name of the destination file
 * Out Parms:   None 
 * Return Values:
 *              FM_OK               - If all works out OK
 *              FM_ERROR
 *              FM_FILE_DOES_NOT_EXIST
 */
RET_t 
FmRename(IN char *sFile, char *dFile)
{
    S32_t i;
    FD_t  *fdp;
    RET_t ret = FM_OK;
    BOOL_t foundFile = FALSE;
    BOOL_t isImageFile = FALSE;

    /* first validate all the input parameters */

    SIW_FUNC_TRACE( FM, FmRename, sFile, dFile, 0, 0, 0);
    
    if ((!VALID_FILE_NAME(sFile)) ||  
        (!VALID_FILE_NAME(dFile)))
      return (FM_ILLEGAL_FILE_NAME);

    /* image file */
    if ((strncmp (sFile,  IMAGE_FILE_NAME_PREFIX,
		  sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0) && 
	(strncmp (dFile,  IMAGE_FILE_NAME_PREFIX,
		  sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0)) {
#ifdef SCC    
      PINFO("   ==> Image files! <==\n");

      isImageFile = TRUE;

      /* rename in vxWorks */
      ret = rename (sFile, dFile);
      if (ret < 0) {
	PERR("FmRename(): FAILED Rename HD file from %s to %s\n", 
	     sFile, dFile);
	return(FM_ERROR);
      }

      ret = fmRenameImageFile(sFile, dFile);
      if (ret <0 ) return FM_ERROR;
#endif /* SCC */

    } /* if image file */

    PINFO("    Check to see if %s exists in fileTable\n", sFile);

    /* See if source file is in the fileTable */
    for (i=0; i < MAX_FILES; i++) {
	if (strcmp(sFile, fileTable[i].fName) == 0) {
	  PINFO("    Found file!\n");

	  /* Got the file */
	  fdp = fileTable[i].fdp;
	  foundFile = TRUE;

	  if (fdp->fType == HD) {
	    PINFO("    HD file!\n");
	    /* For Hard Drive pass it down straight to vxWorks */
	    if (!isImageFile)
	      ret = rename (sFile, dFile);
	    if (ret < 0) {
	      PERR("FmRename(): FAILED Rename HD file from %s to %s\n", 
		   sFile, dFile);
	      return(FM_ERROR);
	    }
	  } /* if HD file */

	  strcpy(fileTable[i].fName, dFile);

#ifdef FM_CB_FILE
	  fmUpdateCb(i);
#endif /* FM_CB_FILE */
	} /* if found file*/
    } /* for */

    if ((foundFile) || (isImageFile)) {
      return(FM_OK);
    } else {
      PERR("FmRename(): File does not exist in fileTable\n");
      return(FM_FILE_DOES_NOT_EXIST);
    }
}

#if 0
/****************************************************************************
 * Function:    FmRename()
 * Descripion:  This routine is used to delete a file in any of the
 *              media such as RAM, Hard Drive, Non-Volatile RAM, and Flash.
 * In Parms:    char *file1     - Name of the source file
 *              char *file2     - Name of the destination file
 * Out Parms:   None 
 * Return Values:
 *              FM_OK               - If all works out OK
 *              FM_ERROR
 *              FM_FILE_DOES_NOT_EXIST
 */
RET_t 
FmRename(IN char *sFile, char *dFile)
{
    S32_t i;
    FD_t  *fdp;
    U32_t checksum;
    RET_t ret = FM_OK;

    /* first validate all the input parameters */

    SIW_FUNC_TRACE( FM, FmRename, sFile, dFile, 0, 0, 0);

    if ((!VALID_FILE_NAME(sFile)) ||  
        (!VALID_FILE_NAME(dFile)))
	return (FM_ILLEGAL_FILE_NAME);

    /* See of source file is in the fileTable */
    for (i=0; i < MAX_FILES; i++) {
	if (strcmp(sFile, fileTable[i].fName) == 0) {
	    /* Got the file */
	    fdp = fileTable[i].fdp;
	    if (fdp->fType == HD) {
		/* For Hard Drive pass it down straight to vxWorks */
		ret = rename (sFile, dFile);
                if (ret < 0) {
                    PERR("FmRename(): FAILED Rename HD file from %s to %s\n", 
                                    sFile, dFile);
                    return(FM_ERROR);
                }
                if ((strncmp (sFile,  IMAGE_FILE_NAME_PREFIX,
                                    sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0) && 
                    (strncmp (dFile,  IMAGE_FILE_NAME_PREFIX,
                                    sizeof(IMAGE_FILE_NAME_PREFIX)-1)==0)){

                    ret = fmRenameImageFile(sFile, dFile);
                    if (ret <0 ) return FM_ERROR;


                }
	    }
            strcpy(fileTable[i].fName, dFile);
            PERR("FmRename(): Renamed file from %s to %s\n", sFile, dFile);
#ifdef FM_CB_FILE
            fmUpdateCb(i);
#endif /* FM_CB_FILE */
	    return (FM_OK);
	}
    }
    /* We must not get here */
    return (FM_FILE_DOES_NOT_EXIST);
}
#endif


/****************************************************************************
 * Function:    FmRemRename()
 * Descripion:  This routine is a wrapper around FmRename(). It takes the
 *              remote message, unpacks the message, calls FmRename() and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 *              Any error returned by lower level routine
 */
S32_t 
FmRemRename(IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    RET_t    ret;
    FM_HDR_t *pHdr, *pOutHdr;
#ifdef NSC
    STATUS_t stat;
    U8_t* ptr;
#endif 

    SIW_FUNC_TRACE( FM, FmRemRename, inMsg, outMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemRename, 0xF0000002,inMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, inMsg);
	FM_DEBUG("FmRemRename(): got inMsg as NULL");
	*outMsg = NULL;
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_ADDRESS);
    }

    /* get FM header address; so that we can see what's up */
    pHdr = FM_HDR_PTR(inMsg);

    /* first of all get a response buffer */
    *outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(*outMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemRename, 0xF0000002,*outMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, *outMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    pOutHdr = FM_HDR_PTR(*outMsg);
    bcopy( (char *)pHdr, (char *)pOutHdr, sizeof(FM_HDR_t)); 

    if (pHdr->arg3 == 1) {
        S32_t i;
        /* Before deleting the destination, make sure that we have source
         * file. If that is not the case, do not delete the dest file.
         * Otherwise we will be left with a situation where we will have
         * neither of the files
         */
        for (i=0; i < MAX_FILES; i++) {
            if (strcmp((char *)&inMsg->data[pHdr->arg1], fileTable[i].fName)
                                                          == 0) {
                PINFO ("FmRemRename(): Deleting File %s before Renaming\n", 
                                    (char *)&inMsg->data[pHdr->arg2]);
                FmDelete((char *)&inMsg->data[pHdr->arg2]);
                break;
            }
        }
    }
     { 
        /* Now here call the routine */
        ret =  FmRename( (char *)&inMsg->data[pHdr->arg1],      /* srcFile */
                        (char *)&inMsg->data[pHdr->arg2]       /* dstFile */
                    );
        if (ret < 0 ) {
            FM_TRACE5(FmRemRename, 0xF0000003,ret, pHdr->arg1, pHdr->arg2,0,0);
            FM_LOG3(FMRENAME_FAIL, ret,pHdr->arg1, pHdr->arg2);
            PACK_FM_ERR_MSG(pOutHdr,ret)

        } else {
            /* Here pack the response and return */
            ((MSG_t *)*outMsg)->data[sizeof(FM_HDR_t)] = ret;
            PACK_FM_RESP_MSG(pOutHdr);
        }
    }

    /* Set the Data Length */
    SIW_SARBUFLEN_SET(*outMsg, sizeof(FM_HDR_t) + sizeof(U32_t) );

    /* Since we are the last one to use the input buffer, we must
     * free it up
     */
    SiwFreeMem(inMsg);
    return (ret);
}


/****************************************************************************
 * Function:    FmRenameRemFile()
 * Descripion:  This routine Create a Remote file on a target card.
 * In Parms:    S32_t        target; 
 *              char         *sFile; 
 *              char         *dFile; 
 *              BOOL_t       delFlag; - TRUE(1) delete dFile before renaming
 * Out Parms:   none
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
FmRenameRemFile(IN S32_t target, char *sFile, char *dFile, BOOL_t delFlag)
{
    FM_HDR_t *fmHdrP;
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    RET_t ret;

    SIW_FUNC_TRACE(FM, FmOenRemFile, target, sFile, dFile, 0, 0);

    if (!VALID_SLOT(target)) 
	return (FM_ILLEGAL_SLOT);

    if ((!VALID_FILE_NAME(sFile)) ||  
        (!VALID_FILE_NAME(dFile)))
	return (FM_ILLEGAL_FILE_NAME);

    /*  Allocate a buffer to send a remote message to the target */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	PERR("FmOenRemFile(): ########  WARNING!!!!! ######\n");
	return(FM_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    strcpy(&outMsg->data[sizeof(FM_HDR_t)], sFile);
    strcpy(&outMsg->data[sizeof(FM_HDR_t)+strlen(sFile) + 1], dFile);
    fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset for Src File*/
    fmHdrP->arg2 = sizeof(FM_HDR_t)+strlen(sFile)+1; /* Offset for dest File*/
    fmHdrP->arg3 =  delFlag;

    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t)+strlen(sFile)+strlen(dFile)+2);

    ret = MrpcSync(target, FM_MOD, FM_RENAME_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
    PINFO("FmRenameRemFile: First Time Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
        if (replyMsg != NULL)
            SiwFreeMem(replyMsg);
        return(ret);
    }
    /* Just make sure that the file got renamed OK */
    if (((FM_HDR_t *)&(replyMsg->data[0]))->err != FM_OK) {
	/* Opsss, we got an error renaming the remote file */
	/* Need to take recovery action */
	SiwFreeMem(replyMsg);
	return(FM_ERROR);
    } 

    SiwFreeMem(replyMsg);
    return(FM_OK);
}

/****************************************************************************
 * Function:    FmRemDelete()
 * Descripion:  This routine is a wrapper around FmDelete(). It takes the
 *              remote message, unpacks the message, calls FmDelete() and
 *              Packages the response and returns. It is assumed that MRPC
 *              will do its job and deliver the response to the rightful
 *              owner.
 * In Parms:    MSG_t  *inMsg      - Pointer to the input Message     
 * Out Parms:   MSG_t  **outMsg    - Pointer to the output Message     
 * Return Values:
 *              FM_OK
 *              FM_ERROR
 *              Any error returned by lower level routine
 */
S32_t 
FmRemDelete(IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    RET_t    ret;
    U8_t *pHdr;
    FM_HDR_t *pOutHdr;
    U8_t fName[MAX_FILE_NAME_LENGTH];

    SIW_FUNC_TRACE( FM, FmRemDelete, inMsg, outMsg, 0, 0, 0);

    if (!VALID_ADDRESS(inMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemRemDelete, 0xF0000002,inMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, inMsg);
	FM_DEBUG("FmRemDelete(): got inMsg as NULL");
	*outMsg = NULL;
	SiwFreeMem(inMsg);
	return (FM_ILLEGAL_ADDRESS);
    }

    /* get FM header address; so that we can see what's up */
    pHdr = (U8_t *)(FM_HDR_PTR(inMsg));

    /* first of all get a response buffer */
    *outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(*outMsg))  {
	/* Nothing can be done in this case. It is a lost cause; we do not
	 * have senders information to respond to . Log this information
	 */
	FM_TRACE5(FmRemDelete, 0xF0000002,*outMsg, 0,0,0,0);
	FM_LOG(NULL_ADDR, *outMsg);
	return (FM_ILLEGAL_ADDRESS);
    }
    if (!VALID_FILE_NAME(pHdr)) {
         *outMsg = inMsg;
         return (-1);
    }

    pOutHdr = FM_HDR_PTR(*outMsg);
    bcopy ( pHdr, fName, MAX_FILE_NAME_LENGTH);


    printf("FmRemDeleteFile(): fName is %s\n", fName);


    /* Now here call the routine */
    ret =  FmDelete( fName);      /* srcFile */
    if (ret < 0 ) {
	/* Here pack the response and return */
        PACK_FM_ERR_MSG(pOutHdr,ret)
    }

    /* Set the Data Length */
    SIW_SARBUFLEN_SET(*outMsg, sizeof(FM_HDR_t) + sizeof(U32_t) );

    /* Since we are the last one to use the input buffer, we must
     * free it up
     */
    SiwFreeMem(inMsg);
    return (ret);
}

/****************************************************************************
 * Function:    FmDeleteRemFile()
 * Descripion:  This routine delete a Remote file on a target card.
 * In Parms:    S32_t        target; 
 *              char         *sFile; 
 * Out Parms:   none
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
FmDeleteRemFile(IN S32_t target, char *sFile)
{
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    RET_t ret;
    U8_t *pData;

    SIW_FUNC_TRACE(FM, FmDeleteRemFile, target, sFile, 0, 0, 0);

    if (!VALID_SLOT(target)) 
	return (FM_ILLEGAL_SLOT);

    if (!VALID_FILE_NAME(sFile))   
	return (FM_ILLEGAL_FILE_NAME);

    /*  Allocate a buffer to send a remote message to the target */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	PERR("FmDeleteRemFile(): ########  WARNING!!!!! ######\n");
	return(FM_ILLEGAL_ADDRESS);
    }
    pData = (U8_t *)(FM_HDR_PTR(outMsg));
    
    bcopy (sFile, pData, MAX_FILE_NAME_LENGTH);

    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t)+ MAX_FILE_NAME_LENGTH);

    ret = MrpcSync(target, FM_MOD, FM_DELETE_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
    PINFO("FmDeleteRemFile: First Time Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
        if (replyMsg != NULL)
            SiwFreeMem(replyMsg);
        return(ret);
    }

    SiwFreeMem(replyMsg);
    return(FM_OK);
}



RET_t
fmAddMiscFiles2ImageFileTb() 
{
    RET_t               ret;
    DIR	         	*pDir;		/* ptr to directory descriptor */
    struct dirent	*pDirEnt;	/* ptr to dirent */
    struct stat		fileStat;	/* file status info    (long listing) */
    STATUS		statResult;     /* return value of stat () call */
    char 		fileName [200];
					/* buffer for building file name */

    if ((pDir = opendir (MISC_FILE_DIR)) == NULL) {
	PERR ("fmAddMiscFiles2ImageFileTb(): Can't open \"%s\".\n",MISC_FILE_DIR );
	return (ERROR);
    }

    while (TRUE) {
    	pDirEnt = readdir (pDir);
	if (pDirEnt == NULL) {
            PINFO("fmAddMiscFiles2ImageFileTb(): %s dir is empty \n", MISC_FILE_DIR);
            ret = closedir(pDir);
            if (ret == ERROR) {
                PERR("fmAddMiscFiles2ImageFileTb(): closedir Failed for dir %s return Code %d\n", 
                                MISC_FILE_DIR, ret);
                return(ERROR);
            }
	    return(OK);
        }


        if (!strncmp(pDirEnt->d_name, ".",1)) {
            continue;
        }
        if (!strncmp(pDirEnt->d_name, "..", 2)) {
            continue;
        }

        /* Construct path/filename for stat */
        (void) pathCat (MISC_FILE_DIR, pDirEnt->d_name, fileName);

        /* Get and print file status info */

        if ((statResult = (stat (fileName, &fileStat))) != OK) {
            /* 
                * Since we got this file from a readdir call, we know
                * something is there.  A failed stat() is probably
                * a symlink that can't be resolved.  Continue with the
                * listing.
                */

            PERR ("fmAddMiscFiles2ImageFileTb(): IO Error - can't stat file %s \n ", fileName);
            return(ERROR);
        }
        else { /* got work on this file */
           S32_t   fd, rst;
           char    buf[32];
           U32_t *tmpBuf;
           U32_t checksum =0;
           U32_t nLoop,i;

           /* check if the fileName has existed in the RAM image file TB */
#if 0
           if ( fmGetImageFileIndex(fileName) >=0 ) {
                 continue;
           }
           else
#endif
            {
            /* initialize the checksum for this file */

            /* read the file and caculate the checksum. 
             * Then save it to RAM and HD ImageFile Table  
             */
            fd = open(fileName, O_RDWR, 0644);
            if (fd < 0) {
                /* Opss... we have a problem here */
                FM_TRACE5(fmAddMiscFiles2ImageFileTb, 0xF000001, fd, 0, 0, 0, 0);
                PERR("fmAddMiscFiles2ImageFileTb: File Open Fail fd(%d) \n", fd);
                return (FM_CB_FILE_OPEN_FAIL);
            }

            /* read a record from fName */
            while((rst = read(fd,  buf, 32)) > 0) {
                tmpBuf = (U32_t *)(&buf);
                if ( (rst %4) != 0) {
                    nLoop = rst/4 +1; 
                    for (i = 0; i < nLoop;  i++, tmpBuf++) 
                    {
                        if ( i == nLoop - 1) {
                            if ( rst % 4 == 1)  {
                                *tmpBuf = *tmpBuf & 0xff000000;
                            } 
                            else if ( rst % 4 == 2) {
                                *tmpBuf = *tmpBuf & 0xffff0000;
                            }
                            else if ( rst % 4 == 3) {
                                *tmpBuf = *tmpBuf & 0xffffff00;
                            } 
                        }
                        checksum += *tmpBuf;
                    }
                }
                else {
                    nLoop = rst/4;
                    /* caculate the checksum */
                    for (i = 0; i < nLoop;  i++, tmpBuf++) 
                        checksum += *tmpBuf;
                }
            }

            if (rst < 0) {
                    /* Oppss... could not read a record from the file */

                    FM_TRACE5(fmAddMiscFiles2ImageFileTb,0xF000003, fd, rst, 0,0, 0);
                    PERR("fmAddMiscFiles2ImageFileTb: read Fail rst(%d) fd(%d) \n", rst, fd);
                    close (fd);
                    return(FM_ERROR);
            }

            if ( rst == 0) { /* end of file */

                close (fd);

                /* update checksum in the Image File Table */
                if( fmUpdateImageFileTable(fileName, checksum) <0) {
                    PERR("fmAddMiscFiles2ImageFileTb(): fails to add %s( checksum %d) into the Ram Inage file TB ", fileName, checksum);
	            FM_LOG(ADD_MICS_FILES_RAM_FAIL, fileName);
                    return (FM_ERROR);
                }

                /* update the checksum in the HD image file table */
                if (fmUpdateHDImageFileTable(HD_IMAGE_FILE_TABLE, 
                            fmGetImageFileIndex(fileName), fileName, checksum)<0) {
                    PERR("fmAddMiscFiles2ImageFileTb(): fails to add %s( checksum %d) into the HD Image file TB ", fileName, checksum);
	            FM_LOG(ADD_MICS_FILES_HD_FAIL, fileName);
                    return (FM_ERROR);

                }
            }
         }
       }
    }
}

/****************************************************************************
 * Function:    closeBothFiles()
 * Descripion:  This routine Opens a local file and verifies the FDP to be
 *              valid. otherwise it makes the FDP = NULL.
 * In Parms:    char    *name; 
 * Out Parms:   FD_t     **fdp; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
closeBothFiles(S32_t targetCard, FD_t *srcFdp, FD_t *destFdp)
{
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    FM_HDR_t *fmHdrP;
    RET_t ret;

    SIW_FUNC_TRACE(FM, closeBothFiles, targetCard, srcFdp, destFdp, 0, 0);

    /* Now close both local and remote files */
    FmClose(srcFdp);

    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("closeBothfiles(): ########  WARNING!!!!! ######\n");
	return(FM_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    fmHdrP->arg1 = (U32_t)destFdp;
    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t));
    { U32_t i;
	FM_DEBUG("Header Info for FmRemClose:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", outMsg->data[i]);
	FM_DEBUG("\n");
    }
    ret = MrpcSync(targetCard, FM_MOD, FM_CLOSE_SERVICE, outMsg, LO, 
					WAIT_FOREVER, &replyMsg);
    if (ret < 0) {
#if 0 /* SSSSSS */
	/* Failed on MRPC Message; Give it one more shot and see if
	 * we can close it the second time
	 */
	outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
	if (!VALID_ADDRESS(outMsg)) {
	    /* Scream!!!!! We just ran out of buffers */
	    FM_DEBUG("closeBothfiles(): 2222 ########  WARNING!!!!! ######\n");
	    return(FM_ILLEGAL_ADDRESS);
	}
	fmHdrP = (FM_HDR_t *)&outMsg->data[0];
	fmHdrP->arg1 = (U32_t)destFdp;
        /* Set the Data Length */
        SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t));
	ret = MrpcSync(targetCard, FM_MOD, FM_CLOSE_SERVICE, outMsg, LO, 
					    WAIT_FOREVER, &replyMsg);
#endif /* 0  SSSSSS */
	if (ret < 0) {
	    /* Failed again. This is not good; Well, we have already
	     * tried twice. So give up
	     */
	    if (replyMsg != NULL)
		SiwFreeMem(replyMsg);
        /* added this to distinguish MRPC timeouts from appl errors */
        if (ret == MRPC_TIMEOUT)
            return(FM_MRPC_TIMEOUT);
        else
            return(FM_ERROR);
	}
    }

    FM_DEBUG("Closed the local and remote files \n");

    /* we are done with the inbuffer, so free it up now */
    SiwFreeMem(replyMsg);

    return (FM_OK);
}

/****************************************************************************
 * Function:    openRemFile()
 * Descripion:  This routine Opens a local file and verifies the FDP to be
 *              valid. otherwise it makes the FDP = NULL.
 * In Parms:    char    *name; 
 * Out Parms:   FD_t     **fdp; 
 * Return Values:
 *              0  - if all OK
 *              -1 - otherwise
 */
RET_t 
openRemFile(S32_t targetCard, char *dstName, FD_t **fdp)
{
    FM_HDR_t *fmHdrP;
    MSG_t *outMsg; 
    MSG_t *replyMsg = NULL;
    FD_t *fd;
    RET_t ret;

    SIW_FUNC_TRACE(FM, openRemFile, targetCard, dstName, fdp, 0, 0);
    /*  
     * At this time need to  open the remote file to write 
     * But, before we do that we need to allocate a message
     * buffer, fill up the FM Header message for the remote
     * File Manager task so that it knows what to do with it
     */
    outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
    if (!VALID_ADDRESS(outMsg)) {
	/* Scream!!!!! We just ran out of buffers */
	FM_DEBUG("openRemFile(): ########  WARNING!!!!! ######\n");
	return(FM_ILLEGAL_ADDRESS);
    }
    fmHdrP = (FM_HDR_t *)&outMsg->data[0];
    strcpy(&outMsg->data[sizeof(FM_HDR_t)], dstName);
    fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset */
    fmHdrP->arg2 = WRITE_ONLY;

    /* Set the Data Length */
    SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + strlen(dstName) + 1);

    { U32_t i;
	FM_DEBUG("Header Info for FmRemOpen:  ");
	for(i=0; i < 20; i++)
	    FM_DEBUG("%x ", outMsg->data[i]);
	FM_DEBUG("\n");
    }
    ret = MrpcSync(targetCard, FM_MOD, FM_OPEN_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
    FM_DEBUG("openRemFile(): First Time Return code from Mrpcsync is %d\n", ret);
    if (ret < 0) {
#if 0 /* SSSSS */
	/* We had problem opening the destination file */
	/* 
	 * We most likly timed out on the MRPC.
	 * Make one more attempt to open it and see if we are 
	 * able to open it second time.
	 */
	SiwFreeMem(replyMsg);
	outMsg = SiwAllocMem(SIW_SAR, sizeof(MSG_t), WAIT_FOREVER);
        /* Set the Data Length */
        SIW_SARBUFLEN_SET(outMsg, sizeof(FM_HDR_t) + strlen(dstName) + 1);
	if (!VALID_ADDRESS(outMsg)) {
	    /* Scream!!!!! We just ran out of buffers */
	    FM_DEBUG("openRemFile():222 ########  WARNING!!!!! ######\n");
	    return(FM_ILLEGAL_ADDRESS);
	}
	fmHdrP = (FM_HDR_t *)&outMsg->data[0];
	strcpy(&outMsg->data[sizeof(FM_HDR_t)], dstName);
	fmHdrP->arg1 = sizeof(FM_HDR_t); /* Offset */
	fmHdrP->arg2 = WRITE_ONLY;
	ret = MrpcSync(targetCard, FM_MOD, FM_OPEN_SERVICE, outMsg, LO, 
				    WAIT_FOREVER, &replyMsg);
	FM_DEBUG("openRemFile():2nd Time Return code from Mrpcsync is %d\n", ret);
#endif /* 0  SSSSS */

	if (ret < 0) {
	    /* Failed to open second time; so give up */
	    *fdp = NULL;
	    if (replyMsg != NULL)
		SiwFreeMem(replyMsg);
	    return(ret);
	}
    }
    /* Just make sure that the file got opened OK, if so, get
     * the dest file descriptor poiner, as it is needed for
     * writing to the destination
     */
    if (((FM_HDR_t *)&(replyMsg->data[0]))->err != FM_OK) {
	/* Opsss, we got an error opening the remote file */
	/* Need to take recovery action */
	SiwFreeMem(replyMsg);
	*fdp = NULL;
	return(FM_COULD_NOT_OPEN_DST_FILE);
    } 

    bcopy(
       (char *)&replyMsg->data[sizeof(FM_HDR_t)], (char *)&fd, sizeof(U32_t));
    SiwFreeMem(replyMsg);

    if (fd == NULL) {
	/* There must have been an error on the way */
	/* need to handle this case and do not proceed from
	 * here on, as we are not working with a valid File
	 * descriptor. The user needs to be informed that 
	 * things did not go OK and therefore a recoverty 
	 * mechanism needs to kick in at this time
	 */
	*fdp = NULL;
	return(FM_COULD_NOT_OPEN_DST_FILE);
    }
    *fdp = fd;
    return(FM_OK);
}

#ifdef SCC
/****************************************************************************
 * Function:    fmIsImageFile()
 * Descripion:  This routine is used to check if a file is a image file.
 *
 * In Parms:    char *      a File name 
 * Out Parms:   none 
 * Return Values:
 *              TRUE or FALSE 
 */
BOOL_t
fmIsImageFile(U8_t *fileName, U8_t *refFileName)
{
  IMAGE_FT_t *l;
  U8_t *pt1, *pt2;
  U32_t flag;

  SIW_FUNC_TRACE( FM, fmIsImageFile, fileName, refFileName, 0, 0, 0);

  flag = 0;

  /* verify the file name */
  if (!VALID_FILE_NAME(fileName)) 
      return (FM_ILLEGAL_FILE_NAME);

  if (!VALID_FILE_NAME(refFileName)) 
      return (FM_ILLEGAL_FILE_NAME);

  pt1 = strrchr ( fileName, 0x2f );
  if (pt1 == NULL) {
        pt1 = fileName;
        flag = 1;
  }

  for (l = (IMAGE_FT_t *)lstFirst(&imageFtList); l != NULL; 
                                    l = (IMAGE_FT_t *)l->node.next ) {
        pt2 = strrchr ( l->fName, 0x2f );
        if ( flag == 1) 
            pt2 += 1 ;

        if (strcmp(pt1, pt2) == 0) {
            strcpy ( refFileName, l->fName );
            return TRUE;
        }
  }
  return FALSE;
}


/****************************************************************************
 * Function:   FmDeleteLater () 
 * Descripion:  This routine is to remove an image file from 
 *              RAM,  HD image file tables and hard disk. 
 * In Parms:    char    *name; 
 * Return Values:
 *              FM_OK   -  OK
 *              FM_ERROR  - otherwise
 */
RET_t
FmDeleteLater(U8_t * fileName)
{
    U32_t idx;

        if (AreYouSure("Are you sure to remove this file (Y/N)? ", "Y") == ERROR)
            return(ERROR);

        /* TODO: the following lines will be replaced by FmDelete()
         *  after FmDelete is enchanced 
         */
        idx = fmGetImageFileIndex(fileName);
        if ( idx >= 0) {
            fmRemoveImageFile(fileName);
            /* replace the filename using "XXXXX" in HD image file table 
            * after the entry is deleted in RAM table. 
            */
            fmUpdateHDImageFileTable(HD_IMAGE_FILE_TABLE, idx, UNUSED_ENTRY, 0);
        }

        /* remove this file from the hard disk */
        return (remove ( fileName ) );

}

/***********************************************************************
 *  Function:     fmNameCompare()
 *
 *  Description:  This function determines whether two names
 *                describe the same existing file.  Note that if the
 *                file named by either <name1> or <name2> does not
 *                exist, this function will return 0 (zero).
 *
 *  In Parms:     char *name1 - relative or absolute path
 *                char *name2 - absolute path
 *
 *  Out Parms:    None
 *
 *  Returns:      1 if <name1> and <name2> describe the same existing file,
 *                otherwise 0.
 ************************************************************************/
RET_t
fmNameCompare(char *name1, char *name2)
{
    char        dirName [MAX_FILENAME_LENGTH];
    char        name1Full [MAX_FILENAME_LENGTH];
    struct stat stat1;  /* stat for name1Full */
    struct stat stat2;  /* stat for name2 */

    if ((name1 == (char *)NULL) || (name2 == (char *)NULL))
    {
        return (0);
    }
    ioDefPathGet (dirName);     /* get the current working directory */

    return ((pathCat(dirName, name1, name1Full) == OK) &&
        (stat(name2, &stat2) == 0) && (stat(name1Full, &stat1) == 0) &&
        (stat2.st_ino == stat1.st_ino) && (stat2.st_dev == stat1.st_dev));
}


/****************************************************************************
 * Function:    Remove()
 * Descripion:  This routine is a wrapper of remove (). 
 *              If a file removed is an image file and existed in the image
 *              file Table, remove it from the table as well.
 * In Parms:    char    *name; 
 * Return Values:
 *              FM_OK   -  OK
 *              FM_ERROR  - otherwise
 */
RET_t
Remove(U8_t* fileName)
{
    U8_t refFileName[MAX_FILE_NAME_LENGTH];

    /* verify the file name */
    if (!VALID_FILE_NAME(fileName)) 
        return (FM_ILLEGAL_FILE_NAME);

    /* check if the file is a image kind of file  */ 
    if ( fmIsImageFile(fileName, refFileName) == TRUE) {
         
        /* check if it is a file existed in the right image file directory */
        if ( fmNameCompare( fileName, refFileName ) == 1) {

            
            FmDeleteLater (refFileName);
			return (FM_OK);
        }         
        /* if the file is a image file, but in the wrong dir. remove it */
        else return ( remove ( fileName ));
    }
    else 
	  return (remove ( fileName ));
}

/****************************************************************************
 * Function: Copy()
 * Description: This is a wrapper function for the copy() command in vxWorks.
 * In parms: char *srcFile             - name of source file
 *           char *dstFile             - name of destination file
 *           (copy from srcFile to dstFile)
 * Out parms: None
 * Return values: FM_OK
 *                FM_ERROR
 *                FM_FILE_DOES_NOT_EXIST
 */
RET_t
Copy(U8_t* srcFile, U8_t* dstFile)
{
  U8_t absFilename[MAX_FILENAME_LENGTH];
  BOOL_t isImageFile = FALSE;
  U32_t checksum = 0;
  char *suffix;
  
  SIW_FUNC_TRACE(FM, Copy, srcFile, dstFile, 0, 0, 0);

  /* first validate all the input parameters */
  if ((!VALID_FILE_NAME(srcFile)) || 
      (!VALID_FILE_NAME(dstFile))) 
    return(FM_ILLEGAL_FILE_NAME);


  /* copy using vxWorks */
  PINFO("Copy(): Copying to vxWorks\n");
  if (copy(srcFile, dstFile) == -1) {
    PERR("Copy(): %s does not exist\n", srcFile);
    return(FM_FILE_DOES_NOT_EXIST);
  }

  bzero (absFilename, MAX_FILENAME_LENGTH);
  isImageFile = fmBuildAbsFilename(dstFile, absFilename);
  PINFO("Copy(): Absolute file name is: %s\n", absFilename);

  /* for image files, check if dstFile is in correct dirpath */
  if (isImageFile == TRUE) {
    if (fmNameCompare(dstFile, absFilename) == 1) {
      if (fmCalculateChecksum(absFilename, &checksum) < 0) {
		PERR("Copy(): Calculate checksum failed.\n");
		return(FM_ERROR);
      }
      
      PINFO("Copy(): Updating image file table\n");
      /* Update RAM image file table */
      if (fmUpdateImageFileTable(absFilename, checksum) < 0)
	return(FM_ERROR);
      
      /* Update HD image file table */
      if (fmUpdateHDImageFileTable(HD_IMAGE_FILE_TABLE, 
				   fmGetImageFileIndex(absFilename), 
				   absFilename, checksum) < 0) 
	return(FM_ERROR);
    } 
  }

  return(FM_OK);
} 

/****************************************************************************
 * Function: Rename()
 * Description: This is a wrapper function for rename().
 * In parms: char *srcFile             - name of source file
 *           char *dstFile             - name of destination file
 * Out parms: None
 * Return values: FM_OK
 *                FM_ERROR
 *                FM_FILE_DOES_NOT_EXIST
 */
RET_t
Rename(U8_t* srcFile, U8_t* dstFile)
{
  U8_t absFilename[MAX_FILENAME_LENGTH];
  BOOL_t isImageFile = FALSE;
  U32_t checksum = 0;

  SIW_FUNC_TRACE(FM, Rename, srcFile, dstFile, 0, 0, 0);
  
  PINFO("*** Rename ***\n");

  /* first validate all the input parameters */
  if ((!VALID_FILE_NAME(srcFile)) || 
      (!VALID_FILE_NAME(dstFile))) 
    return(FM_ILLEGAL_FILE_NAME);

  /* rename using vxWorks */
  PINFO("    Renaming to vxWorks\n");
  if (rename(srcFile, dstFile) == -1) {
    PERR("Rename(): %s does not exist\n", srcFile);
    return(FM_FILE_DOES_NOT_EXIST);
  }
  
  isImageFile = fmBuildAbsFilename(dstFile, absFilename);

  PINFO("    Absolute file name is: %s\n", absFilename);

  /* for image files, check if dstFile is in correct dirpath */
  if (isImageFile == TRUE) {
    if (fmNameCompare(dstFile, absFilename) == 1) {
      if (fmCalculateChecksum(absFilename, &checksum) < 0) {
	PERR("Copy(): Calculate checksum failed.\n");
	return(FM_ERROR);
      }

      /* Update RAM image file table */
      if (fmUpdateImageFileTable(absFilename, checksum) < 0)
	return(FM_ERROR);
      
      /* Update HD image file table */
      if (fmUpdateHDImageFileTable(HD_IMAGE_FILE_TABLE, 
				   fmGetImageFileIndex(absFilename), 
				   absFilename, checksum) < 0) 
	return(FM_ERROR);
    } 
  }

  PINFO("*** End Rename ***\n");
  return(FM_OK);
}

/***********************************************************************
 * Helper Function: fmBuildAbsFilename() 
 * Description: Builds the absolute filename.
 * In parms: char *filename
 *           char *absFilename
 * Out parms: None
 * Return values: TRUE
 *                FALSE
 */
BOOL_t
fmBuildAbsFilename(U8_t *filename, U8_t* absFilename) 
{
  U8_t tempFilename[MAX_FILENAME_LENGTH];
  U8_t *tempPtr;
  U8_t BaseName[BASE_NAME_SIZE+1];
  U8_t Subscript[SUBSR_SIZE+1];
  BOOL_t isImageFile = FALSE;

  SIW_FUNC_TRACE(FM, fmBuildAbsFilename, filename, absFilename, 0, 0, 0);

  PINFO("fmBuildAbsFilename(): filename = %s\n", filename);

  bzero (tempFilename, MAX_FILENAME_LENGTH);  

  /* build the correct dirpath for file */
  tempPtr = strrchr(filename, '/');
    
  if (tempPtr == NULL) {        /* already at end of path */
    strcpy(tempFilename, filename);
    PINFO("    File name is: %s\n", tempFilename);
  }
  else {                      /* extract the file name */
    tempPtr++;
    strcpy(tempFilename, tempPtr);
    PINFO("    Extracted file name is: %s\n", tempFilename);
  }
  
  /* check if file is a Flash file */
  if ((tempFilename[DASH_LOC] == '_') && 
      (tempFilename[FIRST_DOT_LOC] == '.')) {
    bzero (BaseName, BASE_NAME_SIZE+1);
    bzero (Subscript, SUBSR_SIZE+1);
    
    strncpy (BaseName, &tempFilename[BASE_NAME_START], BASE_NAME_SIZE);     
    strncpy (Subscript, &tempFilename[SUBSR_START], SUBSR_SIZE);

    PINFO("    BaseName is: %s\n", BaseName);
    PINFO("    Subscript is: %s\n", Subscript);

	/* Flash files or upgrade image files */

    /* NSC file */
    if ((! (strcmp(BaseName, "nsc") && strcmp(BaseName, "NSC"))) &&
		((! (strcmp(Subscript, "fls"))) || (! (strcmp(Subscript, "fw"))))) {
      strcpy(absFilename, "C:/IMAGE/NSC/");
      strcat(absFilename, tempFilename);
      isImageFile = TRUE;
    }
    /* BSC file */
    if ((! (strcmp(BaseName, "bsc") && strcmp(BaseName, "BSC"))) &&
		((! (strcmp(Subscript, "fls"))) || (! (strcmp(Subscript, "fw"))))) {
      strcpy(absFilename, "C:/IMAGE/BSC/");
      strcat(absFilename, tempFilename);
      isImageFile = TRUE;
    }
    /* SCC file */
    if ((! (strcmp(BaseName, "scc") && strcmp(BaseName, "SCC"))) &&
		((! (strcmp(Subscript, "fls"))) || (! (strcmp(Subscript, "fw"))))) {
      strcpy(absFilename, "C:/IMAGE/SCC/");
      strcat(absFilename, tempFilename);
      isImageFile = TRUE;
    }	
  } else if (! (strcmp(tempFilename, "scc.fw"))) {
    strcpy(absFilename, "C:/IMAGE/SCC/");
    strcat(absFilename, tempFilename);
    isImageFile = TRUE;
  } else if (! (strcmp(tempFilename, "bsc.fw"))) {
    strcpy(absFilename, "C:/IMAGE/BSC/");
    strcat(absFilename, tempFilename);
    isImageFile = TRUE;
  } else if (! (strcmp(tempFilename, "nsc.fw"))) {
    strcpy(absFilename, "C:/IMAGE/NSC/");
    strcat(absFilename, tempFilename);
    isImageFile = TRUE;
  } else {
    strcpy(absFilename, tempFilename);
  }
    
  PINFO("    Absolute file name is: %s\n", absFilename);
  return(isImageFile);
}

/***********************************************************************
 * Helper Function: fmCalculateChecksum() 
 * Description: Calculates the checksum of a file.
 * In parms: char *file
 *           int *checksum
 * Out parms: None
 * Return values: FM_OK
 *                FM_ERROR
 */
RET_t
fmCalculateChecksum(U8_t *file, U32_t* checksum) 
{
  S32_t   fd, rst;
  char    buf[32];
  U32_t *tmpBuf;
  U32_t nLoop,i;
  U32_t tmpChecksum = 0;
  
  /* read the file and calculate the checksum. 
   * Then save it to RAM and HD ImageFile Table  
   */
  fd = open(file, O_RDWR, 0644);
  if (fd < 0) {
    FM_TRACE5(Copy, 0xF000001, fd, 0, 0, 0, 0);
    PERR("Copy: File Open Fail fd(%d) \n", fd);
    return (FM_CB_FILE_OPEN_FAIL);
  }

  /* read a record from file */
  while((rst = (S32_t)(read(fd,  buf, 32))) > 0) {
    tmpBuf = (U32_t *)(&buf);
    if ( (rst %4) != 0) {
      nLoop = rst/4 +1; 
      for (i = 0; i < nLoop;  i++, tmpBuf++) {
	if ( i == nLoop - 1) {
	  if ( rst % 4 == 1)  {
	    *tmpBuf = *tmpBuf & 0xff000000;
	  } 
	  else if ( rst % 4 == 2) {
	    *tmpBuf = *tmpBuf & 0xffff0000;
	  }
	  else if ( rst % 4 == 3) {
	    *tmpBuf = *tmpBuf & 0xffffff00;
	  } 
	}
	tmpChecksum += *tmpBuf;
      }
    }
    else {
      nLoop = rst/4;
      /* calculate the checksum */
      for (i = 0; i < nLoop;  i++, tmpBuf++) 
		tmpChecksum += *tmpBuf;
    }
  } /* while */
  *checksum = tmpChecksum;

  if (rst < 0) {
    FM_TRACE5(fmAddMiscFiles2ImageFileTb,0xF000003, fd, rst, 0,0, 0);
    PERR("fmAddMiscFiles2ImageFileTb: read Fail rst(%d) fd(%d) \n", rst, fd);
    close (fd);
    return(FM_ERROR);
  }

  /* reached end of file */
  if ( rst == 0) { 
    close (fd);
    return(FM_OK);
  }
  
  /* should not get here! */
  return(FM_ERROR);
}
#endif /* SCC */
