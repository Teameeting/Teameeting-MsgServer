#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(SeqnClient, 1.0.1)
AC_CONFIG_SRCDIR([ClientManager.cpp])
AM_INIT_AUTOMAKE()
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
CXXFLAGS=${CXXFLAGS--g}
AC_PROG_CXX
AC_PROG_CC
####AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset select strdup strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
