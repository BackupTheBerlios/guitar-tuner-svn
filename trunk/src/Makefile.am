bin_PROGRAMS = guitartuner

guitartuner_SOURCES = main.c gui.c tuner.c
guitartuner_LDADD = $(DEPS_LIBS)
noinst_HEADERS = tuner.h gui.h

AM_CPPFLAGS = $(DEPS_CFLAGS) \
	      -DPIXMAPS_DIR=\""$(datadir)/pixmaps/guitar-tuner"\" 
                                                                                                    
