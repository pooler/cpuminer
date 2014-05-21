noinst_LIBRARIES= libscrypt.a


libscrypt_a_SOURCES	= scrypt.c

if USE_ASM
if ARCH_x86
libscrypt_a_SOURCES += scrypt-x86.S
endif
if ARCH_x86_64
libscrypt_a_SOURCES += scrypt-x64.S
endif
if ARCH_ARM
libscrypt_a_SOURCES += scrypt-arm.S
endif
endif

