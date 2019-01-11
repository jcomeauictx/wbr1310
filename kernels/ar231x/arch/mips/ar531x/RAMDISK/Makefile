KERNEL_SOURCE=../../../..

# The value for INITRDSIZE is extracted from linux/.config,
# if it exists; otherwise, a default value is used.

CONFIG_FILE = $(KERNEL_SOURCE)/.config

ifeq ($(CONFIG_FILE),$(wildcard $(CONFIG_FILE)))

include $(CONFIG_FILE)
ifdef CONFIG_BLK_DEV_RAM_SIZE
INITRDSIZE  := $(shell echo $(CONFIG_BLK_DEV_RAM_SIZE))
else
INITRDSIZE := 8192
endif

else
INITRDSIZE := 8192
endif

MOUNTPT = /mnt/xtmp

ramdisk.gz: ramdisk
	gzip -f ramdisk

ramdisk:
	#./makelinks  
	@echo "CREATING RAMDISK OF SIZE $(INITRDSIZE) on $@"
	dd if=/dev/zero of=$@ bs=1k count=$(INITRDSIZE)
	/sbin/mke2fs -vFm0 $@ $(INITRDSIZE)
	if [ \! -e $(MOUNTPT) ]; then mkdir -p $(MOUNTPT) ; fi
	mount -o loop $@ $(MOUNTPT)
	@df $(MOUNTPT)
	(cd rootdir; tar cf - . ) | (cd $(MOUNTPT) && tar xf - )
	(cd $(MOUNTPT) ; chown -R root.root . )
	@df $(MOUNTPT)
	umount $(MOUNTPT)

install:
	@(if [ -d $(KERNEL_SOURCE)/arch/mips/ramdisk ]; \
	then \
		if [ -f ramdisk.gz ]; \
		then \
			cp ramdisk.gz $(KERNEL_SOURCE)/arch/mips/ramdisk/; \
		else \
			echo "No ramdisk.gz image"; \
		fi; \
	else \
		echo "No ramdisk directory.  Check KERNEL_SOURCE variable."; \
	fi)

clean:	
	rm -f ramdisk.gz ramdisk
