guidir = $(datadir)/$(PACKAGE)/gui

AM_CPPFLAGS = $(AUTOZGUI_CFLAGS) \
              -DGUIDIR=\""$(guidir)"\" \
              -DG_LOG_DOMAIN=\"AutozGui\"

LIBS = $(AUTOZGUI_LIBS) \
       -export-dynamic

lib_LTLIBRARIES = libautozgui.la

libautozgui_la_SOURCES = \
                ../src/authorization.c \
                ../src/authorizations.c \
                ../src/resource.c \
                ../src/resources.c \
                ../src/role.c \
                ../src/roles.c

libautozgui_la_LDFLAGS = -no-undefined

libautozgui_include_HEADERS = \
                ../src/authorization.h \
                ../src/authorizations.h \
                ../src/commons.h \
                ../src/resource.h \
                ../src/resource.h \
                ../src/role.h \
                ../src/roles.h

libautozgui_includedir = $(includedir)/libautozgui
