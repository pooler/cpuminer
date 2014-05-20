noinst_LIBRARIES= libsha256d.a


libsha256d_a_SOURCES	= sha2.c

if ARCH_x86
libsha256d_a_SOURCES += sha2-x86.S
endif
if ARCH_x86_64
libsha256d_a_SOURCES += sha2-x64.S
endif
if ARCH_ARM
libsha256d_a_SOURCES += sha2-arm.S
endif


