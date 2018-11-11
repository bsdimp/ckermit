char *protv = "C-Kermit Protocol Module 6.0.095, 6 Sep 96"; /* -*-C-*- */

/* C K C P R O  -- C-Kermit Protocol Module, in Wart preprocessor notation. */
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
#include "ckcsym.h"
#include "ckcdeb.h"
#include "ckcasc.h"
#include "ckcker.h"
#ifdef OS2
#ifndef NT
#define INCL_NOPM
#define INCL_VIO			/* Needed for ckocon.h */
#include <os2.h> 
#undef COMMENT
#endif /* NT */
#include "ckocon.h"
#endif /* OS2 */

/*
 Note -- This file may also be preprocessed by the UNIX Lex program, but 
 you must indent the above #include statements before using Lex, and then
 restore them to the left margin in the resulting C program before compilation.
 Also, the invocation of the "wart()" function below must be replaced by an
 invocation  of the "yylex()" function.  It might also be necessary to remove
 comments in the %%...%% section.
*/

/* State definitions for Wart (or Lex) */
%states ipkt rfile rattr rdata ssinit ssfile ssattr ssdata sseof sseot
%states serve generic get rgen

/* External C-Kermit variable declarations */
  extern char *versio, *srvtxt, *cmarg, *cmarg2, **cmlist, *rf_err;
  extern char filnam[], fspec[], ttname[];
  extern CHAR sstate, *rpar(), *srvptr, *data;
  extern int timint, rtimo, nfils, hcflg, xflg, flow, mdmtyp, network;
  extern int rejection, moving, fncact, bye_active;
  extern int protocol, prefixing, filcnt;
  extern struct ck_p ptab[];
  extern int remfile, rempipe;
  extern char * remdest;

#ifndef NOSERVER
  extern char * x_user, * x_passwd, * x_acct;
  extern int x_login, x_logged;
#endif /* NOSERVER */

#ifdef NETCONN
#ifdef CK_SPEED
  extern int ttnproto;			/* Network protocol */
  extern short ctlp[];			/* Control-character prefix table */
#endif /* CK_SPEED */
#ifdef TCPSOCKET
#include "ckcnet.h"
  extern int me_binary, tn_b_nlm, tn_nlm;
#endif /* TCPSOCKET */
#endif /* NETCONN */

#ifdef TCPSOCKET
#ifndef NOLISTEN
  extern int tcpsrfd;
#endif /* NOLISTEN */
#endif /* TCPSOCKET */

  extern int cxseen, czseen, server, srvdis, local, displa, bctu, bctr, bctl;
  extern int quiet, tsecs, parity, backgrd, nakstate, atcapu, wslotn, winlo;
  extern int wslots, success, xitsta, rprintf, discard, cdtimo, keep, fdispla;
  extern int timef, stdinf, rscapu, sendmode, epktflg;
  extern int binary, fncnv;
  extern long speed, ffc, crc16;
  extern char *DIRCMD, *DIRCM2, *DELCMD, *TYPCMD, *SPACMD, *SPACM2, *WHOCMD;
  extern CHAR *rdatap;
  extern struct zattr iattr;

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

#ifdef CK_TMPDIR
extern int f_tmpdir;			/* Directory changed temporarily */
extern char savdir[];			/* For saving current directory */
extern char * dldir;
#endif /* CK_TMPDIR */

#ifndef NOSPL
  extern int cmdlvl;
  char querybuf[QBUFL+1] = { NUL, NUL }; /* QUERY response buffer */
  char *qbufp = querybuf;		/* Pointer to it */
  int qbufn = 0;			/* Length of data in it */
  extern int query;			/* Query-active flag */
#else
  extern int tlevel;
#endif /* NOSPL */

#ifdef NT
  extern int escape;
#endif /* NT */
/*
  If the following flag is nonzero when the protocol module is entered,
  then server mode persists for exactly one transaction, rather than
  looping until BYE or FINISH is received.
*/
int justone = 0;

_PROTOTYP(static VOID xxproto,(void));
_PROTOTYP(static int sgetinit,(int));
_PROTOTYP(int sndspace,(int));
_PROTOTYP(int wart,(void));

/* Flags for the ENABLE and DISABLE commands */
extern int
  en_cpy, en_cwd, en_del, en_dir, en_fin, en_get, en_bye, en_mai, en_pri,
  en_hos, en_ren, en_sen, en_spa, en_set, en_typ, en_who, en_ret;
#ifndef NOSPL
extern int en_asg, en_que;
#endif /* NOSPL */

/* Global variables declared here */

  int what = W_NOTHING;			/* What I am doing */
  int whatru = 0;			/* What are you */

/* Local variables */

  static char vstate = 0;  		/* Saved State   */
  static char vcmd = 0;    		/* Saved Command */
  static int reget = 0;
  static int retrieve = 0;

  static int x;				/* General-purpose integer */
  static char *s;			/* General-purpose string pointer */

/* Macros - Note, BEGIN is predefined by Wart (and Lex) as "state = ", */
/* BEGIN is NOT a GOTO! */
#define ENABLED(x) ((local && (x & 1)) || (!local && (x & 2)))
#define TINIT if (tinit() < 0) return(-9)
#define SERVE TINIT; nakstate=1; what=W_NOTHING; cmarg2=""; \
sendmode=SM_SEND; BEGIN serve;
#define RESUME if (!server) { return(0); } else \
if (justone) { justone=0; return(0); } else { SERVE; }
#define QUIT x=quiet; quiet=1; clsif(); clsof(1); tsecs=gtimer(); quiet=x; \
 return(1)

%%
/*
  Protocol entry points, one for each start state (sstate).
  The lowercase letters are internal "inputs" from the user interface.
*/

s { TINIT;				/* Do Send command */
    if (sinit() > 0) BEGIN ssinit;
       else RESUME; }

v { TINIT; nakstate = 1; BEGIN get; }	/* Receive */

r {					/* Get */
    TINIT;
    vstate = get;
    reget = 0;
    retrieve = 0;
    vcmd = 0;
    if (sipkt('I') >= 0)
      BEGIN ipkt;
    else
      RESUME;
}
h {					/* RETRIEVE */
    TINIT;
    vstate = get;
    reget = 0;
    retrieve = 1;
    vcmd = 0;
    if (sipkt('I') >= 0)
      BEGIN ipkt;
    else
      RESUME;
}
j {					/* REGET */
    TINIT;
    vstate = get;
    reget = 1;
    retrieve = 0;
    vcmd = 0;
    if (sipkt('I') >= 0)
      BEGIN ipkt;
    else
      RESUME;
}
c {					/* Host */
    TINIT;
    vstate = rgen;
    vcmd = 'C';
    if (sipkt('I') >= 0)
      BEGIN ipkt;
    else
      RESUME;
}
k { TINIT;				/* Kermit */
    vstate = rgen;
    vcmd = 'K';
    if (sipkt('I') >= 0)
      BEGIN ipkt;
    else
      RESUME;
}
g {					/* Generic */
    TINIT;
    vstate = rgen;
    vcmd = 'G';
    if (sipkt('I') >= 0)
      BEGIN ipkt;
    else
      RESUME;
}
x {					/* Be a Server */
    int x;
    x = justone;
    debug(F101,"x justone 1","",justone);
    SERVE;				/* tinit() clears justone... */
    debug(F101,"x justone 2","",justone);
    justone = x;
}
a {
    int b1 = 0, b2 = 0;
    if (!data) TINIT;			/* "ABEND" -- Tell other side. */
#ifndef pdp11
    if (epktflg) {			/* If because of E-PACKET command */
	b1 = bctl; b2 = bctu;		/* Save block check type */
	bctl = bctu = 1;		/* set it to 1 */
    }
#endif /* pdp11 */
    errpkt((CHAR *)"User cancelled");	/* Send the packet */
#ifndef pdp11
    if (epktflg) {			/* Restore the block check */
	epktflg = 0;
	bctl = b1; bctu = b2;
    }
    screen(SCR_EM,0,0L,"User cancelled");
#endif /* pdp11 */
    success = 0;
    return(0);				/* Return from protocol. */
}

/*
  Dynamic states: <current-states>input-character { action }
  nakstate != 0 means we're in a receiving state, in which we send ACKs & NAKs.
*/

<rgen,get,serve>S {			/* Receive Send-Init packet. */
#ifndef NOSERVER
    if (state == serve && x_login && !x_logged) {
	errpkt((CHAR *)"Login required");
	SERVE;
    } else
#endif /* NOSERVER */
      
      if (state == serve && !ENABLED(en_sen)) { /* Not in server mode */
	errpkt((CHAR *)"SEND disabled"); /* when SEND is disabled. */
	RESUME;
    } else {				/* OK to go ahead. */
#ifdef CK_TMPDIR
	if (dldir && !f_tmpdir) {	/* If they have a download directory */
	    debug(F110,"receive download dir",dldir,0);
	    if (s = zgtdir()) {		/* Get current directory */
		debug(F110,"receive current dir",s,0);
		if (zchdir(dldir)) {	/* Change to download directory */
		    debug(F100,"receive zchdir ok","",0);
		    strncpy(savdir,s,TMPDIRLEN);
		    f_tmpdir = 1;	/* Remember that we did this */
		} else
		  debug(F100,"receive zchdir failed","",0);
	    }
	}
#endif /* CK_TMPDIR */
	nakstate = 1;			/* Can send NAKs from here. */
	rinit(rdatap);			/* Set parameters */
	bctu = bctr;			/* Switch to agreed-upon block check */
	bctl = (bctu == 4) ? 2 : bctu;	/* Set block-check length */
	what = W_RECV;			/* Remember we're receiving */
	resetc();			/* Reset counters */
	rtimer();			/* Reset timer */
	BEGIN rfile;			/* Go into receive-file state */
    }
}

/* States in which we get replies back from commands sent to a server. */
/* Complicated because direction of protocol changes, packet number    */
/* stays at zero through I-G-S sequence, and complicated even more by  */
/* sliding windows buffer allocation. */

<ipkt>Y {				/* Get ack for I-packet */
    int x = 0;
    spar(rdatap);			/* Set parameters */
    winlo = 0;				/* Set window-low back to zero */
    if (vcmd) {				/* If sending a generic command */
	TINIT;
	x = scmd(vcmd,(CHAR *)cmarg);	/* Do that */
	vcmd = 0;			/* and then un-remember it. */
    } else if (vstate == get) {
	debug(F101,"REGET sstate","",sstate);
	x = srinit(reget, retrieve);	/* GET or REGET */
    }
    if (x < 0) {			/* If command was too long */
	errpkt((CHAR *)"Command too long for server"); /* cancel both sides. */
	ermsg("Command too long for server");
	success = 0;
	RESUME;
    } else {
	rtimer();			/* Reset the elapsed seconds timer. */
	winlo = 0;			/* Window back to 0, again. */
	nakstate = 1;			/* Can send NAKs from here. */
	BEGIN vstate;			/* Switch to desired state */
    }
}

<ipkt>E {				/* Ignore Error reply to I packet */
    int x = 0;
    winlo = 0;				/* Set window-low back to zero */
    if (vcmd) {				/* In case other Kermit doesn't */
	TINIT;
	x = scmd(vcmd,(CHAR *)cmarg);	/* understand I-packets. */
	vcmd = 0;			/* Otherwise act as above... */
    } else if (vstate == get) x = srinit(reget, retrieve);
    if (x < 0) {			/* If command was too long */
	errpkt((CHAR *)"Command too long for server"); /* cancel both sides. */
	ermsg("Command too long for server");
	success = 0;
	RESUME;
    } else {
	winlo = 0;			/* Back to packet 0 again. */
	freerpkt(winlo);		/* Discard the Error packet. */
	nakstate = 1;			/* Can send NAKs from here. */
	BEGIN vstate;
    }
}

<get>Y {		/* Resend of previous I-pkt ACK, same seq number! */
    srinit(reget, retrieve);		/* Send the GET packet again. */
}

/* States in which we're being a server */

<serve,get>I {				/* Get I-packet */
#ifndef NOSERVER
    spar(rdatap);			/* Set parameters from it */
    ack1(rpar());			/* Respond with our own parameters */
    pktinit();				/* Reinitialize packet numbers */
#endif /* NOSERVER */
}

<serve>R {				/* Get Receive-Init (GET) */
#ifndef NOSERVER
    if (x_login && !x_logged) {
	errpkt((CHAR *)"Login required");
	SERVE;
    } else if (sgetinit(0) < 0) {
	RESUME;
    } else {
	BEGIN ssinit;
    }
#endif /* NOSERVER */
}

<serve>H {				/* GET and DELETE ("retrieve") */
#ifndef NOSERVER
    if (x_login && !x_logged) {
	errpkt((CHAR *)"Login required");
	RESUME;
    } else if (!ENABLED(en_ret)) {
	errpkt((CHAR *)"RETRIEVE disabled - use GET");
	RESUME;
    } else if (!ENABLED(en_del)) {
	errpkt((CHAR *)"Deleting files disabled - use GET");
	RESUME;
    } else if (sgetinit(0) < 0) {
	RESUME;
    } else {
	moving = 1;
	BEGIN ssinit;
    }
#endif /* NOSERVER */
}


<serve>J {				/* Get REGET */
#ifndef NOSERVER
    if (x_login && !x_logged) {
	errpkt((CHAR *)"Login required");
	SERVE;
    } else if (sgetinit(1) < 0) {
	RESUME;
    } else {
	BEGIN ssinit;
    }
#endif /* NOSERVER */
}

<serve>G {				/* Generic server command */
#ifndef NOSERVER
    srvptr = srvcmd;			/* Point to command buffer */
    decode(rdatap,putsrv,0);		/* Decode packet data into it */
    putsrv(NUL);			/* Insert a couple nulls */
    putsrv(NUL);			/* for termination */
    if (srvcmd[0]) {
	sstate = srvcmd[0];		/* Set requested start state */
	if (x_login && !x_logged && sstate != 'I') {
	    errpkt((CHAR *)"Login required");
	    SERVE;
	} else {
	    nakstate = 0;		/* Now I'm the sender. */
	    what = W_REMO;		/* Doing a REMOTE command. */
	    if (timint < 1)
	      timint = chktimo(rtimo,timef); /* Switch to per-packet timer */
	    BEGIN generic;		/* Switch to generic command state */
	}
    } else {
	errpkt((CHAR *)"Badly formed server command"); /* report error */
	RESUME;			/* & go back to server command wait */
    }
#endif /* NOSERVER */
}

<serve>C {				/* Receive Host command */
#ifndef NOSERVER
    if (x_login && !x_logged) {
	errpkt((CHAR *)"Login required");
	SERVE;
    } else
      if (!ENABLED(en_hos)) {
	errpkt((CHAR *)"REMOTE HOST disabled");
	RESUME;
    } else {
	srvptr = srvcmd;		/* Point to command buffer */
	decode(rdatap,putsrv,0);	/* Decode command packet into it */
	putsrv(NUL);			/* Null-terminate */
	nakstate = 0;			/* Now sending, not receiving */
	if (syscmd((char *)srvcmd,"")) { /* Try to execute the command */
	    what = W_REMO;		/* Doing a REMOTE command. */
	    if (timint < 1)
	      timint = chktimo(rtimo,timef); /* Switch to per-packet timer */
	    BEGIN ssinit;		/* If OK, send back its output */
	} else {			/* Otherwise */
	    errpkt((CHAR *)"Can't do system command"); /* report error */
	    RESUME;			/* & go back to server command wait */
	}
    }
#endif /* NOSERVER */
}

<serve>q {				/* User typed Ctrl-C... */
#ifndef NOSERVER
    if (!ENABLED(en_fin)) {
	errpkt((CHAR *)"QUIT disabled");
	RESUME;
    } else {
	success = 0; QUIT;
    }
#endif /* NOSERVER */
}

<serve>N {				/* Server got a NAK in command-wait */
#ifndef NOSERVER
    errpkt((CHAR *)"Did you say RECEIVE instead of GET?");
    RESUME;
#endif /* NOSERVER */
}

<serve>. {				/* Any other command in this state */
#ifndef NOSERVER
    if (c != ('E' - SP) && c != ('Y' - SP)) /* except E and Y packets. */
      errpkt((CHAR *)"Unimplemented server function");
    /* If we answer an E with an E, we get an infinite loop. */
    /* A Y (ACK) can show up here if we sent back a short-form reply to */
    /* a G packet and it was echoed.  ACKs can be safely ignored here. */
    RESUME;				/* Go back to server command wait. */
#endif /* NOSERVER */
}

<generic>I {				/* Login/out */
#ifndef NOSERVER
    char f1[LOGINLEN+1], f2[LOGINLEN+1], f3[LOGINLEN+1];
    CHAR *p;
    int len, i;

    f1[0] = NUL; f2[0] = NUL; f3[0] = NUL;
    if (x_login) {			/* Login required */
	len = 0;
	if (srvcmd[1])			/* First length field */
	  len = xunchar(srvcmd[1]);	/* Separate the parameters */
	if (len > 0 && len <= LOGINLEN) { /* Have username */
	    p = srvcmd + 2;		/* Point to it */
	    for (i = 0; i < len; i++)	/* Copy it */
	      f1[i] = p[i];
	    f1[len] = NUL;		/* Terminate it */
	    p += len;			/* Point to next length field */
	    if (*p) {			/* If we have one */
		len = xunchar(*p++);	/* decode it */
		if (len > 0 && len <= LOGINLEN) {
		    for (i = 0; i < len; i++) /* Same deal for password */
		      f2[i] = p[i];
		    f2[len] = NUL;
		    p += len;		/* And account */
		    if (*p) {
			len = xunchar(*p++);	
			if (len > 0 && len <= LOGINLEN) {
			    for (i = 0; i < len; i++)
			      f3[i] = p[i];
			    f3[len] = NUL;
			}
		    }
		}
	    }
	}
	debug(F110,"XXX user",f1,0);
	debug(F110,"XXX pass",f2,0);
	debug(F110,"XXX acct",f3,0);
	if (!f1[0]) {
	    if (x_logged) {
		tlog(F110,"Logged out",x_user,0);
		ack1((CHAR *)"Logged out");
	    } else {
		ack1((CHAR *)"You were not logged in");
	    }
	    x_logged = 0;
	} else {
	    x_logged = 0;
	    if (x_user && x_passwd) {	/* Username and password must match */
		if (!strcmp(x_user,f1))
		  if (!strcmp(x_passwd,f2))
		    x_logged = 1;
	    } else if (x_user) {	/* Only username must match */
		if (!strcmp(x_user,f1))
		    x_logged = 1;
	    }		  
	    if (x_logged) {
		tlog(F110,"Logged in", x_user, 0);
		ack1((CHAR *)"Logged in");
	    } else {
		tlog(F110,"Login failed", f1, 0);
		ack1((CHAR *)"Login failed");
	    }
	}
    } else {
	ack1((CHAR *)"No login required");
    }
    SERVE;
#endif /* NOSERVER */
}

<generic>C {				/* Got REMOTE CD command */
#ifndef NOSERVER
    if (!ENABLED(en_cwd)) {
	errpkt((CHAR *)"REMOTE CD disabled");
	RESUME;
    } else {
	if (!cwd((char *)(srvcmd+1))) errpkt((CHAR *)"Can't change directory");
	RESUME;				/* Back to server command wait */
    }
#endif /* NOSERVER */
}

<generic>A {				/* Got REMOTE PWD command */
#ifndef NOSERVER
    if (!ENABLED(en_cwd)) {
	errpkt((CHAR *)"REMOTE CD disabled");
	RESUME;
    } else {
	if (encstr((CHAR *)zgtdir()) > -1) /* Get & encode current directory */
	  ack1(data);			/* If it fits, send it back in ACK */
	RESUME;				/* Back to server command wait */
    }
#endif /* NOSERVER */
}

<generic>D {				/* REMOTE DIRECTORY command */
#ifndef NOSERVER
    char *n2;
    if (!ENABLED(en_dir)) {		/* If DIR is disabled, */
	errpkt((CHAR *)"REMOTE DIRECTORY disabled"); /* refuse. */
	RESUME;
    } else {				/* DIR is enabled. */
	if (!ENABLED(en_cwd)) {		/* But CWD is disabled */
	    zstrip((char *)(srvcmd+2),&n2); /* and they included a pathname, */
	    if (strcmp((char *)(srvcmd+2),n2)) { /* so refuse. */
		errpkt((CHAR *)"Access denied");
		RESUME;			/* Remember, this is not a goto! */
	    }
	}	
	if (state == generic) {			/* It's OK to go ahead. */
#ifdef COMMENT
	    n2 = (*(srvcmd+2)) ? DIRCMD : DIRCM2;
	    if (syscmd(n2,(char *)(srvcmd+2)))  /* If it can be done */
#else
	    if (snddir((char*)(srvcmd+2)))
#endif /* COMMENT */
    	      BEGIN ssinit;			/* send the results back */
	    else {				/* otherwise */
		errpkt((CHAR *)"Can't list directory"); /* report failure, */
		RESUME;			/* return to server command wait */
	    }
	}
    }
#endif /* NOSERVER */
}

<generic>E {				/* REMOTE DELETE (Erase) command */
#ifndef NOSERVER
    char *n2;
    if (!ENABLED(en_del)) {
	errpkt((CHAR *)"REMOTE DELETE disabled");    
	RESUME;
    } else {				/* DELETE is enabled */
	if (!ENABLED(en_cwd)) {		/* but CWD is disabled */
	    zstrip((char *)(srvcmd+2),&n2); /* and they included a pathname, */
	    if (strcmp((char *)(srvcmd+2),n2)) { /* so refuse. */
		errpkt((CHAR *)"Access denied");
		RESUME;			/* Remember, this is not a goto! */
	    }
	}	
	if (state == generic) {		/* It's OK to go ahead. */
	    if (
#ifdef COMMENT
	    syscmd(DELCMD,(char *)(srvcmd+2)) /* Old way */
#else
	    snddel((char*)(srvcmd+2))	/* New way */
#endif /* COMMENT */
		)
	      BEGIN ssinit;		/* If OK send results back */
	    else {			/* otherwise */
		errpkt((CHAR *)"Can't remove file"); /* report failure */
		RESUME;			/* & return to server command wait */
	    }
	}
    }
#endif /* NOSERVER */
}

<generic>F {				/* FINISH */
#ifndef NOSERVER
    if (!ENABLED(en_fin)) {
	errpkt((CHAR *)"FINISH disabled");    
	RESUME;
    } else {
	ack();				/* Acknowledge */
	screen(SCR_TC,0,0L,"");		/* Display */
	return(0);			/* Done */
    }
#endif /* NOSERVER */
}

<generic>L {				/* BYE */
#ifndef NOSERVER
    if (!ENABLED(en_bye)) {
	errpkt((CHAR *)"BYE disabled");    
	RESUME;
    } else {
	ack();				/* Acknowledge */
	ttres();			/* Reset the terminal */
	screen(SCR_TC,0,0L,"");		/* Display */
	doclean();			/* Clean up files, etc */
#ifdef DEBUG
	debug(F100,"C-Kermit BYE","",0);
	zclose(ZDFILE);
#endif /* DEBUG */
	return(zkself());		/* Try to log self out */
    }
#endif /* NOSERVER */
}

<generic>H {				/* REMOTE HELP */
#ifndef NOSERVER
    extern char * hlptxt;
    if (sndhlp(hlptxt)) BEGIN ssinit;	/* Try to send it */
    else {				/* If not ok, */
	errpkt((CHAR *)"Can't send help"); /* send error message instead */
	RESUME;				/* and return to server command wait */
    }
#endif /* NOSERVER */
}

<generic>R {                            /* REMOTE RENAME */
#ifndef NOSERVER
#ifdef ZRENAME
    char *str1, *str2, f1[256], f2[256];
    int  len1, len2;

    if (!ENABLED(en_ren)) {
	errpkt((CHAR *)"REMOTE RENAME disabled");
	RESUME;
    } else {				/* RENAME is enabled */
	int len1, len2;
	len1 = xunchar(srvcmd[1]);	/* Separate the parameters */
	len2 = xunchar(srvcmd[2+len1]);
	strncpy(f1,(char *)(srvcmd+2),len1);
	f1[len1] = NUL;
	strncpy(f2,(char *)(srvcmd+3+len1),len2);
	f2[len2] = NUL; 

	len2 = xunchar(srvcmd[2+len1]);
	strncpy(f1,(char *)(srvcmd+2),len1);
	f1[len1] = NUL;
	strncpy(f2,(char *)(srvcmd+3+len1),len2);
	f2[len2] = NUL; 
	    
	if (!ENABLED(en_cwd)) {		/* If CWD is disabled */
	    zstrip(f1,&str1);		/* and they included a pathname, */
	    zstrip(f2,&str2);
	    if ( strcmp(f1,str1) || strcmp(f2,str2) ) { /* refuse. */
		errpkt((CHAR *)"Access denied");
		RESUME;			/* Remember, this is not a goto! */
	    }
	}	
	if (state == generic) {		/* It's OK to go ahead. */
	    if (zrename(f1,f2)) {	/* Try */ 
		errpkt((CHAR *)"Can't rename file"); /* Give error msg */
	    } else ack();
	    RESUME;			/* Wait for next server command */
	}
    } 
#else /* no ZRENAME */
    /* Give error message */
    errpkt((CHAR *)"REMOTE RENAME not available"); 
    RESUME;				/* Wait for next server command */
#endif /* ZRENAME */
#endif /* NOSERVER */
}

<generic>K {                            /* REMOTE COPY */
#ifndef NOSERVER
#ifdef ZCOPY
    char *str1, *str2, f1[256], f2[256];
    int  len1, len2;
    if (!ENABLED(en_cpy)) {
	errpkt((CHAR *)"REMOTE COPY disabled");
	RESUME;
    } else {
        len1 = xunchar(srvcmd[1]);	/* Separate the parameters */
        len2 = xunchar(srvcmd[2+len1]);
        strncpy(f1,(char *)(srvcmd+2),len1);
        f1[len1] = NUL;
        strncpy(f2,(char *)(srvcmd+3+len1),len2);
        f2[len2] = NUL; 

	if (!ENABLED(en_cwd)) {		/* If CWD is disabled */
	    zstrip(f1,&str1);		/* and they included a pathname, */
            zstrip(f2,&str2);
	    if (strcmp(f1,str1) || strcmp(f2,str2)) { /* Refuse. */
		errpkt((CHAR *)"Access denied");
		RESUME;			/* Remember, this is not a goto! */
	    }
	}	
	if (state == generic) {		/* It's OK to go ahead. */
            if (zcopy(f1,f2)) {		/* Try */ 
		errpkt((CHAR *)"Can't copy file"); /* give error message */
	    } else ack();
            RESUME;			/* wait for next server command */
	}
    }
#else /* no ZCOPY */
    errpkt((CHAR *)"REMOTE COPY not available"); /* give error message */
    RESUME;				/* wait for next server command */
#endif /* ZCOPY */
#endif /* NOSERVER */
}

<generic>S {				/* REMOTE SET */
#ifndef NOSERVER
    if (!ENABLED(en_set)) {
	errpkt((CHAR *)"REMOTE SET disabled");
	RESUME;
    } else {
	if (remset((char *)(srvcmd+1)))	/* Try to do what they ask */
	  ack();			/* If OK, then acknowledge */
	else				/* Otherwise */
	  errpkt((CHAR *)"Unknown REMOTE SET parameter"); /* give error msg */
	RESUME;				/* Return to server command wait */
    }
#endif /* NOSERVER */
}

<generic>T {				/* REMOTE TYPE */
#ifndef NOSERVER
    char *n2;
    if (!ENABLED(en_typ)) {
	errpkt((CHAR *)"REMOTE TYPE disabled");
	RESUME;
    } else {
	if (!ENABLED(en_cwd)) {		/* If CWD disabled */
	    zstrip((char *)(srvcmd+2),&n2); /* and they included a pathname, */
	    if (strcmp((char *)(srvcmd+2),n2)) { /* refuse. */
		errpkt((CHAR *)"Access denied");
		RESUME;			/* Remember, this is not a goto! */
	    }
	}	
	if (state == generic) {		/* It's OK to go ahead. */
	    binary = XYFT_T;		/* Use text mode for this. */
	    if (			/* (RESUME didn't change state) */
#ifdef COMMENT
	      syscmd(TYPCMD,(char *)(srvcmd+2))	/* Old way */
#else
	      sndtype((char *)(srvcmd+2)) /* New way */
#endif /* COMMENT */
		)
	      BEGIN ssinit;			/* OK */
	    else {				/* not OK */
		errpkt((CHAR *)"Can't type file"); /* give error message */
		RESUME;			/* wait for next server command */
	    }
	}
    }
#endif /* NOSERVER */
}

<generic>U {				/* REMOTE SPACE */
#ifndef NOSERVER
    if (!ENABLED(en_spa)) {
	errpkt((CHAR *)"REMOTE SPACE disabled");
	RESUME;
    } else {
	x = srvcmd[1];			/* Get area to check */
	x = ((x == NUL) || (x == SP)
#ifdef OS2
	     || (x == '!') || (srvcmd[3] == ':')
#endif /* OS2 */
	     );
	if (!x && !ENABLED(en_cwd)) {	/* CWD disabled */
	    errpkt((CHAR *)"Access denied"); /* and non-default area given, */
	    RESUME;			/* refuse. */
	} else {
#ifdef OS2
_PROTOTYP(int sndspace,(int));
	    if (sndspace(x ? toupper(srvcmd[2]) : 0))
	      BEGIN ssinit;		/* Try to send it */
	    else {			/* If not ok, */
		errpkt((CHAR *)"Can't send space"); /* send error message */
		RESUME;			/* and return to server command wait */
	    }
#else
	    x = (x ? syscmd(SPACMD,"") : syscmd(SPACM2,(char *)(srvcmd+2)));
	    if (x) {				/* If we got the info */
		BEGIN ssinit;			/* send it */
	    } else {				/* otherwise */
		errpkt((CHAR *)"Can't check space"); /* send error message */
		RESUME;			/* and await next server command */
	    }
#endif /* OS2 */
	}
    }
#endif /* NOSERVER */
}

<generic>W {				/* REMOTE WHO */
#ifndef NOSERVER
    if (!ENABLED(en_who)) {
	errpkt((CHAR *)"REMOTE WHO disabled");
	RESUME;
    } else {
#ifdef OS2
_PROTOTYP(int sndwho,(char *));
	    if (sndwho((char *)(srvcmd+2)))
	      BEGIN ssinit;		/* Try to send it */
	    else {			/* If not ok, */
		errpkt((CHAR *)"Can't do who command"); /* send error msg */
		RESUME;			/* and return to server command wait */
	    }
#else
	if (syscmd(WHOCMD,(char *)(srvcmd+2))) /* The now-familiar scenario. */
	  BEGIN ssinit;
	else {
	    errpkt((CHAR *)"Can't do who command");
	    RESUME;
	}
#endif /* OS2 */
    }
#endif /* NOSERVER */
}

<generic>V {				/* Variable query or set */
#ifndef NOSERVER
#ifndef NOSPL
_PROTOTYP( int addmac, (char *, char *) );
_PROTOTYP( int zzstring, (char *, char **, int *) );
    char c;
    c = *(srvcmd+2);			/* Q = Query, S = Set */
    if (c == 'Q') {			/* Query */
	if (!ENABLED(en_que)) { /* Security */
	    errpkt((CHAR *)"REMOTE QUERY disabled");
	    RESUME;
	} else {			/* Query allowed */
	    int n; char *p, *q;
	    qbufp = querybuf;		/* Wipe out old stuff */
	    qbufn = 0;
	    querybuf[0] = NUL;
	    p = (char *) srvcmd + 3;	/* Pointer for making wrapper */
	    n = strlen((char *)srvcmd);	/* Position of end */
	    c = *(srvcmd+4);		/* Which type of variable */

	    if (*(srvcmd+6) == CMDQ) {	/* Starts with command quote? */
		p = (char *) srvcmd + 6; /* Take it literally */
		if (*p == CMDQ) p++;
	    } else {			/* They played by the rules */
		if (c == 'K') {		/* Kermit variable */
		    int k;
		    k = (int) strlen(p);
		    if (k > 0 && p[k-1] == ')') {
			p = (char *)(srvcmd + 4);
			*(srvcmd+4) = CMDQ;
			*(srvcmd+5) = 'f'; /* Function, so make it \f...() */
		    } else {
			*(srvcmd+3) = CMDQ; /* Stuff wrapping into buffer */
			*(srvcmd+4) = 'v';  /* Variable, so make it \v(...) */
			*(srvcmd+5) = '(';  /* around variable name */
			*(srvcmd+n) = ')';
			*(srvcmd+n+1) = NUL;
		    }
		} else {
		    *(srvcmd+3) = CMDQ; /* Stuff wrapping into buffer */
		    *(srvcmd+4) = 'v'; /*  Variable, so make it \v(...) */
		    *(srvcmd+5) = '(';	/* around variable name */
		    *(srvcmd+n) = ')';
		    *(srvcmd+n+1) = NUL;
		    if (c == 'S') {	/* System variable */
			*(srvcmd+4) = '$'; /*  so it's \$(...) */
		    } else if (c == 'G') { /* Non-\ Global variable */
			*(srvcmd+4) = 'm'; /*  so wrap it in \m(...) */
		    }
		}
	    }				/* Now evaluate it */
	    n = QBUFL;			/* Max length */
	    q = querybuf;		/* Where to put it */
	    if (zzstring(p,&q,&n) < 0) {
		errpkt((n > 0) ? (CHAR *)"Can't get value"
		               : (CHAR *)"Value too long"
		       );
		RESUME;
	    } else {
		if (encstr((CHAR *)querybuf) > -1) { /* Encode it */
		    ack1(data);		/* If it fits, send it back in ACK */
		    RESUME;
		} else if (sndhlp(querybuf)) { /* Long form response */
		    BEGIN ssinit;
		} else {		/* sndhlp() fails */
		    errpkt((CHAR *)"Can't send value");
		    RESUME;
		}
	    }
	}
    } else if (c == 'S') {		/* Set (assign) */
	if (!ENABLED(en_asg)) {		/* Security */
	    errpkt((CHAR *)"REMOTE ASSIGN disabled");
	    RESUME;
	} else {			/* OK */
	    int n;
	    n = xunchar(*(srvcmd+3));	/* Length of name */
	    n = 3 + n + 1;		/* Position of length of value */
	    *(srvcmd+n) = NUL;		/* Don't need it */
	    if (addmac((char *)(srvcmd+4),(char *)(srvcmd+n+1)) < 0)
	      errpkt((CHAR *)"REMOTE ASSIGN failed");
	    else
	      ack();
	    RESUME;
	}
    } else {
	errpkt((CHAR *)"Badly formed server command");
	RESUME;
    }
#else
    errpkt((CHAR *)"Variable query/set not available");
    RESUME;
#endif /* NOSPL */
#endif /* NOSERVER */
}

<generic>q {
#ifndef NOSERVER
    if (!ENABLED(en_fin)) {		/* Ctrl-C typed */
	errpkt((CHAR *)"QUIT disabled");
	RESUME;
    } else {
	success = 0; QUIT;
    }
#endif /* NOSERVER */
}

<generic>. {				/* Anything else in this state... */
#ifndef NOSERVER
    errpkt((CHAR *)"Unimplemented REMOTE command"); /* Complain */
    RESUME;				/* and return to server command wait */
#endif /* NOSERVER */
}

<rgen>Y {				/* Short-Form reply */
#ifndef NOSERVER
#ifndef NOSPL
    if (query) {			/* If to query, */
	qbufp = querybuf;		/*  initialize query response buffer */
	qbufn = 0;
	querybuf[0] = NUL;
    }
#endif /* NOSPL */
    decode(rdatap,puttrm,0);		/* Text is in ACK Data field */
    if (rdatap)				/* If we had data */
      if (*rdatap)
	 conoll("");			/* Then add a CRLF */
    if (bye_active && network) {	/* I sent a BYE command and got */
	msleep(500);			/* the ACK... */
	tthang();
    }
    success = 1;
    RESUME;
#endif /* NOSERVER */
}

<rgen,rfile>F {				/* File header */
    xflg = 0;				/* Not screen data */
    if (!rcvfil(filnam)) {		/* Figure out local filename */
	errpkt((CHAR *)rf_err);		/* Trouble */
	screen(SCR_EM,0,0L,rf_err);
	RESUME;
    } else {				/* Real file, OK to receive */
	if (filcnt == 1)		/* rcvfil set this to 1 for 1st file */
	  crc16 = 0L;			/* Clear file CRC */
#ifndef NOFULLNAME
#ifdef ZFNQFP				/* Because of zfnqfp() */
#ifdef BIGBUFOK				/* Because it's another 1K buffer */
#define USEFULLNAME			/* Memory to burn - do it */
#endif /* BIGBUFOK */
#endif /* ZFNQFP */
#endif /* NOFULLNAME */

#ifdef USEFULLNAME			/* Name to send back in ACK */
	if (!isabsolute(filnam)) {
	    CHAR tmpbuf[91];		/* Must fit in ACK Data field */
	    struct zfnfp * fnp;
	    fnp = zfnqfp(filnam,90,(char *)tmpbuf);
	    encstr(fnp ? tmpbuf: (CHAR *)filnam); /* Send the full pathname */
	} else
#endif /* USEFULLNAME */
	  encstr((CHAR *)filnam);	/* Encode the local filename */

	ack1(data);			/* Send it back in ACK */
	initattr(&iattr);		/* Clear file attribute structure */
	if (window(wslotn) < 0) {	/* Allocate negotiated window slots */
	    errpkt((CHAR *)"Can't open window");
	    RESUME;
	}
	BEGIN rattr;			/* Now expect Attribute packets */
    }
}

<rgen,rfile>X {				/* X-packet instead of file header */
    xflg = 1;				/* Screen data */
    ack();				/* Acknowledge the X-packet */
    initattr(&iattr);			/* Initialize attribute structure */
    if (window(wslotn) < 0) {		/* allocate negotiated window slots */
	errpkt((CHAR *)"Can't open window");
	RESUME;
    }
#ifndef NOSPL
    if (query) {			/* If this is the response to */
	qbufp = querybuf;		/* a query that we sent, initialize */
	qbufn = 0;			/* the response buffer */
	querybuf[0] = NUL;
    }
#endif /* NOSPL */
    what = W_REMO;			/* we're doing a REMOTE command */
    BEGIN rattr;			/* Expect Attribute packets */
}

<rattr>A {				/* Attribute packet */
    if (gattr(rdatap,&iattr) == 0) {	/* Read into attribute structure */
#ifdef CK_RESEND
	ack1((CHAR *)iattr.reply.val);	/* Reply with data */
#else
	ack();				/* If OK, acknowledge */
#endif /* CK_RESEND */
    } else {				/* Otherwise */
	ack1((CHAR *)iattr.reply.val);	/* refuse to accept the file */
	screen(SCR_ST,ST_REFU,0L,getreason(iattr.reply.val)); /* give reason */
    }
}

<rattr>D {				/* First data packet */
    if (discard) {			/* if we're discarding the file */
	ack1((CHAR *)"X");		/* just ack the data like this. */
	BEGIN rdata;			/* and wait for more data packets. */
    } else {				/* Not discarding. */
	rf_err = "Can't open file";
	if (xflg) {			/* If screen data */
	    if (remfile) {		/* redirected to file */
		if (rempipe)		/* or pipe */
		  x = zxcmd(ZOFILE,remdest); /* Pipe: start command */
		else
		  x = opena(remdest,&iattr); /* File: open with attributes */
	    } else {			/* otherwise */
		x = opent(&iattr);	/* "open" the screen */
	    }
	} else {			/* otherwise */
	    x = opena(filnam,&iattr);	/* open the file, with attributes */
	}
	if (x) {			/* If file was opened ok */
	    if (decode(rdatap, 
#ifndef NOSPL
		       query ? puttrm : 
#endif /* NOSPL */
		       putfil, 1) < 0) {

		errpkt((CHAR *)"Error writing data");
		RESUME;
	    }
	    ack();			/* acknowledge it */
	    BEGIN rdata;		/* and switch to receive-data state */
	} else {			/* otherwise */
	    errpkt((CHAR *) rf_err);	/* send error message */
	    RESUME;			/* and quit. */
	}
    }
}

<rfile>B {				/* EOT, no more files */
    ack();				/* Acknowledge */
    tsecs = gtimer();			/* Get timing for statistics */
    reot();				/* Do EOT things */
#ifdef CK_TMPDIR
/* If we were cd'd temporarily to another device or directory ... */
    if (f_tmpdir) {
	int x;
	x = zchdir((char *) savdir);	/* ... restore previous directory */
	f_tmpdir = 0;			/* and remember we did it. */
	debug(F111,"ckcpro.w B tmpdir restoring",savdir,x);
    }
#endif /* CK_TMPDIR */
    RESUME;				/* and quit */
}

<rdata>D {				/* Data packet */
    if (cxseen || discard)		/* If file interrupt */
      ack1((CHAR *)"X");		/* put "X" in ACK */
    else if (czseen)			/* If file-group interrupt */
      ack1((CHAR *)"Z");		/* put "Z" in ACK */
    else if (decode(rdatap, 
#ifndef NOSPL
		       query ? puttrm : 
#endif /* NOSPL */
		       putfil, 1) < 0) {
	errpkt((CHAR *)"Error writing data"); /* If failure, */
	clsof(!keep);			/*   Close & keep/discard the file */
	RESUME;				/* Send ACK only after data */
    } else ack();			/* written to file OK. */
}

<rattr>Z {				/* EOF immediately after A-Packet. */
    rf_err = "Can't create file";
    if (discard) {			/* Discarding a real file... */
	x = 1;
    } else if (xflg) {			/* If screen data */
	if (remfile) {			/* redirected to file */
	    if (rempipe)		/* or pipe */
	      x = zxcmd(ZOFILE,remdest); /* Pipe: start command */
	    else
	      x = opena(remdest,&iattr); /* File: open with attributes */
	} else {			/* otherwise */
	    x = opent(&iattr);		/* "open" the screen */
	}
    } else {				/* otherwise */
	x = opena(filnam,&iattr);	/* open the file, with attributes */
    }
    if (!x || reof(filnam, &iattr) < 0) { /* Now close & dispose of the file */
	errpkt((CHAR *) rf_err);	/* If problem, send error msg */
	RESUME;				/* and quit */
    } else {				/* otherwise */
	ack();				/* acknowledge the EOF packet */
	BEGIN rfile;			/* and await another file */
    }
}

<rdata>Z {				/* End Of File (EOF) Packet */
/*  wslots = 1;	*/			/* Window size back to 1 */
#ifndef COHERENT
/*
  Coherent compiler blows up on this switch() statement.
*/
    x = reof(filnam, &iattr);		/* Handle the EOF packet */
    switch (x) {			/* reof() sets the success flag */
      case -3:				/* If problem, send error msg */
	errpkt((CHAR *)"Can't print file"); /* Fatal */
        RESUME;
	break;
      case -2:
	errpkt((CHAR *)"Can't mail file"); /* Fatal */
        RESUME;
	break;
      case 2:
      case 3:
	screen(SCR_EM,0,0L,"Can't delete temp file"); /* Not fatal */
        RESUME;
	break;
      default:
	if (x < 0) {			/* Fatal */
	    errpkt((CHAR *)"Can't close file");
	    RESUME;
	} else {			/* Success */
#ifndef NOSPL
	    if (query)			/* Query reponses generally */
	      conoll("");		/* don't have line terminators */
#endif /* NOSPL */
	    ack();			/* Acknowledge the EOF packet */
	    BEGIN rfile;		/* and await another file */
	}
    }
#else
    if (reof(filnam, &iattr) < 0) {	/* Close and dispose of the file */
	errpkt((CHAR *)"Error at end of file");
	RESUME;
    } else {				/* reof() sets success flag */
	ack();
	BEGIN rfile;
    }
#endif /* COHERENT */
}

<ssinit>Y {				/* ACK for Send-Init */
    spar(rdatap);			/* set parameters from it */
    bctu = bctr;			/* switch to agreed-upon block check */
    bctl = (bctu == 4) ? 2 : bctu;	/* Set block-check length */
#ifdef CK_RESEND
    if ((sendmode == SM_RESEND) && (!atcapu || !rscapu)) { /* RESEND */
	errpkt((CHAR *) "RESEND capabilities not negotiated");
	ermsg("RESEND capabilities not negotiated");
	RESUME;
    } else {
#endif /* CK_RESEND */
	what = W_SEND;			/* Remember we're sending */
	x = sfile(xflg);		/* Send X or F header packet */
	if (x) {			/* If the packet was sent OK */
	    if (!xflg && filcnt == 1)	/* and it's a real file */
	      crc16 = 0L;		/* Clear the file CRC */
	    resetc();			/* reset per-transaction counters */
	    rtimer();			/* reset timers */
	    BEGIN ssfile;		/* and switch to receive-file state */
	} else {			/* otherwise send error msg & quit */
	    s = xflg ? "Can't execute command" : "Can't open file";
	    errpkt((CHAR *)s);
	    RESUME;
	}
#ifdef CK_RESEND
    }
#endif /* CK_RESEND */
}

/*
 These states are necessary to handle the case where we get a server command
 packet (R, G, or C) reply with an S packet, but the client retransmits the 
 command packet.  The input() function doesn't catch this because the packet 
 number is still zero.
*/
<ssinit>R {				/* R packet was retransmitted. */
    xsinit();				/* Resend packet 0 */
}

<ssinit>G {				/* Same deal if G packet comes again */
    xsinit();
}

<ssinit>C {				/* Same deal if C packet comes again */
    xsinit();
}

<ssfile>Y {				/* ACK for F packet */
    srvptr = srvcmd;			/* Point to string buffer */
    decode(rdatap,putsrv,0);		/* Decode data field, if any */
    putsrv(NUL);			/* Terminate with null */
    ffc = 0L;				/* Reset file byte counter */
    if (*srvcmd) {			/* If remote name was recorded */
        if (sendmode != SM_RESEND) {
	    if (fdispla == XYFD_C) 
	      screen(SCR_AN,0,0L,(char *)srvcmd);
	    tlog(F110," remote name:",(char *) srvcmd,0L);
        }
    }
    if (atcapu) {			/* If attributes are to be used */
	if (sattr(xflg | stdinf) < 0) {	/* set and send them */
	    errpkt((CHAR *)"Can't send attributes"); /* if problem, say so */
	    RESUME;			     /* and quit */
	} else BEGIN ssattr;		/* if ok, switch to attribute state */
    } else {
	if (window(wslotn) < 0) {
	    errpkt((CHAR *)"Can't open window");
	    RESUME;
	}
	if ((x = sdata()) == -2) {	/* No attributes, send data */
	    return(success = 0);	/* Failed */
	} else if (x == -1) {		/* EOF (eh?) */
	    clsif();			/* If not ok, close input file, */
	    window(1);			/* put window size back to 1, */
	    seof((CHAR *)"");		/* send EOF packet, */
	    BEGIN sseof;		/* and switch to EOF state. */
	} else BEGIN ssdata;		/* All ok, switch to send-data state */
    }
}

<ssattr>Y {				/* Got ACK to A packet */
    ffc = 0L;				/* Reset file byte counter */
    if (rsattr(rdatap) < 0) {		/* Was the file refused? */
	discard = 1;			/* Set the discard flag */
	clsif();			/* Close the file */
	sxeof((CHAR *)"D");		/* send EOF with "discard" code */
	BEGIN sseof;			/* switch to send-EOF state */
    } else {
	if (window(wslotn) < 0) {	/* Allocate negotiated window slots */
	    errpkt((CHAR *)"Can't open window");
	    RESUME;
	}
	if ((x = sdata()) == -2) {	/* File accepted, send data */
	    return(success = 0);	/* Failed */
	} else if (x == -1) {		/* EOF */
	    clsif();			/* If not ok, close input file, */
	    window(1);			/* put window size back to 1, */
	    seof((CHAR *)"");		/* send EOF packet, */
	    BEGIN sseof;		/* and switch to EOF state. */
	} else BEGIN ssdata;		/* All ok, switch to send-data state */
    }
}

<ssdata>Y {				/* Got ACK to Data packet */
    canned(rdatap);			/* Check if file transfer cancelled */
    if ((x = sdata()) == -2) {		/* Try to send next data */
	return(success = 0);		/* Failed */
    } else if (x == -1) {		/* EOF - finished sending data */
	clsif();			/* Close file */
	window(1);			/* Set window size back to 1... */
	if (cxseen || czseen)		/* If interrupted */
	  seof((CHAR *)"D");		/* send special EOF packet */
	else				/* otherwise */
	  seof((CHAR *)"");		/* regular EOF packet */
	BEGIN sseof;			/* and enter send-eof state */
    }
}

<sseof>Y {				/* Got ACK to EOF */
    success = (cxseen == 0 && czseen == 0); /* Transfer status... */
    if (success && rejection > 0)	    /* If rejected, succeed if */
      if (rejection != '#' &&		    /* reason was date */
	  rejection != 1 && rejection != '?') /* or name; */
	success = 0;			    /* fail otherwise. */
    cxseen = 0;				/* This goes back to zero. */
    if (success && moving) {		/* If MOVE'ing */
	tlog(F110," deleting",filnam,0); /* delete the file */
	zdelet(filnam);
    }
    if (gnfile() > 0) {			/* Any more files to send? */
	if (sfile(xflg))		/* Yes, try to send next file header */
	  BEGIN ssfile;			/* if ok, enter send-file state */
	else {				/* otherwise */
	    errpkt((CHAR *)"Can't open file");	/* send error message */
	    RESUME;			/* and quit */
	}
    } else {				/* No next file */
	tsecs = gtimer();		/* get statistics timers */
	seot();				/* send EOT packet */
	BEGIN sseot;			/* enter send-eot state */
    }
}

<sseot>Y {				/* Got ACK to EOT */
    debug(F101,"sseot justone","",justone);
    RESUME;				/* All done, just quit */
}

E {					/* Got Error packet, in any state */
    char *s = "";
    if (pktmsg)				/* Or we sent one. */
      if (*pktmsg)			/* If so, this was the message. */
	s = (char *)pktmsg;
    if (!*s)				/* We received an Error packet */
      s = (char *)rdatap;		/* with this message. */
    if (!*s)				/* Hopefully we'll never see this. */
      s = "Unknown error";
    ermsg(s);				/* Issue the message. */
    success = 0;			/* For IF SUCCESS/FAIL. */
    debug(F101,"ckcpro.w justone at E pkt","",justone);
    x = quiet; quiet = 1;		/* Close files silently, */
    clsif(); clsof(1); 			/* discarding any output file. */
    tsecs = gtimer();			/* Get timers */
    quiet = x;				/* restore quiet state */
/*
  If we are executing commands from a command file or macro, let the command
  file or macro decide whether to exit, based on SET { TAKE, MACRO } ERROR.
*/
    if (
#ifndef NOSPL
	cmdlvl == 0
#else
	tlevel < 0
#endif /* NOSPL */
	)
      if (backgrd && !server)
	fatal("Protocol error");
    xitsta |= what;			/* Save this for doexit(). */
#ifdef CK_TMPDIR
/* If we were cd'd temporarily to another device or directory ... */
    if (f_tmpdir) {
	int x;
	x = zchdir((char *) savdir);	/* ... restore previous directory */
	f_tmpdir = 0;			/* and remember we did it. */
	debug(F111,"ckcpro.w E tmpdir restoring",savdir,x);
    }
#endif /* CK_TMPDIR */
    RESUME;
}

q { success = 0; QUIT; }		/* Ctrl-C interrupt during packets. */

. {					/* Anything not accounted for above */
    errpkt((CHAR *)"Unexpected packet type"); /* Give error message */
    xitsta |= what;			/* Save this for doexit(). */
    RESUME;				/* and quit */
}
%%

/*  P R O T O  --  Protocol entry function  */

VOID
proto() {
    extern int b_save, f_save;
#ifdef OS2
    extern int cursorena[], cursor_save;
    extern BYTE vmode;
#endif /* OS2 */
/*
  This is simply a wrapper for the real protocol function just below,
  that saves any items that might be changed automatically by protocol
  negotiations, but which should not be sticky, and then restores them
  upon exit from protocol mode.
*/
#ifdef OS2
    cursor_save = cursorena[vmode] ;
    cursorena[vmode] = 0 ;
#endif /* OS2 */
    b_save = binary;			/* SET FILE TYPE */
    f_save = fncnv;			/* SET FILE NAMES */
    xxproto();				/* Call the real protocol function */
    fncnv  = f_save;
    binary = b_save;
#ifdef OS2
    cursorena[vmode] = cursor_save ;
#endif /* OS2 */
}

static VOID
xxproto() {

    int x;
    long lx;
#ifdef CK_XYZ
#ifdef XYZ_INTERNAL
_PROTOTYP( int pxyz, (int) );
#endif /* XYZ_INTERNAL */
#endif /* CK_XYZ */

    debug(F101,"xxproto entry justone","",justone);

/* Set up the communication line for file transfer. */

    if (local && (speed < 0L) && (network == 0)) {
	screen(SCR_EM,0,0L,"Sorry, you must 'set speed' first");
	return;
    }
    x = -1;
    if (ttopen(ttname,&x,mdmtyp,cdtimo) < 0) {
	debug(F111,"failed: proto ttopen local",ttname,local);
	screen(SCR_EM,0,0L,"Can't open line");
	success = 0;
	return;
    }
    if (x > -1) local = x;
    debug(F111,"proto ttopen local",ttname,local);

    lx = (local && !network) ? speed : -1;
#ifdef NETCONN
#ifdef CK_SPEED
/*
  If we are a TELNET client, force quoting of IAC.
  Note hardwired "1" rather than NP_TELNET symbol, so we don't have
  to schlurp in ckcnet.h.
*/
    if (network && ttnproto == 1) {
	ctlp[255] = ctlp[CR] = 1;
	if (parity == 'e' || parity == 'm') ctlp[127] = 1;
    }
#endif /* CK_SPEED */
#endif /* NETCONN */
    if (ttpkt(lx,flow,parity) < 0) {	/* Put line in packet mode, */
	screen(SCR_EM,0,0L,"Can't condition line");
	success = 0;
	return;
    }
    /* Send remote side "receive" startup string, if any */
    if (local && sstate == 's') {
	char *s;
	s = binary ? ptab[protocol].h_b_init : ptab[protocol].h_t_init;
	if (s) if (*s) {
	    char tmpbuf[356];
	    int stuff = -1, stuff2 = -1, len = 0;
	    extern int tnlm;

	    sprintf(tmpbuf, s, fspec);	/* We need this for XMODEM... */

	    strcat(tmpbuf, "\015");
	    if (tnlm)			/* TERMINAL NEWLINE ON */
	      stuff = LF;		/* Stuff LF */
#ifdef NETCONN
#ifdef TCPSOCKET
	    /* TELNET NEWLINE MODE */
	    if (network) {
		if (ttnproto == NP_TELNET) {
		    switch (me_binary ? tn_b_nlm : tn_nlm) {
		      case TNL_CR:
			break;
		      case TNL_CRNUL:
			break;
		      case TNL_CRLF:
			stuff2 = stuff;
			stuff = LF;
			break;
		    }
		}
#ifdef RLOGCODE
		else if (ttnproto == NP_RLOGIN) {
		    switch (tn_b_nlm) { /* Always BINARY */
		      case TNL_CR:
			break;
		      case TNL_CRNUL:
			stuff2 = stuff;
			stuff  = NUL;
			break;
		      case TNL_CRLF:
			stuff2 = stuff;
			stuff = LF;
			break;
		    }
		}
#endif /* RLOGCODE */
	    }
#endif /* TCPSOCKET */
#endif /* NETCONN */
	    len = strlen(tmpbuf);
	    if (stuff >= 0) {
		tmpbuf[len++] = stuff;
		if (stuff2 >= 0)
		  tmpbuf[len++] = stuff2;
		tmpbuf[len] = NUL;
	    }
	    ttol((CHAR *)tmpbuf,len);
	}
    }
#ifdef CK_XYZ
    if (protocol != PROTO_K) {		/* Non-Kermit protocol selected */
	char tmpbuf[356];
	char * s = "";
#ifdef XYZ_INTERNAL			/* Internal */
	success = !pxyz(sstate); 
#else
#ifdef CK_REDIR				/* External */
	switch (sstate) {
	  case 's':			/* 'Tis better to SEND... */
	    s = binary ? ptab[protocol].p_b_scmd : ptab[protocol].p_t_scmd;
	    break;
	  case 'v':			/* ... than RECEIVE */
	    s = binary ? ptab[protocol].p_b_rcmd : ptab[protocol].p_t_rcmd;
	    break;
	}
	if (!s) s = "";
	if (*s) {
	    sprintf(tmpbuf,s,(sstate == 's') ? fspec : cmarg2);
	    ttruncmd(tmpbuf);
	} else {
	    printf("?Sorry, no external protocol defined for %s\r\n",
		   ptab[protocol].p_name
		   );
	}
#else
	printf(
"Sorry, only Kermit protocol is supported in this version of Kermit\n"
	       );
#endif /* CK_REDIR */
#endif /* XYZ_INTERNAL */
	return;
    }
#endif /* CK_XYZ */

#ifdef NTSIGX
    conraw();
    connoi();
#else
    if (!local)
      connoi();				/* No console interrupts if remote */
#endif /* NTSIG */

    if (sstate == 'x') {		/* If entering server mode, */
	server = 1;			/* set flag, */
	debug(F101,"server backgrd","",backgrd);
	debug(F101,"server quiet","",quiet);
	if (!quiet && !backgrd) {
    	    debug(F100,"SHOULD NOT SEE THIS IF IN BACKGROUND!","",0);
	    if (!local)	{		/* and issue appropriate message. */
	    	conoll(srvtxt);
		conoll("KERMIT READY TO SERVE...");
	    } else {
	    	conol("Entering server mode on ");
		conoll(ttname);
		conoll("Type Ctrl-C to quit.");
		if (srvdis) intmsg(-1L);
	    }
	}
#ifdef TCPSOCKET
#ifndef NOLISTEN
#ifndef NOSPL
	if (network && tcpsrfd > 0) {
	    dooutput("KERMIT READY TO SERVE...\015\012");
	}
#endif /* NOSPL */
#endif /* NOLISTEN */
#endif /* TCPSOCKET */
    } else server = 0;
#ifdef VMS
    if (!quiet && !backgrd)    /* So message doesn't overwrite prompt */
      conoll("");
    if (local) conres();       /* So Ctrl-C will work */
#endif /* VMS */
/*
  If in remote mode, not shushed, not in background, and at top command level,
  issue a helpful message telling what to do...
*/
    if (!local && !quiet && !backgrd) {
	if (sstate == 'v') {
	    conoll("Return to your local Kermit and give a SEND command.");
	    conoll("");
	    conoll("KERMIT READY TO RECEIVE...");
	} else if (sstate == 's') {
	    conoll("Return to your local Kermit and give a RECEIVE command.");
	    conoll("");
	    conoll("KERMIT READY TO SEND...");
	} else if ( sstate == 'g' || sstate == 'r' || sstate == 'h' ||
		    sstate == 'j' || sstate == 'c' ) {
	    conoll("Return to your local Kermit and give a SERVER command.");
	    conoll("");
	    conoll((sstate == 'r' || sstate == 'j' || sstate == 'h') ?
		   "KERMIT READY TO GET..." :
		   "KERMIT READY TO SEND SERVER COMMAND...");
	}
    }
#ifdef COMMENT
    if (!local) sleep(1);
#endif /* COMMENT */
/*
  The 'wart()' function is generated by the wart program.  It gets a
  character from the input() routine and then based on that character and
  the current state, selects the appropriate action, according to the state
  table above, which is transformed by the wart program into a big case
  statement.  The function is active for one transaction.
*/
    rtimer();				/* Reset elapsed-time timer */
    resetc();				/* & other per-transaction counters. */

    debug(F101,"proto calling wart, justone","",justone);

    wart();				/* Enter the state table switcher. */
    
    if (server) {			/* Back from packet protocol. */
    	if (!quiet && !backgrd) {	/* Give appropriate message */
	    conoll("");
	    conoll("C-Kermit server done");
        }
    }
/*
  Note: the following is necessary in case we have just done a remote-mode
  file transfer, in which case the controlling terminal modes have been
  changed by ttpkt().  In particular, special characters like Ctrl-C and
  Ctrl-\ might have been turned off (see ttpkt).  So this call to ttres() is
  essential.
*/
#ifdef OS2
    ttres();				/* Reset the communication device */
#else
    if (!local) {
	msleep(500);
	ttres();			/* Reset the communication device */
    }
#endif /* OS2 */
    screen(SCR_TC,0,0L,"");		/* Transaction complete */
    server = 0;				/* Not a server any more */
}

static int
sgetinit(reget) int reget; {
    if (!ENABLED(en_get)) {		/* Only if not disabled!  */
	errpkt((CHAR *)"GET disabled");
	return(-1);
    } else {				/* OK to go ahead. */
#ifdef WHATAMI
	debug(F101,"sgetinit whatru","",whatru);
	if (whatru & WM_FLAG) {		/* Did we get WHATAMI info? */
	    debug(F101,"sgetinit binary (1)","",binary);
#ifdef VMS
	    if (binary != XYFT_I && binary != XYFT_L)
#else
#ifdef OS2
	    if (binary != XYFT_L)
#endif /* OS2 */
#endif /* VMS */
	    binary = (whatru & WM_FMODE) ?  /* Yes, set transfer mode */
	      XYFT_B : XYFT_T;		    /* automatically */
	    debug(F101,"sgetinit binary (2)","",binary);
	    fncnv = (whatru & WM_FNAME) ? 1 : 0; /* And name conversion */
	}
#endif /* WHATAMI */
	srvptr = srvcmd;		/* Point to server command buffer */
	decode(rdatap,putsrv,0);	/* Decode the GET command into it */
	/* Accept multiple filespecs */
        cmarg2 = "";			/* Don't use cmarg2 */
	cmarg = "";			/* Don't use cmarg */
#ifndef NOMSEND				/* New way. */
	nfils = fnparse((char *)srvcmd); /* Use cmlist instead */
#else
	nfils = 0 - zxpand((char *)srvcmd);
#endif /* NOMSEND */
	nakstate = 0;			/* Now I'm the sender! */
	if (reget) sendmode = SM_RESEND;
	if (sinit() > 0) {		/* Send Send-Init */
	    timint = chktimo(rtimo,timef); /* Switch to per-packet timer */
	    return(0);			/* If successful, switch state */
	} else return(-1);		/* Else back to server command wait */
    }
}
