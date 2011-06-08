guidir = $(datadir)/$(PACKAGE)/gui

AM_CPPFLAGS = $(AUDITGUI_CFLAGS) \
              -DGUIDIR=\""$(guidir)"\"

LIBS = $(AUDITGUI_LIBS) \
       -export-dynamic

bin_PROGRAMS = autoz-gui

autoz_gui_SOURCES = \
                commons.h \
                main.c \
                openauditds.c \
                openauditds.h
