
-include ../config.arch
-include ../../config.path
.EXPORT_ALL_VARIABLES:

all:
	make -C libupnp-1.2.1
	cp libupnp-1.2.1/upnp/bin/libupnp.a linux-igd/.
	cp libupnp-1.2.1/ixml/src/obj/libixml.a linux-igd/.
	make -C linux-igd

install:
	@echo -e "\033[32mInstalling upnpd_v2 ...\033[0m"
	make -C linux-igd install

clean:
	make clean -C libupnp-1.2.1
	make clean -C linux-igd
	rm -f linux-igd/*.a
	rm -f *.elf *.gdb
	
.PHONY: all clean
