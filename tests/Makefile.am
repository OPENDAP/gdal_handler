AUTOMAKE_OPTIONS = foreign

TESTSUITE = $(srcdir)/testsuite

EXTRA_DIST = $(TESTSUITE).at package.m4.in $(TESTSUITE) package.m4 \
atlocal.in template.bescmd.in bes.conf.in bes.conf.modules.in gdal

noinst_DATA = bes.conf
DISTCLEANFILES = bes.conf

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean

distclean-local:
	-rm atconfig
	-rm bes.conf
	
if DAP_MODULES
BES_CONF_IN = bes.conf.modules.in
else
BES_CONF_IN = bes.conf.in
endif

# Build the bes.conf used for testing so that the value substituted for
# @abs_top_srcdir@ does not contain '../'. This happens when using 
# configure's value for the parameter when running the distcheck target.
bes.conf: $(BES_CONF_IN) $(top_srcdir)/configure.ac
	@clean_abs_top_srcdir=`echo ${abs_top_srcdir} | sed 's/\(.*\)\/\(.[^\/]*\)\/\.\./\1/g'`; \
	sed -e "s%[@]abs_top_srcdir[@]%$$clean_abs_top_srcdir%" \
		-e "s%[@]abs_top_builddir[@]%${abs_top_builddir}%" $< > bes.conf

# Use the missing script for better error checking on autom4te run.
AUTOM4TE = $(SHELL) $(top_srcdir)/conf/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest

$(TESTSUITE): $(TESTSUITE).at $(srcdir)/package.m4
	$(AUTOTEST) -I '$(srcdir)' -o ${srcdir}/$@.tmp $@.at
	mv ${srcdir}/$@.tmp ${srcdir}/$@
