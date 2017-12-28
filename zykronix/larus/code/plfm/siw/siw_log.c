/*  
 * Copyright (C) 2005, All Rights Reserved, by
 * Ahoya Networks, Inc. San Jose, CA.
 *
 * Abstract:
 *
 *    Low level support routines for system call logging.
 *          
 * Note:
 *    - Applications do not normally call these routines directly.
 *
 * Author(s):
 *
 *   
 */

#include "vxWorks.h"
#include "stdio.h"
#include "axss_types.h"
#include "siw_if.h"
#include "ioLib.h"

/* function prototypes */

static void SiwRoundFile(FILE *fp, size_t recsize);

/********************************************************************
 * Function:     SiwLogOpen()
 *
 * Description:  This routine opens the named log file for read or update.
 *
 * Note: <filesize> is an output parameter if <mode> is SIW_LOG_OPEN_READ,
 *       or an input parameter if <mode> is SIW_LOG_OPEN_UPDATE.
 *
 * In Parms:     const char * filename  - log file name
 *               int          mode      - SIW_LOG_OPEN_READ or
 *                                            SIW_LOG_OPEN_UPDATE
 *               size_t       recsize   - bytes per record
 *
 * In/Out Parm:  long       * filesize  - size of file in bytes
 *
 * Returns:      A file pointer on success, or NULL on error.
 ********************************************************************/
SIW_LOG_FILE *
SiwLogOpen(IN const char *filename, IN int mode, IN size_t recsize,
           IN OUT long *filesize)
{
    FILE *fp = (FILE *)NULL;
    long len;

    switch (mode)
    {
        case SIW_LOG_OPEN_READ:
            fp = fopen(filename, "r");
            break;

        case SIW_LOG_OPEN_UPDATE:
            if ((fp = fopen(filename, "r+")) == NULL)
            {
                fp = fopen(filename, "w");
            }
            break;

        default:
            break;
    }

    if (fp != (FILE *)NULL)
    {
        (void) fseek(fp, 0, SEEK_END);
        len = ftell(fp);                        /* get file length */
        if (mode == SIW_LOG_OPEN_READ)
        {
            *filesize = len;
        }

        /* if write mode and file is not full, leave file position at end */

        if ((mode == SIW_LOG_OPEN_UPDATE) && (len < *filesize))
        {
            SiwRoundFile(fp, recsize);          /* ensure proper alignment */
            goto siw_log_out;
        }

        /* if there are multiple records, set file position based on mode */

        (void) fseek(fp, 0, SEEK_SET);          /* go to start of file */
        if ((size_t)len >= (2 * recsize))
        {
            SIW_LOG_HEADER_t *header;
            long    this_offset;
            U64_t   this_date;
            long    prev_offset = 0;
            U64_t   prev_date = SIW_LOG_DATE_MASK;
            long    initial_offset = len - recsize;
            int     ii;

            if ((header = (SIW_LOG_HEADER_t *)
                            SiwAllocMem(SIW_DYNAMIC, recsize, 0)) == NULL)
            {
                goto siw_log_out;
            }

            /*
             * search file for place where date goes forward 
             * (note that SiwLogRead reads "backwards")
             */
            for (ii = 0; (size_t)ii < ((len / recsize) + 1); ii++)
            {
                this_offset = ftell(fp);
                if (SiwLogRead(header, recsize, (SIW_LOG_FILE *)fp) != 1)
                {
                    SiwFreeMem((void *)header);
                    goto siw_log_out;
                }
                this_date = header->v2 & SIW_LOG_DATE_MASK;
                if (this_date > prev_date)
                {
                    /*
                     * if read mode, set file position at newest record.
                     * if write mode, set file position at oldest record.
                     */
                    initial_offset = (mode == SIW_LOG_OPEN_READ) ?
                                           this_offset : prev_offset;
                    break;
                }
                prev_date = this_date;
                prev_offset = this_offset;
            }
            SiwFreeMem((void *)header);
            (void) fseek(fp, initial_offset,
                            SEEK_SET);
        }
    }

siw_log_out:
    return ((SIW_LOG_FILE *)fp);
}

/********************************************************************
 * Function:     SiwLogClose()
 *
 * Description:  This routine closes the log file specified by <stream>.
 *
 * In Parms:     SIW_LOG_FILE * stream  - file ptr obtained via SiwLogOpen()
 *
 * Returns:      0 on success, or EOF on error.
 ********************************************************************/
int
SiwLogClose(IN SIW_LOG_FILE *stream)
{
    return (fclose((FILE *)stream));
}

/********************************************************************
 * Function:     SiwLogRead
 *
 * Description:  This routine reads into the buffer <record> one
 *               record of <size> bytes from a specified stream <fp>.
 *               The file position indicator for the  stream  is
 *               decremented to the *previous* record in the file,
 *               or if already at the first record, it is set to
 *               the *last* record in the file.  If an error occurs,
 *               the resulting value of the  file  position indicator
 *               for  the
 *               stream is indeterminate.  If a partial element is
 *               read, its value is indeterminate.
 *
 * In Parms:     size_t         size   - bytes per record
 *               SIW_LOG_FILE * fp     - where to read from
 *
 * Out Parms:    SIW_LOG_HEADER_t * record - where to copy to
 *
 * Returns:      The number of records read.
 ********************************************************************/
size_t
SiwLogRead(OUT SIW_LOG_HEADER_t *record, IN size_t size, IN SIW_LOG_FILE *fp)
{
    size_t rv;
    FILE *ifp = (FILE *)fp;

    /* read at current position */
    rv = fread(record, size, 1, ifp);

    /* back up to prepare for next read (wrap around if necessary) */
    if (rv)
    {
        if (ftell(ifp) >= (long) (2 * size))
        {
            (void) fseek(ifp, (long) (-2 * size), SEEK_CUR);
        }
        else
        {
            (void) fseek(ifp, (long) (-1 * size), SEEK_END);
            SiwRoundFile(ifp, size);          /* ensure proper alignment */
        }
    }

    return (rv);
}

/********************************************************************
 * Function:     SiwLogWrite()
 *
 * Description:  This routine writes, from the array  <record>, up to
 *               <count> records to a specified stream.  The file
 *               position indicator  for  the  stream is advanced by
 *               the number of characters successfully written.  This
 *               routine wraps to the beginning on end-of-file.
 *               If an error occurs, the resulting value of the file
 *               position indicator for the stream is indeterminate.
 *
 * In Parms:     SIW_LOG_HEADER_t * record   - where to copy from
 *               size_t             nrec     - no. of records
 *               size_t             recsize  - bytes per record
 *               SIW_LOG_FILE *     fp       - stream to write to
 *               long               filesize - size of file in bytes
 *
 * Out Parms:    None
 *
 * Returns:      The number of records written.
 ********************************************************************/
size_t
SiwLogWrite(IN SIW_LOG_HEADER_t *record, IN size_t nrec,
            IN size_t recsize, IN SIW_LOG_FILE *fp, IN long fileSize)
{
    STATUS_t rv;
    long curpos;
    FILE *ifp = (FILE *)fp;

    if (nrec != 1)
    {
        /* XXX SiwLogWrite() currently supports only single record writes */
        return (0);
    }

    if ((!recsize) || ((recsize * nrec) > (size_t)fileSize) ||
        (ifp == (FILE *)NULL) || (record == (SIW_LOG_HEADER_t *)NULL))
    {
        return (0);
    }

    curpos = ftell(ifp);
    if (curpos < 0)
    {
#ifdef DEBUG
        perror("ftell");
#endif
        return (0);
    }

    if (curpos > (fileSize - (long)recsize))
    {
        /* set position to beginning of file */
        if (fseek(ifp, 0L, SEEK_SET) != 0)
        {
#ifdef DEBUG
            perror("fseek");
#endif
            return (0);
        }
    }

    /* write to file */
    rv = fwrite((const void *)record, recsize, nrec, ifp);

    /* flush to disk */
    (void) fflush(ifp);
    /* ioctl is run on file descriptor field fdtrap->_file in FILE struct */
    (void) ioctl(ifp->_file,FIOSYNC,0);

    if (rv == 0)
    {
#ifdef DEBUG
        perror("fwrite");
#endif
    }
    return (rv);
}

/********************************************************************
 * Function:     SiwRoundFile()
 *
 * Description:  This routine ensures that the file position is
 *               aligned on a record boundary.
 *
 * In Parms:     SIW_LOG_FILE *     fp       - stream to write to
 *               size_t             recsize  - bytes per record
 *
 * Out Parms:    None
 *
 * Returns:      None
 ********************************************************************/
static void
SiwRoundFile(FILE *fp, size_t recsize)
{
    long position;
    long remainder;

    if ((position = ftell(fp)) == -1)
    {
        /* error */
        return;
    }

    if ((remainder = (position % recsize)) != 0)
    {
        (void) fseek(fp, -1L * remainder, SEEK_CUR);
    }
}
