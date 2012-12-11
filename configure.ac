#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([autoz-gui], [0.0.1], [azagli@libero.it])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(-Wall)

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

AC_CANONICAL_SYSTEM

LT_INIT

AC_LIBTOOL_WIN32_DLL

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_RANLIB

# Checks for libraries.
PKG_CHECK_MODULES(AUTOZGUI, [libgda-ui-4.0 >= 4.1.0])

AC_SUBST(AUTOZGUI_CFLAGS)
AC_SUBST(AUTOZGUI_LIBS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
	libautozgui.pc
	Makefile
	src/Makefile
	library/Makefile
	data/Makefile
	data/autoz-gui/Makefile
	data/autoz-gui/gui/Makefile
	tests/Makefile
])
AC_OUTPUT
