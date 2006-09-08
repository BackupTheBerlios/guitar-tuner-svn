dnl Process this file with autoconf to produce a configure script.

AC_INIT(src/main.c)
AM_CONFIG_HEADER(config.h)

PACKAGE=hello
VERSION=0.1

AM_INIT_AUTOMAKE($PACKAGE,$VERSION)

AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_INSTALL
AC_CHECK_FUNC(ioctl,,AC_MSG_ERROR(ioctl() not found))

# Check for Math Function
AC_CHECK_LIB(m,cos,,AC_MSG_ERROR(cos() not found))

# Check for FFTW3 Function
AC_CHECK_LIB(fftw3,fftw_plan_r2r_1d,,AC_MSG_ERROR(fftw_plan_r2r_1d() not found))
AC_CHECK_LIB(fftw3,fftw_execute,,AC_MSG_ERROR(fftw_execute() not found))
AC_CHECK_LIB(fftw3,fftw_destroy_plan,,AC_MSG_ERROR(fftw_destroy_plan() not found))

PKG_CHECK_MODULES(DEPS, fftw3 >= 3.1 gtk+-2.0 >= 2.8 glib-2.0 >= 2.8 gthread-2.0 >= 0.2)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_OUTPUT([
Makefile 
src/Makefile 
pixmaps/Makefile 
])
