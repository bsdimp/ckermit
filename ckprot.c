
/* WARNING -- This C source program generated by Wart preprocessor. */
/* Do not edit this file; edit the Wart-format source file instead, */
/* and then run it through Wart to produce a new C source file.     */

char *protv = "C-Kermit Protocol Module 4.2(015), 5 Mar 85"; /* -*-C-*- */

/* C K P R O T  -- C-Kermit Protocol Module, in Wart preprocessor notation. */

/* Authors: Jeff Damens, Bill Catchings, Frank da Cruz (Columbia University) */

#include "ckermi.h"
/*
 Note -- This file may also be preprocessed by the Unix Lex program, but 
 you must indent the above #include statement before using Lex, and then
 restore it to the left margin in the resulting C program before compilation.
 Also, the invocation of the "wart()" function below must be replaced by an
 invocation  of the "yylex()" function.  It might also be necessary to remove
 comments in the %%...%% section.
*/


/* State definitions for Wart (or Lex) */

#define ipkt 1
#define rfile 2
#define rdata 3
#define ssinit 4
#define ssdata 5
#define sseof 6
#define sseot 7
#define serve 8
#define generic 9
#define get 10
#define rgen 11


/* External C-Kermit variable declarations */

  extern char sstate, *versio, *srvtxt, *cmarg, *cmarg2;
  extern char data[], filnam[], srvcmd[], ttname[], *srvptr;
  extern int pktnum, timint, nfils, image, hcflg, xflg, speed, flow, mdmtyp;
  extern int prvpkt, cxseen, czseen, server, local, displa, bctu, bctr, quiet;
  extern int putsrv(), puttrm(), putfil(), errpkt();
  extern char *DIRCMD, *DELCMD, *TYPCMD, *SPACMD, *SPACM2, *WHOCMD;


/* Local variables */

  static char vstate = 0;  		/* Saved State   */
  static char vcmd = 0;    		/* Saved Command */
  static int x;				/* General-purpose integer */


/* Macros - Note, BEGIN is predefined by Wart (and Lex) */

#define SERVE  tinit(); BEGIN serve
#define RESUME if (server) { SERVE; } else return


#define BEGIN state =

int state = 0;

wart()
{
  int c,actno;
  extern int tbl[];
  while (1) {
	c = input();
	if ((actno = tbl[c + state*128]) != -1)
	  switch(actno) {
case 1:
{ tinit();	    	    	    	/* Do Send command */
    if (sinit()) BEGIN ssinit;
       else RESUME; }
break;
case 2:
{ tinit(); BEGIN get; }
break;
case 3:
{ tinit(); vstate = get;  vcmd = 0;   sipkt(); BEGIN ipkt; }
break;
case 4:
{ tinit(); vstate = rgen; vcmd = 'C'; sipkt(); BEGIN ipkt; }
break;
case 5:
{ tinit(); vstate = rgen; vcmd = 'G'; sipkt(); BEGIN ipkt; }
break;
case 6:
{ SERVE; }
break;
case 7:
{ rinit(data); bctu = bctr; BEGIN rfile; }
break;
case 8:
{ spar(data);			/* Get ack for I-packet */
    	   if (vcmd) { scmd(vcmd,cmarg); vcmd = 0; }
    	   if (vstate == get) srinit();
	   BEGIN vstate; }
break;
case 9:
{ if (vcmd) scmd(vcmd,cmarg);	/* Get E for I-packet (ignore) */
    	   vcmd = 0; if (vstate == get) srinit();
	   BEGIN vstate; }
break;
case 10:
{ srvptr = srvcmd; decode(data,putsrv); /* Get Receive-Init */
	   cmarg = srvcmd;  nfils = -1;
    	   if (sinit()) BEGIN ssinit; else { SERVE; } }
break;
case 11:
{ spar(data); rpar(data); ack1(data);	 /* Get Init Parameters */
	   pktnum = 0; prvpkt = -1; }
break;
case 12:
{ srvptr = srvcmd; decode(data,putsrv); /* Get & decode command. */
	   putsrv('\0'); putsrv('\0');
	   sstate = srvcmd[0]; BEGIN generic; }
break;
case 13:
{ srvptr = srvcmd;		    	 /* Get command for shell */
	   decode(data,putsrv); putsrv('\0');
	   if (syscmd("",srvcmd)) BEGIN ssinit;
	   else { errpkt("Can't do shell command"); SERVE; } }
break;
case 14:
{ errpkt("Unimplemented server function"); SERVE; }
break;
case 15:
{ if (!cwd(srvcmd+1)) errpkt("Can't change directory"); /* CWD */
    	     SERVE; }
break;
case 16:
{ if (syscmd(DIRCMD,srvcmd+2)) BEGIN ssinit;	/* Directory */
    	     else { errpkt("Can't list directory"); SERVE; } }
break;
case 17:
{ if (syscmd(DELCMD,srvcmd+2)) BEGIN ssinit;	/* Erase */
    	     else { errpkt("Can't remove file"); SERVE; } }
break;
case 18:
{ ack(); return(0); }
break;
case 19:
{ ack(); ttres(); return(kill(0,9)); }
break;
case 20:
{ if (sndhlp()) BEGIN ssinit;
    	     else { errpkt("Can't send help"); SERVE; } }
break;
case 21:
{ if (syscmd(TYPCMD,srvcmd+2)) BEGIN ssinit;
    	     else { errpkt("Can't type file"); SERVE; } }
break;
case 22:
{ x = *(srvcmd+1);			/* Disk Usage query */
    	     x = ((x == '\0') || (x == unchar(0)));
	     x = (x ? syscmd(SPACMD,"") : syscmd(SPACM2,srvcmd+2));
    	     if (x) BEGIN ssinit; else { errpkt("Can't check space"); SERVE; }}
break;
case 23:
{ if (syscmd(WHOCMD,srvcmd+2)) BEGIN ssinit;
    	     else { errpkt("Can't do who command"); SERVE; } }
break;
case 24:
{ errpkt("Unimplemented generic server function"); SERVE; }
break;
case 25:
{ decode(data,puttrm); RESUME; }
break;
case 26:
{ if (rcvfil()) { ack(); BEGIN rdata; }	/* A file is coming */
		else { errpkt("Can't open file"); RESUME; } }
break;
case 27:
{ opent(); ack(); BEGIN rdata; }
break;
case 28:
{ ack(); reot(); RESUME; }
break;
case 29:
{ if (cxseen) ack1("X");	/* Got data. */
    	   else if (czseen) ack1("Z");
	   else ack();
	   decode(data,putfil); }
break;
case 30:
{ ack(); reof(); BEGIN rfile; }
break;
case 31:
{ resend(); }
break;
case 32:
{  int x; char *s;		/* Got ACK to Send-Init */
    	     spar(data);
    	     bctu = bctr;
	     if (xflg) { x = sxpack(); s = "Can't execute command"; }
	    	  else { x = sfile(); s = "Can't open file"; }
	     if (x) BEGIN ssdata; else { errpkt(s); RESUME; }
          }
break;
case 33:
{ if (canned(data) || !sdata()) { /* Got ACK to data */
		clsif(); seof();
		BEGIN sseof; } }
break;
case 34:
{ if (gnfile() > 0) {		/* Got ACK to EOF, get next file */
		if (sfile()) BEGIN ssdata;
		else { errpkt("Can't open file") ; RESUME; }
	   } else {			/* If no next file, EOT */
		seot();
		BEGIN sseot; } }
break;
case 35:
{ RESUME; }
break;
case 36:
{ ermsg(data);			/* Error packet, issue message */
    x = quiet; quiet = 1;		/* Close files silently */
    clsif(); clsof();
    quiet = x; RESUME; }
break;
case 37:
{ nack(); }
break;

    }
  }
}

int tbl[] = {
-1, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 4, 37, 
37, 37, 5, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 3, 1, 37, 37, 2, 37, 6, 
37, 37, 37, 37, 37, 37, 37, -1, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 9, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 8, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 4, 37, 37, 37, 5, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 3, 1, 37, 37, 2, 37, 6, 37, 37, 37, 37, 37, 37, 37, -1, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 28, 37, 37, 36, 26, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 27, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 4, 37, 37, 37, 5, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 3, 1, 37, 37, 2, 37, 6, 37, 37, 37, 37, 
37, 37, 37, -1, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 29, 36, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 30, 37, 37, 37, 37, 37, 37, 
37, 37, 4, 37, 37, 37, 5, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 3, 1, 37, 
37, 2, 37, 6, 37, 37, 37, 37, 37, 37, 37, -1, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
36, 37, 37, 37, 37, 37, 37, 37, 37, 31, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
32, 37, 37, 37, 37, 37, 37, 37, 37, 37, 4, 37, 37, 37, 5, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 3, 1, 37, 37, 2, 37, 6, 37, 37, 37, 37, 37, 37, 37, -1, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 36, 37, 37, 37, 37, 37, 37, 37, 37, 31, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 33, 37, 37, 37, 37, 37, 37, 37, 37, 37, 4, 37, 
37, 37, 5, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 3, 1, 37, 37, 2, 37, 6, 
37, 37, 37, 37, 37, 37, 37, -1, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 36, 37, 37, 37, 
37, 37, 37, 37, 37, 31, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 34, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 4, 37, 37, 37, 5, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 3, 1, 37, 37, 2, 37, 6, 37, 37, 37, 37, 37, 37, 37, -1, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 36, 37, 37, 37, 37, 37, 37, 37, 37, 31, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 35, 37, 37, 37, 37, 37, 37, 37, 37, 37, 4, 37, 37, 37, 5, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 3, 1, 37, 37, 2, 37, 6, 37, 37, 37, 37, 
37, 37, 37, -1, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 
14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 14, 14, 14, 12, 14, 11, 14, 14, 14, 
14, 14, 14, 14, 14, 10, 7, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 
14, 14, 4, 14, 14, 14, 5, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 3, 1, 14, 
14, 2, 14, 6, 14, 14, 14, 14, 14, 14, 14, -1, 24, 24, 24, 24, 24, 24, 24, 24, 
24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 
24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 
24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 15, 16, 
17, 18, 24, 20, 24, 24, 24, 19, 24, 24, 24, 24, 24, 24, 24, 21, 22, 24, 23, 24, 
24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 4, 24, 24, 24, 5, 24, 24, 24, 24, 24, 
24, 24, 24, 24, 24, 3, 1, 24, 24, 2, 24, 6, 24, 24, 24, 24, 24, 24, 24, -1, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 7, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 4, 37, 
37, 37, 5, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 3, 1, 37, 37, 2, 37, 6, 
37, 37, 37, 37, 37, 37, 37, 0, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 36, 26, 37, 37, 
37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 7, 37, 37, 37, 37, 27, 25, 37, 37, 37, 
37, 37, 37, 37, 37, 37, 4, 37, 37, 37, 5, 37, 37, 37, 37, 37, 37, 37, 37, 37, 
37, 3, 1, 37, 37, 2, 37, 6, 37, 37, 37, 37, 37, 37, 37, };


/*  P R O T O  --  Protocol entry function  */

proto() {

    extern int sigint();
    int x;

    conint(sigint);			/* Enable console interrupts */

/* Set up the communication line for file transfer. */

    if (local && (speed < 0)) {
	screen(2,0l,"Sorry, you must 'set speed' first");
	return;
    }
    if (ttopen(ttname,local,mdmtyp) < 0) {
	screen(2,0l,"Can't open line");
	return;
    }
    x = (local) ? speed : -1;
    if (ttpkt(x,flow) < 0) {		/* Put line in packet mode, */
	screen(2,0l,"Can't condition line"); /* setting speed, flow control */
	return;
    }
    if (sstate == 'x') {		/* If entering server mode, */
	server = 1;			/* set flag, */
	if (!quiet) {
	    if (!local)			/* and issue appropriate message. */
	    	conol(srvtxt);
	    else {
	    	conol("Entering server mode on ");
		conoll(ttname);
	    }
	}
    } else server = 0;
    sleep(1);

/*
 The 'wart()' function is generated by the wart program.  It gets a
 character from the input() routine and then based on that character and
 the current state, selects the appropriate action, according to the state
 table above, which is transformed by the wart program into a big case
 statement.  The function is active for one transaction.
*/

    wart();				/* Enter the state table switcher. */
    
    if (server) {			/* Back from packet protocol. */
	server = 0;
    	if (!quiet)  			/* Give appropriate message */
	    conoll("C-Kermit server done");
    } else
    	screen(BEL,0l,"");		/* Or beep */
}
