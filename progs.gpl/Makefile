
.EXPORT_ALL_VARIABLES:

-include config.arch

all:
	@echo -e "\033[32mStart building progs.gpl ...\033[0m"
	@for i in $(PROGS_GPL_SUBDIRS); do make -C $$i || exit $? ; done

install:
	@echo -e "\033[32mInstalling progs.gpl ...\033[0m"
	@for i in $(PROGS_GPL_SUBDIRS); do make -C $$i install || exit $? ; done

clean:
	@echo -e "\033[32mStart cleaning progs.gpl ...\033[0m"
	@for i in $(PROGS_GPL_SUBDIRS); do make -C $$i clean || exit $? ; done

.PHONY: all install clean




