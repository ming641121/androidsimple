
#include "vxWorks.h"
#include "ioLib.h"
#include "mms_types.h"
#include "mms_if.h"
#include "sys_if.h"
#include "siw_if.h"
#include "fm_if.h"
#include "nwg.h"
#include "tftpLib.h"
#include "id_if.h"
#include "cfg_if.h"
#include "sm_if.h"
#include "cm_if.h"
#include "crm_if.h"
#include "dbm_if.h"
#include "tarLib.h"
#include "nwg_events.h"
#include "errno.h"
#include "nwg_if.h"
#include <net/if.h>
#include <netinet/in.h>
#include <inetLib.h>
#include <ctype.h>
#include "routeLib.h"
#include "arpLib.h"
#include "dia_if.h"
#include "snmp_errcode.h"  /* SNMP stuff */

#ifndef SIM

U32_t   nwgDbg = 0;
U32_t   nwgStep = 0;

#ifdef SCC

#define MAX_EVENT_FILES            20
#define EVENT_FILENAME             "C:/LOG/event"
#define LOG_EXTENSION              ".log"
#define OPEN_TRY_COUNT             3

STATUS_t updateflash (U32_t slot, U8_t *source);

/* Externs */
extern  CRM_t   crm;
extern  U16_t   GetSysEventSlot();
extern  U8_t    *mms_version;
extern RET_t    moveFile2Target(S32_t, char *, char *, FILE_TYPE_t);
void     scPswdDnld (U32_t slot, U8_t *source, U8_t *dest, FILE_TYPE_t type);
STATUS_t upfls (U32_t slot, U8_t *source);

extern STATUS_t NwgGetKey(U8_t* key);

extern RET_t Remove(U8_t* fileName );
extern RET_t Copy(U8_t* srcFile, U8_t* dstFile );
extern RET_t Rename(U8_t* srcFile, U8_t* dstFile );

extern char     dbbackupMsg[];
extern char     dbrestoreMsg[];

/* Function prototypes */
STATUS_t SetPassword (U8_t type, U8_t *password);
STATUS_t SetRoute (U8_t *ipaddr);
STATUS_t addroute (U8_t *dest, U8_t *gateway, S32_t mask, S32_t tos, S32_t
				   flags);
STATUS_t delroute (U8_t *dest, S32_t mask, S32_t tos, S32_t flags);
STATUS_t program_flash (S32_t fd, U32_t remaining, U32_t bankId, U32_t offset, 
                        U32_t blockLen, U32_t numBlocks, U32_t slot, U8_t *source);

#ifndef FLASH_BOOT
STATUS_t bulkupfls (U8_t *source)
{
    SIW_PRIORITY_t pri;
    SIW_TASKINFO_t info;
    STATUS_t       status;
    SIW_TID_t      tid = SIW_INVALID_TID;
    FILE_HDR_t     *FwImageHdr;
    U32_t          slot = 0;
    U32_t          imageType;
    U8_t           taskName [20];
    S32_t          fd;
    U8_t           *buf;

    if ( (fd = open (source, READ_ONLY, 0644)) != NULL)
    {
       buf = SiwAllocMem (SIW_STATIC, 400, WAIT_FOREVER);
 
       if (read (fd, buf, 400) == 400)
       {
          FwImageHdr = (FILE_HDR_t *) (buf+16);

          imageType = FwImageHdr->cardType;
          SiwFreeMem (buf);
          close (fd);
       }
       else
       {
          printf ("bulkupfls: error reading %s\n", source);
          SiwFreeMem (buf);
          close (fd);
          return (ERROR);
       }
    }
    else
    {
       printf ("bulkupfls: Cannot open file %s\n", source);
       return (ERROR);
    }


    for (slot = 0; slot < MAX_SLOTS; slot++)
    {
        pri.applId     = SIW_PLATFORM_SW;
        pri.prioGroup  = SIW_PRI_GRP_NORMAL;
        pri.priority   = DEFAULT_TASK_PRI;
        info.entryPt = (FUNC_PTR_t)upfls;
        info.stackSize = SIW_DEFAULT_STACK;
        info.arg1 = slot;
        info.arg2 = (S32_t)source;
        info.waitAck = 0;

        if (imageType == GetCardType(slot))
        {
           bzero (taskName, 20);
           sprintf (taskName, "tUpFls%d", slot);
           /* this task will be deleted by itself */
           tid = SiwTaskSpawn(taskName, &pri, &info,
                              SIW_RECV_LOG_ERROR,
                              SIW_RECV_LOG_ERROR,
                              NULL,
                              &status);

           if (tid == SIW_INVALID_TID )
           {
               SiwEventLog (NULL, SIW_EVENT_SYM(NWG, UPFLSFAIL), slot, source);
               printf ("bulkupfls: Cannot spawn flash update task %s\n", taskName);
           }

        SiwTaskDelay (10);

        }
     }
     return (OK);
 
} /* bulkupfls */

STATUS_t upfls (U32_t slot, U8_t *source)
{
   STATUS_t  ret;

   ret = updateflash (slot, source);
   SiwTaskDelete (SIW_TID_SELF);
   exit (ret);
   return (ret);
}
#endif

#if defined(SCC) && !defined(FLASH_BOOT)
STATUS_t PswdDnld (U32_t slot)
{
    SIW_PRIORITY_t pri;
    SIW_TASKINFO_t info;
    STATUS_t       status;
    SIW_TID_t      tid = SIW_INVALID_TID;
    U8_t           taskName[20];

    pri.applId     = SIW_PLATFORM_SW;
    pri.prioGroup  = SIW_PRI_GRP_NORMAL;
    pri.priority   = DEFAULT_TASK_PRI;
    info.entryPt = (FUNC_PTR_t)scPswdDnld;
    info.stackSize = SIW_DEFAULT_STACK;
    info.arg1 = (S32_t)slot;
    info.arg2 = (S32_t)PASSWORD_FILENAME;
    info.arg3 = (S32_t)"C:/pswd";
    info.arg4 = RAM;
    info.waitAck = 0;

    bzero (taskName, 20);
    sprintf (taskName, "tPswdDnld%d", slot);
    /* this task will be deleted by itself */
    tid = SiwTaskSpawn(taskName, &pri, &info,
                       SIW_RECV_LOG_ERROR,
                       SIW_RECV_LOG_ERROR,
                       NULL,
                       &status);

    if (tid == SIW_INVALID_TID )
    {
        printf ("pswd_dnld: Failed to spawn task\n");
        return (ERROR);
    }
	return (OK);
}

void scPswdDnld (U32_t slot, U8_t *source, U8_t *dest, FILE_TYPE_t type)
{
   taskDelay (2000);
   moveFile2Target (slot, source, dest, type);
   SiwTaskDelete (SIW_TID_SELF);
   exit (0);
}
#endif

STATUS_t updateflash (U32_t slot, U8_t *source)
{
    struct   stat   filestat;
    S32_t    fd;
    U8_t     *buf;
    RET_t    flash1_ret;
    U32_t    flash1_len;
    U32_t    flash2_len;
    U32_t    j;
#ifndef FLASH_BOOT
	RET_t    ret;
	U32_t    remaining;
    U32_t    i;
    U32_t    bankId;
    U32_t    blockLen;
    U32_t    numBlocks1;
    U32_t    numBlocks2;
    U32_t    offset;
#endif
    FILE_HDR_t   *FwImageHdr;

	if (GetCardState(slot) != CARD_REMOVED) {
	  if (stat (source, &filestat) == ERROR)
		return (UPDATEFLS_GET_FILE_STATS_FAILED);

	  printf ("Open file %s  --- length = %ld\n", source, filestat.st_size);
	  if ( (fd = open (source, READ_ONLY, 0644)) != NULL) 
	  {
		buf = SiwAllocMem (SIW_STATIC, filestat.st_size, WAIT_FOREVER);
		
		if (read (fd, buf, filestat.st_size) == (long)filestat.st_size)
		{
		  printf ("updateflash:  Buffer @ 0x%p   Size = %ld \n", buf, filestat.st_size);
			
		  FwImageHdr = (FILE_HDR_t *) (buf+16);
		  
		  if (FwImageHdr->cardType != GetCardType (slot))
		  {
			SiwFreeMem (buf);
			close (fd);
			PERR ("updateflash: Error image type [%d] and card mismatch [%d]\n",
				  FwImageHdr->cardType, GetCardType (slot));
			return (UPDATEFLS_FW_IMG_NOT_MATCH_CARD_TYPE);
		  }

		  for (j = 0; j < MAX_VERSION_LENGTH; j++)
		  {
			if ((FwImageHdr->version[j] == '\0') ||
				(FwImageHdr->version[j] == '_'))
			  break;
		  }
		  if ((j >= MAX_VERSION_LENGTH) || (FwImageHdr->version[j] == '\0')) 
		  {
			SiwFreeMem (buf);
			close (fd);
			PERR ("updateflash: Error file header version string is corrupt.\n");
			return (UPDATEFLS_CORRUPT_FILE_HDR);
		  }
		  if ((FwImageHdr->version[j + 1] != 'B') || (FwImageHdr->version[j + 2] != '_'))
		  {
			SiwFreeMem (buf);
			close (fd);
			PERR ("updateflash: Error image is a run time image not a flash image\n");
			return (UPDATEFLS_INVALID_FLASH_FILE);
		  }
		}
		else { /* read != size */
		  PERR ("updateflash: Failed to read file %s\n", source);
		  SiwFreeMem (buf);
		  return (UPDATEFLS_READ_FILE_FAILED);
		}
	  } else { /* open == NULL */
		PERR ("updateflash: Failed to open file %s\n", source);
		SiwFreeMem (buf);
		return (UPDATEFLS_OPEN_FILE_FAILED);
	  }

	  /* At this point we've already checked the cardType (look above) and 
	   * verified that it matches the slot type. But at this point this     
	   * can be either a flash image or a run time image for this card. We
	   * now have to differentiate between the two. We can't use size since 
	   * the two types have very similar file sizes for the NSC and BSC. We 
	   * can't use filenames as the user may have accidentally renamed the 
	   * file during upload (uploaded a run time image but named it as a 
	   * flash image). The only alternative left is to check the version
	   * string. The version string cannot be changed after the image is
	   * built, resides in the file header  and follows the following
	   * format:         
	   *       [XXX]_B_r[version] for boot images where XXX is the card 
	   *          identifier (NSC, SCC, BSC) and [version] is the revision.
	   *       [XXX]_r[version] for runtime images where XXX is the card 
	   *          identifier (NSC, SCC, BSC) and [version] is the revision.
	   * Thus we can check the 5th and 6th character for "B_" and if it
	   * equal then we know that this image isn't a valid boot image.                 */
	  
	  if ( ((slot == 8) || (slot == 9)) && ((S32_t)slot == SiwMyPhySlotNo()) )
	  {
		/* This is SCC slot */
		
		if (filestat.st_size > ONE_MEG_FLASH)
		{
		  flash1_len = ONE_MEG_FLASH;
		  flash2_len = filestat.st_size - ONE_MEG_FLASH;
		}
		else
		{
		  flash1_len = filestat.st_size;
		  flash2_len = 0;
		}
	  
		if (flash1_len != 0)
		{
		  if ( (flash1_ret=FlashStore ((VU16_t *)buf, flash1_len, 0)) !=
			   OK) 
			PERR ("updateflash: Failed to program flash device [0]\n");
		  else
			printf ("updateflash: Flash device [0] programmed successfully\n");
		  
		  if (flash2_len != 0)
		  {
			if ((FlashStore ((VU16_t *)buf+ONE_MEG_FLASH, flash2_len, 1)
				 != OK) && (flash1_ret == OK) )
			  PERR ("updateflash: Failed to program flash device [1]\n");
			else {
			  SiwEventLog (NULL, SIW_EVENT_SYM(NWG, UPFLS), slot, source);
			  printf ("updateflash: Flash device [1] programmed successfully\n");
			}
		  }
		}
	  } /* SCC slot */
	  
#ifndef FLASH_BOOT
	  else
	  {
		if ((GetCardState(slot) == CARD_ACTIVE) ||
			(GetCardState(slot) == CARD_STANDBY)) {
		  /* Close and re-open file */
		
		  close (fd);
		  if ( (fd = open (source, READ_ONLY, 0644)) != NULL) {
			blockLen = 512;
			
			remaining = filestat.st_size;
			i = 0;
			
			if (filestat.st_size > ONE_MEG_FLASH)
			{
			  flash1_len = ONE_MEG_FLASH;
			  flash2_len = filestat.st_size - ONE_MEG_FLASH;
			  numBlocks1 = flash1_len / blockLen;
			  numBlocks2 = flash2_len / blockLen;
			  if ((numBlocks2 * blockLen) < flash2_len)
				numBlocks2++;
			}
			else
			{
			  flash1_len = filestat.st_size;
			  flash2_len = 0;
			  numBlocks1 = filestat.st_size / blockLen;
			  if ((numBlocks1 * blockLen) < flash1_len)
				numBlocks1++;
			}
		  
			remaining = flash1_len;
			bankId = 0;
			offset = FLASH_BANK0_START;
			
			ret = program_flash (fd, flash1_len, bankId, offset, 
								 blockLen, numBlocks1, slot, source);			   
			if (ret == ERROR)
			  return (ERROR);
			
			if (flash2_len)
			{
			  remaining = flash2_len;
			  bankId = 1;
			  offset = FLASH_BANK1_START;
			  
			  ret = program_flash (fd, flash2_len, bankId, offset, 
								   blockLen, numBlocks2, slot, source);
			  if (ret == ERROR)
				return (ERROR);
			}
			
			SiwEventLog (NULL, SIW_EVENT_SYM(NWG, UPFLS), slot, source);
			printf ("updateflash: Successfully update flash for slot [%d]\n", slot);
			close (fd);
		  } /* Fail to open file */
		  else { 
			PERR ("updateflash: Failed to open file %s\n", source);
			SiwFreeMem (buf);
			return (UPDATEFLS_OPEN_FILE_FAILED);
		  }	  
		} else {
		  SiwFreeMem(buf);
		  PERR ("updateflash: Slot %d is not active/standby\n", slot);
		  return (UPDATEFLS_CARD_NOT_PRESENT);
		}
	  } /* !SCC */
#endif /* FLASH_BOOT */    
	} else {  /* CARD_REMOVED */
	  PERR ("updateflash: There is no card in slot [%d]\n", slot);
	  return (UPDATEFLS_CARD_NOT_PRESENT);
	}

	SiwFreeMem (buf);
    return (UPDATEFLS_SUCCESS);
} /* updateflash */

#ifndef FLASH_BOOT
STATUS_t program_flash (S32_t fd, U32_t remaining, U32_t bankId, U32_t offset, 
                        U32_t blockLen, U32_t numBlocks, U32_t slot, U8_t *source)
{
   U8_t     *buf;
   MSG_t    *reqPtr, *ReplyPtr, *eraseReqPtr;
   NWG_FLASH_UPDATE_t       *UpdatePtr;
   NWG_FLASH_UPDATE_ACK_t   *UpdateAckPtr;
   NWG_FLASH_ERASE_t        *ErasePtr;
   NWG_FLASH_ERASE_ACK_t    *EraseAckPtr;
   U32_t    toRead;
   U32_t    i = 0;
   U32_t    j = 0;
   FILE_HDR_t   *FwImageHdr;
   RET_t    ret;

   printf ("updateflash: n[%d] l[%d] bl[%d] o[%x]\n", numBlocks,
                           remaining, blockLen, offset);
   do 
   {
      if (remaining > blockLen)
         toRead = blockLen;
      else 
         toRead = remaining;

      reqPtr = (MSG_t *) SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));
      if (reqPtr == NULL)
      {
         printf ("program_flash: Failed to allocate SAR memory\n");
         return (ERROR);
      }

      if (read (fd, &reqPtr->data[NWG_FLASH_UPDATE_HDR], toRead) == (S32_t)toRead)
      {
         if (i == 0)
         {
            if (bankId == 0)
            {
               FwImageHdr = (FILE_HDR_t *) &reqPtr->data[NWG_FLASH_UPDATE_HDR+16]; 
               if (FwImageHdr->cardType != GetCardType (slot))
               {
                  SiwFreeMem (reqPtr);
                  close (fd);
                  printf ("program_flash: Error image type [%d] and card mismatch [%d]\n",
                                        FwImageHdr->cardType, GetCardType (slot));
                  return (ERROR);
               }
               /* At this point we've already checked the cardType (look above) and verified */
               /* that it matches the slot type. This means that this image is a valid image */
               /* and is assigned to this slot type. We have thus filtered out all non image */
               /* files and image files for the other card types. But at this point this     */
               /* can be either a flash image or a run time image for this card. We now have */
               /* to differentiate between the two. We can't use size since the two types    */
               /* have very similar file sizes for the NSC and BSC. We can't use filenames   */
               /* as the user may have accidentally renamed the file during upload (uploaded */
               /* a run time image but named it as a flash image). The only alternative left */
               /* is to check the version string. The version string cannot be changed after */
               /* the image is built, resides in the file header  and follows the following  */
               /* format:                                                                    */
               /* [XXX]_B_r[version] for boot images where XXX is the card identifier (NSC,  */
               /*         SCC, BSC) and [version] is the revision.                           */
               /* [XXX]_r[version] for runtime images where XXX is the card identifier (NSC, */
               /*         SCC, BSC) and [version] is the revision.                           */
               /* Thus we can check the 5th and 6th character for "B_" and if it equal then  */
               /* we know that this image isn't a valid boot image.                          */
               for (j = 0; j < MAX_VERSION_LENGTH; j++)
               {
                   if ((FwImageHdr->version[j] == '\0') ||
                       (FwImageHdr->version[j] == '_'))
                       break;
               }
               if ((j >= MAX_VERSION_LENGTH) || (FwImageHdr->version[j] == '\0')) 
               {
                   SiwFreeMem (buf);
                   close (fd);
                   printf ("updateflash: Error file header version string is corrupt.\n");
                   return (ERROR);
               }
               if ((FwImageHdr->version[j + 1] != 'B') || (FwImageHdr->version[j + 2] != '_'))
               {
                  SiwFreeMem (reqPtr);
                  close (fd);
                  printf ("program_flash: Error image is a run time image not a flash image\n");
                  return (ERROR);
               }
            }

            /* Erase flash */
            eraseReqPtr = (MSG_t *) SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));
            if (eraseReqPtr == NULL)
            {
               printf ("program_flash: Failed to allocate SAR memory\n");
               close (fd);
               return (ERROR);
            }
            ErasePtr = (NWG_FLASH_ERASE_t *) &eraseReqPtr->data[0];
            ErasePtr->BankId = bankId;
            SIW_SARBUFLEN_SET (eraseReqPtr, sizeof(NWG_FLASH_ERASE_t));

            ret = MrpcSync ((U32_t)slot, NWG_MOD, NWG_FLASH_ERASE, eraseReqPtr,
                                         LO, WAIT_FOREVER, &ReplyPtr);

            if (ret == MRPC_OK)
            {
               EraseAckPtr = (NWG_FLASH_ERASE_ACK_t *) &(ReplyPtr->data[0]);
               if (EraseAckPtr->retStatus == OK)
               {
                  printf ("program_flash: Successful erase flash to slot [%d]\n", slot);
               }
               else
               {
                  printf ("program_flash: Failed erase flash to slot [%d]\n",
                                        slot);
                  SiwEventLog (NULL, SIW_EVENT_SYM(NWG, UPFLSFAIL), slot, source);
                  close (fd);
                  SiwFreeMem (ReplyPtr);
                  return (ERROR);
               }
            }
            else
            {
               printf ("program_flash: Failed MRPC call to erase flash\n");
               SiwEventLog (NULL, SIW_EVENT_SYM(NWG, UPFLSFAIL), slot, source);
               close (fd);
               return (ERROR);
            }
            SiwFreeMem (ReplyPtr);
        }

        SIW_SARBUFLEN_SET (reqPtr, (sizeof(NWG_FLASH_UPDATE_t)+toRead));

        UpdatePtr = (NWG_FLASH_UPDATE_t *) &reqPtr->data[0];
        UpdatePtr->Slot = slot;
        UpdatePtr->Length = toRead;
        UpdatePtr->Offset = offset;
        UpdatePtr->BankId = bankId;

        if (nwgDbg)
           printf ("program_flash: program flash s[%d] b[%d] l[%d] r[%d] R[%x] P[%x]\n", slot, i, toRead, remaining, (unsigned int)reqPtr, (unsigned int)UpdatePtr);
                   
        ret = MrpcSync ((U32_t)slot, NWG_MOD, NWG_FLASH_UPDATE, reqPtr,
                                     HI, WAIT_FOREVER, &ReplyPtr);

        if (ret == MRPC_OK)
        {
           UpdateAckPtr = (NWG_FLASH_UPDATE_ACK_t *) &(ReplyPtr->data[0]);
           if (UpdateAckPtr->retStatus != OK)
           {
              SiwEventLog (NULL, SIW_EVENT_SYM(NWG, UPFLSFAIL), slot, source);
              printf ("program_flash: Failed flash update to slot [%d]\n",
                                    slot);
              SiwFreeMem (ReplyPtr);
              close (fd);
              return (ERROR);
           }
           SiwFreeMem (ReplyPtr);
        }
        else
        {
           SiwEventLog (NULL, SIW_EVENT_SYM(NWG, UPFLSFAIL), slot, source);
           printf ("program_flash: failed MRPC call to update flash\n");
           close (fd);
           return (ERROR);
        }
     } /* if (read (....) ) */
     else
     {
        SiwFreeMem (reqPtr);
        close (fd);
        printf ("program_flash: failed to read %s\n", source);
        return (ERROR);
     }

     i++;
     remaining -= toRead;
     offset += toRead;

   } while (i < numBlocks);
   return (OK);
}
#endif

STATUS_t dbbackup ()
{
#ifndef FLASH_BOOT
   char     fileName[256];
   char     backUpFileName[256];
   char     ver[256];
   int      i;
   STATUS_t ret;
   U32_t checksum = 0;

   if ( GetCardState (SiwMyPhySlotNo()) != CARD_ACTIVE )
   {
      printf ("dbbackup: This command can ONLY be executed on ACTIVE SCC\n");

      sprintf ( dbbackupMsg, 
                "This command can ONLY be executed on ACTIVE SCC\n");

      return (ERROR);
   }

   bzero (fileName, 256);
   bzero (backUpFileName, 256);
   bzero (ver, 256);
   strcpy (ver, mms_version);
   for (i = 0; i < (int)(strlen(ver)); i++)
       ver[i] = (char)tolower((int)ver[i]);

   sprintf (fileName, "C:/IMAGE/%s.cfg", ver);
   
   /* rename the existing config file to a backup copy */ 
   sprintf (backUpFileName, "%s.BACK", fileName);
   rename (fileName, backUpFileName);

   /* Temporarily copy IMAGE cfg files to CONFIG directory */
   Copy("C:/IMAGE/misc/ifType.cfg", "C:/CONFIG/ifType.cfg.tmp");
   Copy("C:/IMAGE/misc/mgcpIpdc.cfg", "C:/CONFIG/mgcpIpdc.cfg.tmp");
   
   /* Tar entire CONFIG dir */
   ret = (STATUS_t) tarArchive (fileName, 1, 0, "C:/CONFIG");

   if (fmCalculateChecksum(fileName, &checksum) < 0) {
      PERR("dbbackup(): Calculate checksum failed.\n");
      return(ERROR);
   }
   /* Update RAM image file table */
   if (fmUpdateImageFileTable(fileName, checksum) < 0) {
       PERR("dbbackup(): fmUpdateImageFileTable failed.\n");
       return(ERROR);
   }
   /* Update HD image file table */
   if (fmUpdateHDImageFileTable(HD_IMAGE_FILE_TABLE,
                                 fmGetImageFileIndex(fileName),
                                 fileName, checksum) < 0) {
       PERR("dbbackup(): fmUpdateHDImageFileTable failed.\n");
       return(ERROR);
   }

   if (ret == OK)
   {
       S32_t this, peer;

       /* get Peer card Slot Number */
       this = SiwMyPhySlotNo();
       if (!VALID_SCC_SLOT(this))
           return (ERROR);
       peer = this ^ 1;
       if (GetCardState(peer) == CARD_STANDBY) 
       {
           ret = moveFile2Target(peer, fileName, fileName, HD);
           if (ret < 0) {
               printf ("dbbackup: Failed to move file %s to card %d\n",
                                   fileName, peer);

               sprintf ( dbbackupMsg, 
                         "Failed to move file %s to card %d\n",
                         fileName, 
                         peer);

               return (ERROR);
           }
       }
      printf ("dbbackup: Successfully back-up configuration file [%s]\n", fileName);


      sprintf ( dbbackupMsg, 
                "Successfully back-up configuration file:\n [%s]\n",
                fileName);

      SiwEventLog (NULL, SIW_EVENT_SYM(NWG, CFGBKUP), fileName);
   }

   /* Remove the temp IMAGE cfg files from CONFIG directory */
   remove ("C:/CONFIG/ifType.cfg.tmp");
   remove ("C:/CONFIG/mgcpIpdc.cfg.tmp");

   return (OK);
#endif
   return (OK);
}

STATUS_t dbrestore (char *version)
{
/* Modification note: MGX8260 uses 0-based slot number intenally, but
 * cli uses 1-based slot for customer. Up on a request from customer
 * the printf in this function will display 1-based slot number
 */ 
#ifndef FLASH_BOOT
   NWG_DB_RESTORE_t       *UpdatePtr;
   NWG_DB_RESTORE_ACK_t   *UpdateAckPtr;
   char     fileName[256];
   char     userVers[256];
   char     mmsVers[256];
   U32_t    slot;
   MSG_t    *reqPtr, *ReplyPtr;
   RET_t    ret;
   int      i;
   struct   stat   filestat;

   if ( GetCardState (SiwMyPhySlotNo()) != CARD_ACTIVE )
   {
      printf ("dbrestore: This command can ONLY be executed on ACTIVE SCC\n");

      sprintf ( dbrestoreMsg, 
                "This command can ONLY be executed on ACTIVE SCC\n");

      return (ERROR);
   }
             
   if (version == NULL)
   {
      printf ("dbrestore: Invalid version { dbrestore [\"3-digit version\"] }\n");

      sprintf ( dbrestoreMsg, 
                "Invalid version { dbrestore [\"3-digit version\"] }\n");

      return (ERROR);
   }

   bzero(userVers,0);
   bzero(mmsVers,0);
   strcpy(userVers,version);
   strcpy(mmsVers,mms_version);

   for (i = 0; i < (int)(strlen(userVers)); i++)
       userVers[i] = (char)tolower((int)userVers[i]);

   for (i = 0; i < (int)(strlen(mmsVers)); i++)
       mmsVers[i] = (char)tolower((int)mmsVers[i]);

   if (strcmp(mmsVers, userVers) !=0) {
      printf("dbrestore: Invalide version %s, the existing backup version is %s.
              The restore operation is rejected \n", userVers, mmsVers);

      sprintf ( dbrestoreMsg, 
                "Invalid configuration %s,\n the existing backup configuration is\n %s.cfg \n The restore operation is rejected \n", 
                userVers, 
                mmsVers);

      return (ERROR);
   }

   slot = SiwStandbySlotNo ();
   bzero (fileName, 256);
   for (i = 0; i < (int)(strlen(version)); i++)
       version[i] = (char)tolower((int)version[i]);
   sprintf (fileName, "C:/IMAGE/%s.cfg", version);

   if (stat (fileName, &filestat) == ERROR)
   {
      printf ("dbrestore: Error [%d] operate on file [%s]\n", errno, fileName);

      sprintf ( dbrestoreMsg,
                "dbrestore: Error [%d] operate on file [%s]\n", 
                errno, 
                fileName);

      return (ERROR);
   }

   dbmBlockDbmApis();
   rmDir ("C:/CONFIG.BKUP");
   if ( rename ("C:/CONFIG","C:/CONFIG.BKUP") == ERROR)
   {
      dbmUnblockDbmApis();
      printf ("dbrestore: Could not backup the CONFIG directory\n");

      sprintf ( dbrestoreMsg,
                "Could not backup the CONFIG directory\n");

      return (ERROR);
   }
   else
   {
      if ( tarExtract (fileName, 1, 0) == ERROR )
      {
         rmDir ("C:/CONFIG");
         rename ("C:/CONFIG.BKUP", "C:/CONFIG");
         printf ("dbrestore: Could not un-tar file %s\n", fileName);

         sprintf ( dbrestoreMsg,
                   "Could not un-tar file %s\n", 
                   fileName);
 
         dbmUnblockDbmApis();
         return (ERROR);
      }
      else
      {
         dbmUnblockDbmApis();
         if (rmDir ("C:/CONFIG.BKUP") == ERROR)
         {
            printf ("dbrestore: Could not remove CONFIG.BKUP directory\n");

            sprintf ( dbrestoreMsg,
                      "Could not remove CONFIG.BKUP directory\n");

            return (ERROR);
         }

         printf ("dbrestore:  DB has been successfully restored to ACTIVE SCC\n");

	 /* Move IMAGE cfg files to IMAGE dir */
	 Rename ("C:/CONFIG/ifType.cfg.tmp", "C:/IMAGE/misc/ifType.cfg");
	 Rename ("C:/CONFIG/mgcpIpdc.cfg.tmp", "C:/IMAGE/misc/mgcpIpdc.cfg");

	 /* if Standby card exists, move cfg files to standby card */
         if ( (GetCardState(slot) != CARD_REMOVED) )
         {
            printf ("Open file %s\n", fileName);
            if ((moveFile2Target (slot, fileName, fileName, HD) == ERROR))
            {
               printf ("dbrestore: Failed to download DB file to slot %d\n",
                                     slot+1);


               return (ERROR);
            }

            reqPtr = (MSG_t *) SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));
          
            if (reqPtr == NULL)
            {
               printf ("dbrestore: Failed to allocate SAR memory\n");

               sprintf ( dbrestoreMsg,
                         "dbrestore: Failed to allocate SAR memory\n");

               return (ERROR);
            }

            UpdatePtr = (NWG_DB_RESTORE_t *) &reqPtr->data[0];

            SIW_SARBUFLEN_SET (reqPtr, sizeof (NWG_DB_RESTORE_t));

            UpdatePtr->Slot = slot;
            strncpy (UpdatePtr->FileName, fileName, MAX_VERSION_LENGTH);

            ret = MrpcSync ((U32_t)slot, NWG_MOD, NWG_DB_RESTORE, reqPtr,
                            LO, WAIT_FOREVER, &ReplyPtr);

            if (ret == MRPC_OK)
            {
               UpdateAckPtr = (NWG_DB_RESTORE_ACK_t *) &(ReplyPtr->data[0]);
               if (UpdateAckPtr->retStatus == OK)
               {
                  printf ("dbrestore: Successful restore DB to slot [%d]\n",
                                        slot+1);
                  SiwFreeMem (ReplyPtr);
               }
               else
               {
                  printf ("dbrestore: Failed restoring DB to slot [%d]\n",
                                        slot+1);
                  SiwFreeMem (ReplyPtr);
                  return (ERROR);
               }
            }
            else
               printf ("dbrestore: Failed restoring DB to slot [%d] rc[%d]\n",
                                     slot+1, ret);
         }
      }
   }   

   printf ("dbrestore: Restored configuration from [%s]\n", fileName);
   SiwEventLog (NULL, SIW_EVENT_SYM(NWG, CFGRST), fileName);
   printf ("Node will now be reset to apply new configuration\n");

   sprintf ( dbrestoreMsg,
             "Restored configuration from \n [%s].\n Node will now be reset to apply new configuration.\n",
             fileName);
   for (i = 0; i < 10000; i++)
     ;
   SccResetNode ();

   return (OK);
#endif
   return (OK);
}

STATUS_t RemDbRestore (IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
#ifndef FLASH_BOOT
   NWG_DB_RESTORE_t             *ReqPtr;
   NWG_DB_RESTORE_ACK_t         *ReplyPtr;
   RET_t                        retCode = OK;

   ReqPtr = (NWG_DB_RESTORE_t *) &inMsg->data[0];
   printf ("RemDbRestore: Received [%s] [s=%d]\n", ReqPtr->FileName, ReqPtr->Slot);

   if ( (ReqPtr->Slot == SiwMyPhySlotNo()) &&
        (ReqPtr->FileName != NULL) )
   {
      rmDir ("C:/CONFIG.BKUP");
      if ( rename ("C:/CONFIG","C:/CONFIG.BKUP") == ERROR)
      {
         printf ("RemDbRetore: Could not backup the CONFIG directory\n");
         retCode = ERROR;
      }
#if 0
      else
      {
         if (taskPrioritySet (taskIdSelf(), 50) == ERROR)
         {
            printf ("RemDbRestore: Failed to raise task priority\n");
            retCode == ERROR;
         }
         else
         {
           if ( tarExtract (ReqPtr->FileName, 1, 0) == ERROR )
           {
              printf ("RemDbRetore: Could not un-tar file %s\n", ReqPtr->FileName);
              retCode = ERROR;
           }
           else
           {
              printf ("RemDbRestore: config successfully restored\n");
              if (rmDir ("C:/CONFIG.BKUP") == ERROR)
              {
                 printf ("RemDbRetore: Could not remove CONFIG.BKUP directory\n");
                 retCode = ERROR;
              }
            }
         }
      }
#endif
   }
   else
   {
      printf ("RemDbRestore:  Invalid parameters [%s] [%d]\n", ReqPtr->FileName,
                              ReqPtr->Slot);
      retCode = ERROR;
   }
   
   printf ("RemDbRestore: successfully remove the CONFIG directory on STANDBY SCC\n");
   *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));
   SIW_SARBUFLEN_SET(*outMsg, sizeof(U32_t));
   ReplyPtr = (NWG_DB_RESTORE_ACK_t *) &(*outMsg)->data[0];
   ReplyPtr->retStatus = retCode;

   SiwFreeMem (inMsg);
   return (OK);
#endif
   return (OK);
}

#ifndef FLASH_BOOT
STATUS_t cnfkey (U8_t *key)
{
    S32_t   len;
    NWG_UPDATE_KEY_t       *UpdatePtr;
    NWG_UPDATE_KEY_ACK_t   *UpdateAckPtr;
    MSG_t    *reqPtr, *ReplyPtr;
    RET_t    ret;
    U32_t    slot;
    int      i;

    if (key == NULL) {
       printf(" Need another parameter \n");
       printf(" Usage: chkey key (size of key must be 6)\n");
       return (ERROR);
    }

    len = strlen (key);
    
    for (i = 0; i < TFTP_KEY_SIZE; i++)
    {
         if (key[i] == 0x20)
         {
             printf(" It is not allowed to put space inside of the key\n");
             return (ERROR);
         }
    }

    for (i = TFTP_KEY_SIZE; i < len; i++)
    {
         if (key[i] != 0x20)
         {
             printf("Usage: chkey key (size of key must be 6)\n");
             return (ERROR);
         }
    }
    
    key[TFTP_KEY_SIZE] = '\0';

    for (i = 0; i < (int)(strlen(key)); i++)
       key[i] = (char)tolower((int)key[i]);

    if ( SysSetNovramInfo (TFTP_KEY, key) == ERROR )
       return (ERROR);

    CrmSetTFTPKey (key);

    slot = SiwStandbySlotNo ();
    if ( (GetCardState(slot) != CARD_REMOVED) )
    {
       reqPtr = (MSG_t *) SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

       if (reqPtr == NULL)
       {
           printf ("cnfkey: Failed to allocate SAR memory\n");
           return (ERROR);
       }

       UpdatePtr = (NWG_UPDATE_KEY_t *) &reqPtr->data[0];
       SIW_SARBUFLEN_SET (reqPtr, sizeof (NWG_UPDATE_KEY_t));
       strncpy (UpdatePtr->Key, key, TFTP_KEY_SIZE);
       ret = MrpcSync ((U32_t)slot, NWG_MOD, NWG_UPDATE_KEY, reqPtr,
                                  LO, WAIT_FOREVER, &ReplyPtr);

       if (ret == MRPC_OK)
       {
          UpdateAckPtr = (NWG_UPDATE_KEY_ACK_t *) &(ReplyPtr->data[0]);
          if (UpdateAckPtr->retStatus == OK)
          {
             printf ("cnfkey: Successful set TFTP key to standby slot\n");
             SiwFreeMem (ReplyPtr);
          }
          else
          {
             printf ("cnfkey: Failed set TFTP key to standby slot\n");
             SiwFreeMem (ReplyPtr);
             return (ERROR);
          }
       }
    }

    return (OK);
}

STATUS_t dspkey ()
{
   U8_t  key[10];

   strncpy (key, (char *)(&crm.this.cTftpKey), TFTP_KEY_SIZE);
   printf ("TFTP key = [%s]\n", key);
   return (OK);
}

STATUS_t 
NwgGetKey(U8_t* key)
{
    strncpy ( key, (char *)(&crm.this.cTftpKey), TFTP_KEY_SIZE);
    return (OK);
}


STATUS_t RemUpdateKey (IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    NWG_UPDATE_KEY_t          *ReqPtr;
    NWG_UPDATE_KEY_ACK_t      *ReplyPtr;

    ReqPtr = (NWG_UPDATE_KEY_t *) &inMsg->data[0];
    CrmSetTFTPKey (ReqPtr->Key);
    *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

    SIW_SARBUFLEN_SET(*outMsg, sizeof(U32_t));
    ReplyPtr = (NWG_UPDATE_KEY_ACK_t *) &(*outMsg)->data[0];
    ReplyPtr->retStatus = OK;
    SiwFreeMem (inMsg);
    return (OK);
}

STATUS_t RemUpdatePassword (IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    NWG_UPDATE_PSWD_t          *ReqPtr;
    NWG_UPDATE_PSWD_ACK_t      *ReplyPtr;

    ReqPtr = (NWG_UPDATE_PSWD_t *) &inMsg->data[0];
    SetPassword (ReqPtr->Type, ReqPtr->Password);
    *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

    SIW_SARBUFLEN_SET(*outMsg, sizeof(U32_t));
    ReplyPtr = (NWG_UPDATE_PSWD_ACK_t *) &(*outMsg)->data[0];
    ReplyPtr->retStatus = OK;
    SiwFreeMem (inMsg);
    return (OK);
}

STATUS_t RemUpdateRoute (IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    NWG_UPDATE_ROUTE_t          *ReqPtr;
    NWG_UPDATE_ROUTE_ACK_t      *ReplyPtr;

    ReqPtr = (NWG_UPDATE_ROUTE_t *) &inMsg->data[0];
    SetRoute (ReqPtr->ipAddr);
    *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

    SIW_SARBUFLEN_SET(*outMsg, sizeof(U32_t));
    ReplyPtr = (NWG_UPDATE_ROUTE_ACK_t *) &(*outMsg)->data[0];
    ReplyPtr->retStatus = OK;
    SiwFreeMem (inMsg);
    return (OK);
}

STATUS_t RemAddRoute (IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    STATIC_ROUTE_t          *ReqPtr;
    NWG_UPDATE_ROUTE_ACK_t      *ReplyPtr;

    ReqPtr = (STATIC_ROUTE_t *) &inMsg->data[0];
    addroute (ReqPtr->destAddr, ReqPtr->gatewayAddr, ReqPtr->mask, ReqPtr->tos, ReqPtr->flags);
    *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

    SIW_SARBUFLEN_SET(*outMsg, sizeof(U32_t));
    ReplyPtr = (NWG_UPDATE_ROUTE_ACK_t *) &(*outMsg)->data[0];
    ReplyPtr->retStatus = OK;
    SiwFreeMem (inMsg);
    return (OK);
}

STATUS_t RemDelRoute (IN MSG_t *inMsg, OUT MSG_t **outMsg)
{
    NWG_DELETE_STATIC_ROUTE_t          *ReqPtr;
    NWG_UPDATE_ROUTE_ACK_t      *ReplyPtr;

    ReqPtr = (NWG_DELETE_STATIC_ROUTE_t *) &inMsg->data[0];
    delroute (ReqPtr->destAddr, ReqPtr->mask, ReqPtr->tos, ReqPtr->flags);
    *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

    SIW_SARBUFLEN_SET(*outMsg, sizeof(U32_t));
    ReplyPtr = (NWG_UPDATE_ROUTE_ACK_t *) &(*outMsg)->data[0];
    ReplyPtr->retStatus = OK;
    SiwFreeMem (inMsg);
    return (OK);
}
#endif /* !FLASH_BOOT */

void
dumpevt(char *outFile)
{  
    int  i, offset, slot, tryCnt;
    int  fd;
    int  outfd;
    char outStr[255];
    int  size = sizeof(EVENT_LOG_ENTRY_t);
    char fname[25];
    EVENT_LOG_ENTRY_t event;

    outfd = open (outFile, O_RDWR | O_CREAT, 0644);

    for (i=0, slot=(int)GetSysEventSlot(); i<MAX_EVENT_FILES; i++, slot--)
    {
        sprintf(fname, "%s%02d%s", EVENT_FILENAME, slot,
                LOG_EXTENSION);
        if (slot == 0)
        {
            slot = MAX_EVENT_FILES;
        }
    
        /* request the Semaphore to access the Event Log File */
        SiwSemTake(SEM_EVENTLOG, WAIT_FOREVER);

        tryCnt = 0;
        while (tryCnt++ < OPEN_TRY_COUNT)
        /* file system is not reliable, try 3 times */
        {
            if ( (fd = open(fname, O_RDONLY, 0644)) != ERROR)
                break;
        }
        if (fd == ERROR)
        {
             /* release the Semaphore for accessing to the Event Log File */ 
             SiwSemGive(SEM_EVENTLOG); 
             continue;
        }

        /* read from the end of the file */
        offset = lseek(fd, 0, SEEK_END);

        /* release the Semaphore for accessing to the Event Log File */ 
        SiwSemGive(SEM_EVENTLOG); 

        /* print out Event Log */
        while (offset >  0)
        {
            offset -= size;
            offset = lseek(fd, offset, SEEK_SET);
            read(fd, (char *)&event, size);  
            sprintf(outStr, "%s\n %s\n", &event.datestamp[0], event.message);
            write (outfd, outStr, strlen (outStr));
        }
        close (fd);
    }
    close (outfd);
}

#endif /* SCC */

#if defined(SCC) && !defined(FLASH_BOOT)
STATUS_t SetPassword (U8_t type, U8_t *password)
{
    struct  stat            filestat;
    PASSWORD_t              passfile;
    FD_t                    *fd;
    NWG_UPDATE_PSWD_t       *UpdatePtr;
    NWG_UPDATE_PSWD_ACK_t   *UpdateAckPtr;
    MSG_t                   *reqPtr, *ReplyPtr;
    RET_t                   ret;
    U32_t                   slot;
    U32_t                   i;

    if ((type != ROOT_USER) && (type != SUPER_USER))
    {
       printf ("SetPassword: password type must be either 1 or 2\n");
       return (ERROR);
    }
    if (password == NULL)
    {
       printf ("SetPassword: password parameter is (NULL)\n");
       return (ERROR);
    } 
    if (strlen(password) >= PASSWORD_SIZE)
    {
       printf ("SetPassword: password length must be 10 characters long\n");
       return (ERROR);
    }

    bzero ((void *)(&passfile), sizeof (PASSWORD_t));
    
    if (stat (PASSWORD_FILENAME, &filestat) == ERROR)
    {  /* This file does not exist */
       fd = FmOpen (PASSWORD_FILENAME, O_CREAT|O_RDWR);
       if (fd == NULL)
       {
          printf ("SetPassword: Can not open file %s\n", PASSWORD_FILENAME);
          return (ERROR);
       }
       strncpy ((char *)(&passfile.root), "cisco",PASSWORD_SIZE);
       strncpy ((char *)(&passfile.super),"cisco",PASSWORD_SIZE);
       FmClose (fd);
    }
    else
    {
       fd = FmOpen (PASSWORD_FILENAME, O_RDWR); 
       if (fd == NULL)
       {
          printf ("SetPassword: Can not open file %s\n", PASSWORD_FILENAME);
          return (ERROR);
       }
       FmRead (fd, (char *)(&passfile), 0, sizeof(PASSWORD_t));
       FmClose (fd);
    }

    switch (type)
    {
       case ROOT_USER:
          strncpy ((char *)(&passfile.root), password, PASSWORD_SIZE);
          break;
       case SUPER_USER:
          strncpy ((char *)(&passfile.super), password, PASSWORD_SIZE);
          break;
       default:
          printf ("SetPassword: Incorrect password type[%d]\n", type);
          return (ERROR);
          break;
    }

    fd = FmOpen (PASSWORD_FILENAME, O_RDWR);
    if (fd == NULL)
    {
       printf ("SetPassword: Can not open file %s\n", PASSWORD_FILENAME);
       return (ERROR);
    }
    FmWrite (fd, (char *)(&passfile), 0, sizeof(PASSWORD_t), TRUE);
    FmClose (fd);


    /* If this is the active SCC, then find out if there is a standby card */
    /* If there is one, set the update over to update the password file on */
    /* the standby                                                         */
    if (GetCardState(SiwMyPhySlotNo()) == CARD_ACTIVE)
    {
       slot = SiwStandbySlotNo ();
       if ( (GetCardState(slot) != CARD_REMOVED) )
       {
          reqPtr = (MSG_t *) SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

          if (reqPtr == NULL)
          {
              printf ("SetPassword: Failed to allocate SAR memory\n");
              return (ERROR);
          }

          UpdatePtr = (NWG_UPDATE_PSWD_t *) &reqPtr->data[0];
          SIW_SARBUFLEN_SET (reqPtr, sizeof (NWG_UPDATE_PSWD_t));
          UpdatePtr->Type = type;
          strncpy (UpdatePtr->Password, password, PASSWORD_SIZE);
          ret = MrpcSync ((U32_t)slot, NWG_MOD, NWG_UPDATE_PSWD, reqPtr,
                                     LO, WAIT_FOREVER, &ReplyPtr);

          if (ret == MRPC_OK)
          {
             UpdateAckPtr = (NWG_UPDATE_PSWD_ACK_t *) &(ReplyPtr->data[0]);
             if (UpdateAckPtr->retStatus == OK)
             {
                printf ("SetPassword: Successful set password to standby slot\n");
                SiwFreeMem (ReplyPtr);
             }
             else
             {
                printf ("SetPassword: Failed set password to standby slot\n");
                SiwFreeMem (ReplyPtr);
                return (ERROR);
             }
          }
       }
    }

    for (i = 0; i < 16; i++)
    {
       if ( (GetCardType(i) != CARD_SCC) &&
            ( (GetCardState(i) == CARD_ACTIVE) || 
              (GetCardState(i) == CARD_STANDBY)) )
       {
          ret = moveFile2Target(i, PASSWORD_FILENAME, "C:/pswd", RAM);
          if (ret < 0) {
             printf ("dbmDumpDb2Sc: Failed to move file %s to card %d\n",
                                PASSWORD_FILENAME, i);
          }
       }
    }

    return (OK);
}

STATUS_t GetRoute (U8_t *ipAddr)
{
    FD_t                    *fd;
    ROUTE_t                 routeEntry;

    fd = FmOpen (ROUTE_FILENAME, O_RDONLY); 
    if (fd == NULL)
    {
       printf ("GetRoute: Can not open file %s\n", ROUTE_FILENAME);
       return (ERROR);
    }
    FmRead (fd, (char *)(&routeEntry), 0, sizeof(ROUTE_t));
    strncpy (ipAddr, (char *)(&routeEntry.ipAddr), IP_ADDR_SIZE);
    FmClose (fd);
	return (OK);
}


STATUS_t SetRoute (U8_t *ipaddr)
{
    struct  stat            filestat;
    ROUTE_t                 routeEntry;
    FD_t                    *fd;
    NWG_UPDATE_ROUTE_t      *UpdatePtr;
    NWG_UPDATE_ROUTE_ACK_t  *UpdateAckPtr;
    MSG_t                   *reqPtr, *ReplyPtr;
    RET_t                   ret;
    U32_t                   slot;

    if ( ((int)(inet_addr(ipaddr)) == ERROR) ||
         (inet_addr(ipaddr) == 0) )
    {
       printf ("SetRoute: invalid internet address -- format [xxx.xxx.xxx.xxx]\n");
       return (ERROR);
    }

    bzero ((char *)(&routeEntry), sizeof (ROUTE_t));

    if (stat (ROUTE_FILENAME, &filestat) == ERROR)
    {  /* This file does not exist */
       fd = FmOpen (ROUTE_FILENAME, O_CREAT|O_RDWR);
       if (fd == NULL)
       {
          printf ("SetRoute: Can not open file %s\n", ROUTE_FILENAME);
          return (ERROR);
       }
       if (routeAdd ("0", ipaddr) == ERROR)
       {
          printf ("SetRoute: add new route failed because gateway [%s] is in diffent subnet\n", ipaddr);
          FmClose (fd);
          return (ERROR);
       }
       strncpy ((char *)(&routeEntry.ipAddr), ipaddr,IP_ADDR_SIZE);
       FmWrite (fd, (char *)(&routeEntry), 0, sizeof(ROUTE_t), TRUE);
       FmClose (fd);
    }
    else
    {
       fd = FmOpen (ROUTE_FILENAME, O_RDWR); 
       if (fd == NULL)
       {
          printf ("SetRoute: Can not open file %s\n", ROUTE_FILENAME);
          return (ERROR);
       }
       FmRead (fd, (char *)(&routeEntry), 0, sizeof(ROUTE_t));
       FmClose (fd);
       if ((int)(inet_addr(routeEntry.ipAddr)) == ERROR)
          printf ("SetRoute: invalid route address [%s]\n", routeEntry.ipAddr);
       else
       {
          if (routeDelete ("0", routeEntry.ipAddr) == ERROR)
             printf ("SetRoute: previous route entry does not exist [%s]\n", routeEntry.ipAddr);
       }
      
       if (routeAdd ("0", ipaddr) == ERROR)
       {
          printf ("SetRoute: add new route failed because gateway [%s] is in diffent subnet\n", ipaddr);
          routeAdd ("0", routeEntry.ipAddr);  /* Add previous route back */
          return (ERROR);
       }
       strncpy ((char *)(&routeEntry.ipAddr), ipaddr,IP_ADDR_SIZE);
       fd = FmOpen (ROUTE_FILENAME, O_RDWR); 
       if (fd == NULL)
       {
          printf ("SetRoute: Can not open file %s\n", ROUTE_FILENAME);
          return (ERROR);
       }
       FmWrite (fd, (char *)(&routeEntry), 0, sizeof(ROUTE_t), FALSE);
       FmClose (fd);
    }


    /* If this is the active SCC, then find out if there is a standby card */
    /* If there is one, set the update over to update the password file on */
    /* the standby                                                         */
    if (GetCardState(SiwMyPhySlotNo()) == CARD_ACTIVE)
    {
       slot = SiwStandbySlotNo ();
       if ( (GetCardState(slot) != CARD_REMOVED) )
       {
          reqPtr = (MSG_t *) SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

          if (reqPtr == NULL)
          {
              printf ("SetPassword: Failed to allocate SAR memory\n");
              return (ERROR);
          }

          UpdatePtr = (NWG_UPDATE_ROUTE_t *) &reqPtr->data[0];
          SIW_SARBUFLEN_SET (reqPtr, sizeof (NWG_UPDATE_ROUTE_t));
          strncpy (UpdatePtr->ipAddr, ipaddr, IP_ADDR_SIZE);;
          ret = MrpcSync ((U32_t)slot, NWG_MOD, NWG_UPDATE_ROUTE, reqPtr,
                                     LO, WAIT_FOREVER, &ReplyPtr);

          if (ret == MRPC_OK)
          {
             UpdateAckPtr = (NWG_UPDATE_ROUTE_ACK_t *) &(ReplyPtr->data[0]);
             if (UpdateAckPtr->retStatus == OK)
             {
                printf ("SetRoute: Successful set route to standby slot\n");
                SiwFreeMem (ReplyPtr);
             }
             else
             {
                printf ("SetRoute: Failed set route to standby slot\n");
                SiwFreeMem (ReplyPtr);
                return (ERROR);
             }
          }
       }
    }


    return (OK);
}

STATUS_t addroute (U8_t *dest, U8_t *gateway, S32_t mask, S32_t tos, S32_t flags)
{
    struct  stat            filestat;
    STATIC_ROUTE_t          routeEntry;
    FD_t                    *fd;
    STATIC_ROUTE_t          *UpdatePtr;
    NWG_UPDATE_STATIC_ROUTE_ACK_t  *UpdateAckPtr;
    MSG_t                   *reqPtr, *ReplyPtr;
    RET_t                   ret;
    U32_t                   slot;
    U8_t                    found = 0;
    U8_t                    maskedDest[IP_ADDR_SIZE];
    U32_t                   destIpAddr;  
    struct in_addr          destAddr;  

    if ((int)(inet_addr(dest)) == ERROR)
    {
       printf ("addroute: invalid internet address -- format [xxx.xxx.xxx.xxx]\n");
       return (ERROR);
    }

    if ((int)(inet_addr(gateway)) == ERROR)
    {
       printf ("addroute: invalid internet address -- format [xxx.xxx.xxx.xxx]\n");
       return (ERROR);
    }

    destIpAddr = inet_network(dest);
    destAddr.s_addr = destIpAddr & mask;

    if (!(destIpAddr & mask) &&
        stat(ROUTE_FILENAME, &filestat) != ERROR)
    {
       printf ("addroute: can not add the entry -- default gateway exist\n");
       return (ERROR);

    }

    /* convert back to string */
    inet_ntoa_b(destAddr, maskedDest);

    bzero((void *)(&routeEntry), sizeof (STATIC_ROUTE_t));

    if (stat (STATIC_ROUTE_FILENAME, &filestat) == ERROR)
    {  /* This file does not exist */
       fd = FmOpen (STATIC_ROUTE_FILENAME, O_CREAT|O_RDWR);
       if (fd == NULL)
       {
          printf ("addroute: Can not open file %s\n", STATIC_ROUTE_FILENAME);
          return (ERROR);
       }
       if (mRouteAdd (maskedDest, gateway, mask, tos, flags) == ERROR)
       {
          printf ("addroute: add new route from [%s] to [%s] failed\n", dest, gateway);
          FmClose (fd);
          return (ERROR);
       }
       arpFlush ();
       strncpy ((char *)(&routeEntry.destAddr), maskedDest,IP_ADDR_SIZE);
       strncpy ((char *)(&routeEntry.gatewayAddr), gateway,IP_ADDR_SIZE);
       routeEntry.mask = (long) mask;
       routeEntry.tos = tos;
       routeEntry.flags = flags;
       FmWrite (fd, (char *)(&routeEntry), 0, sizeof(STATIC_ROUTE_t), TRUE);
       FmClose (fd);
    }
    else
    {
       U32_t checksum =0;

       /* When a file is opend for wirte, the checksum will be reset
        * to be zero. If only write one entry at every time of open, 
        * the checksum in the image file table is for the new entry only,
        * not for the whole file.  So, we have to do some tricky thing here to
        * make the whole thing work.
        */

       /* first keep the origial checksum. */
       if (fmGetImageFileCheckSum(STATIC_ROUTE_FILENAME, &checksum) < 0) {
           printf("addroute: image file(%s) is not existed in image file table \n", STATIC_ROUTE_FILENAME);
           return (ERROR);
       }

       fd = FmOpen (STATIC_ROUTE_FILENAME, O_CREAT|O_RDWR); 
       if (fd == NULL)
       {
          printf ("addroute: Can not open file %s\n", STATIC_ROUTE_FILENAME);
          return (ERROR);
       }
      
       if (mRouteAdd (maskedDest, gateway, mask, tos, flags) == ERROR)
       {
          printf ("addroute: add new route from [%s] to [%s] failed \n", dest, gateway);
          FmClose (fd);
          return (ERROR);
       }
       arpFlush ();

       while (FmRead (fd, (char *)(&routeEntry), 0, sizeof (STATIC_ROUTE_t)) == sizeof (STATIC_ROUTE_t))
       {
          if ( (!strncmp((char *)(&routeEntry.destAddr), maskedDest, IP_ADDR_SIZE)) &&
               (!strncmp((char *)(&routeEntry.gatewayAddr), gateway, IP_ADDR_SIZE)) &&
               (routeEntry.mask == mask) &&
               (routeEntry.tos == tos) &&
               (routeEntry.flags == flags) )
          {
               found = 1;
               /* write the original checksum back to the image file table.
                * otherwies, the checksum is zero.
                */
               fmUpdateImageFileTable(STATIC_ROUTE_FILENAME, checksum);
               break;
          }
          else if (!strncmp((char *)(&routeEntry.destAddr), maskedDest, IP_ADDR_SIZE))
          {
               lseek (fd->vxWFd, -sizeof(STATIC_ROUTE_t), SEEK_CUR);
               break;
          }
       }
 
       if (!found)
       {
          U32_t  entryCksum=0;

          printf ("addroute: update d:%s g:%s m:0x%x t:%d f:%d\n",
                  dest, gateway, mask, tos, flags); 
          strncpy ((char *)(&routeEntry.destAddr), maskedDest,IP_ADDR_SIZE);
          strncpy ((char *)(&routeEntry.gatewayAddr), gateway,IP_ADDR_SIZE);
          routeEntry.mask = (long) mask;
          routeEntry.tos = tos;
          routeEntry.flags = flags;
          FmWrite (fd, (char *)(&routeEntry), 0, sizeof(STATIC_ROUTE_t), FALSE);

          /* get the checksum for the new entry */
          if (fmGetImageFileCheckSum(STATIC_ROUTE_FILENAME, &entryCksum) < 0) {
            printf("addroute: image file(%s) is not existed in image file table \n", STATIC_ROUTE_FILENAME);
            FmClose (fd);
            return (ERROR);
          }
          /* write the totol checksum back to the image file table,
           * otherwise, the checksum is for the new entry only
           */
          fmUpdateImageFileTable(STATIC_ROUTE_FILENAME, checksum+entryCksum);
       }

       FmClose (fd);
    }

    /* If this is the active SCC, then find out if there is a standby card */
    /* If there is one, set the update over to update the password file on */
    /* the standby                                                         */
    if ((GetCardState(SiwMyPhySlotNo()) == CARD_ACTIVE) && !found)
    {
       slot = SiwStandbySlotNo ();
       if ( (GetCardState(slot) != CARD_REMOVED) )
       {
          reqPtr = (MSG_t *) SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

          if (reqPtr == NULL)
          {
              printf ("SetPassword: Failed to allocate SAR memory\n");
              return (ERROR);
          }

          UpdatePtr = (STATIC_ROUTE_t *) &reqPtr->data[0];
          SIW_SARBUFLEN_SET (reqPtr, sizeof (STATIC_ROUTE_t));
          strncpy (UpdatePtr->destAddr, dest, IP_ADDR_SIZE);;
          strncpy (UpdatePtr->gatewayAddr, gateway,IP_ADDR_SIZE);
          UpdatePtr->mask = (long) mask;
          UpdatePtr->tos = tos;
          UpdatePtr->flags = flags;
          ret = MrpcSync ((U32_t)slot, NWG_MOD, NWG_ADD_ROUTE, reqPtr,
                                     LO, WAIT_FOREVER, &ReplyPtr);

          if (ret == MRPC_OK)
          {
             UpdateAckPtr = (NWG_UPDATE_STATIC_ROUTE_ACK_t *) &(ReplyPtr->data[0]);
             if (UpdateAckPtr->retStatus == OK)
             {
                printf ("addroute: Successfully add route to standby slot\n");
                SiwFreeMem (ReplyPtr);
             }
             else
             {
                printf ("addroute: Failed add route to standby slot\n");
                SiwFreeMem (ReplyPtr);
                return (ERROR);

             }
          }
       }
    }

    return (OK);
}

STATUS_t delroute (U8_t *dest, S32_t mask, S32_t tos, S32_t flags)
{
    struct  stat            filestat;
    STATIC_ROUTE_t          routeEntry;
    FD_t                    *fd, *outFd;
    NWG_DELETE_STATIC_ROUTE_t      *UpdatePtr;
    NWG_UPDATE_STATIC_ROUTE_ACK_t  *UpdateAckPtr;
    MSG_t                   *reqPtr, *ReplyPtr;
    RET_t                   ret;
    U32_t                   slot;
    U32_t                   len;
    U8_t                    found = 0;
    U8_t                    *buf, *tmpBuf;
    STATIC_ROUTE_t          *routePtr;
    U8_t                    tmpName[40];

    if ((int)(inet_addr(dest)) == ERROR)
    {
       printf ("delroute: invalid internet address -- format [xxx.xxx.xxx.xxx]\n");
       return (ERROR);
    }

    bzero ((void *)(&routeEntry), sizeof (STATIC_ROUTE_t));

    if (stat (STATIC_ROUTE_FILENAME, &filestat) != ERROR)
    {  /* This file exists */
       fd = FmOpen (STATIC_ROUTE_FILENAME, READ_ONLY);
       buf = tmpBuf = SiwAllocMem (SIW_STATIC, filestat.st_size, WAIT_FOREVER);
       FmRead (fd, buf, 0, filestat.st_size);
       FmClose (fd);

       sprintf (tmpName, "%s.tmp", STATIC_ROUTE_FILENAME);
       outFd = FmOpen (tmpName, O_CREAT|O_RDWR);
       len = 0;
       while (len < filestat.st_size)
       {
          routePtr = (STATIC_ROUTE_t *)buf;
          if ( !strncmp(dest, routePtr->destAddr, IP_ADDR_SIZE) &&
               (routePtr->mask == mask) &&
               (routePtr->tos == tos) &&
               (routePtr->flags == flags) )
          {
             len += sizeof(STATIC_ROUTE_t);
             buf += sizeof(STATIC_ROUTE_t);
             mRouteDelete (dest, mask, tos, flags);
             found = 1;
             continue;
          }
          FmWrite (outFd, (char *)routePtr, 0, sizeof(STATIC_ROUTE_t), TRUE);
          len += sizeof(STATIC_ROUTE_t);
          buf += sizeof(STATIC_ROUTE_t);
       }
       FmClose (outFd);

       if (found)
       {
          FmRename (tmpName, STATIC_ROUTE_FILENAME);
          if (GetCardState(SiwMyPhySlotNo()) == CARD_ACTIVE)
          {
            slot = SiwStandbySlotNo ();
            if ( (GetCardState(slot) != CARD_REMOVED) )
            {
                reqPtr = (MSG_t *) SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

                if (reqPtr == NULL)
                {
                    printf ("delroute(): Failed to allocate SAR memory\n");
                    SiwFreeMem (tmpBuf);
                    return (ERROR);
                }

                UpdatePtr = (NWG_DELETE_STATIC_ROUTE_t *) &reqPtr->data[0];
                SIW_SARBUFLEN_SET (reqPtr, sizeof (NWG_DELETE_STATIC_ROUTE_t));
                strncpy (UpdatePtr->destAddr, dest, IP_ADDR_SIZE);;
                UpdatePtr->mask = (long) mask;
                UpdatePtr->tos = tos;
                UpdatePtr->flags = flags;
                ret = MrpcSync ((U32_t)slot, NWG_MOD, NWG_DELETE_ROUTE, reqPtr,
                                            LO, WAIT_FOREVER, &ReplyPtr);

                if (ret == MRPC_OK)
                {
                    UpdateAckPtr = (NWG_UPDATE_STATIC_ROUTE_ACK_t *) &(ReplyPtr->data[0]);
                    if (UpdateAckPtr->retStatus == OK)
                    {
                        printf ("delroute: Successful delete route to standby slot\n");
                        SiwFreeMem (ReplyPtr);
                    }
                    else
                    {
                        printf ("delroute: Failed delete route to standby slot\n");
                        SiwFreeMem (ReplyPtr);
                        SiwFreeMem (tmpBuf);
                        return (ERROR);

                    }
                }
            }
          }
       }
       else {
             SiwFreeMem (tmpBuf);
             FmDelete (tmpName);
             return (NWG_INVALID_STATIC_ROUTE);
       }
       SiwFreeMem (tmpBuf);
       return (OK);
    }
    return (ERROR);
} 
       

void lsroute ()
{
   struct  stat    filestat;
   STATIC_ROUTE_t  routeEntry;
   FD_t            *fd;

   if (stat (STATIC_ROUTE_FILENAME, &filestat) != ERROR)
   {
      fd = FmOpen (STATIC_ROUTE_FILENAME, READ_ONLY);
      if (fd == NULL)
      {
         printf ("AddStaticRoute: Can not open file %s\n", STATIC_ROUTE_FILENAME);
         return;
      }

      while (FmRead(fd, (char *)(&routeEntry), 0, sizeof(STATIC_ROUTE_t)) == sizeof(STATIC_ROUTE_t))
      {
         printf ("d:%-16s g:%-16s m:0x%08x t:%d f:%d\n",
				 (char *)(&routeEntry.destAddr), 
				 (char *)(&routeEntry.gatewayAddr), routeEntry.mask, 
				 routeEntry.tos, routeEntry.flags);
      }
      FmClose (fd);
   }
}


STATUS_t AddStaticRoute ()
{
   struct  stat    filestat;
   STATIC_ROUTE_t  routeEntry;
   FD_t            *fd;
   U32_t           gwExist;

   if (stat (STATIC_ROUTE_FILENAME, &filestat) != ERROR)
   {    

      fd = FmOpen (STATIC_ROUTE_FILENAME, READ_ONLY);
      if (fd == NULL)
      {
         printf ("AddStaticRoute: Can not open file %s\n", STATIC_ROUTE_FILENAME);
         return (ERROR);
      }
      gwExist = (stat(ROUTE_FILENAME, &filestat) != ERROR);

      while (FmRead(fd, (char *)(&routeEntry), 0, sizeof(STATIC_ROUTE_t)) == sizeof(STATIC_ROUTE_t))
      {
	 /* if default route exist and static route is another default route
           don't set this statics route entry */
         if (!(inet_addr(&routeEntry.destAddr) & routeEntry.mask) && gwExist)
             continue;
         if ( mRouteAdd ((char *)(&routeEntry.destAddr), (char *)(&routeEntry.gatewayAddr), routeEntry.mask,
                    routeEntry.tos, routeEntry.flags) == ERROR) {
            printf("AddStaticRoute(): mRouteAdd return error \n");
            FmClose(fd);
            return (ERROR);
         }
      }
#if 0
      printf("**************invoked AddStaticRoute!***************\n");
#endif
      FmClose (fd);
      return (OK);
   }
   return (ERROR);
}



STATUS_t RestoreIpRoute ()
{
   struct  stat    filestat;
   STATIC_ROUTE_t  routeEntry;
   FD_t            *fd;
   U32_t           gwExist;
   U8_t            inAddr[40];

   if ((gwExist = GetRoute (inAddr)) == OK)
   {
       routeDelete("0", inAddr);
       routeAdd ("0", inAddr);
   }
   if (stat (STATIC_ROUTE_FILENAME, &filestat) != ERROR)
   {    

      fd = FmOpen (STATIC_ROUTE_FILENAME, READ_ONLY);
      if (fd == NULL)
      {
         return (OK);
      }

      while (FmRead(fd, (char *)(&routeEntry), 0, sizeof(STATIC_ROUTE_t)) == sizeof(STATIC_ROUTE_t))
      {
	 /* if default route exist and static route is another default route
           don't set this statics route entry */
         if (!(inet_addr(&routeEntry.destAddr) & routeEntry.mask) && gwExist == OK)
             continue;
         mRouteDelete ((char *)(&routeEntry.destAddr), routeEntry.mask, 
					   routeEntry.tos, routeEntry.flags);
         mRouteAdd ((char *)(&routeEntry.destAddr), (char *)(&routeEntry.gatewayAddr), 
                 routeEntry.mask, routeEntry.tos, routeEntry.flags);
      }
      FmClose (fd);
   }
   return (OK);
}
#endif /* SCC && !FLASH_BOOT */


#if !defined(FLASH_BOOT)
STATUS_t GetPassword (U8_t type, U8_t *password)
{
#ifdef SCC
    struct  stat            filestat;
#endif
    PASSWORD_t              passfile;
    FD_t                    *fd;

    if ((type != ROOT_USER) && (type != SUPER_USER))
       return (ERROR);
    
    if (password == NULL)
    {
       printf ("SetPassword: password parameter is (NULL)\n");
       return (ERROR);
    } 

#ifdef SCC
    if (stat (PASSWORD_FILENAME, &filestat) == ERROR)
    {  /* This file does not exist */
       fd = FmOpen (PASSWORD_FILENAME, O_CREAT|O_RDWR);
       if (fd == NULL)
       {
          printf ("GetPassword: Can not open file %s\n", PASSWORD_FILENAME);
          return (ERROR);
       }
       bzero ((void *)(&passfile), sizeof (PASSWORD_t));
       strncpy ((char *)(&passfile.root), "cisco",PASSWORD_SIZE);
       strncpy ((char *)(&passfile.super),"cisco",PASSWORD_SIZE);
       FmWrite (fd, (char *)(&passfile), 0, sizeof(PASSWORD_t), TRUE);
       FmClose (fd);
    }
#endif /* SCC */

    if ((fd=FmOpen (PASSWORD_FILENAME, O_RDONLY)) == NULL)
    {
#ifndef SCC
       strncpy (password, "cisco", PASSWORD_SIZE);
       return (OK);
#else
       printf ("GetPassword: Can not open file %s\n", PASSWORD_FILENAME);
       return (ERROR);
#endif
    }

    FmRead (fd, (char *)(&passfile), 0, sizeof(PASSWORD_t));
    FmClose (fd);
 
    switch (type)
    {
       case ROOT_USER:
          strncpy (password, passfile.root, PASSWORD_SIZE);
          break;
       case SUPER_USER:
          strncpy (password, passfile.super, PASSWORD_SIZE);
          break;
       default:
          printf ("GetPassword: Incorrect password type[%d]\n", type);
          return (ERROR);
          break;

    }
    return (OK);
}
#endif /* !FLASH_BOOT */

STATUS_t RemUpdateFlash (IN MSG_t *inMsg, OUT MSG_t **outMsg)
{

    NWG_FLASH_UPDATE_t          *ReqPtr;
    NWG_FLASH_UPDATE_ACK_t      *ReplyPtr;
    RET_t    flashRet = OK;
    U8_t     *buf;
    STATUS_t    flash1_ret = OK;
    
    ReqPtr = (NWG_FLASH_UPDATE_t *) &inMsg->data[0];

    if (nwgDbg)
    {
       printf ("RemUpdateFlash: P[0x%x] L[%d] [s=%d] [l=%d] [o=%x] [b=%d]\n", 
                (unsigned int)inMsg, SIW_SARBUFLEN_GET (inMsg), ReqPtr->Slot, 
                ReqPtr->Length, ReqPtr->Offset, ReqPtr->BankId);
    }

    if ((S32_t)(ReqPtr->Slot) == SiwMyPhySlotNo())
    {
       taskPrioritySet (taskIdSelf(), 70);
       buf = (U8_t *)((U32_t)ReqPtr + NWG_FLASH_UPDATE_HDR);
       if (nwgDbg)
          printf ("updateflash:  b[0x%x] s[%d] o[%x] bnk[%d]\n", 
				  (unsigned int)buf, ReqPtr->Length/2,
				  ReqPtr->Offset, ReqPtr->BankId);
       if ( (flash1_ret=FlashLoad ((VU16_t *)buf, 
                                        (U16_t *)(ReqPtr->Offset), (U8_t)ReqPtr->BankId)) != OK)
          printf ("updateflash: Failed to program flash device [0]\n");
       else
       {
          if (nwgDbg)
             printf ("updateflash: program [s=%d] [l=%d] [o=%x] [b=%d]\n",
                         ReqPtr->Slot, ReqPtr->Length, ReqPtr->Offset,ReqPtr->BankId);
       }
       if (flash1_ret != OK)
          flashRet = ERROR;
 
       *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));

       SIW_SARBUFLEN_SET(*outMsg, sizeof(NWG_FLASH_UPDATE_ACK_t));
       ReplyPtr = (NWG_FLASH_UPDATE_ACK_t *) &(*outMsg)->data[0];
       ReplyPtr->retStatus = flashRet;
    }
    SiwFreeMem (inMsg);
    return (OK);
}        

STATUS_t RemEraseFlash (IN MSG_t *inMsg, OUT MSG_t **outMsg)
{

    NWG_FLASH_ERASE_t          *ReqPtr;
    NWG_FLASH_ERASE_ACK_t      *ReplyPtr;
    RET_t                      ret;

    taskPrioritySet (taskIdSelf(), 10);
    ReqPtr = (NWG_FLASH_ERASE_t *) &inMsg->data[0];
    printf ("RemEraseFlash: Erase flash bank [%d]\n", ReqPtr->BankId);
    ret = FlashChipErase (ReqPtr->BankId);
    *outMsg = (MSG_t *)SiwAllocMem(SIW_SAR, sizeof(MSG_t), SECONDS(1));
    SIW_SARBUFLEN_SET(*outMsg, sizeof(NWG_FLASH_ERASE_ACK_t));
    ReplyPtr = (NWG_FLASH_ERASE_ACK_t *) &(*outMsg)->data[0];
    ReplyPtr->retStatus = ret;

    SiwFreeMem (inMsg); 
    return (OK);
}

void testsp ()
{
   taskLock ();
   while (1)
     ;
}
#endif
