bin_PROGRAMS=source-browser
noinst_LTLIBRARIES=
check_LTLIBRARIES=
check_PROGRAMS=test-async-io
TESTS=

## FIXME: make the schemas translatable
schemas_DATA=source-browser.schemas
schemasdir=$(GCONF_SCHEMA_FILE_DIR)

install-data-local:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schemas_DATA) ; do \
			GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/data/$$p; \
		done \
	fi

GFC_CFLAGS=$(AM_CPPFLAGS)
GFC_LIBS=$(LDADD)
gfc_src_dir=$(top_srcdir)/gfc
include gfc/Makefile.inc

BUILT_SOURCES=\
	sb-marshallers.c \
	sb-marshallers.h \
	$(NULL)
dist_ige_mac_menu_sources=\
	ige-mac-menu.c \
	ige-mac-menu.h \
	$(NULL)

source_browser_SOURCES=\
	$(BUILT_SOURCES) \
	gobject-helpers.h \
	sb-annotations.c \
	sb-annotations.h \
	sb-comparable.c \
	sb-comparable.h \
	sb-contributor.c \
	sb-contributor.h \
	sb-display.c \
	sb-display.h \
	sb-main.c \
	sb-progress.c \
	sb-progress.h \
	sb-reference.c \
	sb-reference.h \
	sb-reference-label.c \
	sb-reference-label.h \
	sb-revision.c \
	sb-revision.h \
	sb-settings.c \
	sb-settings.h \
	sb-statusbar.c \
	sb-statusbar.h \
	sb-window.c \
	sb-window.h \
	$(NULL)
source_browser_LDADD=\
	libgfc.la \
	$(LDADD)
AM_CPPFLAGS=\
	-I$(top_srcdir)/gfc \
	$(SB_CFLAGS) \
	$(PLATFORM_CFLAGS) \
	$(NULL)
LDADD=$(SB_LIBS) $(PLATFORM_LDFLAGS)

if HAVE_PLATFORM_OSX
source_browser_SOURCES+=$(dist_ige_mac_menu_sources)
endif

sb-marshallers.c: sb-marshallers.list Makefile
	glib-genmarshal --prefix=sb_cclosure_marshal --body $< > $@

sb-marshallers.h: sb-marshallers.list Makefile
	glib-genmarshal --prefix=sb_cclosure_marshal --header $< > $@

