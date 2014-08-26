# -----------------------------------------------------------------------------
# iXiON library root makefile
# -----------------------------------------------------------------------------
# (c) iXiONmedia 1999
# -----------------------------------------------------------------------------

SUBDIRS=intl src test po 

mrproper:
	find -name '*.bck' -exec rm '{}' \;

documentation:
	rm -R doc/* || true
	mkdir doc || true
	doxygen doxygen.cfg

dist-hook: documentation
	cp -R doc/html ixlib-doc-@VERSION@
	tar chozf ixlib-doc-@VERSION@.tar.gz ixlib-doc-@VERSION@ 
	rm -R ixlib-doc-@VERSION@

update-web-doc: documentation
	rsync -e ssh --verbose --progress -r doc/html/ inducer@shell.sourceforge.net:/home/groups/i/ix/ixlib/htdocs/doc

update-web-site: 
	rsync -e ssh --verbose --progress web/* inducer@shell.sourceforge.net:/home/groups/i/ix/ixlib/htdocs

update-web: update-web-doc update-web-site

include_HEADERS=ixlib_config.hh

bin_SCRIPTS=ixlib-config

m4datadir=$(datadir)/aclocal
m4data_DATA=ixlib.m4

EXTRA_DIST=ixlib.m4 ABOUT-NLS doxygen.cfg ixlib.doc ixlib.lsm.in
