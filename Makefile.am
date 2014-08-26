# -----------------------------------------------------------------------------
# iXiON library root makefile
# -----------------------------------------------------------------------------
# (c) iXiONmedia 1999
# -----------------------------------------------------------------------------

SUBDIRS=po intl src test 

mrproper:
	find -name '*.bck' -exec rm '{}' \;

documentation:
	rm -R doc
	mkdir doc
	doxygen doxygen.cfg

dist-hook: documentation
	tar cvfz ixlib-doc-@VERSION@.tar.gz -C doc/html .

update-web-doc: documentation
	rsync -e ssh --verbose --progress -r doc/html/ inducer@shell.sourceforge.net:/home/groups/ixlib/htdocs/doc

update-web-site: 
	rsync -e ssh --verbose --progress web/* inducer@shell.sourceforge.net:/home/groups/ixlib/htdocs

update-web: update-web-doc update-web-site

include_HEADERS=ixlib_config.hh

bin_SCRIPTS=ixlib-config

m4datadir=$(datadir)/aclocal
m4data_DATA=ixlib.m4

EXTRA_DIST=ixlib.m4 ABOUT-NLS doxygen.cfg ixlib.doc
