AUTOMAKE_OPTIONS = subdir-objects

noinst_LIBRARIES= libx11.a


libx11_a_SOURCES	= xcoin.c \
			  x5/aes_helper.c x5/shavite.c x5/vect128/nist.c x5/vect128/vector.c \
			  x5/echo.c x6/blake.c 

if HAVE_AES_NI
libx11_a_SOURCES += x5/echo512/ccalik/aesni/hash.c x6/groestl/aesni/hash-groestl.c
else
libx11_a_SOURCES += x6/grso.c x6/grso-asm.c
endif
if ARCH_x86
libx11_a_SOURCES += x5/luffa_for_sse2.c x5/cubehash_sse2.c
endif
if ARCH_x86_64
libx11_a_SOURCES += x5/luffa_for_sse2.c x5/cubehash_sse2.c
endif


