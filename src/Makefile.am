# -----------------------------------------------------------------------------
# iXiON library main makefile
# -----------------------------------------------------------------------------
# (c) iXiONmedia 1999
# -----------------------------------------------------------------------------

# Source files ----------------------------------------------------------------
basic_headers=ixlib_base.hh ixlib_array.hh ixlib_garbage.hh \
  ixlib_exbase.hh ixlib_exgen.hh ixlib_exio.hh \
  ixlib_string.hh ixlib_re.hh \
  ixlib_numconv.hh ixlib_numeric.hh \
  ixlib_random.hh 

basic_objects=base.cc exbase.cc exgen.cc exio.cc \
  string.cc numconv.cc numeric.cc re.cc 

advanced_headers=ixlib_cmdline.hh \
  ixlib_geometry.hh ixlib_geometry_impl.hh \
  ixlib_matrix.hh ixlib_matrix_impl.hh \
  ixlib_textfile.hh ixlib_scanner.hh ixlib_token_lex.hh \
  ixlib_token_xml.hh ixlib_scanxml.hh ixlib_xml.hh

advanced_objects=cmdline.cc textfile.cc scanner.cc lex.xml.cc xml.cc

headers=$(basic_headers) $(advanced_headers)

# Generated sources -----------------------------------------------------------
lex.xml.cc: lex.xml.yy
	$(LEX) $< 

BUILT_SOURCES=lex.xml.cc

CLEANFILES=lex.xml.cc

EXTRA_DIST=lex.xml.yy

# Targets ---------------------------------------------------------------------
lib_LTLIBRARIES=libixion.la

libixion_la_SOURCES=$(headers) $(basic_objects) $(advanced_objects)
libixion_la_LIBADD=@LEXLIB@
libixion_la_LDFLAGS=-version-info @IXLIB_BINARY_IFACE_VERSION@:@IXLIB_BINARY_REVISION@:@IXLIB_BINARY_AGE@

include_HEADERS=$(headers)
