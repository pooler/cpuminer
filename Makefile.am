
if WANT_JANSSON
JANSSON_INCLUDES= -I$(top_srcdir)/compat/jansson
else
JANSSON_INCLUDES=
endif

SUBDIRS		= compat

INCLUDES	= -pthread -fno-strict-aliasing $(JANSSON_INCLUDES)

bin_PROGRAMS	= minerd

EXTRA_DIST	= sha256_generic.c

minerd_SOURCES	= util.c cpu-miner.c miner.h compat.h
minerd_LDFLAGS	= -pthread
minerd_LDADD	= @LIBCURL@ @JANSSON_LIBS@ @PTHREAD_LIBS@

