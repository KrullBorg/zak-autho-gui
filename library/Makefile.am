guidir = $(datadir)/$(PACKAGE)/gui

AM_CPPFLAGS = $(AUTOZGUI_CFLAGS) \
              -DGUIDIR=\""$(guidir)"\" \
              -DG_LOG_DOMAIN=\"ZakAuthoGui\"

LIBS = $(AUTOZGUI_LIBS) \
       -export-dynamic

lib_LTLIBRARIES = libzakauthogui.la

libzakauthogui_la_SOURCES = \
                ../src/authorization.c \
                ../src/authorizations.c \
                ../src/resource.c \
                ../src/resources.c \
                ../src/role.c \
                ../src/roles.c

libzakauthogui_la_LDFLAGS = -no-undefined

libzakauthogui_include_HEADERS = \
                ../src/authorization.h \
                ../src/authorizations.h \
                ../src/commons.h \
                ../src/resource.h \
                ../src/resources.h \
                ../src/role.h \
                ../src/roles.h

libzakauthogui_includedir = $(includedir)/libzakauthogui
