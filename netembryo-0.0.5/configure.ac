dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

m4_define([netembryo_major_version], [0])
m4_define([netembryo_minor_version], [0])
m4_define([netembryo_micro_version], [5])
m4_define([netembryo_interface_age], [0])
m4_define([netembryo_binary_age],
          [m4_eval(100 * netembryo_minor_version + netembryo_micro_version)])
m4_define([netembryo_version],
          [netembryo_major_version.netembryo_minor_version.netembryo_micro_version])
m4_define([netembryo_lt_current],
          [m4_eval(100 * netembryo_minor_version + netembryo_micro_version - netembryo_interface_age)])
m4_define([netembryo_lt_revision], [netembryo_interface_age])
m4_define([netembryo_lt_age], [m4_eval(netembryo_binary_age - netembryo_interface_age)])

AC_INIT(netembryo, [netembryo_version], [http://streaming.polito.it])

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_TARGET([])
AM_INIT_AUTOMAKE(dist-bzip2 check-news)

AC_PREFIX_DEFAULT("/usr")

AC_ARG_ENABLE(debug,
[  --enable-debug          enable gcc dubugging flags [[default=no]]],,
        enable_debug="no")

AC_ARG_ENABLE(ipv6,
[  --enable-ipv6            enable IPv6 support [[default=autodetected]]],,
        enable_ipv6="yes")

AC_ARG_ENABLE(sctp,
[  --enable-sctp            enable SCTP support [[default=autodetected]]],, 
        enable_sctp="yes")

dnl Check for LibTool Dynamic Loading
m4_undefine([AC_PROG_CXX])
m4_defun([AC_PROG_CXX],[])
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_SUBST([LIBTOOL_DEPS])

AC_SUBST(HAVE_SSL)

dnl Check for Endianess
AC_C_BIGENDIAN([BIGENDIAN="Big Endian"] AC_DEFINE([WORDS_BIGENDIAN], 1, [Define if manchine is big-endian]),[BIGENDIAN="Little Endian"] )

dnl Checks for libraries.
AC_CHECK_LIBM

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(sys/errno.h fcntl.h getopt.h sys/time.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_INLINE
AC_CHECK_TYPES(struct sockaddr_storage,,, [#include <sys/socket.h>])

#init CFLAG and PKG_LIB
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(socket getaddrinfo getopt_long gettimeofday uname)

dnl test enable
if test "x$enable_ipv6" = "xyes"; then
        AC_CHECK_TYPE(struct sockaddr_in6, AC_DEFINE([IPV6], 1, [Define IPv6 support]), , [#include <netinet/in.h>])
else
        AC_MSG_WARN([IPv6 disabled: check skipped.])
fi

dnl Check for SCTP support
if test "$enable_sctp" = "yes"; then
        AC_CHECK_TYPE(struct sctp_sndrcvinfo, [
         AC_CHECK_LIB(sctp,sctp_recvmsg)],
         AC_MSG_WARN([SCTP disabled: headers not found]),
         [#include <netinet/sctp.h>])
else
        AC_MSG_WARN([SCTP disabled: check skipped.])
fi

AM_CONDITIONAL(NETEMBRYO_DEBUG, [test "$enable_debug" = "yes"])
if test "x$enable_debug" = "xyes"; then
        CFLAGS="$CFLAGS -g -ggdb"
        AC_DEFINE([ENABLE_DEBUG], 1,[Debug enabled])
        debug_msg="
   Debuging flags enabled: welcome developer!!!"
fi

case "$enable_errors" in
        pedantic)
                CFLAGS="$CFLAGS -pedantic-errors -Werror"
        ;;
        normal | yes)
                CFLAGS="$CFLAGS -Werror"
        ;;
        none | *)
                CFLAGS="$CFLAGS"
        ;;
esac

AM_CONDITIONAL(HAVE_SSL, [test "x" != "x"])

LT_CURRENT=netembryo_lt_current
LT_REVISION=netembryo_lt_revision
LT_AGE=netembryo_lt_age

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
AC_SUBST(LIBS)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([wsocket/Makefile])
AC_CONFIG_FILES([utils/Makefile])
AC_CONFIG_FILES([libnetembryo.pc])

AC_OUTPUT

AC_MSG_NOTICE([config files successfully generated!
$debug_msg
Byte order ........................ : this is a $BIGENDIAN machine
NetEmbryo version is .............. : $VERSION
NetEmbryo Install prefix .......... : $prefix
NetEmbryo sctp support enabled .... : $enable_sctp

 'make' will now compile NetEmbryo and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
