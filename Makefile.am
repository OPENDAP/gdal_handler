
# Automake file for the gdal_handler
#
# 1/11/12 jhrg
#

AUTOMAKE_OPTIONS = foreign check-news 

ACLOCAL_AMFLAGS = -I conf

lib_besdir=$(libdir)/bes
lib_bes_LTLIBRARIES = libgdal_handler.la

# No tests yet
# SUBDIRS = . tests

GDAL_SRCS = GDALArray.cc GDALGrid.cc GDALTypes.cc gdal_das.cc

GDAL_HDRS = GDALTypes.h

libgdal_handler_la_SOURCES = $(GDAL_SRCS) $(GDAL_HDRS)
libgdal_handler_la_CPPFLAGS = $(GDAL_CFLAGS) $(BES_CPPFLAGS) $(DAP_CFLAGS) 
libgdal_handler_la_LDFLAGS = -avoid-version -module $(GDAL_LDFLAGS)
libgdal_handler_la_LIBADD = $(BES_DAP_LIBS) $(DAP_LIBS)

EXTRA_PROGRAMS = 

EXTRA_DIST = COPYING gdal.conf.in gdal_handler.spec data OSX_Resources

CLEANFILES = *~ gdal.conf

# Sample data primaries for install
sample_datadir = 		$(datadir)/hyrax/data/gdal
sample_data_DATA =

install-data-local: gdal.conf
	@currdate=`date +"%y%m%d%H%M"`; \
	test -d $(DESTDIR)$(sysconfdir)/bes || $(MKDIR_P) $(DESTDIR)$(sysconfdir)/bes; \
	test -d $(DESTDIR)$(sysconfdir)/bes/modules || $(MKDIR_P) $(DESTDIR)$(sysconfdir)/bes/modules; \
	test -f $(DESTDIR)$(sysconfdir)/bes/modules/gdal.conf && mv -f $(DESTDIR)$(sysconfdir)/bes/modules/gdal.conf $(DESTDIR)$(sysconfdir)/bes/modules/ncml.conf.$$currdate; \
	$(INSTALL_DATA) gdal.conf $(DESTDIR)$(sysconfdir)/bes/modules/gdal.conf

uninstall-local:
	rm -f $(DESTDIR)$(sysconfdir)/bes/modules/gdal.conf

gdal.conf: gdal.conf.in config.status
	sed -e "s%[@]bes_modules_dir[@]%${lib_besdir}%" $< > gdal.conf

# Not nearly as clean as it could be, but this removes .svn directories
# in subdirs.
dist-hook:
	rm -rf `find $(distdir) -name .svn`

.PHONY: docs
docs:
	doxygen $(srcdir)/doxy.conf
	(cd docs && tar -czf html.tar.gz html)

C4_DIR=./cccc
.PHONY: cccc
cccc:	
	cccc --outdir=$(C4_DIR) $(libgdal_handler_la_SOURCES)

###########################################################################

# Fortify targets.

.PHONY: fortify
fortify:
	sourceanalyzer -b @PACKAGE@ $(MAKE)
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr

# Use this to clean the fortify project.
.PHONY: fortifyclean
fortifyclean:
	sourceanalyzer -b @PACKAGE@ -clean

###########################################################################

# Build linux RPMs

srpm: dist
	rpmbuild -ts --clean $(RPM_OPTIONS) @PACKAGE@-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb --clean $(RPM_OPTIONS) @PACKAGE@-@PACKAGE_VERSION@.tar.gz

###########################################################################

# Build OS/X Packages. The strange operations with configure and make
# are there so that the values built into dap-config will match the mac
# osx install dirs and not the temp directory used to build the packages

# PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker*

PACKAGEMAKER=/Developer/usr/bin/packagemaker
PKG_CONF_FLAGS=

clean-pkg: 
	-rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg

pkg-build: clean-pkg 
	./configure --prefix=$(prefix) --with-icu-prefix=$(GDAL_PATH) --disable-dependency-tracking $(PKG_CONF_FLAGS)
	make clean all
	DESTDIR=`pwd`/mac_osx make install

pkg-main: pkg-build
	./OSX_Resources/update_mac_package_contents.pl README
	cat ./OSX_Resources/InstallationCheck.proto | sed -e "s^_PREFIX_^$(prefix)^g" > foo
	mv foo ./OSX_Resources/InstallationCheck
	cat ./OSX_Resources/Info.plist.proto | \
	   sed -e "s^_PREFIX_^$(prefix)^g" \
	       -e "s^_FULL_VERSION_^@PACKAGE_VERSION@^g" \
	       -e "s^_MAJOR_VERSION_^@PACKAGE_MAJOR_VERSION@^g" \
	       -e "s^_MINOR_VERSION_^@PACKAGE_MINOR_VERSION@^g" \
	       > foo
	mv foo ./OSX_Resources/Info.plist
	${PACKAGEMAKER} --root mac_osx --id org.opendap.@PACKAGE@ \
		--title "@PACKAGE@ @PACKAGE_VERSION@" --version @PACKAGE_VERSION@ \
		--out @PACKAGE@-@PACKAGE_VERSION@.pkg --resources OSX_Resources

pkg-dmg: pkg-main
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@.dmg
	mkdir @PACKAGE@-@PACKAGE_VERSION@
	cp -r @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@
	cp README NEWS @PACKAGE@-@PACKAGE_VERSION@
	dropdmg -i --sanitize-for-servers --format zlib @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@

pkg: pkg-main



