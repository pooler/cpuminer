AUTOMAKE_OPTIONS = subdir-objects

if WANT_JANSSON
JANSSON_INCLUDES= -I$(top_srcdir)/compat/jansson
else
JANSSON_INCLUDES=
endif

EXTRA_DIST	= example-cfg.json nomacro.pl

SUBDIRS		= compat

INCLUDES	= $(PTHREAD_FLAGS) -fno-strict-aliasing $(JANSSON_INCLUDES)

bin_PROGRAMS	= minerd

dist_man_MANS	= minerd.1

minerd_SOURCES	= elist.h miner.h compat.h \
		  cpu-miner.c util.c \
		  sha2.c scrypt.c \
		  x5/aes_helper.c  x5/shavite.c  x5/vect128/nist.c x5/vect128/vector.c \
		  x5/echo.c x6/blake.c \
		  xcoin.c

if ARCH_x86
minerd_SOURCES += sha2-x86.S scrypt-x86.S x5/luffa_for_sse2.c x5/cubehash_sse2.c
endif
if ARCH_x86_64
minerd_SOURCES += sha2-x64.S scrypt-x64.S x5/luffa_for_sse2.c x5/cubehash_sse2.c
endif
if ARCH_ARM
minerd_SOURCES += sha2-arm.S scrypt-arm.S
endif

if HAVE_AES_NI
minerd_SOURCES += x5/echo512/ccalik/aesni/hash.c x6/groestl/aesni/hash-groestl.c
endif

minerd_LDFLAGS	= $(PTHREAD_FLAGS)
minerd_LDADD	= @LIBCURL@ @JANSSON_LIBS@ @PTHREAD_LIBS@ @WS2_LIBS@
minerd_CPPFLAGS = @LIBCURL_CPPFLAGS@

