
INCLUDES	= -pthread

bin_PROGRAMS	= minerd

minerd_SOURCES	= cpu-miner.c sha256_generic.c
minerd_LDFLAGS	= -pthread
minerd_LDADD	= @CRYPTO_LIBS@ @LIBCURL@ @JANSSON_LIBS@

