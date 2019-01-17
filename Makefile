#
# Main Makefile for rgapp3
#
# Created by David Hsieh
# david_hsieh@alphanetworks.com
#

CONFIG			:= config/config
CONFIG_CONFIG_IN	:= config/Config.in
nonconfig_targets	:= menuconfig config oldconfig config_clean

.EXPORT_ALL_VARIABLES:

TOPDIR := $(shell pwd)
Q?=@

# Pull in the user's configuration file
ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)
-include .config
endif

#########################################################################

ifeq ($(strip $(RGAPPS_HAVE_DOT_CONFIG)),y)
-include config/Makefile.in

# Board name & kernel
BOARD		:= $(RGAPPS_BOARD_NAME)
KERNEL		:= $(RGAPPS_KERNEL_DIR)

# IMAGE_SIGNATURE
ifeq ($(strip $(RGAPPS_BOARD_WRGG15D)),y)
IMAGE_SIGNATURE := wrgg15_di524
else
ifdef RGAPPS_MODEL_NAME
IMAGE_SIGNATURE	:= $(shell echo $(BOARD)_$(BRAND)_$(RGAPPS_MODEL_NAME))
else
IMAGE_SIGNATURE := $(shell echo $(BOARD)_$(BRAND))
endif
endif

# Directory path
TARGET		:= $(TOPDIR)/userland/target
ENV_BOARD	:= $(TOPDIR)/boards/$(BOARD)
ENV_KERNEL	:= $(TOPDIR)/kernels/$(KERNEL)
ENV_HTML	:= $(TOPDIR)/html/$(BRAND)

CONFIG_ARCH	:= $(ENV_BOARD)/config.arch
CONFIG_MK	:= $(ENV_BOARD)/config.mk
BB_HEADER	:= $(ENV_BOARD)/busybox_config.h
ifdef RGAPPS_MODEL_NAME
GENDEF		:= $(ENV_HTML)/$(RGAPPS_MODEL_NAME)/gendef.sh
else
GENDEF		:= $(ENV_HTML)/gendef.sh
endif
endif
TIMESTAMP	?= $(shell date +%Y%m%d%H%M%S)

#########################################################################

ifeq (config.path, $(wildcard config.path))
-include config.path
endif

#########################################################################

ifeq ($(strip $(RGAPPS_HAVE_DOT_CONFIG)),y)

ifeq (config.mk, $(wildcard config.mk))
CLEAR_TO_GO:=y
#all: kernel progs progs_install tftpimage release
all: progs progs_install release
	@echo "Done !!!"


-include config.mk

else	# config.mk
ifeq (boards, $(wildcard boards))
# environment not set up yet
all: env_setup
kernel: env_setup
release: env_setup
tftpimage: env_setup
else	# board
# code not checkout 
all: checkout
kernel: checkout
release: checkout
tftpimage: checkout
endif	# board
endif	# config.mk

-include .config.cmd
showconfig:
	@echo ""
	@echo Current Configuration for RGAPPS:
	@echo ---------------------------------
	@echo BOARD is $(BOARD)
	@echo BRAND is $(BRAND)
ifdef RGAPPS_MODEL_NAME
	@echo MODEL is $(RGAPPS_MODEL_NAME)
else
	@echo MODEL is not defined.
endif
	@echo IMAGE_SIGNATURE is $(IMAGE_SIGNATURE)
	@echo KERNEL is $(KERNEL)
	@echo PROGS_GPL_SUBDIRS = $(PROGS_GPL_SUBDIRS)
	@echo PROGS_PRIV_SUBDIRS = $(PROGS_PRIV_SUBDIRS)
	@echo ---------------------------------

else
# no .config found
all: menuconfig
kernel: menuconfig
release: menuconfig
tftpimage: menuconfig
showconfig:
	@echo -e "\033[31mNot configured yet !!!\033[0m"
endif

.PHONY: all kernel release tftpimage showconfig

#########################################################################
ENV_PATH = config.path

ifeq ($(strip $(RGAPPS_HAVE_DOT_CONFIG)),y)

ifdef TPATH_UC
env_setup: env_clean env_path
	@echo -e "\033[32mSetup environment for $(RGAPPS_BOARD_NAME)_$(BRAND) ...\033[0m"
	$(Q)if [ ! -d $(ENV_BOARD) ];    then echo -e "\033[31mDirectory $(ENV_BOARD) does not exist!\033[0m"; exit 9; fi
	$(Q)if [ ! -d $(ENV_KERNEL) ];   then echo -e "\033[31mDirectory $(ENV_KERNEL) does not exist!\033[0m"; exit 9; fi
	$(Q)if [ ! -d $(ENV_HTML) ];     then echo -e "\033[31mDirectory $(ENV_HTML) does not exist!\033[0m"; exit 9; fi
	$(Q)if [ ! -f $(CONFIG_ARCH) ];  then echo -e "\033[31mFile $(CONFIG_ARCH) does not exist!\033[0m"; exit 9; fi
	$(Q)if [ ! -f $(CONFIG_BRAND) ]; then echo -e "\033[31mFile $(CONFIG_BRAND) does not exist!\033[0m"; exit 9; fi
	$(Q)if [ ! -f $(CONFIG_MK) ];    then echo -e "\033[31mFile $(CONFIG_MK) does not exist!\033[0m"; exit 9; fi
	$(Q)if [ ! -f $(GENDEF) ];       then echo -e "\033[31mFile $(GENDEF) does not exist!\033[0m"; exit 9; fi
	$(Q)#
	$(Q)ln -s $(ENV_KERNEL)			kernel
	$(Q)ln -s $(TOPDIR)/$(ENV_PATH)		kernel/$(ENV_PATH)
	$(Q)ln -s $(CONFIG_MK)			config.mk
	$(Q)ln -s $(GENDEF)			gendef.sh
	$(Q)ln -s $(ENV_BOARD)			userland/board
	$(Q)ln -s $(ENV_HTML)			userland/html
	$(Q)ln -s $(CONFIG_ARCH)		userland/config.arch
	$(Q)ln -s $(CONFIG_ARCH)		progs.gpl/config.arch
	$(Q)ln -s $(CONFIG_ARCH)		progs.priv/config.arch
	$(Q)ln -s $(ENV_BOARD)/progs.board	progs.board
	$(Q)ln -s $(CONFIG_ARCH)		progs.board/config.arch
	$(Q)ln -s $(TOPDIR)/$(ENV_PATH)		progs.board/$(ENV_PATH)
	$(Q)make -C userland bb_setup

env_path:
	$(Q)echo -e "\033[32mGenerate path file for Makefile.\033[0m"
	$(Q)echo "# This file is generated automatically when setup environment" > $(ENV_PATH)
	$(Q)echo "# This environment is setup for $(IMAGE_SIGNATURE) ." >> $(ENV_PATH)
	$(Q)echo "TOPDIR=$(TOPDIR)" >> $(ENV_PATH)
	$(Q)echo "KERNELDIR=$(TOPDIR)/kernel" >> $(ENV_PATH)
	$(Q)echo "TARGET=$(TARGET)" >> $(ENV_PATH)
else
env_setup env_path:
	@echo -e "\033[32mSetup environment failed !!!!"
	@echo ""
	@echo " Can NOT find toolchain path definitions !!!"
	@echo " Please export environment variables TPATH_UC and TPATH_KC."
	@echo "  TPATH_UC is the path of the toolchain that build your user mode programs."
	@echo "  TPATH_KC is the path of the toolchain that build your linux kernel."
	@echo ""
	@echo -e "\033[0m"
endif

else

env_setup: menuconfig
env_path: menuconfig

endif

env_clean:
	@echo -e "\033[32mCleanup the environment ...\033[0m"
	$(Q)rm -f $(ENV_PATH) kernel/$(ENV_PATH) kernel config.mk gendef.sh \
		userland/board userland/busybox/Config.h userland/html userland/config.arch \
		progs.gpl/config.arch progs.priv/config.arch \
		progs.board/config.arch progs.board/$(ENV_PATH) progs.board\
		gendef.sh config.mk

.PHONY: env_setup env_path env_clean tftpimage release

#########################################################################

ifeq ($(CLEAR_TO_GO),y)
user:
	@echo -e "\033[32mBuild user land image ...\033[0m"
	$(Q)make -C userland
	$(Q)make -C tools/alpha/rgdb
	$(Q)make -C tools/alpha/xmldb
	$(Q)./tools/alpha/xmldb/xmldb -n $(IMAGE_SIGNATURE) -s ./rgapps_xmldb_gendef &
	$(Q)sleep 1
	$(Q)./gendef.sh
	$(Q)./tools/alpha/rgdb/rgdb -S ./rgapps_xmldb_gendef -D ./rgdb.xml
	$(Q)cp rgdb.xml defaultvalue.xml
	$(Q)gzip rgdb.xml
	$(Q)mv ./rgdb.xml.gz userland/target/etc/config/rgdb.conf.default
	$(Q)killall xmldb

user_clean:
	@echo -e "\033[32mClean up userland image ...\033[0m"
	$(Q)make -C userland clean

progs:
	@echo -e "\033[32mBuild programs ...\033[0m"
	$(Q)make -C progs.gpl
	$(Q)make -C progs.priv
	$(Q)make -C progs.board

progs_install: user
	@echo -e "\033[32mInstall programs ...\033[0m"
	$(Q)make -C progs.gpl install
	$(Q)make -C progs.priv install
	$(Q)make -C progs.board install

progs_clean:
	@echo -e "\033[32mClean programs ...\033[0m"
	$(Q)make -C progs.gpl clean
	$(Q)make -C progs.priv clean
	$(Q)make -C progs.board clean

else
user: env_setup
user_clean: env_setup
progs: env_setup
progs_install: env_setup
progs_clean: env_setup
endif

clean_CVS:
	@echo -e "\033[32mRemove CVS/SVN from target ...\033[0m"
	$(Q)find $(TARGET) -name CVS -type d | xargs rm -rf
	$(Q)find $(TARGET) -name .svn -type d | xargs rm -rf

.PHONY: user user_clean progs progs_install progs_clean clean_CVS

#########################################################################

SVNFLAG:=
ifdef REV
SVNFLAG+=-r$(REV)
endif

ifeq ($(strip $(RGAPPS_HAVE_DOT_CONFIG)),y)
checkout update:
	@echo -e "\033[33mCHECKOUT rgapp3/Rules.mk ...\033[0m"
	$(Q)svn update $(SVNFLAG) Rules.mk
	@echo -e "\033[33mCHECKOUT rgapp3/config ...\033[0m"
	$(Q)svn update $(SVNFLAG) config
	@echo -e "\033[33mCHECKOUT rgapp3/include ...\033[0m"
	$(Q)svn update $(SVNFLAG) include
	$(Q)if [ -f .tmpconfig.h ]; then mv .tmpconfig.h include/config.h; fi
	@echo -e "\033[33mCHECKOUT rgapps/comlib ...\033[0m"
	$(Q)svn update $(SVNFLAG) comlib
	@echo -e "\033[33mCHECKOUT rgapp3/tools ...\033[0m"
	$(Q)svn update $(SVNFLAG) tools
	@echo -e "\033[33mCHECKOUT rgapp3/userland ...\033[0m"
	$(Q)svn update $(SVNFLAG) userland
	@echo -e "\033[33mCHECKOUT progs.gpl ...\033[0m"
	$(Q)svn update -N $(SVNFLAG) progs.gpl
	@echo -e "\033[33mCHECKOUT progs.priv ...\033[0m"
	$(Q)svn update -N $(SVNFLAG) progs.priv
	@echo -e "\033[33mCHECKOUT progs.priv/runtime ...\033[0m"
	$(Q)svn update $(SVNFLAG) progs.priv/runtime
	@echo -e "\033[33mCHECKOUT kernels ...\033[0m"
	$(Q)svn update -N $(SVNFLAG) kernels
	@echo -e "\033[33mCHECKOUT boards ...\033[0m"
	$(Q)svn update -N $(SVNFLAG) boards
	@echo -e "\033[33mCHECKOUT html ...\033[0m"
	$(Q)svn update -N $(SVNFLAG) html
	$(Q)for i in $(PROGS_GPL_SUBDIRS); do \
		echo -e "\033[33mCHECKOUT progs.gpl/$$i ...\033[0m" ; \
		svn update $(SVNFLAG) progs.gpl/$$i || exit $? ; \
	done
	$(Q)for i in $(PROGS_PRIV_SUBDIRS); do \
		echo -e "\033[33mCHECKOUT progs.priv/$$i ...\033[0m" ; \
		svn update $(SVNFLAG) progs.priv/$$i || exit $? ; \
	done
	@echo -e "\033[33mCHECKOUT rgapp3/kernels/$(KERNEL) ...\033[0m"
	$(Q)svn update $(SVNFLAG) kernels/$(KERNEL)
	@echo -e "\033[33mCHECKOUT rgapp3/boards/$(RGAPPS_BOARD_NAME) ...\033[0m"
	$(Q)svn update $(SVNFLAG) boards/$(RGAPPS_BOARD_NAME)
	@echo -e "\033[33mCHECKOUT rgapp3/html/$(BRAND) ...\033[0m"
	$(Q)svn update $(SVNFLAG) html/$(BRAND)

diff:
	@echo -e "\033[33mDIFF rgapp3/Makefile ...\033[0m"
	$(Q)svn diff $(SVNFLAG) Makefile
	@echo -e "\033[33mDIFF rgapp3/config ...\033[0m"
	$(Q)svn diff $(SVNFLAG) config
	@echo -e "\033[33mDIFF rgapp3/include ...\033[0m"
	$(Q)svn diff $(SVNFLAG) include
	$(Q)if [ -f .tmpconfig.h ]; then mv .tmpconfig.h include/config.h; fi
	@echo -e "\033[33mDIFF rgapp3/tools ...\033[0m"
	$(Q)svn diff $(SVNFLAG) tools
	@echo -e "\033[33mDIFF rgapp3/userland ...\033[0m"
	$(Q)svn diff $(SVNFLAG) userland
	@echo -e "\033[33mDIFF progs.priv/runtime ...\033[0m"
	$(Q)svn diff $(SVNFLAG) progs.priv/runtime
	$(Q)for i in $(PROGS_GPL_SUBDIRS); do \
		echo -e "\033[33mDIFF progs.gpl/$$i ...\033[0m" ; \
		svn diff $(SVNFLAG) progs.gpl/$$i || exit $? ; \
	done
	$(Q)for i in $(PROGS_PRIV_SUBDIRS); do \
		echo -e "\033[33mDIFF progs.priv/$$i ...\033[0m" ; \
		svn diff $(SVNFLAG) progs.priv/$$i || exit $? ; \
	done
	@echo -e "\033[33mDIFF rgapp3/kernels/$(KERNEL) ...\033[0m"
	$(Q)svn diff $(SVNFLAG) kernels/$(KERNEL)
	@echo -e "\033[33mDIFF rgapp3/boards/$(RGAPPS_BOARD_NAME) ...\033[0m"
	$(Q)svn diff $(SVNFLAG) boards/$(RGAPPS_BOARD_NAME)
	@echo -e "\033[33mDIFF rgapp3/html/$(BRAND) ...\033[0m"
	$(Q)svn diff $(SVNFLAG) html/$(BRAND)

else
checkout: menuconfig
update: menuconfig
diff: menuconfig
endif

.PHONY: checkout update

#########################################################################

ifeq (config, $(wildcard config))

conf:
	$(Q)make -C $(CONFIG)

menuconfig: conf 
	$(Q)$(CONFIG)/mconf $(CONFIG_CONFIG_IN)

config: conf
	$(Q)$(CONFIG)/conf $(CONFIG_CONFIG_IN)

oldconfig: conf
	$(Q)$(CONFIG)/conf -o $(CONFIG_CONFIG_IN)

config_clean:
	$(Q)rm -f .config .config.cmd .config.old .defconfig include/config.h
	$(Q)rm -rf include/config.h
	$(Q)make -C $(CONFIG) clean

else

conf menuconfig config oldconfig config_clean:
	$(Q)svn update config

endif


clean: user_clean progs_clean

distclean: clean env_clean config_clean cvslogout

env:
	env
.PHONY: conf menuconfig config oldconfig config_clean clean distclean
