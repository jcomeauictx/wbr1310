-include ../config.arch
-include ../../config.path

BIN = mathopd
CFLAGS += -O -Wall -I../../include
CPPFLAGS = 
LIBS = -lcrypt

# On Solaris, uncomment the following
# CPPFLAGS = -DNEED_INET_ATON -DHAVE_CRYPT_H
# LIBS = -lsocket -lnsl

# On Linux, uncomment the following
 CPPFLAGS = -DHAVE_CRYPT_H

OBJS = base64.o cgi.o config.o core.o log.o main.o \
	   request.o util.o stub.o internal.o \
       upload.o md5c.o web-session.o

DEPENDS = mathopd.h Makefile

# //------------------------------
# Added by Paul Liu 20040326
# Add condition compile for UID/GID
# CPPFLAGS = -DUSE_ID

# Add condition compile for IMAP/REDIRECT/DUMP
# CPPFLAGS = -DIMAP
# OBJS += imap.o

# CPPFLAGS = -DREDIRECT
# OBJS += redirect.o

# CPPFLAGS = -DDUMP
# OBJS += dump.o
# //------------------------------

# Uncomment the following if your system does not support the poll() function
 CPPFLAGS += -DPOLL_EMULATION
 OBJS += poll-emul.o

# Uncomment the following if your system does not have the socklen_t type
# CPPFLAGS += -DNEED_SOCKLEN_T

# //------------------------------
# Modified by Paul Liu 20040326
# Add condition compile from config.arch for vfork() function
# Modified by Paul Liu 20060116
# We don't want vfork anymore...
#ifndef NO_VFORK
#CPPFLAGS += -DHAVE_VFORK
#endif

# Currently, sendfile support is available in two flavours: Linux and FreeBSD
# Uncomment one of the following two to enable sendfile() support
# CPPFLAGS += -DLINUX_SENDFILE
# CPPFLAGS += -DFREEBSD_SENDFILE
# If you define any of the SENDFILE conditionals, make sure to uncomment
# the next line as well.
# OBJS += sendfile.o

all: $(BIN)
install: $(BIN)
	install -c $(BIN) $(TARGET)/sbin/httpd
	@if [ ! -f $(TARGET)/etc/httpd.cfg ]; then echo "install -c httpd.cfg"; install -c httpd.cfg $(TARGET)/etc/; fi
uninstall:
	rm -f $(TARGET)/sbin/httpd
	rm -f $(TARGET)/etc/httpd.cfg
	rm -f $(TARGET)/var/httpd.cfg $(TARGET)/var/httpasswd
$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LIBS)
	[ -z "$(STRIP)" ] || $(STRIP) $(BIN)
$(OBJS): $(DEPENDS)
.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
clean:
	rm -f *.o *~ *.gdb $(BIN)
.PHONY: install clean
