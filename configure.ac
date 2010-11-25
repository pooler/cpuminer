AC_INIT([cpuminer], [0.1.1])

AC_PREREQ(2.52)
AC_CONFIG_SRCDIR([cpu-miner.c])
AM_INIT_AUTOMAKE([gnu])
AC_CONFIG_HEADERS([cpuminer-config.h])

dnl Make sure anyone changing configure.ac/Makefile.am has a clue
AM_MAINTAINER_MODE

dnl Checks for programs
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AM_PROG_CC_C_O

dnl Checks for header files.
AC_HEADER_STDC

AC_CHECK_LIB(jansson, json_loads, JANSSON_LIBS=-ljansson)

PKG_PROG_PKG_CONFIG()

LIBCURL_CHECK_CONFIG(, 7.10.1, ,
  [AC_MSG_ERROR([Missing required libcurl >= 7.10.1])])

AC_SUBST(JANSSON_LIBS)

AC_CONFIG_FILES([
	Makefile
	])
AC_OUTPUT

