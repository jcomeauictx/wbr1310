#
# Makefile for wlxmlpatch.
#

-include ../config.arch
-include ../../config.path
-include ../../.config

APPLET     := wlxmlpatch

all:
clean:

install:
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	$(STRIP) $(APPLET)
	cp -f $(APPLET) $(TARGET)/usr/sbin/.

.PHONY: all install clean
