/*
 *
 * Copyright (C) 1996-2003 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#ifndef SR_VXWORKS_H
#define SR_VXWORKS_H

#ifdef  __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* This section contains #defines included manually */

/* VxWorks uses _NSIGS instead of NSIG */
#define NSIG _NSIGS

/* End manually inserted #defines */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
/* #undef _ALL_SOURCE */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to the type of elements in the array set by `getgroups'.
   Usually this is either `int' or `gid_t'.  */
#define GETGROUPS_T int

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have close()  */
/* #undef HAVE_CLOSE */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have the gethostbyname function.  */
/* #undef HAVE_GETHOSTBYNAME */

/* Define if you have the gethostname function.  */
/* #undef HAVE_GETHOSTNAME */

/* Define if you have the getmntent function.  */
/* #undef HAVE_GETMNTENT */

/* Define if you have the getpagesize function */
/* #undef HAVE_GETPAGESIZE */

/* Define if you have getpeername() */
/* #undef HAVE_GETPEERNAME */

/* Define if you have the getservbyname function */
/* #undef HAVE_GETSERVBYNAME */

/* Define if you have the inet_addr function */
#define HAVE_INET_ADDR

/* Define if you have the inet_ntoa function */
#define HAVE_INET_NTOA 1

/* Define if you have a working `mmap' system call.  */
/* #undef HAVE_MMAP */

/* Define if you have a the `recvfrom' system call.  */
#define HAVE_RECVFROM 1

/* Define if you have a the `rename' system call.  */
/* #undef HAVE_RENAME */

/* Define if you have a the `sendto' system call.  */
#define HAVE_SENDTO 1

/* Define if your struct stat has st_blksize.  */
/* #undef HAVE_ST_BLKSIZE */

/* Define if your struct stat has st_blocks.  */
/* #undef HAVE_ST_BLOCKS */

/* Define if your struct stat has st_rdev.  */
/* #undef HAVE_ST_RDEV */

/* Define if you have sys_errlist defined in a header file.  */
/* #undef HAVE_SYS_ERRLIST */

/* Define if you have sys_nerr defined in a header file.  */
/* #undef HAVE_SYS_NERR */

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if your struct tm has tm_zone.  */
/* #undef HAVE_TM_ZONE */

/* Define if you don't have tm_zone but do have the external array
   tzname.  */
/* #undef HAVE_TZNAME */

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
/* #undef HAVE_UTIME_NULL */

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
/* #undef HAVE_VPRINTF */

/* Define if you have the vsprintf function.  */
/* #undef HAVE_VSPRINTF */

/* Define if you have the snprintf function */
/* #undef HAVE_SNPRINTF */

/* Define if you have the vsnprintf function */
/* #undef HAVE_VSNPRINTF */

/* Define if you have the wait3 system call.  */
/* #undef HAVE_WAIT3 */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef mode_t */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly.  */
/* #undef STAT_MACROS_BROKEN */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
/* #undef TIME_WITH_SYS_TIME */

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define vfork as fork if vfork does not work.  */
#define vfork fork

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a char *.  */
#define SIZEOF_CHAR_P 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a int *.  */
#define SIZEOF_INT_P 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* Define this is a long is 8 octets  */
/* #undef HAVE_EIGHT_BYTE_LONGS */

/* Define if you have the alarm function.  */
/* #undef HAVE_ALARM */

/* Define if you have the ftime function.  */
/* #undef HAVE_FTIME */

/* Define if you have the getcwd function.  */
/* #undef HAVE_GETCWD */

/* Define if you have the getenv function.  */
#define HAVE_GETENV 1

/* Define if you have the geteuid function.  */
/* #undef HAVE_GETEUID */

/* Define if you have the getpid function.  */
/* #undef HAVE_GETPID */

/* Define if you have the gettimeofday function.  */
/* #undef HAVE_GETTIMEOFDAY */

/* Define if you have the localtime function.  */
#define HAVE_LOCALTIME 1

/* Define if you have the mkdir function.  */
/* #undef HAVE_MKDIR */

/* Define if you have the putenv function.  */
/* #undef HAVE_PUTENV */

/* Define if you have the sc_delay function.  */
/* #undef HAVE_SC_DELAY */

/* Define if you have the select function.  */
/* #undef HAVE_SELECT */

/* Define if you have the sigaddset function.  */
/* #undef HAVE_SIGADDSET */

/* Define if you have the sigdelset function.  */
/* #undef HAVE_SIGDELSET */

/* Define if you have the sigemptyset function.  */
/* #undef HAVE_SIGEMPTYSET */

/* Define if you have the siginterrupt function.  */
/* #undef HAVE_SIGINTERRUPT */

/* Define if you have the sigismember function.  */
/* #undef HAVE_SIGISMEMBER */

/* Define if you have the signal function.  */
#define HAVE_SIGNAL 1

/* Define if you have the sigprocmask function.  */
/* #undef HAVE_SIGPROCMASK */

/* Define if you have the sleep function.  */
/* #undef HAVE_SLEEP */

/* Define if you have the socket function.  */
#define HAVE_SOCKET 1

/* Define if you have the strcasecmp function.  */
/* #undef HAVE_STRCASECMP */

/* Define if you have the strcmpi function.  */
/* #undef HAVE_STRCMPI */

/* Define if you have the strcspn function.  */
/* #undef HAVE_STRCSPN */

/* Define if you have the strdup function.  */
/* #undef HAVE_STRDUP */

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strspn function.  */
/* #undef HAVE_STRSPN */

/* Define if you have the strstr function.  */
/* #undef HAVE_STRSTR */
 
/* Define if you have the strtok_r function.  */
#define HAVE_STRTOK_R 1

/* Define if you have the strtol function.  */
/* #undef HAVE_STRTOL */

/* Define if you have the taskDelay function.  */
#define HAVE_TASKDELAY 1

/* Define if you have the tm_wkafter function.  */
/* #undef HAVE_TM_WKAFTER */

/* Define if you have the valloc function.  */
/* #undef HAVE_VALLOC */

/* Define if you have the <arpa/inet.h> header file.  */
/* #define HAVE_ARPA_INET_H 1 */

/* Define if you have the <bsd.h> header file.  */
/* #undef HAVE_BSD_H */

/* Define if you have the <common.h> header file.  */
/* #undef HAVE_COMMON_H */

/* Define if you have the <ctype.h> header file.  */
#define HAVE_CTYPE_H 1

/* Define if you have the <dirent.h> header file.  */
/* #undef HAVE_DIRENT_H */

/* Define if you have the <dl.h> header file.  */
/* #undef HAVE_DL_H */

/* Define if you have the <dlfcn.h> header file.  */
/* #undef HAVE_DLFCN_H */

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <hostLib.h> header file.  */
#define HAVE_HOSTLIB_H 1

/* Define if you have the <inetLib.h> header file.  */
#define HAVE_INETLIB_H 1

/* Define if you have the <io.h> header file.  */
/* #undef HAVE_IO_H */

/* Define if you have the <ioLib.h> header file.  */
#define HAVE_IOLIB_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <malloc.h> header file.  */
/* #undef HAVE_MALLOC_H */

/* Define if you have the <memory.h> header file.  */
/* #undef HAVE_MEMORY_H */

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <net/inet.h> header file.  */
#define HAVE_NET_INET_H 1

/* Define if you have the <net/route.h> header file */
#define HAVE_NET_ROUTE_H 1

/* Define if you have the <net/if.h> header file */
#define HAVE_NET_IF_H 1

/* Define if you have the <net/if_arp.h> header file */
#define HAVE_NET_IF_ARP_H 1

/* Define if you have the <netdb.h> header file.  */
/* #undef HAVE_NETDB_H */

/* Define if you have the <netinet/in.h> header file.  */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <netinet/in_systm.h> header file.  */
#define HAVE_NETINET_IN_SYSTM_H 1

/* Define if you have the <netinet/ip.h> header file.  */
#define HAVE_NETINET_IP_H 1

/* Define if you have the <netinet/tcp.h> header file.  */
/* This was added on the assumption that if netinet/ip.h
   exists, netinet/tcp.h would, too */
#define HAVE_NETINET_TCP_H 1

/* Define if you have the <netinet/ip_icmp.h> header file.  */
#define HAVE_NETINET_IP_ICMP_H 1

/* Define if you have the <netns/idp.h> header file.  */
/* #undef HAVE_NETNS_IDP_H */

/* Define if you have the <netns/ns.h> header file.  */
/* #undef HAVE_NETNS_NS_H */

/* Define if you have the <os2.h> header file.  */
/* #undef HAVE_OS2_H */

/* Define if you have the <paths.h> header file.  */
/* #undef HAVE_PATHS_H */

/* Define if you have the <pctcp.h> header file.  */
/* #undef HAVE_PCTCP_H */

/* Define if you have the <psos.h> header file.  */
/* #undef HAVE_PSOS_H */

/* Define if you have the <sgtty.h> header file.  */
/* #undef HAVE_SGTTY_H */

/* Define if you have the <signal.h> header file.  */
#define HAVE_SIGNAL_H 1

/* Define if you have the <socket.h> header file.  */
#define HAVE_SOCKET_H 1

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdio.h> header file */
#define HAVE_STDIO_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <synch.h> header file.  */
/* #undef HAVE_SYNCH_H */

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/fcntl.h> header file.  */
/* #undef HAVE_SYS_FCNTL_H */

/* Define if you have the <sys/file.h> header file.  */
/* #undef HAVE_SYS_FILE_H */

/* Define if you have the <sys/filio.h> header file.  */
/* #undef HAVE_SYS_FILIO_H */

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file.  */
/* #undef HAVE_SYS_PARAM_H */

/* Define if you have the <sys/select.h> header file.  */
/* #undef HAVE_SYS_SELECT_H */

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file.  */
/* #undef HAVE_SYS_TIME_H */

/* Define if you have the <sys/timeb.h> header file.  */
/* #undef HAVE_SYS_TIMEB_H */

/* Define if you have the <sys/timers.h> header file.  */
/* #undef HAVE_SYS_TIMERS_H */

/* Define if you have the <sys/times.h> header file.  */
#define HAVE_SYS_TIMES_H 1

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/un.h> header file.  */
/* #undef HAVE_SYS_UN_H */

/* Define if you have the <syscall.h> header file.  */
/* #undef HAVE_SYSCALL_H */

/* Define if you have the <taskLib.h> header file.  */
#define HAVE_TASKLIB_H 1

/* Define if you have the <termio.h> header file.  */
/* #undef HAVE_TERMIO_H */

/* Define if you have the <thread.h> header file.  */
/* #undef HAVE_THREAD_H */

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <timers.h> header file.  */
#define HAVE_TIMERS_H 1

/* Define if you have the <types.h> header file.  */
#define HAVE_TYPES_H 1

/* Define if you have the <types/vxTypes.h> header file.  */
#define HAVE_TYPES_VXTYPES_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <utils.h> header file.  */
/* #undef HAVE_UTILS_H */

/* Define if you have the <varargs.h> header file.  */
#define HAVE_VARARGS_H 1

/* Define if you have the <vrtxil.h> header file.  */
/* #undef HAVE_VRTXIL_H */

/* Define if you have the <winsock.h> header file.  */
/* #undef HAVE_WINSOCK_H */

/* Define if you have the <a.out.h> header file. */
/* #undef HAVE_A_OUT_H */

/* Define if you have the <kstat.h> header file. */
/* #undef HAVE_KSTAT_H */

/* Define if you have the <kvm.h> header file. */
/* #undef HAVE_KVM_H */

/* Define if you have the memmove function. */
#define HAVE_MEMMOVE 1

/* Define if you have the <nlist.h> header file. */
/* #undef HAVE_NLIST_H */

/* Define if you have the rand function. */
#define HAVE_RAND 1

/* Define if you have the random function. */
#define HAVE_RANDOM 1

/* Define if you have the <setjmp.h> header file. */
#define HAVE_SETJMP_H 1

/* Define if you have the srand function. */
#define HAVE_SRAND 1

/* Define if you have the srandom function. */
#define HAVE_SRANDOM 1

/* Define if you have the <stab.h> header file. */
/* #undef HAVE_STAB_H */

/* Define if you have the <sys/fsid.h> header file. */
/* #undef HAVE_SYS_FSID_H */

/* Define if you have the <sys/fstyp.h> header file. */
/* #undef HAVE_SYS_FSTYP_H */

/* Define if you have the <sys/mnttab.h> header file. */
/* #undef HAVE_SYS_MNTTAB_H */

/* Define if you have the <sys/proc.h> header file. */
/* #undef HAVE_SYS_PROC_H */

/* Define if you have the <sys/processor.h> header file. */
/* #undef HAVE_SYS_PROCESSOR_H */

/* Define if you have the <sys/procfs.h> header file. */
/* #undef HAVE_SYS_PROCFS_H */

/* Define if you have the <sys/resource.h> header file. */
/* #undef HAVE_SYS_RESOURCE_H */

/* Define if you have the <sys/statvfs.h> header file. */
/* #undef HAVE_SYS_STATVFS_H */

/* Define if you have the <sys/swap.h> header file. */
/* #undef HAVE_SYS_SWAP_H */

/* Define if you have the <sys/sysinfo.h> header file. */
/* #undef HAVE_SYS_SYSINFO_H */

/* Define if you have the <sys/uio.h> header file. */
/* #undef HAVE_SYS_UIO_H */

/* Define if you have the <sys/var.h> header file. */
/* #undef HAVE_SYS_VAR_H */

/* Define if you have the <sys/vfs.h> header file. */
/* #undef HAVE_SYS_VFS_H */

/* Define if you have the <utmp.h> header file. */
/* #undef HAVE_UTMP_H */

/* Define if you have the <utmpx.h> header file. */
/* #undef HAVE_UTMPX_H */

/* Define if you have the <vm/page.h> header file. */
/* #undef HAVE_VM_PAGE_H */

/*
 * Define this if the OS supports a "local" Subagent mechanism as
 * Windows/VxWorks/pSOS/etc. do.  This is NOT the same as "shared
 * library" subagents supported on most UNIX systems.
 */ 
#define SR_SUPPORT_LOCAL_SUBS 1

/*
 * Define this if the OS supports a shared library subagents using
 * dlopen and family.  MAKE SURE YOU SET SR_DLOPEN_FLAGS either to
 * 1 or 2 if you define SR_SUPPORT_SHLIB_SUBS.
 */
/* #define SR_SUPPORT_SHLIB_SUBS 1 */

/*
 * Set one of these depending on whether the flags sent to dlopen are
 * RTLD_LAZY or RTLD_NOW.
 */
/* #define SR_USE_RTLD_LAZY 1 */
/* #undef SR_USE_RTLD_NOW */

/*
 * Set this if you need to have Master Agent symbols (such as do_trap())
 * put into the shlib library in order to avoid conflict with what's
 * in the EMANATE libraries.  This will only affect those platforms
 * which define SR_SUPPORT_SHLIB_SUBS.
 */
/* #undef SR_NEED_MASTER_SYMS */

#ifndef SR_BSD
#define SR_BSD
#endif /* SR_BSD */

/* Define if you have the atoi function */
#define HAVE_ATOI 1

/*
 * Some operating systems croak if you pass into select a timeout
 * parameter (struct) with values greater than expected.  Set those
 * values here.
 */
#define SR_SELECT_MAX_TIMEOUT_SEC 1000000
#define SR_SELECT_MAX_TIMEOUT_USEC 1000000


#ifdef  __cplusplus
}
#endif	/* __cplusplus */

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_VXWORKS_H */
