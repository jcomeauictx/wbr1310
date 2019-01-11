#
# drivers/net/wireless/Makefile
#
# Makefile for the Linux Wireless network device drivers.
#

O_TARGET := wireless_net.o

obj-y		:=
obj-m		:=
obj-n		:=
obj-		:=

# Things that need to export symbols
export-objs	:= airo.o orinoco.o hermes.o


#subdir-$(CONFIG_ATHEROS_HAL)    += _ath_hal
#subdir-$(CONFIG_ATHEROS)        += ath
#subdir-$(CONFIG_RATE_ATHEROS)   += ratectrl
#subdir-$(CONFIG_RATE_ONOE)      += onoe

obj-$(CONFIG_HERMES)		+= orinoco.o hermes.o
obj-$(CONFIG_PCMCIA_HERMES)	+= orinoco_cs.o
obj-$(CONFIG_APPLE_AIRPORT)	+= airport.o
obj-$(CONFIG_PLX_HERMES)	+= orinoco_plx.o
obj-$(CONFIG_TMD_HERMES)	+= orinoco_tmd.o
obj-$(CONFIG_PCI_HERMES)	+= orinoco_pci.o

obj-$(CONFIG_AIRO)		+= airo.o
obj-$(CONFIG_AIRO_CS)		+= airo_cs.o airo.o

ifeq ($(CONFIG_ATHEROS),y)
#        obj-y += _ath_hal/ath_hal.o
#        obj-y += ath/ath_ahb.o
ifeq ($(CONFIG_RATE_ATHEROS),y)
#        obj-y += ratectrl/ath_rate_atheros.o
endif
ifeq ($(CONFIG_RATE_ONOE),y)
#        obj-y += onoe/ath_rate_onoe.o
endif
ifeq ($(CONFIG_ATHEROS_RADAR),y)
#	subdir-y	+= radar
#	obj-y += radar/ath_radar.o
else
#	subdir-y	+= ath_phyerr
#	obj-y += ath_phyerr/ath_phyerr.o
endif
endif


include $(TOPDIR)/Rules.make
