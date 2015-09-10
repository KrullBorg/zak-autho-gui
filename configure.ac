#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([zak-autho-gui], [0.0.5], [azagli@libero.it])
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
AM_PROG_AR

# Checks for libraries.
PKG_CHECK_MODULES(AUTOZGUI, [libgda-ui-5.0 >= 5.0.0])

AC_SUBST(AUTOZGUI_CFLAGS)
AC_SUBST(AUTOZGUI_LIBS)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
	libzakauthogui.pc
	Makefile
	src/Makefile
	library/Makefile
	data/Makefile
	data/zak-autho-gui/Makefile
	data/zak-autho-gui/gui/Makefile
	tests/Makefile
])
AC_OUTPUT
