
INCLUDES	= -pthread

bin_PROGRAMS	= minerd

EXTRA_DIST	= sha256_generic.c

minerd_SOURCES	= cpu-miner.c
minerd_LDFLAGS	= -pthread
minerd_LDADD	= @CRYPTO_LIBS@ @LIBCURL@ @JANSSON_LIBS@

