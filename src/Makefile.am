# -----------------------------------------------------------------------------
# ixlib main makefile
# -----------------------------------------------------------------------------
# (c) Andreas Kloeckner 2000. See COPYING for license.
# -----------------------------------------------------------------------------




localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
INCLUDES = -I..



# Source files ----------------------------------------------------------------
basic_headers=ixlib_base.hh ixlib_array.hh ixlib_garbage.hh \
  ixlib_exbase.hh ixlib_exgen.hh ixlib_exio.hh \
  ixlib_string.hh ixlib_re.hh ixlib_re_impl.hh \
  ixlib_numconv.hh ixlib_numeric.hh \
  ixlib_random.hh ixlib_ring_queue.hh

basic_objects=base.cc exbase.cc exgen.cc exio.cc \
  string.cc numconv.cc numeric.cc re.cc

advanced_headers=ixlib_cmdline.hh \
  ixlib_geometry.hh ixlib_geometry_impl.hh \
  ixlib_polygon.hh ixlib_polygon_impl.hh \
  ixlib_matrix.hh ixlib_matrix_impl.hh \
  ixlib_textfile.hh ixlib_scanner.hh ixlib_token_lex.hh \
  ixlib_xml.hh \
  ixlib_javascript.hh \
  ixlib_drawing_functions.hh 

advanced_objects=cmdline.cc textfile.cc scanner.cc xml.cc \
  js_value.cc js_expression.cc js_declaration.cc \
  js_instruction.cc js_array.cc js_library.cc \
  js_interpreter.cc

internal_headers=ixlib_token_xml.hh ixlib_scanxml.hh ixlib_scanjs.hh \
  ixlib_js_internals.hh ixlib_token_javascript.hh \
  ixlib_i18n.hh 

internal_objects=lex.xml.cc lex.javascript.cc

# Generated sources -----------------------------------------------------------
lex.xml.cc: lex.xml.yy
	$(LEX) $< 

lex.javascript.cc: lex.javascript.yy
	$(LEX) $< 

BUILT_SOURCES=lex.xml.cc lex.javascript.cc 

CLEANFILES=lex.xml.cc lex.javascript.cc

EXTRA_DIST=lex.xml.yy lex.javascript.yy flex-gcc3-patch.ed

# Targets ---------------------------------------------------------------------
lib_LTLIBRARIES=libixion.la

libixion_la_SOURCES=$(basic_headers) $(basic_objects) \
   $(advanced_headers) $(advanced_objects) \
   $(internal_headers) $(internal_objects)
libixion_la_LIBADD=@LEXLIB@ $(INTLLIBS)
libixion_la_LDFLAGS=-version-info @IXLIB_BINARY_IFACE_VERSION@:@IXLIB_BINARY_REVISION@:@IXLIB_BINARY_AGE@ \
  -no-undefined

include_HEADERS=$(basic_headers) $(advanced_headers)
noinst_HEADERS=$(internal_headers)
