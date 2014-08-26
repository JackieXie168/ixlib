# -----------------------------------------------------------------------------
# iXiON library root makefile
# -----------------------------------------------------------------------------
# (c) iXiONmedia 1999
# -----------------------------------------------------------------------------

SUBDIRS=po intl src test 

mrproper:
	find -name '*.bck' -exec rm '{}' \;

include_HEADERS=ixlib_config.hh

bin_SCRIPTS=ixlib-config

m4datadir=$(datadir)/aclocal
m4data_DATA=ixlib.m4

EXTRA_DIST=ixlib.m4
