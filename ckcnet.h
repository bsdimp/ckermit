/* ckcnet.h -- Symbol and macro definitions for C-Kermit network support */

/*
  Author: Frank da Cruz (fdc@columbia.edu, FDCCU@CUVMA.BITNET),
  Columbia University Academic Information Systems, New York City.

  Copyright (C) 1985, 1994, Trustees of Columbia University in the City of New
  York.  The C-Kermit software may not be, in whole or in part, licensed or
  sold for profit as a software product itself, nor may it be included in or
  distributed with commercial products or otherwise distributed by commercial
  concerns to their clients or customers without written permission of the
  Office of Kermit Development and Distribution, Columbia University.  This
  copyright notice must not be removed, altered, or obscured.
*/
#ifndef CKCNET_H
#define CKCNET_H

/* Network types */

#define NET_NONE 0			/* None */
#define NET_TCPB 1			/* TCP/IP Berkeley (socket) */
#define NET_TCPA 2			/* TCP/IP AT&T (streams) */
#define NET_SX25 3			/* SUNOS SunLink X.25 */
#define NET_DEC  4			/* DECnet */
#define NET_VPSI 5			/* VAX PSI */
#define NET_PIPE 6			/* LAN Manager Named Pipe */
#define NET_VX25 7			/* Stratus VOS X.25 */
#define NET_BIOS 8			/* IBM NetBios */

#ifdef OS2				/* In OS/2, only the 32-bit */
#ifndef __32BIT__			/* version gets NETBIOS */
#ifdef CK_NETBIOS
#undef CK_NETBIOS
#endif /* CK_NETBIOS */
#endif /* __32BIT__ */
#endif /* OS2 */

#ifdef NPIPE				/* For items in common to */
#define NPIPEORBIOS			/* Named Pipes and NETBIOS */
#endif /* NPIPE */
#ifdef CK_NETBIOS
#ifndef NPIPEORBIOS
#define NPIPEORBIOS
#endif /* NPIPEORBIOS */
#endif /* CK_NETBIOS */

/* Network virtual terminal protocols */

#define NP_NONE 0			/* None (async) */
#define NP_TELNET 1			/* TCP/IP telnet */
#define NP_VTP 2			/* ISO Virtual Terminal Protocol */
#define NP_X3 3				/* CCITT X.3 */
#define NP_X28 4			/* CCITT X.28 */
#define NP_X29 5			/* CCITT X.29 */
#define NP_CTERM 20			/* DEC CTERM */
#define NP_LAT 21			/* DEC LAT */
/* others here... */

/* TELNET Newline Mode */
       
#define TNL_CR     0			/* CR sends bare carriage return */
#define TNL_CRNUL  1			/* CR and NUL */
#define TNL_CRLF   2			/* CR and LF */

#ifdef COMMENT /* no longer used but might come in handy again later... */
/*
  CK_READ0 can (and should) be defined if and only if:
  (a) read(fd,&x,0) can be used harmlessly on a TCP/IP socket connection.
  (b) read(fd,&x,0) returns 0 if the connection is up, -1 if it is down.
*/
#ifndef CK_READ0
#ifdef TCPSOCKET
#ifdef SUNOS41				/* It works in SunOS 4.1 */
#define CK_READ0
#else
#ifdef NEXT				/* and NeXTSTEP */
#define CK_READ0
#endif /* NEXT */
#endif /* SUNOS41 */
#endif /* TCPSOCKET */
#endif /* CK_READ0 */
#endif /* COMMENT */

/* Basic network function prototypes, common to all. */

_PROTOTYP( int netopen, (char *, int *, int) );
_PROTOTYP( int netclos, (void) );
_PROTOTYP( int netflui, (void) );
_PROTOTYP( int nettchk, (void) );
_PROTOTYP( int netbreak, (void) );
_PROTOTYP( int netinc, (int) );
_PROTOTYP( int nettol, (char *, int) );
_PROTOTYP( int nettoc, (char) );

/*
  SunLink X.25 support by Marcello Frutig, Catholic University,
  Rio de Janeiro, Brazil, 1990.

  Maybe this can be adapted to VAX PSI and other X.25 products too.
*/
#ifndef SUNOS4				/* Only valid for SUNOS4 */
#ifndef SOLARIS
#ifdef SUNX25
#undef SUNX25
#endif /* SUNX25 */
#endif /* SOLARIS */
#endif /* SUNOS4 */

#ifdef STRATUSX25
#define ANYX25
#define MAX_USER_DATA 128 /* SUN defines this in a header file, I believe. */
#endif /* STRATUSX25 */

#ifdef SUNX25
#define ANYX25
#endif /* SUNX25 */

#ifdef ANYX25
#ifndef NETCONN				/* ANYX25 implies NETCONN */
#define NETCONN
#endif /* NETCONN */

#define MAXPADPARMS                18	/* Number of PAD parameters */
#define MAXCUDATA		   12	/* Max length of X.25 call user data */
#define X29PID			    1   /* X.29 protocol ID */
#define X29PIDLEN		    4   /* X.29 protocol ID length */

#define X29_SET_PARMS               2
#define X29_READ_PARMS              4
#define X29_SET_AND_READ_PARMS      6
#define X29_INVITATION_TO_CLEAR     1
#define X29_PARAMETER_INDICATION    0
#define X29_INDICATION_OF_BREAK     3
#define X29_ERROR                   5

#define INVALID_PAD_PARM            1

#define PAD_BREAK_CHARACTER         0

#define PAD_ESCAPE                  1
#define PAD_ECHO                    2
#define PAD_DATA_FORWARD_CHAR       3
#define PAD_DATA_FORWARD_TIMEOUT    4
#define PAD_FLOW_CONTROL_BY_PAD     5
#define PAD_SUPPRESSION_OF_SIGNALS  6
#define PAD_BREAK_ACTION            7
#define PAD_SUPPRESSION_OF_DATA     8
#define PAD_PADDING_AFTER_CR        9
#define PAD_LINE_FOLDING           10
#define PAD_LINE_SPEED             11
#define PAD_FLOW_CONTROL_BY_USER   12
#define PAD_LF_AFTER_CR            13
#define PAD_PADDING_AFTER_LF       14
#define PAD_EDITING                15
#define PAD_CHAR_DELETE_CHAR       16
#define PAD_BUFFER_DELETE_CHAR     17
#define PAD_BUFFER_DISPLAY_CHAR    18

#define MAXIX25 MAX_USER_DATA*7
#define MAXOX25 MAX_USER_DATA
#endif /* ANYX25 */

#ifdef SUNX25
#include <sys/ioctl.h>			/* X.25 includes, Sun only */
#include <sys/param.h>
#include <sys/systm.h>
#ifndef SOLARIS
#include <sys/mbuf.h>
#endif /* SOLARIS */
#include <sys/socket.h>
#include <sys/protosw.h>
#ifdef SOLARIS
#include <sys/sockio.h>
#else
#include <sys/domain.h>
#endif /* SOLARIS */
#include <sys/socketvar.h>
#include <net/if.h>
#include <sundev/syncstat.h>
#include <netx25/x25_pk.h>
#include <netx25/x25_ctl.h>
#include <netx25/x25_ioctl.h>
#endif /* SUNX25 */

#ifdef ANYX25

/* C-Kermit X.3 / X.25 / X.29 / X.121 support functions */

_PROTOTYP( VOID shopad, (void) );
_PROTOTYP( VOID shox25, (void) );
_PROTOTYP( VOID initpad, (void) );
_PROTOTYP( VOID setpad, (CHAR *, int) );
_PROTOTYP( VOID readpad, (CHAR *, int, CHAR *) );
_PROTOTYP( int qbitpkt, (CHAR *, int) );
_PROTOTYP( VOID setqbit, (void) );
_PROTOTYP( VOID resetqbit, (void) );
_PROTOTYP( VOID breakact, (void) );
_PROTOTYP( int pkx121, (char *, CHAR *) );
_PROTOTYP( SIGTYP x25oobh, (int) );
_PROTOTYP( int x25diag, (void) );
_PROTOTYP( int x25intr, (char) );
_PROTOTYP( int x25reset, (char, char) );
_PROTOTYP( int x25clear, (void) );
_PROTOTYP( int x25stat, (void) );
_PROTOTYP( int x25in, (int, CHAR *) );
_PROTOTYP( int setpadp, (void) );
_PROTOTYP( int setx25, (void) );
_PROTOTYP( int x25xin, (int, CHAR *) );
_PROTOTYP( int x25inl, (CHAR *, int, int, CHAR) );

#endif /* ANYX25 */

/* CMU-OpenVMS/IP */

#ifdef CMU_TCPIP			/* CMU_TCPIP implies TCPSOCKET */
#ifndef TCPSOCKET
#define TCPSOCKET
#endif /* TCPSOCKET */
#ifndef TCPIPLIB
#define TCPIPLIB
#endif /* TCPIPLIB */
#endif /* CMU_TCPIP */

/* DEC TCP/IP for (Open)VMS, previously known as UCX */

#ifdef DEC_TCPIP			/* DEC_TCPIP implies TCPSOCKET */
#ifndef TCPSOCKET
#define TCPSOCKET
#endif /* TCPSOCKET */
#ifndef TCPIPLIB
#define TCPIPLIB
#endif /* TCPIPLIB */
#endif /* DEC_TCPIP */

/* TGV/SRI MultiNet, TCP/IP for VAX/VMS */

#ifdef MULTINET				/* MULTINET implies TCPSOCKET */
#ifndef TCPSOCKET
#define TCPSOCKET
#endif /* TCPSOCKET */
#ifndef TCPIPLIB
#define TCPIPLIB
#endif /* TCPIPLIB */
#ifndef TGVORWIN			/* MULTINET and WINTCP */
#define TGVORWIN			/* share a lot of code... */
#endif /* TGVORWIN */
#endif /* MULTINET */

/* Wollongong TCP/IP for VAX/VMS */

#ifdef WINTCP				/* WINTCP implies TCPSOCKET */
#ifndef TCPSOCKET
#define TCPSOCKET
#endif /* TCPSOCKET */
#ifndef TCPIPLIB
#define TCPIPLIB
#endif /* TCPIPLIB */
#ifndef TGVORWIN			/* WINTCP and MULTINET */
#define TGVORWIN			/* share a lot of code... */
#endif /* TGVORWIN */
#endif /* WINTCP */

/* Wollongong TCP/IP for AT&T Sys V */

#ifdef WOLLONGONG			/* WOLLONGONG implies TCPSOCKET */
#ifndef TCPSOCKET			/* Don't confuse WOLLONGONG */
#define TCPSOCKET			/* (which is for UNIX) with */
#endif /* TCPSOCKET */			/* WINTCP, which is for VMS! */
#endif /* WOLLONGONG */

#ifdef EXCELAN				/* EXCELAN implies TCPSOCKET */
#ifndef TCPSOCKET
#define TCPSOCKET
#endif /* TCPSOCKET */
#endif /* EXCELAN */

#ifdef INTERLAN				/* INTERLAN implies TCPSOCKET */
#ifndef TCPSOCKET
#define TCPSOCKET
#endif /* TCPSOCKET */
#endif /* INTERLAN */

/* Telnet protocol */

#ifdef TCPSOCKET			/* TCPSOCKET implies TNCODE */
#ifndef TNCODE				/* Which means... */
#define TNCODE				/* Compile in telnet code */
#endif /* TNCODE */
#endif /* TCPSOCKET */

#ifdef SUNX25				/* SUNX25 implies TCPSOCKET */
#ifndef TCPSOCKET			/* But doesn't imply TNCODE */
#define TCPSOCKET
#endif /* TCPSOCKET */
#endif /* SUNX25 */

/* This is the TCPSOCKET section... */

#ifdef TCPSOCKET

#ifndef NETCONN				/* TCPSOCKET implies NETCONN */
#define NETCONN
#endif /* NETCONN */

/* BSD sockets library header files */

#ifdef UNIX				/* UNIX section */

#ifdef SVR4
/*
  These suggested by Rob Healey, rhealey@kas.helios.mn.org, to avoid
  bugs in Berkeley compatibility library on Sys V R4 systems, but untested
  by me (fdc).  Remove this bit if it gives you trouble.
  (Later corrected by Marc Boucher <mboucher@iro.umontreal.ca> because
  bzero/bcopy are not argument-compatible with memset/memcpy|memmove.)
*/
#define bzero(s,n) memset(s,0,n)
#ifdef SOLARIS
#ifdef SUNX25
#undef bzero
/*
  WOULD YOU BELIEVE... That the Solaris X.25 /opt/SUNWcomm/lib/libsockx25
  library references bzero, even though the use of bzero is forbidden in
  Solaris?  Look for the function definition in ckcnet.c.
*/
_PROTOTYP( void bzero, (char *, int) );
#endif /* SUNX25 */
#define bcopy(h,a,l) memcpy(a,h,l)
#else
#define bcopy(h,a,l) memmove(a,h,l)
#endif /* SOLARIS */
#else /* !SVR4 */
#ifdef PTX				/* Sequent DYNIX PTX 1.3 */
#define bzero(s,n) memset(s,0,n)
#define bcopy(h,a,l) memcpy(a,h,l)
#endif /* PTX */
#endif /* SVR4 */

#ifdef INTERLAN				/* Racal-Interlan TCP/IP */
#include <interlan/socket.h>
#include <interlan/il_types.h>
#include <interlan/telnet.h>
#include <interlan/il_errno.h>
#include <interlan/in.h>
#include <interlan/telnet.h>		/* Why twice ? ? ? */
#else					/* Normal BSD TCP/IP library */
#ifndef HPUX
#include <arpa/telnet.h>
#endif /* HPUX */
#include <sys/socket.h>
#ifdef WOLLONGONG
#include <sys/in.h>
#else
#include <netinet/in.h>
#endif /* WOLLONGONG */
#endif /* INTERLAN */

#ifndef EXCELAN
#include <netdb.h>
#ifndef INTERLAN
#ifdef WOLLONGONG
#define minor				/* Do not include <sys/macros.h> */
#include <sys/inet.h>
#else
#ifndef OXOS
#ifndef HPUX
#include <arpa/inet.h>
#endif /* HPUX */
#else /* OXOS */
/* In too many releases of X/OS, <arpa/inet.h> declares inet_addr() as
 * ``struct in_addr''.  This is definitively wrong, and could cause
 * core dumps.  Instead of including that bad file, inet_addr() is
 * correctly declared here.  Of course, all the declarations done there
 * has been copied here.
 */
unsigned long inet_addr();
char	*inet_ntoa();
struct	in_addr inet_makeaddr();
unsigned long inet_network();
#endif /* OXOS */
#endif /* WOLLONGONG */
#endif /* INTERLAN */
#endif /* EXCELAN */

#ifdef EXCELAN				/* Excelan TCP/IP */
#ifndef bzero
#define bzero(s,n) memset(s,0,n)
#endif /* bzero */
#ifndef bcopy
#define bcopy(h,a,l) memcpy(a,h,l)
#endif /* bcopy */
#include <ex_errno.h>
#endif /* EXCELAN */

#ifdef I386IX				/* Interactive Sys V R3 network. */
/* #define TELOPTS */			/* This might need defining. */
#define ORG_NLONG ENAMETOOLONG		/* Resolve conflicting symbols */
#undef ENAMETOOLONG			/* in <errno.h> and <net/errno.h> */
#define ORG_NEMPTY ENOTEMPTY
#undef ENOTEMPTY
#include <net/errno.h>
#undef ENAMETOOLONG
#define ENAMETOOLONG ORG_NLONG
#undef ENOTEMPTY
#define ENOTEMPTY ORG_NEMPTY
#endif /* I386IX */
/*
  Data type of the inet_addr() function...
  We define INADDRX if it is of type struct inaddr.
  If it is undefined, unsigned long is assumed.
  Look at <arpa/inet.h> to find out.  The following known cases are
  handled here.  Other systems that need it can be added here, or else
  -DINADDRX can be included in the CFLAGS on the cc command line.
*/
#ifdef DGUX540				/* Data General UX 5.4 */
#ifndef DGUX543				/* But not DG/US 5.4R3.00 */
#define INADDRX
#endif /* DGUX543 */
#endif /* DGUX540 */

#ifdef DU2				/* DEC Ultrix 2.0 */
#define INADDRX
#endif /* DU2 */

#else /* Not UNIX */

#ifdef VMS				/* (Open)VMS section */

#ifdef WINTCP				/* TWG WIN/TCP = PathWay for VMS */
#include <errno.h>
#include "twg$tcp:[netdist.include.sys]types.h"
#include "twg$tcp:[netdist.include.sys]socket.h"
#include "twg$tcp:[netdist.include]netdb.h"
#include "twg$tcp:[netdist.include.sys]domain.h"
#include "twg$tcp:[netdist.include.sys]protosw.h"
#include "twg$tcp:[netdist.include.netinet]in.h"
#include "twg$tcp:[netdist.include.sys]ioctl.h"
#endif /* WINTCP */

#ifdef MULTINET				/* TGV MultiNet */
#include "multinet_root:[multinet.include]errno.h"
#include "multinet_root:[multinet.include.sys]types.h"
#include "multinet_root:[multinet.include.sys]socket.h"
#include "multinet_root:[multinet.include]netdb.h"
#include "multinet_root:[multinet.include.netinet]in.h"
#include "multinet_root:[multinet.include.sys]ioctl.h"
#ifdef __DECC
/*
   If compiling under DEC C the socket calls must not be prefixed with
   DECC$.  This is done by using the compiler switch /Prefix=Ansi_C89.
   However, this causes some calls that should be prefixed to not be
   (which I think is a bug in the compiler - I've been told these calls
   are present in ANSI compilers).  At any rate, such calls are fixed
   here by explicitly prefixing them.
*/
#define close decc$close
#define alarm decc$alarm
#endif /* __DECC */
#endif /* MULTINET */

#ifdef DEC_TCPIP
#include <in.h>
#include <netdb.h>
#include <socket.h>
#include "ckvioc.h"
#define socket_errno errno
#define bzero(s,n) memset(s,0,n)
#define bcopy(h,a,l) memmove(a,h,l)
#define socket_read 	read
#define socket_write 	write
#define socket_ioctl	ioctl
#define socket_close    close
/*
  UCX supports select(), but does not provide the needed symbol and
  structure definitions in any header file, so ...
*/
#include <types.h>
#ifndef NBBY
/*-
 * Copyright (c) 1982, 1986, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.1 (Berkeley) 6/2/93
 */

#define	NBBY	8		/* number of bits in a byte */

/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	256
#endif

typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */

#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define	FD_COPY(f, t)	bcopy(f, t, sizeof(*(f)))
#define	FD_ZERO(p)	bzero(p, sizeof(*(p)))

#endif /* !NBBY */
#endif /* DEC_TCPIP */

#ifdef CMU_TCPIP
#include <types.h>
#include <in.h>
#include <netdb.h>
#include <socket.h>
#include <ioctl.h>
#include "ckvioc.h"
#define socket_errno errno
#define bzero(s,n) memset(s,0,n) 
#define bcopy(h,a,l) memmove(a,h,l)
/*
 * Routines supplied in LIBCMU.OLB
 */
#define socket_ioctl	ioctl
#define socket_read 	cmu_read
#define socket_write 	cmu_write
#define socket_close    cmu_close
#endif /* CMU_TCPIP */

#else /* Not VMS */

#ifdef OS2
#include "ckonet.h"
#endif /* OS2 */

#ifdef STRATUS  /* Stratus VOS using OS TCP/IP products S235, S236, S237 */
#include <tcp_socket.h>
/* This gets used some places when TCPSOCKET is defined. */
/* OS TCP provides bzero(), but not bcopy()... go figure. */
#define bcopy(s,d,z) memcpy(d,s,z)
#endif /* STRATUS */

#endif /* VMS */
#endif /* UNIX */
#endif /* TCPSOCKET */

#ifdef TNCODE				/* If we're compiling telnet code... */
/*
  Make sure telnet symbols are defined; can't rely on library header files
  for any of them.
*/
#ifndef IAC				/* First the telnet commands */
#define IAC 255
#endif /* IAC */
#ifndef DONT
#define DONT 254
#endif /* DONT */
#ifndef DO
#define DO 253
#endif /* DO */
#ifndef WONT
#define WONT 252
#endif /* WONT */
#ifndef WILL
#define WILL 251
#endif /* WILL */
#ifndef SB
#define SB 250
#endif /* SB */
#ifndef BREAK
#define BREAK 243
#endif /* BREAK */
#ifndef SE
#define SE 240
#endif /* SE */

#ifndef TELOPT_ECHO			/* Then the options */
#define TELOPT_ECHO 1
#endif /* TELOPT_ECHO */
#ifndef TELOPT_SGA
#define	TELOPT_SGA 3
#endif /* TELOPT_SGA */
#ifndef TELOPT_STATUS
#define	TELOPT_STATUS 5
#endif /* TELOPT_STATUS */
#ifndef TELOPT_TTYPE
#define	TELOPT_TTYPE 24
#endif /* TELOPT_TTYPE */
#ifndef NTELOPTS
#define	NTELOPTS 24
#endif /* NTELOPTS */

/* Systems where we know we can define NAWS automatically. */

/*
   NOTE: in the future we should separate the notion of TELNET NAWS
   negotiation from "finding out my own screen size".  This might be as
   simple as moving the following section out of the TNCODE section,
   but then its name will be confusing, etc.
*/
#ifndef NONAWS				/* Unless they said not to... */
#ifndef CK_NAWS				/* and it's not already enabled... */
#ifdef SUNOS41				/* SunOS 4.1 */
#define CK_NAWS
#else
#ifdef NEXT				/* NeXTSTEP */
#define CK_NAWS
#else
#ifdef BSD44				/* 4.4BSD */
#define CK_NAWS
#else
#ifdef OS2				/* OS/2 */
#define CK_NAWS
#else
#ifdef SVR4				/* System V R4 */
#define CK_NAWS
#else
#ifdef OSF				/* OSF/1 */
#define CK_NAWS
#else
#ifdef VMS				/* VMS */
#define CK_NAWS
#else
#ifdef AIXRS				/* AIX/6000 */
#define CK_NAWS
#else
#ifdef SOLARIS				/* Solaris 2.x */
#define CK_NAWS
#else
#ifdef DU4				/* DEC ULTRIX 4.x */
#define CK_NAWS
#else
#ifdef QNX				/* DEC ULTRIX 4.x */
#define CK_NAWS
#endif /* QNX */
#endif /* DU4 */
#endif /* SOLARIS */
#endif /* AIXRS */
#endif /* VMS */
#endif /* OSF */
#endif /* SVR4 */
#endif /* OS2 */
#endif /* BSD44 */
#endif /* NEXT */
#endif /* SUNOS41 */
#endif /* CK_NAWS */
#endif /* NONAWS */

#ifdef CK_NAWS
#ifndef TELOPT_NAWS
#define TELOPT_NAWS 31
#endif /* TELOPT_NAWS */
#endif /* CK_NAWS */

/* Telnet protocol functions defined in C-Kermit */

_PROTOTYP( int tn_ini, (void) );	/* Telnet protocol support */
_PROTOTYP( int tn_sopt, (int, int) );
_PROTOTYP( int tn_doop, (CHAR, int, int (*)(int) ) );
_PROTOTYP( int tn_sttyp, (void) );
_PROTOTYP( int tnsndbrk, (void) );

#endif /* TNCODE */

#endif /* CKCNET_H */
