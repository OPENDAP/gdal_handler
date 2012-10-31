# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# Configure script for OPeNDAP NCML data handler

AC_PREREQ(2.61)

AC_INIT(gdal_handler, 0.9.2, support@opendap.org)
AC_CONFIG_AUX_DIR([conf])
AC_CONFIG_MACRO_DIR([conf])

AM_INIT_AUTOMAKE
LT_INIT

PACKAGE_MAJOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
PACKAGE_MINOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
PACKAGE_SUBMINOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
AC_MSG_NOTICE(Package Major Version:     $PACKAGE_MAJOR_VERSION)
AC_MSG_NOTICE(Package Minor Version:     $PACKAGE_MINOR_VERSION)
AC_MSG_NOTICE(Package SubMinor Version:  $PACKAGE_SUBMINOR_VERSION)
AC_SUBST(PACKAGE_MAJOR_VERSION)
AC_SUBST(PACKAGE_MINOR_VERSION)
AC_SUBST(PACKAGE_SUBMINOR_VERSION)

AC_CONFIG_SRCDIR([GDALTypes.h])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_TESTDIR(tests, [.])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

if test "$CC" = "gcc"
then
    AM_CONDITIONAL([COMPILER_IS_GCC],[true])
else
    AM_CONDITIONAL([COMPILER_IS_GCC],[false])
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Make sure we have the cctype library
AC_SEARCH_LIBS([isdigit], [cctype])

# Checks for library functions.
AC_CHECK_FUNCS([strchr isdigit])

dnl Checks for specific libraries
AC_CHECK_LIBDAP([3.11.2],
 [
 ],
 [ AC_MSG_ERROR([Cannot find expected version of libdap])
])

AC_CHECK_BES([3.11.0],
[
],
[ AC_MSG_ERROR([Could not find expected version of bes library and headers])
])

# Which copy of GDAL should be used for the build?
GDAL_FOUND=

AC_ARG_WITH(gdal, AS_HELP_STRING([--with-gdal], 
		  	         [Use the copy of GDAL at this location]),
            with_gdal_prefix="$withval", with_gdal_prefix="")

if test -n "$with_gdal_prefix" -a -x $with_gdal_prefix/bin/gdal-config
then
    AC_MSG_NOTICE([Using $with_gdal_prefix as the GDAL prefix directory.])
    GDAL_LDFLAGS="`$with_gdal_prefix/bin/gdal-config --libs`"
    GDAL_CFLAGS="`$with_gdal_prefix/bin/gdal-config --cflags`"
    GDAL_FOUND="yes"
elif test -n "$with_gdal_prefix"
then
    AC_MSG_ERROR([You set the gdal-prefix directory to $with_gdal_prefix, but gdal-config is not there.])
fi

dnl A failed idea... jhrg 10/31/12
dnl
dnl AC_ARG_ENABLE(local-gdal, AS_HELP_STRING([--enable-local-gdal], [Build and use the local copy of GDAL]) )

dnl if test -n "$enable_local_gdal" -a "$enable_local_gdal" = "yes"
dnl then
dnl     AC_MSG_NOTICE([Building the local GDAL library.])
dnl     AM_CONDITIONAL(LOCAL_GDAL, true)

dnl     # configure bundled gdal here; the Makefile in third_party forces
dnl     # install to be run for the 'all' target there, so this lib will
dnl     # install in the prefix defined below.
dnl     # Added --with-pic for Linux (CentOS 6). jhrg 10/16/12
dnl     gdal_prx=`pwd`/third_party/gdal-1.9.1/build
dnl     if (cd third_party/gdal-1.9.1 && ./configure --prefix=$gdal_prx --disable-shared --with-pic)
dnl     then
dnl         GDAL_LDFLAGS="-Lthird_party/gdal-1.9.1/build/lib -lgdal"
dnl         GDAL_CFLAGS="-Ithird_party/gdal-1.9.1/build/include"
dnl         AC_SUBST([GDAL_LDFLAGS])
dnl         AC_SUBST([GDAL_CFLAGS])
dnl         GDAL_FOUND="yes"
    
dnl         # AC_MSG_NOTICE(["GDAL_LDFLAGS status >$GDAL_LDFLAGS<"])
dnl         # AC_MSG_NOTICE(["GDAL_CFLAGS status >$GDAL_CFLAGS<"])
dnl     else
dnl         AC_MSG_ERROR([I could not build GDAL.])
dnl     fi
dnl else
dnl     AM_CONDITIONAL(LOCAL_GDAL, false)
dnl fi

if test -z "$GDAL_FOUND"
then
    AX_LIB_GDAL([1.7.2])
    if test ! -z "$GDAL_CFLAGS" -a ! -z "$GDAL_LDFLAGS"; then
        GDAL_FOUND="yes"
    fi
fi

if test -z "$GDAL_FOUND"
then
    AC_MSG_ERROR([I could not find GDAL.])
fi

AC_ARG_ENABLE([developer],
[AS_HELP_STRING([--enable-developer],
[Build a debug (-g3 -O0) version of this code (default is no)])],
[case "${enableval}" in
       yes) build_hyrax_debug=true ;;
       no)  build_hyrax_debug=false ;;
       *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
     esac],
[build_hyrax_debug=false])

if test "x$build_hyrax_debug" = "xtrue"; then
   AC_MSG_NOTICE(["Building debug version"])
else
   AC_MSG_NOTICE(["Not building debug version"])
   AC_DEFINE([NDEBUG], [1], [Remove this to activate assert() calls.])
fi
AM_CONDITIONAL([BUILD_HYRAX_DEBUG], [test "x$build_hyrax_debug" = "xtrue"])

AC_CONFIG_FILES([Makefile tests/Makefile tests/atlocal tests/package.m4])

AC_CONFIG_FILES([tests/generate_metadata_baseline.sh], [chmod +x tests/generate_metadata_baseline.sh])
AC_CONFIG_FILES([tests/generate_data_baseline.sh], [chmod +x tests/generate_data_baseline.sh])

dnl AC_CONFIG_FILES([third_party/Makefile])

AC_OUTPUT

