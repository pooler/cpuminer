noinst_LIBRARIES	= libx8664.a

SUFFIXES = .asm

libx8664_a_SOURCES	= sha256_xmm_amd64.asm

.asm.o:
	$(YASM) -f elf64 $<
