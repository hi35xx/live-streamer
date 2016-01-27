dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(live-streamer, 0.9.0)

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
dnl Usage:
dnl AC_CHECK_LIBRARY(
dnl     additional-cppflags,
dnl     additional-ldflags,
dnl     input,
dnl     [action-if-found],
dnl     [action-if-false]
dnl )
AC_DEFUN([AC_CHECK_LIBRARY],
[
  new_CPPFLAGS=$1
  new_LDFLAGS=$2
  define([prog_input],$3)
  define([action_if_true],$4)
  define([action_if_false],$5)
  save_CPPFLAGS=$CPPFLAGS
  save_LDFLAGS=$LDFLAGS
  CPPFLAGS="$CPPFLAGS $new_CPPFLAGS"
  LDFLAGS="$LDFLAGS $new_LDFLAGS"
  AC_LINK_IFELSE(prog_input, [action_if_true], [action_if_false])
  CPPFLAGS=$save_CPPFLAGS
  LDFLAGS=$save_LDFLAGS
])

ac_sysroot=${lt_sysroot:-`$CC --print-sysroot 2>/dev/null`}


dnl check for live555 library
AC_ARG_WITH(live555,
	[AS_HELP_STRING([--with-live555=PATH],
		[specify prefix directory for live555 package.])])

AC_MSG_CHECKING([for live555 library])
live555pfx=
case ${with_live555} in
 yes|'')
   live555pfx="$ac_sysroot/usr"
   ;;
 /*)
   live555pfx=`echo "$with_live555" | sed -e "$sed_quote_subst"`
   ;;
 no)
   AC_MSG_RESULT([$with_live555])
   AC_MSG_ERROR([live555 must be specified.])
   ;;
 *)
   AC_MSG_RESULT([$with_live555])
   AC_MSG_ERROR([live555 must be an absolute path."])
   ;;
esac

AC_MSG_RESULT([$live555pfx])

live555inc="-I$live555pfx/include/BasicUsageEnvironment \
 -I$live555pfx/include/UsageEnvironment \
 -I$live555pfx/include/groupsock \
 -I$live555pfx/include/liveMedia"
live555lib="-L$live555pfx/lib \
 -lBasicUsageEnvironment -lUsageEnvironment \
 -lgroupsock -lliveMedia"

AC_CHECK_LIBRARY(
  [${live555inc}], [${live555lib}],
  [AC_LANG_PROGRAM(
    [[#include <BasicUsageEnvironment.hh>]],
    [[TaskScheduler* scheduler = BasicTaskScheduler::createNew();]])
  ],
  [
    LIVE555_CPPFLAGS=${live555inc}
    LIVE555_LDFLAGS=${live555lib}
    AC_SUBST(LIVE555_CPPFLAGS)
    AC_SUBST(LIVE555_LDFLAGS)
  ],
  [AC_MSG_ERROR([unable to find live555 library.])]
)


dnl check for hi3518 mpp library
AC_ARG_WITH(hi3518mpp,
	[AS_HELP_STRING([--with-hi3518mpp=PATH],
		[specify prefix directory for himpp-hi3518 package.])],
		[], [with_hi3518mpp=no])

AC_MSG_CHECKING([for hi3518 mpp library])
case $with_hi3518mpp in
 yes)
   hi3518mpppfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3518mpppfx=`echo "$with_hi3518mpp" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3518mpp])
   AC_MSG_ERROR([hi3518mpp must be an absolute path."])
   ;;
esac
if test "x$with_hi3518mpp" != xno; then
  hi3518mppinc="-I$hi3518mpppfx/include/hi3518mpp"
  hi3518mpplib="-L$hi3518mpppfx/lib \ 
    -lmpi -lresampler -laec -lvqev2 -lanr -lVoiceEngine \
    -lisp -l_hiae -l_hiawb -l_hiaf -lpthread"
  AC_CHECK_LIBRARY([${hi3518mppinc}], $[{hi3518mpplib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3518mpp=yes], [have_hi3518mpp=no]
  )
  if test x$have_hi3518mpp = xyes; then
    HI3518MPP_CPPFLAGS=$hi3518mppinc
    HI3518MPP_LDFLAGS=$hi3518mpplib
    AC_DEFINE(HAVE_HI3518MPP_SUPPORT, 1, [support Hi3518 MPP])
    AC_SUBST(HI3518MPP_CPPFLAGS)
    AC_SUBST(HI3518MPP_LDFLAGS)
  fi
else
  have_hi3518mpp=no
fi
AC_MSG_RESULT([$have_hi3518mpp])
AM_CONDITIONAL([ENABLE_HI3518MPP], [test x$have_hi3518mpp = xyes])


dnl check for hi3520 mpp library
AC_ARG_WITH(hi3520mpp,
	[AS_HELP_STRING([--with-hi3520mpp=PATH],
		[specify prefix directory for himpp-hi3520 package.])],
		[], [with_hi3520mpp=no])

AC_MSG_CHECKING([for hi3520 mpp library])
case $with_hi3520mpp in
 yes)
   hi3520mpppfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3520mpppfx=`echo "$with_hi3520mpp" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3520mpp])
   AC_MSG_ERROR([hi3520mpp must be an absolute path."])
   ;;
esac
if test "x$with_hi3520mpp" != xno; then
  hi3520mppinc="-I$hi3520mpppfx/include/hi3520mpp"
  hi3520mpplib="-L$hi3520mpppfx/lib \ 
    -lmpi -lresampler -laec -lanr -ljpeg -lhdmi -lVoiceEngine -lpthread"
  AC_CHECK_LIBRARY([${hi3520mppinc}], $[{hi3520mpplib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3520mpp=yes], [have_hi3520mpp=no]
  )
  if test x$have_hi3520mpp = xyes; then
    HI3520MPP_CPPFLAGS=$hi3520mppinc
    HI3520MPP_LDFLAGS=$hi3520mpplib
    AC_DEFINE(HAVE_HI3520MPP_SUPPORT, 1, [support Hi3520 MPP])
    AC_SUBST(HI3520MPP_CPPFLAGS)
    AC_SUBST(HI3520MPP_LDFLAGS)
  fi
else
  have_hi3520mpp=no
fi
AC_MSG_RESULT([$have_hi3520mpp])
AM_CONDITIONAL([ENABLE_HI3520MPP], [test x$have_hi3520mpp = xyes])


dnl check for libev library
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
src/rtsp-server/Makefile
src/dbus/Makefile
src/platform/Makefile
src/platform/hi3518mpp/Makefile
src/platform/hi3520mpp/Makefile
])
