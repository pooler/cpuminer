
if WANT_JANSSON
JANSSON_INCLUDES= -I$(top_srcdir)/compat/jansson
else
JANSSON_INCLUDES=
endif

EXTRA_DIST	= example-cfg.json

SUBDIRS		= compat

INCLUDES	= $(PTHREAD_CFLAGS) -fno-strict-aliasing $(JANSSON_INCLUDES)

bin_PROGRAMS	= minerd

minerd_SOURCES	= elist.h miner.h compat.h			\
		  cpu-miner.c util.c scrypt.c scrypt-x86.S scrypt-x64.S
minerd_LDFLAGS  = $(PTHREAD_CFLAGS)
minerd_LDADD	= @LIBCURL@ @JANSSON_LIBS@ $(PTHREAD_LIBS) @WS2_LIBS@
minerd_CPPFLAGS = @LIBCURL_CPPFLAGS@

