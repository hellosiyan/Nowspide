#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([nowspide], [0.1], [xsisqox@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])


# Checks for programs.
AC_PROG_LIBTOOL
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_CC_C_O
AC_HEADER_STDC

NSP_WARNINGS=-Wall
AC_SUBST(NSP_WARNINGS)

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

# Checks for library functions.

# ******
# GNOME Libs
# ******


GTK_REQUIRED=2.20
GLIB_REQUIRED=2.20


NSP_MODULES="gtk+-2.0 >= $GTK_REQUIRED \
             glib-2.0 >= $GLIB_REQUIRED \
             gio-2.0 >= $GLIB_REQUIRED"


# ****************
# CFLAGS/LIBS init
# ****************

PKG_CHECK_MODULES(NSP, $NSP_MODULES)
AC_SUBST(NSP_CFLAGS)
AC_SUBST(NSP_LIBS)

SHAVE_INIT([], [enable])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 shave
                 shave-libtool])
AC_OUTPUT

echo "
    Nowspide ........... : Version $VERSION
    Location ........... : $prefix/bin
    CFLAGS ............. : $CFLAGS
"

