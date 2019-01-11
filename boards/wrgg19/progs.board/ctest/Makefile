
PATH_CONFIG = ../config.path
ARCH_CONFIG = ../config.arch

-include $(PATH_CONFIG)
-include $(ARCH_CONFIG)

EXEC = ctest

all:
clean:

install:
	cp -f $(EXEC) $(TARGET)/usr/sbin/.
	[ ! -f $(TARGET)/usr/sbin/psts ] || rm -f $(TARGET)/usr/sbin/psts
	ln -s ctest $(TARGET)/usr/sbin/psts
	[ ! -f $(TARGET)/usr/sbin/slinktype ] || rm -f $(TARGET)/usr/sbin/slinktype
	ln -s ctest $(TARGET)/usr/sbin/slinktype

.PHONY: all install clean
