#
# Makefile for the Alphanetworks Tag Parser
#  for the Linux OS
#

-include ../config.arch
-include ../../config.path

NAME:=atp

all:

clean:

install:
	install -c $(NAME) $(TARGET)/sbin/atp
	ln -fs  /sbin/atp $(TARGET)/sbin/xgi

.PHONY: all install clean
