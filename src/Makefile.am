## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/awt/glade
glade_DATA = awt.glade

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(AWT_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = awt

awt_SOURCES = \
	callbacks.c \
	callbacks.h \
	main.c \
	Global.h \
	frame_grabber.c

awt_LDFLAGS = \
	-Wl,--export-dynamic

awt_LDADD = $(AWT_LIBS)

EXTRA_DIST = $(glade_DATA)
