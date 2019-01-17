#
#	Makefile - Build instruction for root file system
#

.EXPORT_ALL_VARIABLES:

-include config.arch
-include ../config.path
-include ../.config

Q ?= @
APPS := scripts board
TARGET_DIRS := bin dev etc home lib mnt proc usr var
BB_CONFIG_TEMPLATE := board/busybox-1.00.config
BB_PREFIX := $(shell sed -n 's/^PREFIX=\(.*\)/\1/p' $(BB_CONFIG_TEMPLATE))
BB_CROSS_COMPILER_PREFIX := $(shell sed -n 's/^CROSS_COMPILER_PREFIX=\(.*\)/\1/p' $(BB_CONFIG_TEMPLATE))
TIMESTAMP ?= $(shell date +%Y%m%d%H%M%S).userland

all:	app target

env:
	env

app: bb
	@echo -e "\033[32mBuilding application programs...\033[0m"
	$(Q)for i in $(APPS) ; do make -C $$i || exit $? ; done

clean: bb_clean
	$(Q)for i in $(APPS); do [ ! -d $$i ] || make -C $$i clean; done
	$(Q)if [ -d $(TARGET) ]; then rm -rf $(TARGET); fi
	$(Q)rm -f cramfs.img squashfs.img

clean_target:
	@echo -e "\033[32mEmpty target file system\033[0m"
	$(Q)[ -d $(TARGET) ] || mkdir -p $(TARGET)
	$(Q)rm -rf $(TARGET)/*

target: app clean_target bb_install
	@echo -e "\033[32mBuilding target file system\033[0m"
	$(Q)cd $(TARGET) && for i in $(TARGET_DIRS); do \
		[ -d $$i ] || mkdir -p $$i; \
	done
	$(Q)ln -s /var/tmp $(TARGET)/tmp
	$(Q)ln -s /var/TZ $(TARGET)/etc/TZ
	$(Q)for i in $(APPS) ; do make -C $$i install || exit $? ; done

	$(Q)cp -r html/www $(TARGET)/.
ifdef RGAPPS_MODEL_NAME
	$(Q)install html/$(RGAPPS_MODEL_NAME)/submit $(TARGET)/usr/sbin/.
	$(Q)if [ -f html/$(RGAPPS_MODEL_NAME)/brand.sh ]; then \
		install html/$(RGAPPS_MODEL_NAME)/brand.sh $(TARGET)/etc/scripts/. ; \
	fi
	$(Q)cp -r --remove-destination html/$(RGAPPS_MODEL_NAME)/www $(TARGET)/.
else
	$(Q)install html/submit $(TARGET)/usr/sbin/.
	$(Q)if [ -f html/brand.sh ]; then install html/brand.sh $(TARGET)/etc/scripts/. ; fi
endif
	$(Q)ln -s /var/log/messages $(TARGET)/www/syslog.rg
	$(Q)ln -s /var/log/tlogsmsg $(TARGET)/www/tsyslog.rg
	$(Q)make -C ../tools/buildimg
	$(Q)#./mkdevs.sh $(TARGET)
	$(Q)./mark.sh "$(TARGET)/etc/config/layout.conf.default" "`../tools/buildimg/genbn`" "$(RGAPPS_FIREWARE_VERSION)"
	$(Q)./signwebs.sh "$(TARGET)/etc/config/defines" "$(IMAGE_SIGNATURE)"

.PHONY: all app clean target

ifeq ($(strip $(RGAPPS_BUSYBOX_1_00)),y)
bb_setup:
	@echo -e "\033[32mSetup busybox v1.00 configuration ...\033[0m"
	$(Q)sed -e 's|@@CROSS_COMPILER@@|$(TPATH_UC)/bin/$(CROSS_COMPILE)|' board/busybox-1.00.config > tmpconfig
	$(Q)sed -e 's|@@TARGET@@|../target|' tmpconfig > busybox-1.00/.config

bb_updatedefaults: board/busybox-1.00.config busybox-1.00/.config
	# reverse of bb_setup, only do this after testing resulting image
	cp $< $<.$(TIMESTAMP)
	sed -e 's/^\(CROSS_COMPILER_PREFIX=\).*/\1$(BB_CROSS_COMPILER_PREFIX)/' -e 's/^\(PREFIX=\).*/\1$(BB_PREFIX)/' $(word 2, $+) > $<

bb:
	@echo -e "\033[32mBuilding busybox v1.00 ...\033[0m"
	$(Q)make -C busybox-1.00 oldconfig
	$(Q)make -C busybox-1.00

bb_install:
	@echo -e "\033[32mInstalling busybox v1.00 ...\033[0m"
	$(Q)[ -d $(TARGET) ] || mkdir -p $(TARGET)
	$(Q)make -C busybox-1.00 install

bb_clean:
	@echo -e "\033[32mCleaning busybox v1.00 ...\033[0m"
	$(Q)make -C busybox-1.00 clean
else
bb_setup:
	@echo -e "\033[32mSetup busybox v0.60.3 configuration ...\033[0m"
	$(Q)rm -f busybox/Config.h
	$(Q)cp board/busybox_config.h busybox/Config.h
bb:
	@echo -e "\033[32mBuilding busybox v0.60.3 ...\033[0m"
	$(Q)make -C busybox

bb_install:
	@echo -e "\033[32mInstalling busybox v0.60.3 ...\033[0m"
	$(Q)[ -d $(TARGET) ] || mkdir -p $(TARGET)
	$(Q)make -C busybox install

bb_clean:
	@echo -e "\033[32mCleaning busybox v0.60.3 ...\033[0m"
	$(Q)make -C busybox clean
	$(Q)rm busybox/Config.h
endif


.PHONY: bb bb_install
