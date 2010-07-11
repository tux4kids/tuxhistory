## Makefile.am for tuxhisotry src:
## Process with Automake to create Makefile.in

# Support for gettext:
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

AM_CFLAGS=-Wall -g -DDATA_PREFIX=\"${DATA_PREFIX}\" -DDEBUG \
	-D$(SOUND)SOUND

AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" \
	-I../intl -I$(top_srcdir)/intl

LDADD = @LIBINTL@ \
  ../linebreak/liblinebreak.a

if BUILD_MINGW32
  bin_PROGRAMS = TuxHistory 
  DATA_PREFIX=@MINGW32_PACKAGE_DATA_DIR@
else
  TUXMATHRC =
  bin_PROGRAMS = tuxhistory 
  DATA_PREFIX=${pkgdatadir}
endif


tuxhistory_SOURCES = tuxhistory.c \
	setup.c 	\
	titlescreen.c	\
	map.c		\
	menu.c		\
	game.c 		\
	fileops_media.c \
	options.c	\
	credits.c	\
	hashtable.c \
	highscore.c	\
	llist.c		\
	linewrap.c	\
	loaders.c	\
	audio.c 	\
	fileops.c	\
	convert_utf.c	\
	SDL_extras.c	\
	SDL_rotozoom.c	\
	scandir.c	\
	pixels.c	\
	throttle.c	


# HACK "TuxHistory" is the Windows program, whereas "tuxhistory" is the Unix program
TuxHistory_SOURCES  = $(tuxhistory_SOURCES) tuxmathrc.rc


EXTRA_DIST = 	credits.h 	\
	fileops.h 	\
	game.h		\
	menu.h		\
	globals.h	\
	hashtable.h \
	highscore.h 	\
	linewrap.h	\
	llist.h		\
	loaders.h	\
	titlescreen.h   \
	map.h		\
	options.h	\
	setup.h		\
	tuxhistory.h	\
	convert_utf.h	\
	SDL_extras.h	\
	SDL_rotozoom.h	\
	gettext.h	\
	scandir.h	\
	pixels.h	\
	compiler.h	\
	transtruct.h	\
	throttle.h
        


WINDRES=@WINDRES@

# How to make an RC file
tuxmathrc.o: tuxmathrc.rc
	$(WINDRES) -i $< -o $@
