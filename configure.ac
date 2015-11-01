dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(live-streamer, 0.8.0)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CXX
dnl AX_CXX_COMPILE_STDCXX_11([ext|noext],[mandatory|optional])
AX_CXX_COMPILE_STDCXX_11([ext], [mandatory])
AC_LANG([C++])

AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIR([m4])

AC_USE_SYSTEM_EXTENSIONS


LT_INIT


PKG_CHECK_MODULES(DBUSXX, [dbus-c++-1])


# --------------------- #
# Check for libraries.  #
# --------------------- #

dnl Utility to simplify finding libraries.
dnl $1 = library to find
dnl $2 = the shell variable to assign the result to
dnl      If library is found we store $version here.
dnl $3 = additional flags to add to CPPFLAGS
dnl $4 = additional flags to add to LDFLAGS
dnl $5 = program input

AC_DEFUN([AC_CHECK_LIBRARY],
[
  libname=$1
  define([have_library_var],$2)
  new_CPPFLAGS=$3
  new_LDFLAGS=$4
  define([prog_input],$5)
  AC_MSG_CHECKING([for $libname library])
  save_CPPFLAGS=$CPPFLAGS
  save_LDFLAGS=$LDFLAGS
  CPPFLAGS="$CPPFLAGS $new_CPPFLAGS"
  LDFLAGS="$LDFLAGS $new_LDFLAGS"
  AC_LINK_IFELSE(prog_input,
                 [have_library_var=yes],
                 [have_library_var=no])
  CPPFLAGS=$save_CPPFLAGS
  LDFLAGS=$save_LDFLAGS
  AC_MSG_RESULT([$have_library_var])
])


live555inc=""
live555lib="-lBasicUsageEnvironment -lUsageEnvironment -lgroupsock -lliveMedia"

AC_ARG_WITH(live555,
[AS_HELP_STRING([--with-live555=PATH],
		[specify prefix directory for live555 package.
		 Equivalent to --with-live555-include=PATH/include
		 plus --with-live555-lib=PATH/lib])])
AC_ARG_WITH(live555-include,
[AS_HELP_STRING([--with-live555-include=PATH],
		[specify directory for installed live555 include files])])
AC_ARG_WITH(live555-lib,
[AS_HELP_STRING([--with-live555-lib=PATH],
		[specify directory for the installed live555 library])])

if test "x$with_live555" != x; then
  live555inc="-I$with_live555/include/BasicUsageEnvironment $live555inc"
  live555inc="-I$with_live555/include/UsageEnvironment $live555inc"
  live555inc="-I$with_live555/include/groupsock $live555inc"
  live555inc="-I$with_live555/include/liveMedia $live555inc"
  live555lib="-L$with_live555/lib $live555lib"
fi
if test "x$with_live555_include" != x; then
  live555inc="-I$with_live555_include/BasicUsageEnvironment $live555inc"
  live555inc="-I$with_live555_include/UsageEnvironment $live555inc"
  live555inc="-I$with_live555_include/groupsock $live555inc"
  live555inc="-I$with_live555_include/liveMedia $live555inc"
fi
if test "x$with_live555_lib" != x; then
  live555lib="-L$with_live555_lib $live555lib"
fi

AC_CHECK_LIBRARY(live555, have_live555,
               ${live555inc}, ${live555lib},
               [AC_LANG_PROGRAM([#include <BasicUsageEnvironment.hh>],
                                [TaskScheduler* scheduler = BasicTaskScheduler::createNew();])])

if test "x$have_live555" = xyes; then
  LIVE555_CPPFLAGS=$live555inc
  LIVE555_LDFLAGS="$live555lib"
  AC_SUBST(LIVE555_CPPFLAGS)
  AC_SUBST(LIVE555_LDFLAGS)
else
  AC_MSG_ERROR([unable to find live555 library.])
fi


himppinc=
himpplib="-lmpi -lresampler -laec -lvqev2 -lanr -lVoiceEngine -lisp -l_hiae -l_hiawb -l_hiaf -lpthread"

AC_ARG_WITH(himpp,
[AS_HELP_STRING([--with-himpp=PATH],
		[specify prefix directory for himpp package.
		 Equivalent to --with-himpp-include=PATH/include
		 plus --with-himpp-lib=PATH/lib])])
AC_ARG_WITH(himpp-include,
[AS_HELP_STRING([--with-himpp-include],
		[specify directory for installed himpp include files])])
AC_ARG_WITH(himpp-lib,
[AS_HELP_STRING([--with-himpp-lib],
		[specify directory for the installed himpp library])])


if test "x$with_himpp" != x; then
  himppinc="-I$with_himpp/include $himppinc"
  himpplib="-L$with_himpp/lib $himpplib"
fi
if test "x$with_himpp_include" != x; then
  himppinc="-I$with_himpp_include $himppinc"
fi
if test "x$with_himpp_lib" != x; then
  himpplib="-L$with_himpp_lib $himpplib"
fi

AC_CHECK_LIBRARY(himpp, have_himpp,
               ${himppinc}, ${himpplib},
               [AC_LANG_PROGRAM([#include <hi_comm_sys.h>
                                 #include <mpi_sys.h>],
                                [HI_MPI_SYS_Init();
                                 HI_MPI_SYS_Exit();])])

if test "x$have_himpp" = xyes; then
  HIMPP_CPPFLAGS=$himppinc
  HIMPP_LDFLAGS=$himpplib
  AM_CONDITIONAL(ENABLE_HIMPP, true)
  AC_DEFINE(HAVE_HIMPP_SUPPORT, 1, [support hisi MPP])
  AC_SUBST(HIMPP_CPPFLAGS)
  AC_SUBST(HIMPP_LDFLAGS)
else
  AM_CONDITIONAL(ENABLE_HIMPP, false)
  dnl AC_MSG_ERROR([unable to find himpp library.])
fi

AC_CHECK_LIB([ev], ev_default_loop,
    [AC_CHECK_HEADERS(ev.h, have_libev=true, have_libev=false)],
    have_libev=false)

if ! $have_libev; then
  AC_MSG_ERROR([You need the libev libraries]
    [http://software.schmorp.de/pkg/libev.html])
fi
LIBEV_LDFLAGS="-lev"
AC_SUBST(LIBEV_LDFLAGS)
AM_CONDITIONAL(ENABLE_LIBEV, true)


AC_OUTPUT([
Makefile
src/Makefile
src/himpp/Makefile
])
