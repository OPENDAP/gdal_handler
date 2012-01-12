AUTOMAKE_OPTIONS = foreign

# Generate the m4 defines from top level configure.ac
# The `:;' works around a Bash 3.2 bug when the output is not writeable.
$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	:;{ \
	echo '# Signature of the current package.' && \
	echo 'm4_define([AT_PACKAGE_NAME],' && \
	echo '  [@PACKAGE_NAME@])' && \
	echo 'm4_define([AT_PACKAGE_TARNAME],' && \
	echo '  [@PACKAGE_TARNAME@])' && \
	echo 'm4_define([AT_PACKAGE_VERSION],' && \
	echo '  [@PACKAGE_VERSION@])' && \
	echo 'm4_define([AT_PACKAGE_STRING],' && \
	echo '  [@PACKAGE_STRING@])' && \
	echo 'm4_define([AT_PACKAGE_BUGREPORT],' && \
	echo '  [@PACKAGE_BUGREPORT@])'; \
	} >'$(srcdir)/package.m4'

TESTSUITE = $(srcdir)/testsuite
TEST_FILES = 	aggregations.at \
		attribute_tests.at \
		parse_error_misc.at \
		variable_misc.at \
		variable_new_arrays.at \
		variable_new_multi_arrays.at \
		variable_new_scalars.at \
		variable_new_structures.at \
		variable_remove.at \
		variable_rename.at 

EXTRA_DIST = testsuite.at $(TEST_FILES) $(srcdir)/package.m4 $(TESTSUITE) atlocal.in \
	template.bescmd.in bes.conf.in baselines

noinst_DATA = bes.conf
DISTCLEANFILES = bes.conf

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

# Technically, we don't need this target now, but it might be useful
# to add it later to be complete.  It would only need to run
# besstandalone on the INSTALLED bes.conf file after it was processed
# with 'make bes-conf' to add the NCML module.  We'd have to figure
# out a way to let the testsuite script know whether to use the local
# bes.conf or the installed one.  
installcheck-local: atconfig atlocal
# $(TESTSUITE) $(SHELL) '$(TESTSUITE)' AUTOTEST_PATH='$(bindir)' \
# $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || \
	$(SHELL) '$(TESTSUITE)' --clean

distclean-local:
	-rm atconfig
	-rm bes.conf


# Use the missing script for better error checking on autom4te run.
AUTOM4TE = $(SHELL) $(top_srcdir)/conf/missing --run autom4te
#AUTOM4TE = autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest
$(TESTSUITE): $(srcdir)/testsuite.at $(srcdir)/package.m4 bes.conf $(TEST_FILES)
	$(AUTOTEST) -I '$(srcdir)' -o ${srcdir}/$@.tmp $@.at
	mv ${srcdir}/$@.tmp ${srcdir}/$@

# Resolve the ../ in the path so the BES doesn't complain about them
canonical_abs_top_srcdir =  `${READLINK_F} ${abs_top_srcdir}`

# this fails on distcheck...
#		-e "s%[@]libdir[@]%${libdir}%" 
bes.conf: bes.conf.in ../config.status
	sed -e "s%[@]pkgdatadir[@]%${pkgdatadir}%" \
		-e "s%[@]sysconfdir[@]%${sysconfdir}%" \
		-e "s%[@]beslibdir[@]%${BES_MODULE_DIR}%" \
		-e "s%[@]datadir[@]%${datadir}%" \
		-e "s%[@]abs_top_srcdir[@]%${canonical_abs_top_srcdir}%" \
		-e "s%[@]abs_top_builddir[@]%${abs_top_builddir}%" \
		-e "s%[@]bindir[@]%${bindir}%" $< > bes.conf

