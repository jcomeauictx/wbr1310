##
## Please check the configurion parameters below
##
## Uncomment this to force a particular version of wireless extensions.
## This would use private copy of Wireless Extension definition instead
## of the system wide one in /usr/include/linux. Use with care.
## Can be used to create multiple versions of the tools on the same system
## for multiple kernels or get around broken distributions.
# FORCE_WEXT_VERSION = 16

## Uncomment this to build without using libm (less efficient)
## This is mostly useful for embedded platforms
# BUILD_NOLIBM = y

ARCH_CONFIG=../config.arch
-include $(ARCH_CONFIG)
-include ../../config.path

# ***************************************************************************
# ***** Most users should not need to change anything beyond this point *****
# ***************************************************************************

# Targets to build
IWLIB=libiw.a
# merge iwpriv into iwconfig to save memory by siyou 2005.05.04.
#PROGS= iwconfig iwlist iwpriv iwspy iwgetid iwevent
PROGS= iwconfig iwlist iwspy iwgetid iwevent

MANPAGES8=iwconfig.8 iwlist.8 iwpriv.8 iwspy.8 iwgetid.8 iwevent.8
MANPAGES7=wireless.7
EXTRAPROGS= macaddr iwredir

# Composition of the library :
OBJS = iwlib.o

# Use local header if the version of wireless extensions is specified
FORCE_WEXT_VERSION=16
ifdef FORCE_WEXT_VERSION
  WEXT_FLAG = -DWEXT_HEADER=\"wireless.$(FORCE_WEXT_VERSION).h\"
endif

RM = rm -f


#ifdef KERNEL_SRC
#  ifeq ($(wildcard $(KERNEL_SRC)/include/linux/wireless.h),)
#    $(error Kernel source is missing or incomplete)
#  endif
#  KERNEL_INCLUDES = -I $(KERNEL_SRC)/include
#endif
KERNEL_INCLUDES = -I $(ROOTDIR)/$(LINUXDIR)/include
:
# Do we want to build with or without libm ?
BUILD_NOLIBM = 1
ifdef BUILD_NOLIBM
  LIBS=
  WELIB_FLAG = -DWE_NOLIBM=y
else
  LIBS= -lm
endif

#XCFLAGS=$(CFLAGS) $(LDFLAGS) $(WARN) $(HEADERS) $(WEXT_FLAG) $(WELIB_FLAG) $(KERNEL_INCLUDES)
XCFLAGS=$(CFLAGS) $(LDFLAGS) $(WARN) $(HEADERS) $(WEXT_FLAG) $(WELIB_FLAG)

all:: $(IWLIB) $(PROGS)

%: %.o
	$(CC) $(XCFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(XCFLAGS)  -c $<

iwconfig: iwconfig.o iwpriv.o $(IWLIB)

iwlist: iwlist.o $(IWLIB)
# merge iwpriv into iwconfig by siyou 2005.05.04.
# iwpriv: iwpriv.o $(IWLIB)

iwspy: iwspy.o $(IWLIB)

iwgetid: iwgetid.o

iwevent: iwevent.o $(IWLIB)

iwredir: iwredir.o

macaddr: macaddr.o $(IWLIB)

# Compilation of the dynamic library
$(DYNAMIC): $(OBJS:.o=.so)
	$(CC) -shared -o $@ -Wl,-soname,$@ $(LIBS) -lc $^

# Compilation of the static library
$(IWLIB): $(OBJS)
	$(RM) $@
	$(AR) cru $@ $^
	$(RANLIB) $@

# So crude but so effective ;-)
# Less crude thanks to many contributions ;-)
romfs:
	$(ROMFSINST) -e CONFIG_USER_WIRELESS_TOOLS_IWCONFIG /bin/iwconfig
	$(ROMFSINST) -e CONFIG_USER_WIRELESS_TOOLS_IWLIST   /bin/iwlist
	$(ROMFSINST) -e CONFIG_USER_WIRELESS_TOOLS_IWPRIV   /bin/iwpriv
	$(ROMFSINST) -e CONFIG_USER_WIRELESS_TOOLS_IWSPY    /bin/iwspy
	$(ROMFSINST) -e CONFIG_USER_WIRELESS_TOOLS_IWGETID  /bin/iwgetid
	$(ROMFSINST) -e CONFIG_USER_WIRELESS_TOOLS_IWEVENT  /bin/iwevent

clean::
	@rm -f *.gdb *.elf *.o *.a $(PROGS)

realclean::
	$(RM_CMD) 
	$(RM) $(STATIC) $(DYNAMIC) $(PROGS) $(EXTRAPROGS)

# Versioned install...
# We rename each of the tool with a suffix corresponding to the version
# of WE it was compiled with. We use "iwredir" for the redirection...

vinstall:: iwredir install
	@wev=$(FORCE_WEXT_VERSION) ; \
	if [ "$$wev" == "" ] ; then \
	  wev=`./iwredir -wev`; \
	else :; fi ; \
	echo "Installing tools for WE-$$wev..." ; \
	for f in $(PROGS); do \
	  mv $(INSTALL_DIR)/$$f $(INSTALL_DIR)/$$f$$wev; \
	done;
	install -m 755 iwredir $(INSTALL_DIR)
	for f in $(PROGS); do \
	  ln $(INSTALL_DIR)/iwredir $(INSTALL_DIR)/$$f; \
	done;

vuninstall:: uninstall
	@wev=$(FORCE_WEXT_VERSION) ; \
	if [ "$$wev" == "" ] ; then \
	  wev=`./iwredir -wev`; \
	else :; fi ; \
	for f in $(PROGS); do \
	  rm $(INSTALL_DIR)/$$f$$wev; \
	done;
	$(RM) $(INSTALL_DIR)/iwredir;

install:
	@echo -e "\033[32mInstalling wireless tools ...\033[0m"
	[ -d $(TARGET)/usr/sbin ] || mkdir -p $(TARGET)/usr/sbin
	install iwconfig	$(TARGET)/usr/sbin
	ln -s ./iwconfig $(TARGET)/usr/sbin/iwpriv

depend::
	makedepend -s "# DO NOT DELETE" -- $(INCLUDES) -- $(SRCS)
# DO NOT DELETE
