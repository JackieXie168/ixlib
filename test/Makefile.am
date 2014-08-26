# -----------------------------------------------------------------------------
# iXiON library test makefile
# -----------------------------------------------------------------------------
# (c) iXiONmedia 1999
# -----------------------------------------------------------------------------

LDADD=../src/libixion.la
CXXFLAGS=-I../src -g

check_PROGRAMS=teststring testnumconv testre testcmdline testtextfile \
  testscanner testgeo testxml testmat

teststring_SOURCES=teststring.cc
testnumconv_SOURCES=testnumconv.cc
testre_SOURCES=testre.cc
testcmdline_SOURCES=testcmdline.cc frobnitz.par
testtextfile_SOURCES=testtextfile.cc
testscanner_SOURCES=testscanner.cc
testgeo_SOURCES=testgeo.cc
testxml_SOURCES=testxml.cc test.xml
testmat_SOURCES=testmat.cc

check-local:
	./runem $(check_PROGRAMS)
