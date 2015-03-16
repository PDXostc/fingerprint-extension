AC_PREREQ([2.69])
AC_INIT([fingerprint], [0.0.1], [clambers@jaguarlandrover.com])
AC_COPYRIGHT([Copyright (C) 2015 Jaguar Land Rover Automotive PLC])
AC_CONFIG_SRCDIR([src/fingerprint.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror])
AM_SILENT_RULES([yes])
AM_PROG_AR
LT_INIT
AC_PROG_CC
AM_PROG_CC_C_O
AC_PATH_PROG([XXD], [xxd])
PKG_CHECK_MODULES([libfprint], [libfprint])
PKG_CHECK_MODULES([jansson], [jansson])
PKG_CHECK_MODULES([check], [check >= 0.9.8])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile fingerprint.pc])
AC_OUTPUT
