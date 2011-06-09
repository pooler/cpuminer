
if WANT_JANSSON
JANSSON_INCLUDES= -I$(top_srcdir)/compat/jansson
else
JANSSON_INCLUDES=
endif

EXTRA_DIST	= example-cfg.json

SUBDIRS		= compat

INCLUDES	= $(PTHREAD_FLAGS) -fno-strict-aliasing $(JANSSON_INCLUDES)

bin_PROGRAMS	= minerd

minerd_SOURCES	= elist.h miner.h compat.h			\
		  cpu-miner.c util.c				\
		  sha256_generic.c sha256_4way.c sha256_via.c	\
		  sha256_cryptopp.c sha256_sse2_amd64.c
minerd_LDFLAGS	= $(PTHREAD_FLAGS)
minerd_LDADD	= @LIBCURL@ @JANSSON_LIBS@ @PTHREAD_LIBS@
minerd_CPPFLAGS = @LIBCURL_CPPFLAGS@

if HAVE_x86_64
if HAS_YASM
SUBDIRS		+= x86_64
minerd_LDADD	+= x86_64/libx8664.a
AM_CFLAGS	= -DHAS_YASM
endif
endif
