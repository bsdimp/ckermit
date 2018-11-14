CKVER= "5A(190)"
#
# -- Makefile to build C-Kermit 5A for UNIX and UNIX-like systems --
# Hacked too be small for venix
#
# C-Kermit 5A can be built for 2.10 and 2.11BSD, using overlays, but a
# separate makefile is used because this one is too big.
#
##############################################################################
#
# V7-specific variables.
# These are set up for Perkin-Elmer 3230 V7 Unix:
#
PROC=proc
DIRECT=
NPROC=nproc
NPTYPE=int
BOOTFILE=/edition7
#
# ( For old Tandy TRS-80 Model 16A or 6000 V7-based Xenix, use PROC=_proc,
#   DIRECT=-DDIRECT, NPROC=_Nproc, NPTYPE=short, BOOTFILE=/xenix )
#
###########################################################################
#
#  Compile and Link variables:
#
#  EXT is the extension (file type) for object files, normally o.
#  See MINIX entry for what to do if another filetype must be used.
#
EXT=o
#LNKFLAGS=
SHAREDLIB=
CC= cc
CC2= cc
MAKE= make
SHELL=/bin/sh
#
###########################################################################
# SAMPLE ONLY.
# Easy installation. Modify this to suit your own computer's file organization
# and permissions.  If you don't have write access to the destination
# directories, "make install" will fail.

WERMIT = wermit
DESTDIR =
BINDIR = /usr/local/bin
MANDIR = /usr/man/manl
MANEXT = l
ALL = $(WERMIT)

all: $(ALL)

###########################################################################
#
# Dependencies Section:

manpage: ckuker.nr

wermit:	ckcmai.$(EXT) ckucmd.$(EXT) ckuusr.$(EXT) ckuus2.$(EXT) ckuus3.$(EXT) \
		ckuus4.$(EXT) ckuus5.$(EXT) ckuus6.$(EXT) ckuus7.$(EXT) \
		ckuusx.$(EXT) ckuusy.$(EXT) ckcpro.$(EXT) ckcfns.$(EXT) \
		ckcfn2.$(EXT) ckcfn3.$(EXT) ckuxla.$(EXT) ckucon.$(EXT) \
		ckutio.$(EXT) ckufio.$(EXT) ckudia.$(EXT) ckuscr.$(EXT) \
		ckcnet.$(EXT)
	$(CC2) $(LNKFLAGS) -o wermit ckcmai.$(EXT) ckutio.$(EXT) \
		ckufio.$(EXT) ckcfns.$(EXT) ckcfn2.$(EXT) ckcfn3.$(EXT) \
		ckuxla.$(EXT) ckcpro.$(EXT) ckucmd.$(EXT) ckuus2.$(EXT) \
		ckuus3.$(EXT) ckuus4.$(EXT) ckuus5.$(EXT) ckuus6.$(EXT) \
		ckuus7.$(EXT) ckuusx.$(EXT) ckuusy.$(EXT) ckuusr.$(EXT) \
		ckucon.$(EXT) ckudia.$(EXT) ckuscr.$(EXT) ckcnet.$(EXT) $(LIBS)

ckuker.nr: ckuker.cpp
	$(CC) $(CFLAGS) -E ckuker.cpp |grep -v "^$$" |grep -v "^\#" > ckuker.nr

###########################################################################
# Dependencies for each module...
#
ckcmai.$(EXT): ckcmai.c ckcker.h ckcdeb.h ckcsym.h ckcasc.h ckcnet.h

ckcpro.$(EXT): ckcpro.c ckcker.h ckcdeb.h ckcsym.h ckcasc.h

ckcpro.c: ckcpro.w wart ckcdeb.h ckcsym.h ckcasc.h ckcker.h
	./wart ckcpro.w ckcpro.c

ckcfns.$(EXT): ckcfns.c ckcker.h ckcdeb.h ckcsym.h ckcasc.h ckcxla.h \
		ckuxla.h

ckcfn2.$(EXT): ckcfn2.c ckcker.h ckcdeb.h ckcsym.h ckcasc.h ckcxla.h ckuxla.h

ckcfn3.$(EXT): ckcfn3.c ckcker.h ckcdeb.h ckcsym.h ckcasc.h ckcxla.h \
		ckuxla.h

ckuxla.$(EXT): ckuxla.c ckcker.h ckcsym.h ckcdeb.h ckcxla.h ckuxla.h

ckuusr.$(EXT): ckuusr.c ckucmd.h ckcker.h ckuusr.h ckcsym.h ckcdeb.h ckcxla.h \
		ckuxla.h ckcasc.h ckcnet.h

ckuus2.$(EXT): ckuus2.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h ckcxla.h ckuxla.h \
		ckcasc.h ckcnet.h ckcsym.h

ckuus3.$(EXT): ckuus3.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h ckcxla.h ckuxla.h \
		ckcasc.h ckcnet.h ckcsym.h

ckuus4.$(EXT): ckuus4.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h ckcxla.h ckuxla.h \
		ckcasc.h ckcnet.h ckuver.h ckcsym.h

ckuus5.$(EXT): ckuus5.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h ckcasc.h ckcnet.h \
		 ckcsym.h

ckuus6.$(EXT): ckuus6.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h ckcasc.h ckcnet.h \
		 ckcsym.h

ckuus7.$(EXT): ckuus7.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h ckcxla.h ckuxla.h \
		ckcasc.h ckcnet.h ckcsym.h

ckuusx.$(EXT): ckuusx.c  ckcker.h ckuusr.h ckcdeb.h ckcasc.h ckcsym.h

ckuusy.$(EXT): ckuusy.c  ckcker.h ckcdeb.h ckcasc.h ckcnet.h ckcsym.h

ckucmd.$(EXT): ckucmd.c ckcasc.h ckucmd.h ckcdeb.h ckcsym.h

ckufio.$(EXT): ckufio.c ckcdeb.h ckuver.h ckcsym.h

ckutio.$(EXT): ckutio.c ckcdeb.h ckcnet.h ckuver.h ckcsym.h

ckucon.$(EXT): ckucon.c ckcker.h ckcdeb.h ckcasc.h ckcnet.h ckcsym.h

ckcnet.$(EXT): ckcnet.c ckcdeb.h ckcker.h ckcnet.h ckcsym.h

wart: ckwart.$(EXT)
	$(CC) $(LNKFLAGS) -o wart ckwart.$(EXT) $(LIBS)

ckcmdb.$(EXT): ckcmdb.c ckcdeb.h ckcsym.h

ckwart.$(EXT): ckwart.c

ckudia.$(EXT): ckudia.c ckcker.h ckcdeb.h ckucmd.h ckcasc.h ckcsym.h

ckuscr.$(EXT): ckuscr.c ckcker.h ckcdeb.h ckcasc.h ckcsym.h

#DEC Rainbow or IBM XT/AT with Venix/86 V2.0
# Requires separate I/D space and a low, 8k stack. Otherwise
# simplar to Pro version.
venix86:
	make wart "CFLAGS= -DVENIX86 -DVENIX" "LNKFLAGS= "
	make wermit "CFLAGS = -DVENIX86 -DVENIX -DNOFILEH -z -i -O" \
		"LNKFLAGS=-z -i "
#Clean up intermediate and object files
clean:
	@echo 'Removing object files...'
	-rm -f ckcmai.$(EXT) ckucmd.$(EXT) ckuusr.$(EXT) ckuus2.$(EXT) \
ckuus3.$(EXT) ckuus4.$(EXT) ckuus5.$(EXT) ckcpro.$(EXT) ckcfns.$(EXT) \
ckcfn2.$(EXT) ckcfn3.$(EXT) ckuxla.$(EXT) ckucon.$(EXT) ckutio.$(EXT) \
ckufio.$(EXT) ckudia.$(EXT) ckuscr.$(EXT) ckwart.$(EXT) ckuusx.$(EXT) \
ckuusy.$(EXT) ckcnet.$(EXT) ckuus6.$(EXT) ckuus7.$(EXT) ckcmdb.$(EXT) \
ckcpro.c wart

ckuuid:
	@echo 'building C-Kermit $(CKVER) set-UID/set-GID test programs'
	$(CC) -DANYBSD -DSAVEDUID -o ckuuid1 ckuuid.c
	$(CC) -DANYBSD -o ckuuid2 ckuuid.c
	$(CC) -DANYBSD -DNOSETREU -o ckuuid3 ckuuid.c
	$(CC) -DANYBSD -DSETEUID -DNOSETREU -o ckuuid4 ckuuid.c
	$(CC) -o ckuuid5 ckuuid.c
	@echo 'Read the top of ckuuid.c for directions...for testing'
	@echo 'you must make these programs setuid and setgid'

#Remember TECO?
love:
	@echo 'Not war?'
