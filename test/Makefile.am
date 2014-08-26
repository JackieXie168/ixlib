# -----------------------------------------------------------------------------
# iXiON library test makefile
# -----------------------------------------------------------------------------
# (c) iXiONmedia 1999
# -----------------------------------------------------------------------------

LDADD=../src/libixion.la
CXXFLAGS=-I../src -g

check_PROGRAMS=teststring testnumconv testre testcmdline testtextfile \
  testgeo testxml testmat

EXTRA_DIST=test.xml src.txt frobnitz.par runem

noinst_SCRIPTS=runem

teststring_SOURCES=teststring.cc test.hh
testnumconv_SOURCES=testnumconv.cc test.hh
testre_SOURCES=testre.cc test.hh
testcmdline_SOURCES=testcmdline.cc test.hh
testtextfile_SOURCES=testtextfile.cc test.hh
# testscanner_SOURCES=testscanner.cc
testgeo_SOURCES=testgeo.cc test.hh
testxml_SOURCES=testxml.cc test.hh
testmat_SOURCES=testmat.cc test.hh

check-local:
	./runem $(check_PROGRAMS)
