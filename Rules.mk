###############################################################
# General rules
#
#	$(APPLET)  - target file
#	$(LOCAL_SRCS) - local %.c source files
#	$(CMLIB_SRCS) - comlib %.c source files
#

-include $(TOPDIR)/.config

COMLIB:=$(TOPDIR)/comlib
COMINC:=$(TOPDIR)/include

LOCAL_OBJS:=$(patsubst %c,%o,$(LOCAL_SRCS))
CMLIB_OBJS:=$(patsubst %c,%o,$(CMLIB_SRCS))
COMLIBSRCS:=$(foreach T,$(CMLIB_SRCS),$(COMLIB)/$(T))

###############################################################
# Setup CFLAGS for comlib.
ifeq ($(strip $(RGAPPS_COMLIB_MEM_HELPER_DISABLE)),y)
CFLAGS += -DCONFIG_MEM_HELPER_DISABLE=y
endif

###############################################################
# make dependency file
ifeq (.depend, $(wildcard .depend))
all: $(APPLET)
-include .depend
else
all: .depend
	$(Q)make

.depend: Makefile $(LOCAL_SRCS) $(COMLIBSRCS)
	@echo -e "\033[32mCreating dependency file for APPLET: $(APPLET) ...\033[0m"
	$(Q)$(CC) -I$(COMINC) $(CFLAGS) -M $(LOCAL_SRCS) > .depend
	$(Q)$(CC) -I$(COMINC) $(CFLAGS) -M $(COMLIBSRCS) >> .depend
endif

dep:
	$(Q)rm -f .depend
	$(Q)make .depend

# build rule for APPLET.
$(APPLET): $(LOCAL_OBJS) $(CMLIB_OBJS)
	@echo -e "\033[32mbuilding APPLET: $(APPLET) ...\033[0m"
	$(Q)$(CC) $(LOCAL_OBJS) $(CMLIB_OBJS) $(LDFLAGS) -o $@

# build rule for source in comlib.
$(CMLIB_OBJS): %.o: $(COMLIB)/%.c
	$(Q)$(CC) -I$(COMINC) $(CFLAGS) -c -o $@ $<

# build rule for our sources.
$(LOCAL_OBJS): %.o: %.c
	$(Q)$(CC) -I$(COMINC) $(CFLAGS) -c -o $@ $<

clean_objs:
	@echo -e "\033[32mCleaning APPLET: $(APPLET) ...\033[0m"
	$(Q)rm -rf $(LOCAL_OBJS) $(CMLIB_OBJS) $(APPLET) .depend

.PHONY: generate_dep clean_objs

