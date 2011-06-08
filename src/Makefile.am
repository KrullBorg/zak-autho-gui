guidir = $(datadir)/$(PACKAGE)/gui

AM_CPPFLAGS = $(AUTOZGUI_CFLAGS) \
              -DGUIDIR=\""$(guidir)"\"

LIBS = $(AUTOZGUI_LIBS) \
       -export-dynamic

bin_PROGRAMS = autoz-gui

autoz_gui_SOURCES = \
                commons.h \
                main.c \
                openauditds.c \
                openauditds.h \
                role.c \
                role.h \
                roles.c \
                roles.h
