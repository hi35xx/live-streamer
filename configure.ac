dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(live-streamer, 1.0.0)

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

PKG_CHECK_MODULES(FREETYPE, [freetype2])

PKG_CHECK_MODULES(FONTCONFIG, [fontconfig])

PKG_CHECK_MODULES(SDL, [sdl SDL_ttf SDL_image])


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


dnl check for hi3518v100 mpp library
AC_ARG_WITH(hi3518v100mpp,
	[AS_HELP_STRING([--with-hi3518v100mpp=PATH],
		[specify prefix directory for himpp-hi3518v100 package.])],
		[], [with_hi3518v100mpp=no])

AC_MSG_CHECKING([for hi3518v100 mpp library])
case $with_hi3518v100mpp in
 yes)
   hi3518v100mpppfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3518v100mpppfx=`echo "$with_hi3518v100mpp" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3518v100mpp])
   AC_MSG_ERROR([hi3518v100mpp must be an absolute path."])
   ;;
esac
if test "x$with_hi3518v100mpp" != xno; then
  hi3518v100mppinc="-I$hi3518v100mpppfx/include/hi3518v100mpp"
  hi3518v100mpplib="-L$hi3518v100mpppfx/lib \ 
    -lmpi -lresampler -laec -lvqev2 -lanr -lVoiceEngine \
    -lisp -l_hiae -l_hiawb -l_hiaf -lpthread"
  AC_CHECK_LIBRARY([${hi3518v100mppinc}], $[{hi3518v100mpplib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3518v100mpp=yes], [have_hi3518v100mpp=no]
  )
  if test x$have_hi3518v100mpp = xyes; then
    HI3518V100MPP_CPPFLAGS=$hi3518v100mppinc
    HI3518V100MPP_LDFLAGS=$hi3518v100mpplib
    AC_DEFINE(HAVE_HI3518V100MPP_SUPPORT, 1, [support Hi3518v100 MPP])
    AC_SUBST(HI3518V100MPP_CPPFLAGS)
    AC_SUBST(HI3518V100MPP_LDFLAGS)
  fi
else
  have_hi3518v100mpp=no
fi
AC_MSG_RESULT([$have_hi3518v100mpp])
AM_CONDITIONAL([ENABLE_HI3518V100MPP], [test x$have_hi3518v100mpp = xyes])


dnl check for hi3518v200 mpp library
AC_ARG_WITH(hi3518v200mpp,
	[AS_HELP_STRING([--with-hi3518v200mpp=PATH],
		[specify prefix directory for himpp-hi3518v200 package.])],
		[], [with_hi3518v200mpp=no])

AC_MSG_CHECKING([for hi3518v200 mpp library])
case $with_hi3518v200mpp in
 yes)
   hi3518v200mpppfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3518v200mpppfx=`echo "$with_hi3518v200mpp" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3518v200mpp])
   AC_MSG_ERROR([hi3518v200mpp must be an absolute path."])
   ;;
esac
if test "x$with_hi3518v200mpp" != xno; then
  hi3518v200mppinc="-I$hi3518v200mpppfx/include/hi3518v200mpp"
  hi3518v200mpplib="-L$hi3518v200mpppfx/lib \ 
    -lmpi -ldnvqe -lhive_RES -lhive_AEC -lupvqe -lhive_ANR -lVoiceEngine \
    -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog -lpthread"
  AC_CHECK_LIBRARY([${hi3518v200mppinc}], $[{hi3518v200mpplib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3518v200mpp=yes], [have_hi3518v200mpp=no]
  )
  if test x$have_hi3518v200mpp = xyes; then
    HI3518V200MPP_CPPFLAGS=$hi3518v200mppinc
    HI3518V200MPP_LDFLAGS=$hi3518v200mpplib
    AC_DEFINE(HAVE_HI3518V200MPP_SUPPORT, 1, [support Hi3518v200 MPP])
    AC_SUBST(HI3518V200MPP_CPPFLAGS)
    AC_SUBST(HI3518V200MPP_LDFLAGS)
  fi
else
  have_hi3518v200mpp=no
fi
AC_MSG_RESULT([$have_hi3518v200mpp])
AM_CONDITIONAL([ENABLE_HI3518V200MPP], [test x$have_hi3518v200mpp = xyes])


dnl check for hi3520v100 mpp library
AC_ARG_WITH(hi3520v100mpp,
	[AS_HELP_STRING([--with-hi3520v100mpp=PATH],
		[specify prefix directory for himpp-hi3520v100 package.])],
		[], [with_hi3520v100mpp=no])

AC_MSG_CHECKING([for hi3520v100 mpp library])
case $with_hi3520v100mpp in
 yes)
   hi3520v100mpppfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3520v100mpppfx=`echo "$with_hi3520v100mpp" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3520v100mpp])
   AC_MSG_ERROR([hi3520v100mpp must be an absolute path."])
   ;;
esac
if test "x$with_hi3520v100mpp" != xno; then
  hi3520v100mppinc="-I$hi3520v100mpppfx/include/hi3520v100mpp"
  hi3520v100mpplib="-L$hi3520v100mpppfx/lib \ 
    -lmpi -lresampler -laec -lanr -ljpeg -lhdmi -lVoiceEngine -lpthread"
  AC_CHECK_LIBRARY([${hi3520v100mppinc}], $[{hi3520v100mpplib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3520v100mpp=yes], [have_hi3520v100mpp=no]
  )
  if test x$have_hi3520v100mpp = xyes; then
    HI3520V100MPP_CPPFLAGS=$hi3520v100mppinc
    HI3520V100MPP_LDFLAGS=$hi3520v100mpplib
    AC_DEFINE(HAVE_HI3520V100MPP_SUPPORT, 1, [support Hi3520 MPP])
    AC_SUBST(HI3520V100MPP_CPPFLAGS)
    AC_SUBST(HI3520V100MPP_LDFLAGS)
  fi
else
  have_hi3520v100mpp=no
fi
AC_MSG_RESULT([$have_hi3520v100mpp])
AM_CONDITIONAL([ENABLE_HI3520V100MPP], [test x$have_hi3520v100mpp = xyes])


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
src/platform/hi3518v100mpp/Makefile
src/platform/hi3518v200mpp/Makefile
src/platform/hi3520v100mpp/Makefile
])
