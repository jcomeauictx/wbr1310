#
# Makefile for progs.board (wrgg15)
#
-include config.arch
-include config.path

all:
clean:

install: uclibc
	make -C wireless install
	make -C ctest install
	cp ar231x_gpio.o $(TARGET)/lib/modules/.
	cp reboot $(TARGET)/usr/sbin/.
	$(STRIP) fresetd
	cp fresetd $(TARGET)/usr/sbin/.
	cp mfc $(TARGET)/usr/sbin/.

uclibc:
	@echo -e "\033[32mInstalling uClibc ...\033[0m"
	[ -d $(TARGET)/lib ] || mkdir -p $(TARGET)/lib
	cp -f $(CC_PATH)/lib/ld-uClibc.so.0     $(TARGET)/lib/.
	cp -f $(CC_PATH)/lib/libc.so.0          $(TARGET)/lib/.
	[ ! -f $(TARGET)/lib/libc.so ] || rm -f $(TARGET)/lib/libc.so
	ln -s libc.so.0                         $(TARGET)/lib/libc.so
	cp -f $(CC_PATH)/lib/libcrypt.so.0      $(TARGET)/lib/.
	[ ! -f $(TARGET)/lib/libcrypt.so ] || rm -f $(TARGET)/lib/libcrypt.so
	ln -s libcrypt.so.0                     $(TARGET)/lib/libcrypt.so
	cp -f $(CC_PATH)/lib/libdl.so.0         $(TARGET)/lib/.
	[ ! -f $(TARGET)/lib/libdl.so ] || rm -f $(TARGET)/lib/libdl.so
	ln -s libdl.so.0                        $(TARGET)/lib/libdl.so
	cp -f $(CC_PATH)/lib/libm.so.0          $(TARGET)/lib/.
	[ ! -f $(TARGET)/lib/libm.so ] || rm -f $(TARGET)/lib/libm.so
	ln -s libm.so.0                         $(TARGET)/lib/libm.so
	cp -f $(CC_PATH)/lib/libnsl.so.0	$(TARGET)/lib/.
	[ ! -f $(TARGET)/lib/libnsl.so ] || rm -f $(TARGET)/lib/libnsl.so
	ln -s libnsl.so.0                       $(TARGET)/lib/libnsl.so
	cp -f $(CC_PATH)/lib/libresolv.so.0     $(TARGET)/lib/.
	[ ! -f $(TARGET)/lib/libresolv.so ] || rm -f $(TARGET)/lib/libresolv.so
	ln -s libresolv.so.0                    $(TARGET)/lib/libresolv.so

.PHONY: all install clean uclibc
