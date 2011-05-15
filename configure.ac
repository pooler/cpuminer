AC_INIT([cpuminer], [1.0.1])

AC_PREREQ(2.52)
AC_CANONICAL_SYSTEM
AC_CONFIG_SRCDIR([cpu-miner.c])
AM_INIT_AUTOMAKE([gnu])
AC_CONFIG_HEADERS([cpuminer-config.h])

dnl Make sure anyone changing configure.ac/Makefile.am has a clue
AM_MAINTAINER_MODE

dnl Checks for programs
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AM_PROG_CC_C_O
AC_PROG_RANLIB

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(syslog.h)

AC_FUNC_ALLOCA

case $target in
  *-*-mingw*)
    have_x86_64=false
    have_win32=true
    PTHREAD_FLAGS=""
    ;;
  x86_64-*)
    have_x86_64=true
    have_win32=false
    PTHREAD_FLAGS="-pthread"
    ;;
  *)
    have_x86_64=false
    have_win32=false
    PTHREAD_FLAGS="-pthread"
    ;;
esac


AC_CHECK_LIB(jansson, json_loads, request_jansson=false, request_jansson=true)
AC_CHECK_LIB(pthread, pthread_create, PTHREAD_LIBS=-lpthread)

AM_CONDITIONAL([WANT_JANSSON], [test x$request_jansson = xtrue])
AM_CONDITIONAL([HAVE_WINDOWS], [test x$have_win32 = xtrue])
AM_CONDITIONAL([HAVE_x86_64], [test x$have_x86_64 = xtrue])

if test x$request_jansson = xtrue
then
	JANSSON_LIBS="compat/jansson/libjansson.a"
else
	JANSSON_LIBS=-ljansson
fi

dnl Find YASM
has_yasm=false
AC_PATH_PROG(YASM,[yasm],[true yasm])
if test "$YASM" = "false" ; then
  AC_MSG_WARN([yasm is required for the x86_64 SSE2 code, but it was not found])
else
  AC_MSG_CHECKING([if yasm version is greater than 1.0.1])
  yasmver=`yasm --version | head -1 | cut -d\  -f2`
  yamajor=`echo $yasmver | cut -d. -f1`
  yaminor=`echo $yasmver | cut -d. -f2`
  yamini=`echo $yasmver | cut -d. -f3`
  if test "$yamajor" -ge "1" ; then
    if test "$yamajor" -eq "1" ; then
      if test "$yaminor" -ge "0" ; then
        if test "$yaminor" -eq "0"; then
          if test "$yamini" -ge "1"; then
            has_yasm=true
          fi
        else
          has_yasm=true
        fi
      fi
    fi
  else
    has_yasm=false
  fi
fi
if test "x$has_yasm" == "xtrue" ; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AM_CONDITIONAL([HAS_YASM], [test x$has_yasm = xtrue])

PKG_PROG_PKG_CONFIG()

LIBCURL_CHECK_CONFIG(, 7.10.1, ,
  [AC_MSG_ERROR([Missing required libcurl >= 7.10.1])])

AC_SUBST(JANSSON_LIBS)
AC_SUBST(PTHREAD_FLAGS)
AC_SUBST(PTHREAD_LIBS)

AC_CONFIG_FILES([
	Makefile
	compat/Makefile
	compat/jansson/Makefile
	x86_64/Makefile
	])
AC_OUTPUT

