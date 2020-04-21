dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(live-streamer, 1.9.5)

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

PKG_CHECK_MODULES(JSONCPP, [jsoncpp],
	[have_jsoncpp=yes], [have_jsoncpp=no])
if test "x$have_jsoncpp" = "xyes"; then
	AC_DEFINE([HAVE_JSONCPP_SUPPORT], [1], [jsoncpp support])
fi
AM_CONDITIONAL([ENABLE_JSONCPP], [ test "x$have_jsoncpp" = "xyes" ])

PKG_CHECK_MODULES([VO_AACENC], [vo-aacenc],
	[have_vo_aacenc=yes], [have_vo_aacenc=no])
if test "x$have_vo_aacenc" = "xyes"; then
	AC_DEFINE([HAVE_VO_AACENC_SUPPORT], [1], [vo-aacenc support])
fi
AM_CONDITIONAL([ENABLE_VO_AACENC], [ test "x$have_vo_aacenc" = "xyes" ])

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
AC_ARG_WITH(hi3518v100,
	[AS_HELP_STRING([--with-hi3518v100=PATH],
		[specify prefix directory for himpp-hi3518v100 package.])],
		[], [with_hi3518v100=no])

AC_MSG_CHECKING([for hi3518v100 mpp library])
case $with_hi3518v100 in
 yes)
   hi3518v100pfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3518v100pfx=`echo "$with_hi3518v100" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3518v100])
   AC_MSG_ERROR([hi3518v100 must be an absolute path."])
   ;;
esac
if test "x$with_hi3518v100" != xno; then
  hi3518v100inc="-I$hi3518v100pfx/include/hi3518v100mpp"
  hi3518v100lib="-L$hi3518v100pfx/lib \ 
    -lmpi -lresampler -laec -lvqev2 -lanr -lVoiceEngine \
    -lisp -l_hiae -l_hiawb -l_hiaf -lpthread"
  AC_CHECK_LIBRARY([${hi3518v100inc}], $[{hi3518v100lib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3518v100=yes], [have_hi3518v100=no]
  )
  if test x$have_hi3518v100 = xyes; then
    HI3518V100_CPPFLAGS=$hi3518v100inc
    HI3518V100_LDFLAGS=$hi3518v100lib
    AC_DEFINE(HAVE_HI3518V100_SUPPORT, 1, [support Hi3518v100 MPP])
    AC_SUBST(HI3518V100_CPPFLAGS)
    AC_SUBST(HI3518V100_LDFLAGS)
  fi
else
  have_hi3518v100=no
fi
AC_MSG_RESULT([$have_hi3518v100])
AM_CONDITIONAL([ENABLE_HI3518V100], [test x$have_hi3518v100 = xyes])


dnl check for hi3518v200 mpp library
AC_ARG_WITH(hi3518v200,
	[AS_HELP_STRING([--with-hi3518v200=PATH],
		[specify prefix directory for himpp-hi3518v200 package.])],
		[], [with_hi3518v200=no])

AC_MSG_CHECKING([for hi3518v200 mpp library])
case $with_hi3518v200 in
 yes)
   hi3518v200pfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3518v200pfx=`echo "$with_hi3518v200" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3518v200])
   AC_MSG_ERROR([hi3518v200 must be an absolute path."])
   ;;
esac
if test "x$with_hi3518v200" != xno; then
  hi3518v200inc="-I$hi3518v200pfx/include/hi3518v200mpp"
  hi3518v200lib="-L$hi3518v200pfx/lib \
    -lmpi -ldnvqe -lhive_RES -lhive_AEC -lupvqe -lhive_ANR -lVoiceEngine \
    -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog -lpthread"
  AC_CHECK_LIBRARY([${hi3518v200inc}], $[{hi3518v200lib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3518v200=yes], [have_hi3518v200=no]
  )
  if test x$have_hi3518v200 = xyes; then
    HI3518V200_CPPFLAGS=$hi3518v200inc
    HI3518V200_LDFLAGS=$hi3518v200lib
    AC_DEFINE(HAVE_HI3518V200_SUPPORT, 1, [support Hi3518v200 MPP])
    AC_SUBST(HI3518V200_CPPFLAGS)
    AC_SUBST(HI3518V200_LDFLAGS)
  fi
else
  have_hi3518v200=no
fi
AC_MSG_RESULT([$have_hi3518v200])
AM_CONDITIONAL([ENABLE_HI3518V200], [test x$have_hi3518v200 = xyes])


dnl check for hi3516cv300 mpp library
AC_ARG_WITH(hi3516cv300,
	[AS_HELP_STRING([--with-hi3516cv300=PATH],
		[specify prefix directory for himpp-hi3516cv300 package.])],
		[], [with_hi3516cv300=no])

AC_MSG_CHECKING([for hi3516cv300 mpp library])
case $with_hi3516cv300 in
 yes)
   hi3516cv300pfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3516cv300pfx=`echo "$with_hi3516cv300" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3516cv300])
   AC_MSG_ERROR([hi3516cv300 must be an absolute path."])
   ;;
esac
if test "x$with_hi3516cv300" != xno; then
  hi3516cv300inc="-I$hi3516cv300pfx/include/hi3516cv300mpp"
  hi3516cv300lib="-L$hi3516cv300pfx/lib \
    -lmpi -ldnvqe -lhive_RES -lhive_AEC -lupvqe -lhive_ANR -lVoiceEngine \
    -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog -lpthread"
  AC_CHECK_LIBRARY([${hi3516cv300inc}], $[{hi3516cv300lib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3516cv300=yes], [have_hi3516cv300=no]
  )
  if test x$have_hi3516cv300 = xyes; then
    HI3516CV300_CPPFLAGS=$hi3516cv300inc
    HI3516CV300_LDFLAGS=$hi3516cv300lib
    AC_DEFINE(HAVE_HI3516CV300_SUPPORT, 1, [support Hi3516cv300 MPP])
    AC_SUBST(HI3516CV300_CPPFLAGS)
    AC_SUBST(HI3516CV300_LDFLAGS)
  fi
else
  have_hi3516cv300=no
fi
AC_MSG_RESULT([$have_hi3516cv300])
AM_CONDITIONAL([ENABLE_HI3516CV300], [test x$have_hi3516cv300 = xyes])


dnl check for hi3516ev200 mpp library
AC_ARG_WITH(hi3516ev200,
	[AS_HELP_STRING([--with-hi3516ev200=PATH],
		[specify prefix directory for himpp-hi3516ev200 package.])],
		[], [with_hi3516ev200=no])

AC_MSG_CHECKING([for hi3516ev200 mpp library])
case $with_hi3516ev200 in
 yes)
   hi3516ev200pfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3516ev200pfx=`echo "$with_hi3516ev200" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3516ev200])
   AC_MSG_ERROR([hi3516ev200 must be an absolute path."])
   ;;
esac
if test "x$with_hi3516ev200" != xno; then
  hi3516ev200inc="-I$hi3516ev200pfx/include/hi3516ev200mpp"
  hi3516ev200lib="-L$hi3516ev200pfx/lib \
    -lmpi -lsecurec -ldnvqe -lhive_RES -lhive_AEC -lupvqe -lhive_ANR -lhive_EQ -lVoiceEngine \
    -lisp -l_hiae -l_hiawb -l_hidrc -l_hildci -l_hidehaze -lpthread"
  AC_CHECK_LIBRARY([${hi3516ev200inc}], $[{hi3516ev200lib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3516ev200=yes], [have_hi3516ev200=no]
  )
  if test x$have_hi3516ev200 = xyes; then
    HI3516EV200_CPPFLAGS=$hi3516ev200inc
    HI3516EV200_LDFLAGS=$hi3516ev200lib
    AC_DEFINE(HAVE_HI3516EV200_SUPPORT, 1, [support Hi3516ev200 MPP])
    AC_SUBST(HI3516EV200_CPPFLAGS)
    AC_SUBST(HI3516EV200_LDFLAGS)
  fi
else
  have_hi3516ev200=no
fi
AC_MSG_RESULT([$have_hi3516ev200])
AM_CONDITIONAL([ENABLE_HI3516EV200], [test x$have_hi3516ev200 = xyes])


dnl check for hi3520v100 mpp library
AC_ARG_WITH(hi3520v100,
	[AS_HELP_STRING([--with-hi3520v100=PATH],
		[specify prefix directory for himpp-hi3520v100 package.])],
		[], [with_hi3520v100=no])

AC_MSG_CHECKING([for hi3520v100 mpp library])
case $with_hi3520v100 in
 yes)
   hi3520v100pfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3520v100pfx=`echo "$with_hi3520v100" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3520v100])
   AC_MSG_ERROR([hi3520v100 must be an absolute path."])
   ;;
esac
if test "x$with_hi3520v100" != xno; then
  hi3520v100inc="-I$hi3520v100pfx/include/hi3520v100mpp"
  hi3520v100lib="-L$hi3520v100pfx/lib \ 
    -lmpi -lresampler -laec -lanr -ljpeg -lhdmi -lVoiceEngine -lpthread"
  AC_CHECK_LIBRARY([${hi3520v100inc}], $[{hi3520v100lib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3520v100=yes], [have_hi3520v100=no]
  )
  if test x$have_hi3520v100 = xyes; then
    HI3520V100_CPPFLAGS=$hi3520v100inc
    HI3520V100_LDFLAGS=$hi3520v100lib
    AC_DEFINE(HAVE_HI3520V100_SUPPORT, 1, [support Hi3520 MPP])
    AC_SUBST(HI3520V100_CPPFLAGS)
    AC_SUBST(HI3520V100_LDFLAGS)
  fi
else
  have_hi3520v100=no
fi
AC_MSG_RESULT([$have_hi3520v100])
AM_CONDITIONAL([ENABLE_HI3520V100], [test x$have_hi3520v100 = xyes])

dnl check for hi3520dv200 mpp library
AC_ARG_WITH(hi3520dv200,
	[AS_HELP_STRING([--with-hi3520dv200=PATH],
		[specify prefix directory for himpp-hi3520dv200 package.])],
		[], [with_hi3520dv200=no])

AC_MSG_CHECKING([for hi3520dv200 mpp library])
case $with_hi3520dv200 in
 yes)
   hi3520dv200pfx="$ac_sysroot/usr" ;;
 no|'')
   ;;
 /*)
   hi3520dv200pfx=`echo "$with_hi3520dv200" | sed -e "$sed_quote_subst"` ;;
 *)
   AC_MSG_RESULT([$with_hi3520dv200])
   AC_MSG_ERROR([hi3520dv200 must be an absolute path."])
   ;;
esac
if test "x$with_hi3520dv200" != xno; then
  hi3520dv200inc="-I$hi3520dv200pfx/include/hi3520dv200mpp"
  hi3520dv200lib="-L$hi3520dv200pfx/lib \
    -lmpi -lresampler -laec -lanr -ljpeg -lhdmi -lVoiceEngine -lvqev2 -lpthread"
  AC_CHECK_LIBRARY([${hi3520dv200inc}], $[{hi3520dv200lib}],
    [AC_LANG_PROGRAM(
      [#include <hi_comm_sys.h>
       #include <mpi_sys.h>],
      [HI_MPI_SYS_Init();
       HI_MPI_SYS_Exit();])
    ], [have_hi3520dv200=yes], [have_hi3520dv200=no]
  )
  if test x$have_hi3520dv200 = xyes; then
    HI3520DV200_CPPFLAGS=$hi3520dv200inc
    HI3520DV200_LDFLAGS=$hi3520dv200lib
    AC_DEFINE(HAVE_HI3520DV200_SUPPORT, 1, [support Hi3520dv200 MPP])
    AC_SUBST(HI3520DV200_CPPFLAGS)
    AC_SUBST(HI3520DV200_LDFLAGS)
  fi
else
  have_hi3520dv200=no
fi
AC_MSG_RESULT([$have_hi3520dv200])
AM_CONDITIONAL([ENABLE_HI3520DV200], [test x$have_hi3520dv200 = xyes])


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
src/common/Makefile
src/platform/hi3518v200/Makefile
src/platform/hi3516cv300/Makefile
src/platform/hi3516ev200/Makefile
])
