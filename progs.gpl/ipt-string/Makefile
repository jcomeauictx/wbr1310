-include ../config.arch
-include ../../config.path
OBJS := ipt_string.o
CFLAGS = $(KCFLAGS) -I$(KERNELDIR)/include

# Common make rules
%.o:	%.c %.h
	$(KCC) $(CFLAGS) -c $<

all:    $(OBJS) 

install:
	@echo -e "\033[32mInstalling driver (blocking)...\033[0m"
	[ -d $(TARGET)/lib/modules ] || mkdir -p $(TARGET)/lib/modules
	cp -a ./ipt_string.o $(TARGET)/lib/modules/ipt_string.o
clean:
	rm -f $(OBJS)

