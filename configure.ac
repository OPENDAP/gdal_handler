# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# Configure script for OPeNDAP NCML data handler

AC_PREREQ(2.61)

AC_INIT(gdal_handler, 0.9.0, support@opendap.org)
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
# AC_PROG_RANLIB

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
AC_CHECK_LIBDAP([3.11.0],
 [
 ],
 [ AC_MSG_ERROR([Cannot find expected version of libdap])
])

AC_CHECK_BES([3.9.0],
[
],
[ AC_MSG_ERROR([Could not find expected version of bes library and headers])
])

GDAL_FOUND=

AC_ARG_WITH(gdal,[  --with-gdal=PFX   Prefix where gdal is installed (optional).],
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

if test -z "$GDAL_FOUND"
then
    AX_LIB_GDAL([1.7.2])
    GDAL_FOUND="no"
    if test ! -z "$GDAL_CFLAGS" -a ! -z "$GDAL_LDFLAGS"; then
        GDAL_FOUND="yes"
    fi
fi

if test -z "$GDAL_FOUND"
then
    AC_MSG_ERROR([I could not find GDAL.])
fi

OPENDAP_DEBUG_OPTION

# When we have tests, add these: tests/Makefile tests/atlocal
AC_CONFIG_FILES([Makefile tests/Makefile tests/atlocal tests/package.m4])
AC_OUTPUT

