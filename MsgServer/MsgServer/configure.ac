#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(MSCommon, 1.0.1)
AC_CONFIG_SRCDIR([MSCommon/PlatformHeader.h])
AM_INIT_AUTOMAKE()
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
####remove optimized
####CXXFLAGS=${CXXFLAGS--g}
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
#####AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lQtCore':
AC_CHECK_LIB([QtCore], [main])
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lev':
AC_CHECK_LIB([ev], [main])
# FIXME: Replace `main' with a function in `-levent':
AC_CHECK_LIB([event], [main])
# FIXME: Replace `main' with a function in `-lhiredis':
AC_CHECK_LIB([hiredis], [main])
# FIXME: Replace `main' with a function in `-livykis':
AC_CHECK_LIB([ivykis], [main])
# FIXME: Replace `main' with a function in `-lnsl':
AC_CHECK_LIB([nsl], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])
# FIXME: Replace `main' with a function in `-lsocket':
AC_CHECK_LIB([socket], [main])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h limits.h mach/mach.h malloc.h memory.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/socket.h sys/statfs.h sys/statvfs.h sys/systeminfo.h sys/time.h sys/timeb.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([atexit clock_gettime dup2 getcwd gethostbyaddr gethostbyname gethostname gettimeofday inet_ntoa localtime_r memmove memset mkdir modf rmdir select socket strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol strtoul tzset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
