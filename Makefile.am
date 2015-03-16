XXD = @XXD@ -i
ACLOCAL_AMFLAGS = -I m4
BUILT_SOURCES = src/fingerprint.h
TESTS = tests/check-fingerprint.c
SUFFIXES = .js

pcdatadir = $(datarootdir)/pkgconfig
pcdata_DATA = fingerprint.pc

extdir = $(libdir)/tizen-extensions-crosswalk
ext_LTLIBRARIES = src/libfingerprint.la
src_libfingerprint_la_SOURCES = src/fingerprint.c src/fingerprint.js src/XW_Extension.h src/uthash.h
src_libfingerprint_la_CFLAGS = @jansson_CFLAGS@ @libfprint_CFLAGS@
src_libfingerprint_la_LDFLAGS = -version 0:0:0
src_libfingerprint_la_LIBADD = @jansson_LIBS@ @libfprint_LIBS@

check_PROGRAMS = tests/check-fingerprint
tests_check_fingerprint_SOURCES = tests/check-fingerprint.c
tests_check_fingerprint_CFLAGS = @check_CFLAGS@
tests_check_fingerprint_LDADD = src/libfingerprint.la @check_LIBS@

.js.h:
	$(XXD) $< $@
