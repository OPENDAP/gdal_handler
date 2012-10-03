
EXTRA_DIST = 

SUBDIRS = gdal-1.9.1

# This Makefile.am is 'glue' so that a 'make' in this directory will
# build al of the third party code. Note that the code is configured
# in/by the top-level configure script and not AC_CONFIG_SUBDIRS
# because gdal has autotools file that seem to break more recent
# versions of the autotools macros. jhrg 10/2/12

# distclean is supposed to remove everything built by make or configure
distclean-local:
	cd gdal-1.9.1 && rm -rf build

# This version of all-recurrsive runs 'all install' (two targets) for
# all of the subdirs. It then runs all for the CWD unless there's a
# dot in SUBDIRS. See the second 'local_target' line for the forced
# 'install'

all:
	@failcom='exit 1';\
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	    ($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$target \
		&& $(MAKE) $(AM_MAKEFLAGS) install) || eval $$failcom; \
	done; test -z "$$fail"

check-recursive:
	true

distdir: 
	true
