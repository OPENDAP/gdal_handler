AUTOMAKE_OPTIONS = foreign

TESTSUITE = $(srcdir)/testsuite

EXTRA_DIST = testsuite.at package.m4.in $(TESTSUITE) $(srcdir)/package.m4 \
        atlocal.in template.bescmd.in bes.conf.in bes.conf.modules.in gdal

noinst_DATA = bes.conf
DISTCLEANFILES = bes.conf

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || \
	$(SHELL) '$(TESTSUITE)' --clean

distclean-local:
	-rm atconfig
	-rm bes.conf


# Use the missing script for better error checking on autom4te run.
AUTOM4TE = $(SHELL) $(top_srcdir)/conf/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest

$(TESTSUITE): $(srcdir)/testsuite.at $(srcdir)/package.m4 bes.conf
	$(AUTOTEST) -I '$(srcdir)' -o ${srcdir}/$@.tmp $@.at
	mv ${srcdir}/$@.tmp ${srcdir}/$@
