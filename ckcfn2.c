#define NEWDPL

/*  C K C F N 2  --  System-independent Kermit protocol support functions... */

/*  ...Part 2 (continued from ckcfns.c)  */

/*
  Author: Frank da Cruz <fdc@columbia.edu>,
  Columbia University Academic Information Systems, New York City.

  Copyright (C) 1985, 1996, Trustees of Columbia University in the City of New
  York.  The C-Kermit software may not be, in whole or in part, licensed or
  sold for profit as a software product itself, nor may it be included in or
  distributed with commercial products or otherwise distributed by commercial
  concerns to their clients or customers without written permission of the
  Office of Kermit Development and Distribution, Columbia University.  This
  copyright notice must not be removed, altered, or obscured.
*/
/*
 Note -- if you change this file, please amend the version number and date at
 the top of ckcfns.c accordingly.
*/

#include "ckcsym.h"			/* Compilation options */
#include "ckcdeb.h"			/* Debugging and other symbols */
#include "ckcasc.h"			/* ASCII symbols */
#include "ckcker.h"			/* Kermit symbols */
#include "ckcxla.h"			/* Translation */

#ifdef TCPSOCKET			/* For TELNET business in spack() */
#ifndef NP_TELNET			/* Avoid dragging in ckcnet.h */
#define NP_TELNET  1
#define TNL_CR     0			/* CR sends bare carriage return */
#define TNL_CRNUL  1			/* CR and NUL */
#define TNL_CRLF   2			/* CR and LF */
#define TNL_LF     3			/* LF instead of CR */
#endif /* NP_TELNET */
extern int tn_nlm, ttnproto;
extern int me_binary, tn_b_nlm;
#endif /* TCPSOCKET */

#ifdef DYNAMIC
extern struct pktinfo *s_pkt;		/* array of pktinfo structures */
extern struct pktinfo *r_pkt;		/* array of pktinfo structures */
#else
extern struct pktinfo s_pkt[];		/* array of pktinfo structures */
extern struct pktinfo r_pkt[];		/* array of pktinfo structures */
#endif /* DYNAMIC */

extern int sseqtbl[], rseqtbl[], sbufuse[], sacktbl[], wslots, winlo,
  sbufnum, rbufnum, pktpaus;

extern int ttprty;			/* from ck*tio.c */
extern int autopar;

extern int spsiz, spmax, rpsiz, timint, timef, npad, bestlen, maxsend;
extern int rpt, rptq, rptflg, capas, spsizf, en_fin, tsecs, network, flow;
extern int pktnum, sndtyp, rcvtyp, bctr, bctu, bctl, rsn, rln, maxtry, size;
extern int osize, maxsize, spktl, rpktl, nfils, stdouf, warn, parity;
extern int turn, turnch,  delay, displa, pktlog, tralog, seslog, xflg, mypadn;
extern int hcflg, local, server, cxseen, czseen, discard, slostart;
extern int nakstate, quiet, success, xitsta, what;
extern int spackets, rpackets, timeouts, retrans, crunched, urpsiz;

extern long filcnt, filrej, ffc, flci, flco, tlci, tlco, tfc, speed;

extern char *cmarg, filnam[], *hlptxt;

extern CHAR padch, mypadc, eol, seol, ctlq, sstate;
extern CHAR *recpkt, *data, myinit[];
extern CHAR *srvptr, stchr, mystch, *rdatap;
extern CHAR padbuf[];
#ifdef pdp11
  extern CHAR srvcmd[];
  extern CHAR *pktmsg;
#else
#ifdef DYNAMIC
  extern CHAR *srvcmd;
  extern CHAR *pktmsg;
#else
  extern CHAR srvcmd[];
  extern CHAR pktmsg[];
#endif /* DYNAMIC */
#endif /* pdp11 */

int numerrs = 0;			/* Number of packet errors so far */
int rcvtimo = 0;			/* Timeout for receiving a packet */

#ifdef CK_TIMERS
int rrttbl[64], srttbl[64];		/* Packet timestamp tables */
extern int rttflg;
#define RTT_SCALE 1000
long
  rttsamples,				/* Round trip time samples */
  rttdelay,				/* RTT delay */
  pktintvl,				/* Interpacket arrival time */
  rttvariance,				/* RTT variance */
  rttstddev;				/* RTT standard deviation */
#endif /* CK_TIMERS */

static CHAR p_tbl[] = {			/* Even parity table for dopar(). */
    (CHAR) '\000',			/* ANSI C casts '\ooo' constants  */
    (CHAR) '\201',			/* to signed char, so we have to  */
    (CHAR) '\202',			/* cast back to unsigned char...  */
    (CHAR) '\003',
    (CHAR) '\204',
    (CHAR) '\005',
    (CHAR) '\006',
    (CHAR) '\207',
    (CHAR) '\210',
    (CHAR) '\011',
    (CHAR) '\012',
    (CHAR) '\213',
    (CHAR) '\014',
    (CHAR) '\215',
    (CHAR) '\216',
    (CHAR) '\017',
    (CHAR) '\220',
    (CHAR) '\021',
    (CHAR) '\022',
    (CHAR) '\223',
    (CHAR) '\024',
    (CHAR) '\225',
    (CHAR) '\226',
    (CHAR) '\027',
    (CHAR) '\030',
    (CHAR) '\231',
    (CHAR) '\232',
    (CHAR) '\033',
    (CHAR) '\234',
    (CHAR) '\035',
    (CHAR) '\036',
    (CHAR) '\237',
    (CHAR) '\240',
    (CHAR) '\041',
    (CHAR) '\042',
    (CHAR) '\243',
    (CHAR) '\044',
    (CHAR) '\245',
    (CHAR) '\246',
    (CHAR) '\047',
    (CHAR) '\050',
    (CHAR) '\251',
    (CHAR) '\252',
    (CHAR) '\053',
    (CHAR) '\254',
    (CHAR) '\055',
    (CHAR) '\056',
    (CHAR) '\257',
    (CHAR) '\060',
    (CHAR) '\261',
    (CHAR) '\262',
    (CHAR) '\063',
    (CHAR) '\264',
    (CHAR) '\065',
    (CHAR) '\066',
    (CHAR) '\267',
    (CHAR) '\270',
    (CHAR) '\071',
    (CHAR) '\072',
    (CHAR) '\273',
    (CHAR) '\074',
    (CHAR) '\275',
    (CHAR) '\276',
    (CHAR) '\077',
    (CHAR) '\300',
    (CHAR) '\101',
    (CHAR) '\102',
    (CHAR) '\303',
    (CHAR) '\104',
    (CHAR) '\305',
    (CHAR) '\306',
    (CHAR) '\107',
    (CHAR) '\110',
    (CHAR) '\311',
    (CHAR) '\312',
    (CHAR) '\113',
    (CHAR) '\314',
    (CHAR) '\115',
    (CHAR) '\116',
    (CHAR) '\317',
    (CHAR) '\120',
    (CHAR) '\321',
    (CHAR) '\322',
    (CHAR) '\123',
    (CHAR) '\324',
    (CHAR) '\125',
    (CHAR) '\126',
    (CHAR) '\327',
    (CHAR) '\330',
    (CHAR) '\131',
    (CHAR) '\132',
    (CHAR) '\333',
    (CHAR) '\134',
    (CHAR) '\335',
    (CHAR) '\336',
    (CHAR) '\137',
    (CHAR) '\140',
    (CHAR) '\341',
    (CHAR) '\342',
    (CHAR) '\143',
    (CHAR) '\344',
    (CHAR) '\145',
    (CHAR) '\146',
    (CHAR) '\347',
    (CHAR) '\350',
    (CHAR) '\151',
    (CHAR) '\152',
    (CHAR) '\353',
    (CHAR) '\154',
    (CHAR) '\355',
    (CHAR) '\356',
    (CHAR) '\157',
    (CHAR) '\360',
    (CHAR) '\161',
    (CHAR) '\162',
    (CHAR) '\363',
    (CHAR) '\164',
    (CHAR) '\365',
    (CHAR) '\366',
    (CHAR) '\167',
    (CHAR) '\170',
    (CHAR) '\371',
    (CHAR) '\372',
    (CHAR) '\173',
    (CHAR) '\374',
    (CHAR) '\175',
    (CHAR) '\176',
    (CHAR) '\377'
};

/* CRC generation tables */

long crcta[16] = { 0L, 010201L, 020402L, 030603L, 041004L,
  051205L, 061406L, 071607L, 0102010L, 0112211L, 0122412L, 0132613L, 0143014L,
  0153215L, 0163416L, 0173617L
};

long crctb[16] = { 0L, 010611L, 021422L, 031233L, 043044L,
  053655L, 062466L, 072277L, 0106110L, 0116701L, 0127532L, 0137323L, 0145154L,
  0155745L, 0164576L, 0174367L
};

#ifdef CK_TIMERS
/*
  Round-trip timer calculations adapted from Tim Kientzle's article,
  "Improving Kermit Performance", Dr Dobb's Journal, February 1996.
*/

/*  R T T I N I T  --  Initialize timers at start of transaction  */

VOID
rttinit() {				/* Initialize round-trip timing */
    int i;

    rttsamples  = 0L;			/* Samples (packets) */
    rttvariance = 0L;			/* Variance in delay */
    rttdelay    = (long) timint * RTT_SCALE; /* Delay */
    pktintvl    = (long) timint * RTT_SCALE; /* Delay */
    rttstddev   = (long) timint * RTT_SCALE; /* Standard deviation of delay */

    /* Tables of timestamps indexed by packet sequence number */

    for (i = 0; i < 64; i++) {		
	rrttbl[i] = -1;			/* Time each packet was received */
	srttbl[i] = -1;			/* Time each packet was sent */
    }
    rcvtimo = timint;			/* Initial timeout is what user said */
}

/*  G E T R T T  --  Get packet round trip time  */
/*
  Call with nakstate == 0 if file sender, nonzero if receiver,
  and n == packet sequence number of the packet we just received.
 
  Returns:
  -1 on failure with rcvtimo set to timint (what the user said), or:
   0 on success with rcvtimo set to dynamically calculated value:
     1 <= rcvtimo <= timint * 3.
*/
int
getrtt(nakstate, n) int nakstate, n; {
    long rttdiff;
    extern int mintime, maxtime;

    int x, y, yy, z = 0, zz = 0;	/* How long did it take to get here? */

    rcvtimo = timint;			/* Default timeout is what user said */

    if (timint == 0)			/* We're not timing out. */
      return(0);

    if (!rttflg)			/* Not supposed to be doing this? */
      return(-1);			/*  So don't */

    if (!RTT_SCALE)			/* Paranoia... */
      return(-1);

    /* rtimer() (reset timer) is not called until 1st data packet */
    /* S (F [ A ] D* Z)* B */

    /* NOTE: we calculate both the round-trip time AND the packet */
    /* arrival rate.  We don't use the RTT for anything, we just display it. */
    /* Timeouts are based on the packet arrival rate. */

    if (spackets > 3) {			/* Don't start till 4th packet */
	if (nakstate) {			/* File receiver */
	    x = rrttbl[n];		     /* Time when I got packet n */
	    y = rrttbl[n > 0 ? n - 1 : 63];  /* Time when I got packet n-1 */
	    yy = srttbl[n > 0 ? n - 1 : 63]; /* Time when I sent ACK(n-1) */
	    if (x > -1 && y > -1) {	/* Be careful */
		z = x - y;		/* Packet rate */
		zz = x - yy;		/* Round trip time */
		debug(F101,"RTT RECV","",z);
	    } else {			/* This shouldn't happen */
		debug(F101,"RTT RECV ERROR spackets","",spackets);
		debug(F101,"RTT RECV ERROR sequence","",n);
		return(-1);
	    }
	} else {			/* File sender */
	    x = rrttbl[n];		/* Time when I got ACK(n) */
	    y = rrttbl[n > 0 ? n - 1 : 63]; /* Time when I got packet n-1 */
	    yy = srttbl[n];		/* Time when I sent n */
	    if (x > -1 && y > -1) {
		z = x - y;		/* Packet rate */
		zz = x - yy;		/* Round trip time */
		debug(F101,"RTT SEND","",z);
	    } else {
		debug(F100,"RTT SEND ERROR","",0);
		return(-1);
	    }
	}
	if (z < 1)			/* For fast connections */
	  z = RTT_SCALE / 2;		/* Convert to scale... */
	else
	  z *= RTT_SCALE;

	if (zz < 1)			/* For fast connections */
	  zz = RTT_SCALE / 2;		/* Convert to scale... */
	else
	  zz *= RTT_SCALE;

	rttdelay = zz;			/* Round trip time of this packet */
	if (rttsamples++ == 0L) {	/* First sample */
	    pktintvl = z;
	    rttdiff = 0;
	} else {			/* Subsequent samples */
	    long oldavg = pktintvl;
	    long rttdiffsq;

	    if (rttsamples > 30)	/* Use real average for first 30 */
	      rttsamples = 30;		/*  then decaying average. */

	    /* Average delay, difference squared, variance, std deviation */

	    pktintvl += (z - pktintvl) / rttsamples;
	    rttdiffsq = (z - oldavg) * (z - oldavg);
	    rttvariance += (rttdiffsq - rttvariance) / rttsamples;
	    debug(F101,"RTT stddev1","",rttstddev);
	    if (rttstddev < 1L)		/* It can be zero, in which case */
	      rttstddev = RTT_SCALE / 3; /* set it to something small... */
	    rttstddev = (rttstddev + rttvariance / rttstddev) / 2;
	}
	debug(F101,"RTT stddev2","",rttstddev);
	debug(F101,"RTT delay  ","",pktintvl);
	rcvtimo = (pktintvl + (3L * rttstddev)) / RTT_SCALE + 1;
	if (rpackets < 32)		/* Allow for slow start */
	  rcvtimo += rcvtimo + 2;
	else if (rpackets < 64)
	  rcvtimo += rcvtimo / 2 + 1;
	if (rcvtimo < mintime)		/* Lower bound */
	  rcvtimo = mintime;
	if (maxtime > 0) {		/* User specified an upper bound */
	    if (rcvtimo > maxtime)
	      rcvtimo = maxtime;
	} else if (maxtime == 0) {	/* User didn't specify */
	    if (rcvtimo > timint * 6)
	      rcvtimo = timint * 6;
	}
	debug(F101,"RTT rcvtimo","",rcvtimo);
    }
    return(0);
}
#endif /* CK_TIMERS */

/*  I N P U T  --  Attempt to read packet number 'pktnum'.  */

/*
 This is the function that feeds input to Kermit's finite state machine,
 in the form of a character in the range 32-126, normally a packet type
 (uppercase letter) or pseudo-packet-type (lowercase letter).

 If a special start state is in effect, that state is returned as if it were
 the type of an incoming packet.
*/
int
input() {
    int type, acktype;			/* Received packet type */
    int x, y, k;			/* Workers */
    int z, pi, nf;			/* Worker, packet index, NAK flag */
    int nak2ack = 0;

    debug(F101,"input sstate","",sstate);
    debug(F101," nakstate","",nakstate);
    debug(F000," sndtyp","",sndtyp);

    while (1) {				/* Big loop... */

	if (sstate != 0) {		/* If a start state is in effect, */
	    type = sstate;		/* return it like a packet type, */
	    sstate = 0;			/* and then nullify it. */
	    numerrs = 0;		/* (PWP) no errors so far */
	    return(type);
	}
	
	if (nakstate) {			/* This section for file receiver. */

	    if (wslots > 1) {		/* If we're doing windows, */
		x = rseqtbl[winlo];	/* see if desired packet already in. */
		debug(F101," winlo","",winlo);
		debug(F101," rseqtbl[winlo]","",rseqtbl[winlo]);
		if (x > -1) {		/* Already there? */
		    if (r_pkt[x].pk_seq == winlo) { /* (double check) */
			rsn = winlo;	            /* Yes, return its info */
			debug(F101,"input return pre-stashed packet","",rsn);
			dumprbuf();
			rdatap = r_pkt[x].pk_adr;   /* like rpack would do. */
			rln = (int)strlen((char *) rdatap);
			type = r_pkt[x].pk_typ;
			break;
		    }
		}
	    }
	    type = rpack();	        /* Try to read a packet. */
	    debug(F111,"input recv",(char *) rdatap,(int) type);
	    while (type == 'e') {	/* Handle echoes */
		debug(F000,"echo discarded","",type);
		type = rpack();	
	    }		
#ifndef OLDCHKINT
	    if (type == 'z') {
		errpkt((CHAR *)"User cancelled.");
		strcpy((char *)pktmsg,"User cancelled.");
		type = 'E';
		break;
	    }
#endif /* OLDCHKINT */
	    if (type < -1) return('q'); /* Ctrl-C or connection lost */
	    if (type < 0) {		/* Receive window full */
		/* Another thing to do here would be to delete */
		/* the highest packet and NAK winlo.  But that */
		/* shouldn't be necessary since the other Kermit */
		/* should not have sent a packet outside the window. */
		debug(F101,"rpack receive window full","",0);
		dumprbuf();
		errpkt((CHAR *)"Receive window full.");
		strcpy((char *)pktmsg,"Receive window full.");
		type = 'E';
		break;
	    }
	    dumprbuf();

#ifdef OLDCHKINT
	    if (chkint() < 0) {		/* Check for console interrupts. */
		errpkt((CHAR *)"User cancelled.");
		strcpy((char *)pktmsg,"User cancelled.");
		type = 'E';
		break;
	    }
#endif /* OLDCHKINT */

	    if (type == 'E') {
		debug(F101,"input got E, nakstate","",nakstate);
		break;			/* Error packet */
	    }
	    if (type == 'Q') {		/* Crunched packet. */
		crunched++;
		numerrs++;
/*
  Packet arrived damaged.  It was most likely the packet we were expecting
  next, so we send a NAK for that packet.  Prior to 5A(189), we always
  NAK'd winlo here, but that was bad because if two (or more) different
  packets were damaged, we would keep NAKing the first one and never NAK the
  other ones, which could result in a lengthy series of timeouts.  Now we
  NAK the oldest as-yet-unNAK'd missing packet.
*/
#ifdef CK_TIMERS
		rcvtimo++;		/* Stretch the timeout a little */
#endif /* CK_TIMERS */
		z = (winlo + wslots) % 64;  /* Search from winlo to z */
		debug(F101,"ZZZ crunched z","",z);
		nf = 0;			    /* NAK flag not set yet */
		for (x = winlo; x != z; x = (x + 1) % 64) {
		    debug(F101,"ZZZ x","",x);
		    if (rseqtbl[x] > -1) /* Have I received packet x? */
		      continue;		 /* Yes, go on. */
		    debug(F101,"ZZZ x not recd yet","",x);
		    pi = sseqtbl[x];	 /* No, have I NAK'd it yet? */
		    if (pi < 0 || s_pkt[pi].pk_rtr == 0) {
			debug(F101,"ZZZ x not NAK'd yet","",x);
			nack(x);	 /* No, NAK it now. */
			nf = 1;		 /* Flag that I did. */
			break;
		    }
		}
		if (!nf) {		/* If we didn't NAK anything above, */
		    debug(F101,"ZZZ NAKing winlo","",winlo);
		    if (nack(winlo) < 0) { /* we have to NAK winlo (again) */
			errpkt((CHAR *)"Too many retries."); /* Too many */
			strcpy((char *)pktmsg,"Timed out."); /* Give up */
			type = 'E';
			break;
		    }
		} 
		continue;
	    }

	    if (type == 'T') {		/* Timeout */
#ifdef CK_TIMERS
		rcvtimo++;		/* Stretch the timeout a little */
#endif /* CK_TIMERS */
		timeouts++;
		debug(F101,"input receive-state timeout, winlo","",winlo);
		/* NAK only the packet at window-low */
		debug(F101,"input sending NAK for winlo","",winlo);
		if (ttchk() > 0)	/* Don't give up if there is still */
		  continue;		/* something to read. */
		if (nack(winlo) < 0) {
		    debug(F101,"input sent too many naks","",winlo);
		    errpkt((CHAR *)"Too many retries.");
		    strcpy((char *)pktmsg,"Sent too many NAKs.");
		    type = 'E';
		    break;
		} else continue;
	    }

	    if (rsn == winlo) {		/* Got the packet we want, done. */
#ifdef CK_TIMERS
		if (rttflg)		/* Dynamic round trip timers? */
		  getrtt(nakstate, rsn); /* yes, do it. */
#endif /* CK_TIMERS */
		debug(F101,"input rsn=winlo","",rsn);
		break;
	    }

	    /* Got a packet out of order. */

	    debug(F101,"input recv got packet out of order","",rsn);
	    k = rseqtbl[rsn];		/* Get window slot of this packet. */
	    debug(F101,"input recv rseqtbl[rsn]","",k);
	    if (k < 0) {
		debug(F101,"input recv can't find index for rcvd pkt","",rsn);
		/* Was "Internal error 21" */
		errpkt((CHAR *)"Sliding windows protocol error.");
		strcpy((char *)pktmsg,"Sliding windows protocol error.");
		type = 'E';
		break;
	    }		
	    y = chkwin(rsn,winlo,wslots); /* See what window it's in. */
	    debug(F101,"input recv chkwin","",y);
	    if (y == 1) {		/* From previous window. */
		resend(rsn);		/* Resend the ACK (might have data) */
		freerpkt(rsn);		/* Get rid of received packet */
		continue;
	    } else {			/* In this window or out of range */
		if (y < 0)		/* If out of range entirely, */
		  freerpkt(rsn);	/* release its buffer */

#ifdef COMMENT /* Ignore this and read what comes afterwards... */
/*
  We have received a packet, but not the one we want.  It would seem to
  make sense to always send a NAK for the most desired packet (winlo).  But
  consider this scenario: a packet arrived damaged so we NAK'd it above;
  then packets winlo+1, winlo+2, ... winlo+n arrive, each one making us
  send another NAK for winlo, so the other Kermit gets n NAKs for winlo, and
  either would have to resend it n times, or if n > retry limit, give up
  because of too many retries.  So we compromise: If a packet arrives
  that is not the most desired packet (winlo), we NAK winlo, BUT ONLY IF
  it has not been NAK'd before.
*/
		x = sseqtbl[winlo];	/* Get index of most desired packet */
		if (s_pkt[x].pk_rtr == 0 ||   /* Not NAK'd before? */
		    rbufnum < 1) {	      /* Or receive window full? */
		    if (nack(winlo) < 0) {    /* One or both, so NAK it now. */
			errpkt((CHAR *)"Too many retries."); /* Too many */
			strcpy((char *)pktmsg,"Timed out."); /* Give up */
			type = 'E';
			break;
		    } else continue;
		} else continue;
#endif /* COMMENT */

/*
  In version 5A(189), the strategy was revised to send NAKs for the oldest
  missing packet that had not been NAK'd before, which requires a search.
  Thus, if winlo was already NAK'd, instead of doing nothing, we send a NAK
  for the "lowest" as-yet-unNAK'd missing packet.

  If our receive window is full, however, we have no choice but to NAK winlo:
*/
		debug(F101,"XXX checking rbufnum","",rbufnum);
		if (rbufnum < 1) {	/* Receive window full? */
		    debug(F101,"XXX out of buffers","",rbufnum); /* Yes */
		    if (nack(winlo) < 0) {    /* No choice, must NAK winlo. */
			errpkt((CHAR *)"Too many retries."); /* Too many */
			strcpy((char *)pktmsg,"Timed out."); /* Give up */
			type = 'E';
			break;
		    } else continue;
		}
/*
  Receive window not full.  This is a packet in the current window but it is
  not the desired packet at winlo.  So therefore there are gaps before this
  packet.  So we find the "lowest" unNAK'd missing packet, if any, between
  winlo and this one, and NAK it.  If there are no as-yet-unNAK'd missing
  packets in the window, then we send nothing and go wait for another packet.
  In theory, this could result in a timeout, but in practice it is likely that
  the already-NAK'd missing packets are already on their way.  Note, we do not
  NAK ahead of ourselves, as that only creates unnecessary retransmissions.
*/
		debug(F101,"XXX winlo","",winlo);
		for (x = winlo; x != rsn; x = (x + 1) % 64) {
		    debug(F101,"XXX x","",x);
		    if (rseqtbl[x] > -1) /* Have I received packet x? */
		      continue;		 /* Yes, check next sequence number. */
		    debug(F101,"XXX missing pkt","",x);
		    pi = sseqtbl[x];	 /* No, have I NAK'd it yet? */
		    if (pi < 0 || s_pkt[pi].pk_rtr == 0) {
			nack(x);	 /* No, NAK it now. */
			debug(F101,"XXX nak","",x);
			break;
		    }
		}
	    }
/*!!!*/
	} else {			/* Otherwise file sender... */

	    if (!nak2ack) {		/* NAK(n+1) = ACK(n) */

		if (wslots > 1) {	/* Packet at winlo already ACK'd? */
		    if (sacktbl[winlo]) { /* If so,  */
			sacktbl[winlo] = 0; /* Turn off the ACK'd flag */
			winlo = (winlo + 1) % 64; /* Rotate the window */
			type = 'Y';	/* And return ACK */
			debug(F101,
			      "input send returning pre-stashed ACK","",
			      winlo-1);
			break;
		    }
		}
		type = rpack();		/* Try to read an acknowledgement */
		debug(F111,"input send",(char *) rdatap,(int) type);
		while (type == 'e') {	/* Handle echoes */
		    debug(F000,"echo discarded","",type);
		    type = rpack();	
		}		
#ifndef OLDCHKINT
		if (type == 'z') {
		    errpkt((CHAR *)"User cancelled.");
		    strcpy((char *)pktmsg,"User cancelled.");
		    type = 'E';
		    break;
		}
#endif /* OLDCHKINT */
		if (type == -2) return('q');
		if (type == -1) {
		    errpkt((CHAR *)"Receive window full"); /* was "internal */
		    debug(F101," wslots","",wslots); /* error 18" */
		    debug(F101," winlo","",winlo);
		    debug(F101," pktnum","",pktnum);
		    dumprbuf();
		    strcpy((char *)pktmsg,"Can't allocate receive buffer");
		    type = 'E';
		    break;
		}
		dumprbuf();		/* Debugging */

#ifdef OLDCHKINT
		if (chkint() < 0) {	/* Check for console interrupts. */
		    errpkt((CHAR *)"User cancelled.");
		    strcpy((char *)pktmsg,"User cancelled.");
		    return(type = 'E');
		}
#endif /* OLDCHKINT */

		/* Got a packet */

		if (type == 'E') {
		    debug(F101,"input send got E, nakstate","",nakstate);
		    break;		/* Error packet */
		}
		if (type == 'Q') {	/* Crunched packet */ 
		    crunched++;		/* For statistics */
		    numerrs++;		/* For packet resizing */
		    x = resend(winlo);	/* Resend window-low */
		    if (x < 0) {
			type = 'E';
			errpkt(pktmsg);
			break;
		    }
		    continue;
		}
		if (type == 'T') {	/* Timeout waiting for ACKs. */
		    timeouts++;		/* Count it */
		    numerrs++;		/* Count an error too */
		    debug(F101,"input send state timeout, winlo","",winlo);

		    /* Retransmit the oldest un-ACK'd packet. */

		    debug(F101,"input send resending winlo","",winlo);
		    if (resend(winlo) < 0) { /* Check retries */
			debug(F101,"input send too many resends","",maxtry);
			errpkt(pktmsg);
			return(type = 'E');
		    }
#ifdef NEWDPL
		    /* Reduce prevailing packet length */
		    x = sseqtbl[winlo];	/* Get length of packet we want ACKd */
		    if (x > -1) {	/* Only if we have a valid index */
			if (s_pkt[x].pk_typ == 'D') { /* only for D packets */
			    spsiz = (s_pkt[x].pk_len + 8) >> 1; /* halve it */
			    if (spsiz < 20) spsiz = 20; /* within reason */
			    debug(F101,"input T cut packet length","",spsiz);
			}
		    }
#endif /* NEWDPL */
		    continue;
		}
	    }
	    /* Got an actual normal packet */

	    nak2ack = 0;		/* Unset this flag. */
	    y = chkwin(rsn,winlo,wslots); /* Is it in the window? */
	    debug(F101,"input send rsn","",rsn);
	    debug(F101,"input send winlo","",winlo);
	    debug(F101,"input send chkwin","",y);

	    if (type == 'Y') {		/* Got an ACK */
		if (y == 0) {		/* In current window */
		    if (spackets < 4)	/* Error counter doesn't count */
		      numerrs = 0;	/* until data phase. */
		    sacktbl[rsn]++;	/* Mark the packet as ACK'd */
		    x = sseqtbl[rsn];	/* Get ACK'd packet's buffer index */
		    debug(F101,"bestlen ack x","",x);
#ifdef NEWDPL
		    if (x > -1) {
			acktype = s_pkt[x].pk_typ; /* Get type */
			debug(F000,"bestlen ack type","",acktype);

			if (acktype == 'D') { /* Adjust data packet length */
			    if (spsiz > bestlen) {
				bestlen = spsiz;
				debug(F101,"bestlen B","",bestlen);
			    }
#ifdef DEBUG			
			    if (deblog) {
				debug(F101,"bestlen retry","",s_pkt[x].pk_rtr);
				debug(F101,"bestlen len","",s_pkt[x].pk_len);
				debug(F101,"bestlen spackets","",spackets);
			    }
#endif /* DEBUG */
			    /* Set new best length */
			    if (s_pkt[x].pk_rtr == 0 &&
				s_pkt[x].pk_len + 8 > bestlen) {
				bestlen = s_pkt[x].pk_len + 8;
				if (bestlen > spmax)
				  bestlen = spmax;
				debug(F101,"bestlen A","",bestlen);
			    }
#ifdef DEBUG			
			    if (deblog) {
				debug(F101,"bestlen wslots","",wslots);
				debug(F101,"bestlen maxsend","",maxsend);
			    }
#endif /* DEBUG */
			    /* Slow start */
			    if (slostart &&
				(maxsend <= spmax) &&
				(rpackets < 11) &&
				(numerrs == 0)) {
				spsiz = spsiz << 1;
				debug(F101,"bestlen spsiz A","",spsiz);

			    /* Creep up to best length */
			    } else if ((spackets > 5) &&
				       (spsiz < bestlen - 8)) {
				spsiz += (bestlen - spsiz) / 3;
				debug(F101,"bestlen spsiz B","",spsiz);

			    /* Push the envelope */
			    } else if ((spackets % (wslots + 1) == 0) &&
				       (spackets > 6) &&
				       (bestlen < spmax - 8) &&
				       (spsiz < spmax)) {
				spsiz += (spmax - bestlen) / 3;
				debug(F101,"bestlen spsiz C","",spsiz);
			    }
			    /* But not too far */
			    if (spsiz > spmax) {
				spsiz = spmax;
				debug(F101,"bestlen spsiz D","",spsiz);
			    }
			}
		    }
#endif /* NEWDPL */

#ifdef CK_TIMERS
		    if (rttflg)		/* If doing dynamic timers */
		      getrtt(nakstate, rsn); /* call routine to set it. */
#endif /* CK_TIMERS */
/*
  NOTE: The following statement frees the buffer of the ACK we just got.
  But the upper layers still need the data, like if it's the ACK to an I,
  S, F, D, Z, or just about any kind of packet.  So for now, freerbuf()
  deallocates the buffer, but does not erase the data or destroy the pointer
  to it.  There's no other single place where these receive buffers can be
  correctly freed (?) ...
*/
		    freerpkt(rsn);	/* Free the ACK's buffer */
		    freesbuf(rsn);	/* *** Free the sent packet's buffer */
		    if (rsn == winlo) {	/* Got the one we want */
			sacktbl[winlo] = 0;
			winlo = (winlo + 1) % 64;
			debug(F101,"input send rotated send window","",winlo);
			break;		/* Return the ACK */
		    } else {
			debug(F101,"input send mark pkt","",rsn);
			continue;	/* Otherwise go read another packet */
		    }
		} else if (y == 1 && wslots < 2) { /* (190) ACK for previous */
		    numerrs++;		/* == NAK for current, count error */
		    debug(F101,"input send ACK for previous","",rsn);
		    freerpkt(rsn);	/* Free NAK's buffer */
		    x = resend(winlo);	/* Resend current packet */
		    if (x < 0) {
			type = 'E';
			errpkt(pktmsg);
			break;
		    } else continue;	/* Resend ok, go read another packet */
		} else {		/* Other cases, just ignore */
		    debug(F101,"input send ACK out of window","",rsn);
		    freerpkt(rsn);
		    continue;
		}
	    }
	    if (type == 'N') {		/* NAK */
		numerrs++;		/* Count an error */
		debug(F101,"input send NAK","",rsn);
#ifdef NEWDPL
		/* Reduce prevailing packet length */
		x = sseqtbl[rsn];	/* Length of packet that was NAK'd */
		if (x > -1) {		/* If it's a Data packet we've sent */
		    if (s_pkt[x].pk_typ == 'D') {
			spsiz = (s_pkt[x].pk_len + 8) >> 1; /* Halve length */
#ifdef COMMENT
			/* This might be a good idea -- haven't tried it ... */
			if (bestlen > 0 && spsiz > bestlen)
			  spsiz = bestlen;
#endif /* COMMENT */
			if (spsiz < 20) spsiz = 20;
			debug(F101,"input N cut packet length","",spsiz);
		    }
		}
#endif /* NEWDPL */
		freerpkt(rsn);		/* Free buffer where NAK lies. */
		if (y == 0) {		/* In current window */		
		    debug(F100," in window","",0);
		    k = sseqtbl[rsn];	/* Get pointer to NAK'd packet. */
		    if (k < 0 || (k > -1 && s_pkt[k].pk_typ == ' ')) {
			x = resend(winlo); /* Packet we haven't sent yet. */
		    } else {
			x = resend(rsn); /* Resend requested packet. */
		    }
		    if (x < 0) {	/* Resend error is fatal.  */
			type = 'E';
			errpkt(pktmsg);
			break;
		    } else continue;	/* Resend ok, go read another packet */
		} else if ((rsn == (pktnum + 1) % 64)) { /* NAK for next pkt */
		    if (wslots > 1) {
			debug( F101,"NAK for next packet, windowing","",rsn);
			x = resend(winlo); /* Resend window-low */
			if (x < 0) {
			    type = 'E';
			    errpkt(pktmsg);
			    break;
			}
			continue;	/* Go back and read another pkt */
		    }
		    debug(F101,"NAK for next packet, no windowing","",rsn);
		    x = (rsn == 0) ? 63 : rsn - 1;
		    if (x == 0 && (sndtyp == 'S' || sndtyp == 'I')) {
			resend(0);	/* ACK for S or I packet missing */
			continue;	/* so resend the S or I */
		    }		
		    rsn = x;		/* Else, treat NAK(n+1) as ACK(n) */
		    nak2ack = 1;	/* Go back and process the ACK */
		    continue;
		} else if (y > 0) {	/* NAK for pkt we can't resend */
		    debug(F101," NAK out of window","",rsn); /* bad... */
		    type = 'E';
		    errpkt((CHAR *)"NAK out of window");
		    strcpy((char *)pktmsg,"NAK out of window.");
		    break;
		} else continue;	/* Ignore other NAKs */
	    }				/* End of file-sender NAK handler */

            if (rsn == winlo) {		/* Not ACK, NAK, timeout, etc. */
		debug(F000,"input send unexpected type","",type);
		break;
	    }
	}				/* End of file-sender section */
    }					/* End of input() while() loop */
    if (wslots == 1) {
	debug(F100,"input about to flush","",0);
	ttflui();		/* Got what we want, clear input buffer. */
    }
#ifndef NEWDPL
    if (!nakstate)		/* When sending */
      rcalcpsz();		/* recalculate size every packet */
#endif /* NEWDPL */
    debug(F000,"input returning type","",type);
    return(rcvtyp = type);	/* Success, return packet type. */
}

/*  D O P A R  --  Add an appropriate parity bit to a character  */

/*
  (PWP) this is still used in the Mac terminal emulator, so we have to keep it
*/
CHAR
#ifdef CK_ANSIC
dopar(register CHAR ch)
#else
dopar(ch) register CHAR ch;
#endif /* CK_ANSIC */
    {
    register unsigned int a;
    if (!parity
#ifdef TCPSOCKET        
        || (network &&		/* TELNET BINARY MODE */
	    (ttnproto == NP_TELNET) && (me_binary) )
#endif /* TCPSOCKET */
        ) return((CHAR) (ch & 255)); else a = ch & 127;
    switch (parity) {
	case 'e':  return(p_tbl[a]);	             /* Even */
	case 'm':  return((CHAR) (a | 128));         /* Mark */
	case 'o':  return((CHAR) (p_tbl[a] ^ 128));  /* Odd */
	case 's':  return((CHAR) a);	             /* Space */
	default:   return((CHAR) a);                 /* Something illegal */
    }
}

#ifdef PARSENSE
/*  P A R C H K  --  Check if Kermit packet has parity  */

/*
  Call with s = pointer to packet, start = packet start character, n = length.
  Returns 0 if packet has no parity, -1 on error, or, if packet has parity:
    'e' for even, 'o' for odd, 'm' for mark.  Space parity cannot be sensed.
  So a return value of 0 really means either space or none.
  Returns -2 if parity has already been checked during this protocol operation.
*/
int
#ifdef CK_ANSIC
parchk(CHAR *s, CHAR start, int n)
#else
parchk(s,start,n) CHAR *s, start; int n;
#endif /* CK_ANSIC */
/* parchk */ {
    CHAR s0, s1, s2, s3;

    debug(F101,"parchk n","",n);
    debug(F101,"parchk start","",start);

    s0 = s[0] & 0x7f;			/* Mark field (usually Ctrl-A) */

    if (s0 != start || n < 5) return(-1); /* Not a valid packet */

/* Look at packet control fields, which never have 8th bit set */
/* First check for no parity, most common case. */

    if (((s[0] | s[1] | s[2] | s[3]) & 0x80) == 0)
      return(0);			/* No parity or space parity */

/* Check for mark parity */

    if (((s[0] & s[1] & s[2] & s[3]) & 0x80) == 0x80)
      return('m');			/* Mark parity */

/* Packet has some kind of parity */
/* Make 7-bit copies of control fields */

    s1 = s[1] & 0x7f;			/* LEN */
    s2 = s[2] & 0x7f;			/* SEQ */
    s3 = s[3] & 0x7f;			/* TYPE */

/* Check for even parity */

    if ((s[0] == p_tbl[s0]) &&
        (s[1] == p_tbl[s1]) &&
        (s[2] == p_tbl[s2]) &&
	(s[3] == p_tbl[s3]))
      return('e');

/* Check for odd parity */

    if ((s[0] != p_tbl[s0]) &&
        (s[1] != p_tbl[s1]) &&
        (s[2] != p_tbl[s2]) &&
	(s[3] != p_tbl[s3]))
      return('o');

/* Otherwise it's probably line noise.  Let checksum calculation catch it. */

    return(-1);
}
#endif /* PARSENSE */

/*
  Check to make sure timeout intervals are long enough to allow maximum
  length packets to get through before the timer goes off.  If not, the
  timeout interval is adjusted upwards.

  This routine is called at the beginning of a transaction, before we
  know anything about the delay characteristics of the line.  It works
  only for serial communication devices; it trusts the speed reported by
  the operating system.

  Call with a timout interval.  Returns it, adjusted if necessary.
*/  
int
chktimo(timo,flag) int timo, flag; {
    long cps, z; int x, y;
    debug(F101,"chktimo timo","",timo); /* Timeout before adjustment */
    debug(F101,"chktimo flag","",flag);

    if (flag)				/* Don't change timeout if user */
      return(timo);			/* gave SET SEND TIMEOUT command. */
    debug(F101,"chktimo spmax","",spmax);
    debug(F101,"chktimo urpsiz","",urpsiz);

    speed = ttgspd();			/* Get current speed. */
    if (speed > 0L && !network) {
	cps = speed / 10L;		/* Convert to chars per second */
	if (cps > 0L) {
	    long plen;			/* Maximum of send and rcv pkt size */
	    z = cps * (long) timo;	/* Chars per timeout interval */
	    z -= z / 10L;		/* Less 10 percent */
	    plen = spmax;
	    if (urpsiz > spmax) plen = urpsiz;
	    debug(F101,"chktimo plen","",plen);
	    if (z < plen) {		/* Compare with packet size */
		x = (int) ((long) plen / cps); /* Adjust if necessary */
		y = x / 10;		/* Add 10 percent for safety */
		if (y < 2) y = 2;	/* Or 2 seconds, whichever is more */
		x += y;
		if (x > timo)		/* If this is greater than current */
		  timo = x;		/* timeout, change the timeout */
		debug(F101,"chktimo new timo","",timo);
	    }
	}
    }
    return(timo);
}

/*  S P A C K  --  Construct and send a packet  */

/*
  spack() sends a packet of the given type, sequence number n, with len data
  characters pointed to by d, in either a regular or extended- length packet,
  depending on len.  Returns the number of bytes actually sent, or else -1
  upon failure.  Uses global npad, padch, mystch, bctu, data.  Leaves packet 
  fully built and null-terminated for later retransmission by resend().
  Updates global sndpktl (send-packet length).

  NOTE: The global pointer "data" is assumed to point into the 7th position
  of a character array (presumably in packet buffer for the current packet).
  It was used by getpkt() to build the packet data field.  spack() fills in
  the header to the left of the data pointer (the data pointer is defined
  in getsbuf() in ckcfn3.c).  If the address "d" is the same as "data", then
  the packet's data field has been built "in place" and need not be copied.
*/
int
#ifdef CK_ANSIC
spack(char pkttyp, int n, int len, CHAR *d)
#else
spack(pkttyp,n,len,d) char pkttyp; int n, len; CHAR *d;
#endif /* CK_ANSIC */
/* spack */ {
    register int i;
    int j, k, x, lp, longpkt, copy;

#ifdef GETMSEC
    long t1, t2;
#endif /* GETMSEC */

    register CHAR *cp, *mydata;
    unsigned crc;

    debug(F101,"spack n","",n);
    debug(F111,"spack data",data,data);
    /* debug(F101,"spack d","",d); */
    debug(F101,"spack len","",len);

    copy = (d != data);			/* Flag whether data must be copied  */
    longpkt = (len + bctl + 2) > 94;	/* Decide whether it's a long packet */
    mydata = data - 7 + (longpkt ? 0 : 3); /* Starting position of header */
    debug(F101," mydata","",mydata);

    k = sseqtbl[n];			/* Packet structure info for pkt n */ 
    debug(F101," sseqtbl[n]","",k);
    if (k < 0) {
	debug(F101,"spack sending packet out of window","",n);
    } else {				/* Record packet info */
	s_pkt[k].pk_adr = mydata;	/* Remember address of packet. */
	s_pkt[k].pk_seq = n;		/* Record sequence number */
	s_pkt[k].pk_typ = pkttyp;	/* Record packet type */
    }
    spktl = 0;				/* Initialize length of this packet */
    i = 0;				/* and position in packet. */

/* Now fill the packet */

    mydata[i++] = mystch;		/* MARK */
    lp = i++;				/* Position of LEN, fill in later */

    mydata[i++] = tochar(n);		/* SEQ field */
    mydata[i++] = pkttyp;		/* TYPE field */
    j = len + bctl;			/* Length of data + block check */
    if (longpkt) {			/* Long packet? */
	int x;				/* Work around SCO Xenix/286 */
	x = j / 95;			/* compiler bug... */
        mydata[lp] = tochar(0);		/* Yes, set LEN to zero */
        mydata[i++] = tochar(x);	/* High part */
        mydata[i++] = tochar(j % 95);	/* Low part */
        mydata[i] = '\0';		/* Header checksum */
        mydata[i++] = tochar(chk1(mydata+lp));
    } else mydata[lp] = tochar(j+2);	/* Normal LEN */

    if (copy)				/* Data field built in place? */
      for ( ; len--; i++) mydata[i] = *d++; /* No, must copy. */
    else				/* Otherwise, */
      i += len;				/* Just skip past data field. */
    mydata[i] = '\0';			/* Null-terminate for checksum calc. */

    switch (bctu) {			/* Block check */
	case 1:				/* 1 = 6-bit chksum */
	    mydata[i++] = tochar(chk1(mydata+lp));
	    break;
	case 2:				/* 2 = 12-bit chksum */
	    j = chk2(mydata+lp);
	    mydata[i++] = (unsigned)tochar((j >> 6) & 077);
   	    mydata[i++] = (unsigned)tochar(j & 077);
	    break;
        case 3:				/* 3 = 16-bit CRC */
	    crc = chk3(mydata+lp,parity);
	    mydata[i++] = (unsigned)tochar(((crc & 0170000)) >> 12);
	    mydata[i++] = (unsigned)tochar((crc >> 6) & 077);
	    mydata[i++] = (unsigned)tochar(crc & 077);
	    break;
	case 4:				/* 2 = 12-bit chksum, blank-free */
	    j = chk2(mydata+lp);	
	    mydata[i++] =
	      (unsigned)(tochar((unsigned)(((j >> 6) & 077) + 1)));
   	    mydata[i++] = (unsigned)(tochar((unsigned)((j & 077) + 1)));
	    break;
    }
    mydata[i++] = seol;			/* End of line (packet terminator) */
#ifdef TCPSOCKET
/*
  If TELNET connection and packet terminator is carriage return,
  we must stuff either LF or NUL, according to SET TELNET NEWLINE-MODE
  (tn_nlm), to meet the TELNET specification, unless user said RAW.

  If NEWLINE-MODE is set to LF instead of CR, we still send CR-NUL
  on a NVT connection and CR on a binary connection.
*/
    if (seol == CR && network && ttnproto == NP_TELNET) {
        switch (me_binary ? tn_b_nlm : tn_nlm) { /* NVT or BINARY */
            case TNL_CR:
                break;
            case TNL_CRNUL:
                mydata[i++] = NUL ;
                break;
            case TNL_CRLF:
                mydata[i++] = LF ;
                break;
            }
      }
#endif /* TCPSOCKET */
    mydata[i] = '\0';			/* Terminate string */
    logpkt('s',n,mydata);		/* Log packet */

    /* (PWP) add the parity quickly at the end */
    switch (parity) {
      case 'e':				/* Even */
	for (cp = &mydata[i-1]; cp >= mydata; cp--)
	  *cp = p_tbl[*cp];
	break;
      case 'm':				/* Mark */
	for (cp = &mydata[i-1]; cp >= mydata; cp--)
	  *cp |= 128;
	break;
      case 'o':				/* Odd */
	for (cp = &mydata[i-1]; cp >= mydata; cp--)
	  *cp = p_tbl[*cp] ^ 128;
	break;
      case 's':				/* Space */
	for (cp = &mydata[i-1]; cp >= mydata; cp--)
	  *cp &= 127;
	break;
    }
    if (pktpaus) msleep(pktpaus);	/* Pause if requested */
    if (npad) ttol(padbuf,npad);	/* Send any padding */
#ifdef CK_TIMERS
    if (pkttyp == 'N')
      srttbl[n > 0 ? n-1 : 63] = gtimer();
    else
      srttbl[n] = gtimer();
#endif /* CK_TIMERS */
    spktl = i;				/* Remember packet length */
    if (k > -1)
      s_pkt[k].pk_len = spktl;		/* also in packet info structure */

#ifdef GETMSEC
    if (deblog) t1 = getmsec();
#endif /* GETMSEC */

    x = ttol(mydata,spktl);
    if (spktl > maxsend) maxsend = spktl;

#ifdef GETMSEC
    if (deblog)  {
	t2 = getmsec();
	if (t2 > -1L && t1 > -1L)
	  debug(F101,"spack ttol time","",t2-t1);
	else
	  debug(F100,"spack ttol time error","",0);
    }
#endif /* GETMSEC */

    debug(F101,"spack ttol returns","",x);
    if (x < 0)
      return(x);
    sndtyp = pkttyp;			/* Remember packet type for echos */
    spackets++;				/* Count it. */
    flco += spktl;			/* Count the characters */
    tlco += spktl;			/* for statistics... */
    dumpsbuf();				/* Dump send buffers to debug log */
    debug(F100,"spack about to call screen","",0);
    debug(F111," with mydata=",mydata,n);
    screen(SCR_PT,pkttyp,(long)n,(char *)mydata); /* Update screen */
    return(spktl);			/* Return length */
}

/*  C H K 1  --  Compute a type-1 Kermit 6-bit checksum.  */

int
chk1(pkt) register CHAR *pkt; {
    register unsigned int chk;
    chk = chk2(pkt);
    chk = (((chk & 0300) >> 6) + chk) & 077;
    return((int) chk);
}

/*  C H K 2  --  Compute the numeric sum of all the bytes in the packet.  */

unsigned int
chk2(pkt) register CHAR *pkt; {
    register long chk; register unsigned int m;
    m = (parity) ? 0177 : 0377;
    for (chk = 0; *pkt != '\0'; pkt++)
      chk += *pkt & m;
    return((unsigned int) (chk & 07777));
}


/*  C H K 3  --  Compute a type-3 Kermit block check.  */
/*
 Calculate the 16-bit CRC-CCITT of a null-terminated string using a lookup 
 table.  Assumes the argument string contains no embedded nulls.
*/
unsigned int
chk3(pkt,parity) register CHAR *pkt; int parity; {
    register long c, crc;
    register unsigned int m;
    m = (parity) ? 0177 : 0377;
    for (crc = 0; *pkt != '\0'; pkt++) {
	c = crc ^ (long)(*pkt & m);
	crc = (crc >> 8) ^ (crcta[(c & 0xF0) >> 4] ^ crctb[c & 0x0F]);
    }
    return((unsigned int) (crc & 0xFFFF));
}

int
nxtpkt() {				/* Called by file sender */
    int j, n;

    debug(F101,"nxtpkt pktnum","",pktnum);
    debug(F101,"nxtpkt winlo ","",winlo);
    n = (pktnum + 1) % 64;		/* Increment packet number mod 64 */
#ifdef COMMENT 
/*
  Suggested by Alan Grieg.  A packet can be sent out of window in 
  circumstances involving acks received out of order, ...  Have to think
  about this...
*/
    if (chkwin(n,winlo,wslots)) {
	debug(F101,"nxtpkt n not in window","",n);
	return(-1);
    }
#endif
    j = getsbuf(n);			/* Get a buffer for packet n */
    if (j < 0) {
	debug(F101,"nxtpkt can't getsbuf","",j);
	return(-1);
    }
    pktnum = n;	      
    debug(F101,"nxtpkt bumped pktnum to","",pktnum);
    return(0);
}

/* Functions for sending ACKs and NAKs */

/* Note, we should only ACK the packet at window-low (winlo) */
/* However, if an old packet arrives again (e.g. because the ACK we sent */
/* earlier was lost), we ACK it again. */

int
ack() {					/* Acknowledge the current packet. */
    return(ackns(winlo,(CHAR *)""));
}

int
ackns(n,s) int n; CHAR *s; {		/* Acknowledge packet n */
    int j, k;
    debug(F111,"ackns",s,n);

    k = rseqtbl[n];			/* First find received packet n. */
    debug(F101,"ackns k","",k);
    freesbuf(n);			/* Free current send-buffer, if any */
    if ((j = getsbuf(n)) < 0) {
	/* This can happen if we have to re-ACK an old packet that has */
        /* already left the window.  It does no harm. */
	debug(F101,"ackns can't getsbuf","",n);
    }
    spack('Y',n,(int)strlen((char *)s),s); /* Now send it. */
    debug(F101,"ackns winlo","",winlo);
    debug(F101,"ackns n","",n);
    if (n == winlo) {			/* If we're acking winlo */
	if (k > -1)
	  freerbuf(k);			/* don't need it any more */
	if (j > -1)
	  freesbuf(j);			/* and don't need to keep ACK either */
	winlo = (winlo + 1) % 64;
    }
    return(0);
}

int
ackn(n) int n; {			/* Send ACK for packet number n */
    return(ackns(n,(CHAR *)""));
}

int
ack1(s) CHAR *s; {			/* Send an ACK with data. */
    debug(F110,"ack1",(char *) s,0);
    return(ackns(winlo, s));
}

/* N A C K  --   Send a Negative ACKnowledgment. */
/*
 Call with the packet number, n, to be NAK'd.
 Returns -1 if that packet has been NAK'd too many times, otherwise 0.
 Btw, it is not right to return 0 under error conditions.  This is
 done because the -1 code is used for cancelling the file transfer.
 More work is needed here.
*/
int
nack(n) int n; {
    int i;

    if (n < 0 || n > 63) {
	debug(F101,"nack bad pkt num","",n);
	return(0);
    } else debug(F101,"nack","",n);
    if ((i = sseqtbl[n]) < 0) {		/* If necessary */
	if (getsbuf(n) < 0) {		/* get a buffer for this NAK */
	    debug(F101,"nack can't getsbuf","",n);
	    return(0);
	} else i = sseqtbl[n];		/* New slot number */
    }
    if (s_pkt[i].pk_rtr++ > maxtry)	/* How many times have we done this? */
      return(-1);			/* Too many... */

/* Note, don't free this buffer.  Eventually an ACK will come, and that */
/* will set it free.  If not, well, it's back to ground zero anyway...  */

    spack('N',n,0,(CHAR *) "");		/* NAKs never have data. */
    return(0);
}

#ifndef NEWDPL				/* This routine no longer used */
/*
 * (PWP) recalculate the optimal packet length in the face of errors.
 * This is a modified version of the algorithm by John Chandler in Kermit/370, 
 * see "Dynamic Packet Size Control", Kermit News, V2 #1, June 1988.
 *
 * This implementation minimizes the total overhead equation, which is
 *
 *   Total chars = file_chars + (header_len * num_packs)
 *                            + (errors * (header_len + packet_len))
 *
 * Differentiate with respect to number of chars, solve for packet_len, get:
 *
 *   packet_len = sqrt (file_chars * header_len / errors)
 */

/*
 (FDC) New super-simple algorithm.  If there was an error in the most recent
 packet exchange, cut the send-packet size in half, down to a minimum of 20.
 If there was no error, increase the size by 5/4, up to the maximum negotiated
 length.  Seems to be much more responsive than previous algorithm, which took
 forever to recover the original packet length, and it also went crazy under
 certain conditions.

 Here's another idea for packet length resizing that keeps a history of the
 last n packets.  Push a 1 into the left end of an n-bit shift register if the
 current packet is good, otherwise push a zero.  The current n-bit value, w, of
 this register is a weighted sum of the noise hits for the last n packets, with
 the most recent weighing the most.  The current packet length is some function
 of w and the negotiated packet length, like:

   (2^n - w) / (2^n) * (negotiated length)

 If the present resizing method causes problems, think about this one a little
 more.
*/
VOID
rcalcpsz() {

#ifdef COMMENT
/* Old way */
    register long x, q;
    if (numerrs == 0) return;		/* bounds check just in case */

    /* overhead on a data packet is npad+5+bctr, plus 3 if extended packet */
    /* an ACK is 5+bctr */

    /* first set x = per packet overhead */
    if (wslots > 1)			/* Sliding windows */
      x = (long) (npad+5+bctr);		/* packet only, don't count ack */
    else				/* Stop-n-wait */
      x = (long) (npad+5+3+bctr+5+bctr); /* count packet and ack. */

    /* then set x = packet length ** 2 */
    x = x * ( ffc / (long) numerrs);	/* careful of overflow */
    
    /* calculate the long integer sqrt(x) quickly */
    q = 500;
    q = (q + x/q) >> 1;
    q = (q + x/q) >> 1;
    q = (q + x/q) >> 1;
    q = (q + x/q) >> 1;		/* should converge in about 4 steps */
    if ((q > 94) && (q < 130))	/* break-even point for long packets */
      q = 94;
    if (q > spmax) q = spmax;	/* maximum bounds */
    if (q < 10) q = 10;		/* minimum bounds */
    spsiz = q;			/* set new send packet size */
    debug(F101,"rcalcpsiz","",q);
#else
/* New way */
    debug(F101,"rcalcpsiz numerrs","",numerrs);
    debug(F101,"rcalcpsiz spsiz","",spsiz);
    if (spackets < 3) {
	numerrs = 0;
	return;
    }
    if (numerrs)
      spsiz = spsiz / 2;
    else
      spsiz = (spsiz / 4) * 5;
    if (spsiz < 20) spsiz = 20;
    if (spsiz > spmax) spsiz = spmax;
    debug(F101,"rcalcpsiz new spsiz","",spsiz);
    numerrs = 0;
#endif /* COMMENT */
}
#endif /* NEWDPL */

/*  R E S E N D  --  Retransmit packet n.  */

/*
  Returns 0 or positive on success (the number of retries for packet n).
  On failure, returns a negative number, and an error message is placed
  in recpkt.
*/
int
resend(n) int n; {			/* Send packet n again. */
    int j, k, x;
#ifdef GETMSEC
    long t1, t2;
#endif /* GETMSEC */

    debug(F101,"resend seq","",n);

    k = chkwin(n,winlo,wslots);		/* See if packet in current window */
    j = -1;				/* Assume it's lost */
    if (k == 0) j = sseqtbl[n];		/* See if we still have a copy of it */
    if (k != 0 || j < 0) {		/* If not.... */
	if (nakstate && k == 1) {
/*
  Packet n is in the previous window and we are the file receiver.
  We already sent the ACK and deallocated its buffer so we can't just 
  retransmit the ACK.  Rather than give up, we try some tricks...
*/
	    if (n == 0 && spackets < 63 && myinit[0]) { /* ACK to Send-Init */
/*
  If the packet number is 0, and we're at the beginning of a protocol
  operation (spackets < 63), then we have to resend the ACK to an I or S
  packet, complete with parameters in the data field.  So we take a chance and
  send a copy of the parameters in an ACK packet with block check type 1.
*/
		int bctlsav;		/* Temporary storage */
		int bctusav;
		bctlsav = bctl;		/* Save current block check length */
		bctusav = bctu;		/* and type */
		bctu = bctl = 1;	/* Set block check to 1 */
		spack('Y',0,(int)strlen((char *)myinit),(CHAR *)myinit);
		logpkt('#',n,(CHAR *)"<reconstructed>"); /* Log it */
		bctu = bctusav;		/* Restore block check type */
		bctl = bctlsav;		/* and length */

	    } else {			/* Not the first packet */
/*
  It's not the first packet of the protocol operation.  It's some other packet
  that we have already ACK'd and forgotten about.  So we take a chance and
  send an empty ACK using the current block-check type.  Usually this will
  work out OK (like when acking Data packets), and no great harm will be done
  if it was some other kind of packet (F, etc).  If we are requesting an
  interruption of the file transfer, the flags are still set, so we'll catch
  up on the next packet.
*/
		spack('Y',n,0,(CHAR *) "");
	    }
	    retrans++;
	    screen(SCR_PT,'%',(long)pktnum,"Retransmission");
	    return(0);
	} else {
/*
  Packet number is not in current or previous window.  We seem to hit this
  code occasionally at the beginning of a transaction, for apparently no good
  reason.  Let's just log it for debugging, send nothing, and try to proceed
  with the protocol rather than killing it.
*/
	    debug(F101,"RESEND PKT NOT IN WINDOW","",n);
	    debug(F101,"RESEND k","",k);
#ifdef COMMENT
	    sprintf((char *)pktmsg,
		    "   resend error: NIW, n=%d, k=%d.",n,k);
	    return(-2);
#else
	    return(0);
#endif /* COMMENT */
	}
    }

/* OK, it's in the window and it's not lost. */

    debug(F101,"resend pktinfo index","",k);

    if (s_pkt[j].pk_rtr++ > maxtry) {	/* Found it but over retry limit */
	strcpy((char *)pktmsg,"Too many retries.");
	return(-1);
    }
    debug(F101," retry","",s_pkt[j].pk_rtr); /* OK so far */
    dumpsbuf();				/* (debugging) */
    if (s_pkt[j].pk_typ == ' ') {	/* Incompletely formed packet */
	if (nakstate) {			/* (This shouldn't happen any more) */
	    nack(n);
	    retrans++;
	    screen(SCR_PT,'%',(long)pktnum,"(resend)");
	    return(s_pkt[j].pk_rtr);
	} else {			/* No packet to resend! */
#ifdef COMMENT
/*
  This happened (once) while sending a file with 2 window slots and typing
  X to the sender to cancel the file.  But since we're cancelling anyway,
  there's no need to give a scary message.
*/
	    sprintf((char *)pktmsg,
		    "resend logic error: NPS, n=%d, j=%d.",n,j); 
	    return(-2);
#else
/* Just ignore it. */
	    return(0);
#endif /* COMMENT */
	}
    }
#ifdef GETMSEC
    if (deblog) t1 = getmsec();
#endif /* GETMSEC */

    /* Everything ok, send the packet */
#ifdef CK_TIMERS
    srttbl[n] = gtimer();		/* Update the timer */
#endif /* CK_TIMERS */
    x = ttol(s_pkt[j].pk_adr,s_pkt[j].pk_len);

#ifdef GETMSEC
    if (deblog)  {
	t2 = getmsec();
	if (t2 > -1L && t1 > -1L)
	  debug(F101,"resend ttol time","",t2-t1);
	else
	  debug(F100,"resend ttol time error","",0);
    }
#endif /* GETMSEC */
    debug(F101,"resend ttol returns","",x);

    retrans++;				/* Count a retransmission */
    screen(SCR_PT,'%',(long)pktnum,"(resend)");	/* Tell user about resend */
    logpkt('S',n,s_pkt[j].pk_adr);	/* Log the resent packet */
    return(s_pkt[j].pk_rtr);		/* Return the number of retries. */
}

int
errpkt(reason) CHAR *reason; {		/* Send an error packet. */
    int x, y;
    encstr(reason);
    y = spack('E',pktnum,size,data);
    x = quiet; quiet = 1; 		/* Close files silently. */
    clsif(); clsof(1);
    quiet = x;
#ifdef COMMENT
    screen(SCR_TC,0,0l,"");
#endif /* COMMENT */
    if (what < W_CONNECT)
      xitsta |= what;			/* Remember what failed. */
    success = 0;
    return(y);
}

/* scmd()  --  Send a packet of the given type */

int
#ifdef CK_ANSIC
scmd(char t, CHAR *dat)
#else
scmd(t,dat) char t; CHAR *dat;
#endif /* CK_ANSIC */
/* scmd */ {
    if (encstr(dat) < 0)		/* Encode the command string */
      return(-1);
    spack(t,pktnum,size,data);
    return(0);
}

int
srinit(reget, retrieve) int reget, retrieve; { /* Send R, H, or J packet */
    if (encstr((CHAR *)cmarg) < 0)	/* Encode the filename. */
      return(-1);
    if (retrieve)
      spack((char)'H',pktnum,size,data); /* Send the packet. */
    else
      spack((char)(reget ? 'J' : 'R'),pktnum,size,data); /* Send the packet. */
    return(0);
}

/*  K S T A R T  --  Checks for a Kermit packet while in terminal mode.  */

#ifdef CK_APC
int 
#ifdef CK_ANSIC
kstart(CHAR ch)
#else
kstart(ch) CHAR ch;
#endif /* CK_ANSIC */
/* kstart */ {
    static int buflen = 94;
    static CHAR buf[95];
    static CHAR * p = NULL;
    extern CHAR stchr, eol;

    if (ch == stchr) {			/* Start of packet */
	p = buf;
	*p = ch;
	debug(F101,"kstart SOP","",ch);
    } else if (ch == eol) {		/* End of packet */
	if (p) {
	    p++;
	    if (p - buf < 94 ) { 
		int rc = 0;
		*p = ch;
		p++;
		*p = NUL;
		if (rc = chkspkt((char *)buf))
		  debug(F111,"kstart EOP", buf, ch);
		p = NULL;
		if (!rc) return(0);
		return(rc == 1 ? PROTO_K + 1 : 0 - (PROTO_K + 1));
	    } else {
		p = NULL;
	    }
	} 
    } else if (p) {
	p++;
	if (p - buf < 94)
	  *p = ch;
	else 
	  p = NULL;
    }
    return(0);
}

#ifdef CK_XYZ

/*  Z S T A R T  --  Checks for a ZMODEM packet while in terminal mode.  */

int 
#ifdef CK_ANSIC
zstart(CHAR ch)
#else
zstart(ch) CHAR ch;
#endif /* CK_ANSIC */
/* zstart */ {
    static CHAR * matchstr = (CHAR *) "\030B00";
    /* "rz\r**\030B00000000000000\r\033J\021"; */
    static CHAR * p = NULL;

    if (!ch)
      return(0);
    if (!p)
      p = matchstr;

    if (ch == *p) {
	p++;
	if (*p == '\0') {
	    p = matchstr;
	    debug(F100, "zstart Zmodem SOP","",0);
	    return(PROTO_Z + 1);
	}
    } else 
      p = matchstr;
    return(0);
}
#endif /* CK_XYZ */

/*  C H K S P K T  --  Check if buf contains a valid S or I packet  */

int
chkspkt(buf) char *buf; {
    int buflen;
    int len = -1;
    CHAR chk;
    char type = 0;
    char *s = buf;

    if (!buf) return(0);
    buflen = strlen(buf);
    if (buflen < 5) return(0);		/* Too short */
    if (*s++ != stchr) return(0);	/* SOH */
    len = xunchar(*s++);		/* Length */
    if (len < 0) return(0);
    if (*s++ != SP) return(0);		/* Sequence number */
    type = *s++;			/* Type */
    if (type != 'S' && type != 'I')
      return(0);
    if (buflen < len + 2) return(0);
    s += (len - 3);			/* Position of checksum */
    chk = (CHAR) (*s);			/* Checksum */
    *s = NUL;
    if (xunchar(chk) != chk1((CHAR *) (buf+1))) return(0); /* Check it */
    *s = chk;
    return(type == 'S' ? 1 : 2);
}
#endif /* CK_APC */

/* R P A C K  --  Read a Packet */

/*
 rpack reads a packet and returns the packet type, or else Q if the
 packet was invalid, or T if a timeout occurred.  Upon successful return, sets
 the values of global rsn (received sequence number),  rln (received
 data length), and rdatap (pointer to null-terminated data field), and
 returns the packet type.
*/
int
rpack() {
    register int i, j, x, lp;		/* Local variables */
    int k, type, chklen;
    unsigned crc;
    CHAR pbc[4];			/* Packet block check */
    CHAR *sohp;				/* Pointer to SOH */
    CHAR e;				/* Packet end character */

#ifdef GETMSEC
    long t1, t2;
#endif /* GETMSEC */

    debug(F101,"entering rpack, pktnum","",pktnum);

#ifndef OLDCHKINT
    if (chkint() < 0)			/* Check for console interrupts. */
      return('z');
#endif /* OLDCHKINT */

    k = getrbuf();			/* Get a new packet input buffer. */
    debug(F101,"rpack getrbuf","",k);
    if (k < 0) return(-1);		/* Return like this if none free. */
    if (pktmsg) *pktmsg = NUL;
    recpkt = r_pkt[k].bf_adr;
    *recpkt = '\0';			/* Clear receive buffer. */
    sohp = recpkt;			/* Initialize pointers to it. */
    rdatap = recpkt;
    rsn = rln = -1;			/* In case of failure. */
    e = (turn) ? turnch : eol;		/* Use any handshake char for eol */

/* Try to get a "line". */

#ifdef GETMSEC
    if (deblog) t1 = getmsec();
#endif /* GETMSEC */

#ifdef PARSENSE
#ifdef UNIX
/*
  So far the final turn argument is only for ck[uvdl]tio.c.  Should be added
  to the others too.  (turn == handshake character.)
*/
    j = ttinl(recpkt,r_pkt[k].bf_len - 1,rcvtimo,e,stchr,turn);
#else
#ifdef VMS
    j = ttinl(recpkt,r_pkt[k].bf_len - 1,rcvtimo,e,stchr,turn);
#else
#ifdef datageneral
    j = ttinl(recpkt,r_pkt[k].bf_len - 1,rcvtimo,e,stchr,turn);
#else
#ifdef STRATUS
    j = ttinl(recpkt,r_pkt[k].bf_len - 1,rcvtimo,e,stchr,turn);
#else
#ifdef OS2
    j = ttinl(recpkt,r_pkt[k].bf_len - 1,rcvtimo,e,stchr,turn);
#else
#ifdef OSK
    j = ttinl(recpkt,r_pkt[k].bf_len - 1,rcvtimo,e,stchr,turn);
#else
    j = ttinl(recpkt,r_pkt[k].bf_len - 1,rcvtimo,e,stchr);
#endif /* OSK */
#endif /* OS2 */
#endif /* STRATUS */
#endif /* datageneral */
#endif /* VMS */
#endif /* UNIX */
    if (parity != ttprty) autopar = 1;
    parity = ttprty; 
#else /* !PARSENSE */
    j = ttinl(recpkt,r_pkt[k].bf_len - 1,rcvtimo,e);
#endif /* PARSENSE */

#ifdef GETMSEC
    if (deblog)  {
	t2 = getmsec();
	if (t2 > -1L && t1 > -1L)
	  debug(F101,"rpack ttinl time","",t2-t1);
	else
	  debug(F100,"rpack ttinl time error","",0);
    }
#endif /* GETMSEC */

    if (j < 0) {
	debug(F101,"rpack: ttinl fails","",j); /* Otherwise, */
	freerbuf(k);			/* Free this buffer */
	if (j < -1) {			/* Bail out if ^C^C typed. */
	    debug(F101,"rpack ^C server","",server);
	    debug(F101,"rpack ^C en_fin","",en_fin);
	    if (server == 0) return(j);	/* But not if in server mode */
	    else if (en_fin) return(j);	/* with DISABLE FINISH */
	    else return(-2);
	}
	if (nakstate)			/* Call it a timeout. */
	  screen(SCR_PT,'T',(long)winlo,"");
	else
	  screen(SCR_PT,'T',(long)pktnum,"");
	logpkt('r',-1,(CHAR *)"<timeout>");
	if (flow == 1) ttoc(XON);	/* In case of Xoff blockage. */
	return('T');
    }
    rpktl = j;
    tlci += j;				/* All OK, Count the characters. */
    flci += j;

#ifndef PARSENSE
/* THEN eliminate this loop... */
    for (i = 0; (recpkt[i] != stchr) && (i < j); i++)
      sohp++;				/* Find mark */
    if (i++ >= j) {			/* Didn't find it. */
	logpkt('r',-1,"<timeout>");
	freerbuf(k);
	return('T');
    }    
#else
    i = 1;
#endif /* PARSENSE */

    rpackets++;
    lp = i;				/* Remember LEN position. */
    if ((j = xunchar(recpkt[i++])) == 0) {
        if ((j = lp+5) > MAXRP) return('Q'); /* Long packet */
	x = recpkt[j];			/* Header checksum. */
	recpkt[j] = '\0';		/* Calculate & compare. */
	if (xunchar(x) != chk1(recpkt+lp)) {
	    freerbuf(k);
	    logpkt('r',-1,(CHAR *)"<crunched:hdr>");
	    screen(SCR_PT,'%',(long)pktnum,"Bad packet header");
	    return('Q');
	}
	recpkt[j] = x;			/* Checksum ok, put it back. */
	rln = xunchar(recpkt[j-2]) * 95 + xunchar(recpkt[j-1]) - bctl;
	j = 3;				/* Data offset. */
    } else if (j < 3) {
	debug(F101,"rpack packet length less than 3","",j);
	freerbuf(k);
	logpkt('r',-1,(CHAR *)"<crunched:len>");
	screen(SCR_PT,'%',(long)pktnum,"Bad packet length");
	return('Q');
    } else {
	rln = j - bctl - 2;		/* Regular packet */
	j = 0;				/* No extended header */
    }
    rsn = xunchar(recpkt[i++]);		/* Sequence number */
    logpkt('r',rsn,sohp);
    if (rsn < 0 || rsn > 63) {
	debug(F101,"rpack bad sequence number","",rsn);
	freerbuf(k);
	logpkt('r',rsn,(CHAR *)"<crunched:seq>");
	screen(SCR_PT,'%',(long)pktnum,"Bad sequence number");
	return('Q');
    }
/*
  If this packet has the same type as the packet just sent, assume it is
  an echo and ignore it.  Don't even bother with the block check calculation:
  even if the packet is corrupted, we don't want to NAK an echoed packet.
  (And we certainly don't want to NAK an ACK or NAK!)
*/
    type = recpkt[i++];			/* Get packet's TYPE field */
    if (type == sndtyp || (nakstate && (type == 'N' /* || type == 'Y' */ ))) {
	debug(F000,"rpack echo","",type); /* If it's an echo */
	freerbuf(k);			/* Free this buffer */
	logpkt('#',rsn,(CHAR *)"<echo:ignored>");
	return('e');			/* return special (lowercase) code */
    }
/*
  Separate the data from the block check, accounting for the case where    
  a packet was retransmitted after the block check switched.
*/
    if (type == 'I' || type == 'S') {	/* I & S packets always have type 1 */
	chklen = 1;
	rln = rln + bctl - 1;
    } else if (type == 'N') {		/* A NAK packet never has data */
	chklen = xunchar(recpkt[lp]) - 2;
	rln = rln + bctl - chklen;
    } else chklen = bctl;
    debug(F101,"rpack bctl","",bctl);
    debug(F101,"rpack chklen","",chklen);

    i += j;				/* Buffer index of DATA field */
    rdatap = recpkt+i;			/* Pointer to DATA field */
    if ((j = rln + i) > r_pkt[k].bf_len ) { /* Make sure it fits */
	debug(F101,"packet sticks out too far","",j);
	freerbuf(k);
	logpkt('r',rsn,(CHAR *)"<overflow>");
	return('Q');
    }

    for (x = 0; x < chklen; x++)	/* Copy the block check */
      pbc[x] = recpkt[j+x];
    pbc[x] = '\0';			/* Null-terminate block check string */
    recpkt[j] = '\0';			/*  and the packet DATA field. */
    
    if (chklen == 2 && bctu == 4) {	/* Adjust for Blank-Free-2 */
	chklen = 4;			/* (chklen is now a misnomer...) */
	debug(F100,"rpack block check B","",0);
    }
    switch (chklen) {			/* Check the block check */
	case 1:				/* Type 1, 6-bit checksum */
	    if (xunchar(*pbc) != chk1(recpkt+lp)) {
		debug(F110,"checked chars",recpkt+lp,0);
	        debug(F101,"block check","",(int) xunchar(*pbc));
		debug(F101,"should be","",chk1(recpkt+lp));
		freerbuf(k);
		logpkt('r',-1,(CHAR *)"<crunched:chk1>");
		screen(SCR_PT,'%',(long)pktnum,"Checksum error");
		return('Q');
 	    }
	    break;
	case 2:				/* Type 2, 12-bit checksum */
	    x = xunchar(*pbc) << 6 | xunchar(pbc[1]);
	    if (x != chk2(recpkt+lp)) {	/* No match */
		if (type == 'E') {	/* Allow E packets to have type 1 */
		    recpkt[j++] = pbc[0];    
		    recpkt[j] = '\0';
		    if (xunchar(pbc[1]) == chk1(recpkt+lp))
		      break;
		    else
		      recpkt[--j] = '\0';
		}
		debug(F110,"checked chars",recpkt+lp,0);
	        debug(F101,"block check","", x);
		debug(F101,"should be","", (int) chk2(recpkt+lp));
		freerbuf(k);
		logpkt('r',-1,(CHAR *)"<crunched:chk2>");
		screen(SCR_PT,'%',(long)pktnum,"Checksum error");
		return('Q');
	    }
	    break;
	case 3:				/* Type 3, 16-bit CRC */
	    crc = (xunchar(pbc[0]) << 12)
	        | (xunchar(pbc[1]) << 6)
		| (xunchar(pbc[2]));
	    if (crc != chk3(recpkt+lp,parity)) {
		if (type == 'E') {	/* Allow E packets to have type 1 */
		    recpkt[j++] = pbc[0];    
		    recpkt[j++] = pbc[1];
		    recpkt[j] = '\0';
		    if (xunchar(pbc[2]) == chk1(recpkt+lp))
		      break;
		    else { j -=2; recpkt[j] = '\0'; }
		}
		debug(F110,"checked chars",recpkt+lp,0);
	        debug(F101,"block check","",xunchar(*pbc));
		debug(F101,"should be","",(int) chk3(recpkt+lp,parity));
		freerbuf(k);
		logpkt('r',-1,(CHAR *)"<crunched:chk3>");
		screen(SCR_PT,'%',(long)pktnum,"CRC error");
		return('Q');
	    }
	    break;
	case 4:				/* Type 4 = Type 2, no blanks. */
	    x = (unsigned)((xunchar(*pbc) - 1) << 6) |
	        (unsigned)(xunchar(pbc[1]) - 1);
	    if (x != chk2(recpkt+lp)) {
		if (type == 'E') {	/* Allow E packets to have type 1 */
		    recpkt[j++] = pbc[0];    
		    recpkt[j] = '\0';
		    if (xunchar(pbc[1]) == chk1(recpkt+lp))
		      break;
		    else
		      recpkt[--j] = '\0';
		}
		debug(F101,"bad type B block check","",x);
		freerbuf(k);
		logpkt('r',-1,(CHAR *)"<crunched:chkb>");
		screen(SCR_PT,'%',(long)pktnum,"Checksum error");
		return('Q');
	    }
	    break;
	default:			/* Shouldn't happen... */
	    freerbuf(k);
	    logpkt('r',-1,(CHAR *)"<crunched:chkx>");
	    screen(SCR_PT,'%',(long)pktnum,"(crunched)");
	    return('Q');
    }
    debug(F101,"rpack block check OK","",rsn);

/* Now we can believe the sequence number, and other fields. */
/* Here we violate strict principles of layering, etc, and look at the  */
/* packet sequence number.  If there's already a packet with the same   */
/* number in the window, we remove this one so that the window will not */
/* fill up. */

    if ((x = rseqtbl[rsn]) != -1) {	/* Already a packet with this number */
	retrans++;			/* Count it for statistics */
	debug(F101,"rpack got dup","",rsn);
	logpkt('r',rsn,(CHAR *)"<duplicate>");
	freerbuf(x);			/* Free old buffer, keep new packet. */
	r_pkt[k].pk_rtr++;		/* Count this as a retransmission. */
    }

/* New packet, not seen before, enter it into the receive window. */

#ifdef CK_TIMERS
    rrttbl[rsn] = gtimer();		/* Timestamp */
#endif /* CK_TIMERS */

    rseqtbl[rsn] = k;			/* Make back pointer */
    r_pkt[k].pk_seq = rsn;		/* Record in packet info structure */
    r_pkt[k].pk_typ = type;		/* Sequence, type,... */
    r_pkt[k].pk_adr = rdatap;		/* pointer to data buffer */
    screen(SCR_PT,(char)type,(long)rsn,(char *)sohp); /* Update screen */
    return(type);			/* Return packet type */
}

/*  L O G P K T  --  Log packet number n, pointed to by s.  */

/* c = 's' (send) or 'r' (receive) */

VOID
#ifdef CK_ANSIC
logpkt(char c,int n, CHAR *s)
#else
logpkt(c,n,s) char c; int n; CHAR *s;
#endif /* CK_ANSIC */
/* logpkt */ {
    char plog[20];
    if (pktlog && *s) {
	if (n < 0)
	  sprintf(plog,"%c-xx-%02d-",c,(gtimer()%60));
	else
	  sprintf(plog,"%c-%02d-%02d-",c,n,(gtimer()%60));
	if (zsout(ZPFILE,plog) < 0) pktlog = 0;
	else if (zsoutl(ZPFILE,(char *)s) < 0) pktlog = 0;
    }
}

#ifdef TLOG

/*  T S T A T S  --  Record statistics in transaction log  */

VOID
tstats() {
    char *tp;
    ztime(&tp);				/* Get time stamp */
    tlog(F100,"","",0L);		/* Leave a blank line */
    tlog(F110,"Transaction complete",tp,0L);  /* Record it */

    if (filcnt < 1) return;		/* If no files, done. */

/* If multiple files, record character totals for all files */

    if (filcnt > 1) {
	tlog(F101," files transferred       ","",filcnt - filrej);
	tlog(F101," total file characters   ","",tfc);
	tlog(F101," communication line in   ","",tlci);
	tlog(F101," communication line out  ","",tlco);
    }

/* Record timing info for one or more files */

    tlog(F101," elapsed time (seconds)  ","",(long) tsecs);
    if (tsecs > 0) {
	long lx;
	lx = (tfc * 10L) / (long) tsecs;
	tlog(F101," effective data rate     ","",lx/10L);
	if (speed <= 0L) speed = ttgspd();
#ifdef COMMENT
	if (speed > 0L && speed != 8880L && network == 0) {
	    lx = (lx * 100L) / speed;
	    tlog(F101," efficiency (percent)    ","",lx);
	}
#endif /* COMMENT */
    }
    tlog(F100,"","",0L);		/* Leave a blank line */
}

/*  F S T A T S  --  Record file statistics in transaction log  */

VOID
fstats() {
    tfc += ffc;
    if (!discard && !cxseen && !czseen && what != W_NOTHING)
      tlog(F101," complete, size","",ffc);
}
#else /* NOTLOG */
VOID
tstats() {}

VOID
fstats() {
    tfc += ffc;
}
#endif /* TLOG */
