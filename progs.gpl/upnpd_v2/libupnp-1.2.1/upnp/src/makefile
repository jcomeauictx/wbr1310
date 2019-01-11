###########################################################################
##
## Copyright (c) 2000-2003 Intel Corporation 
## All rights reserved. 
##
## Redistribution and use in source and binary forms, with or without 
## modification, are permitted provided that the following conditions are met: 
##
## * Redistributions of source code must retain the above copyright notice, 
## this list of conditions and the following disclaimer. 
## * Redistributions in binary form must reproduce the above copyright notice, 
## this list of conditions and the following disclaimer in the documentation 
## and/or other materials provided with the distribution. 
## * Neither name of Intel Corporation nor the names of its contributors 
## may be used to endorse or promote products derived from this software 
## without specific prior written permission.
## 
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
## ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
## NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
###########################################################################

-include ../../../../config.arch
-include ../../../../../config.path

SUBDIRS = ssdp soap genlib gena urlconfig api uuid
INCLUDES = -I inc -I ../inc -I inc/tools
OBJECT_DIR = lib

OBJECTS = $(OBJECT_DIR)/api.o $(OBJECT_DIR)/ssdp.o $(OBJECT_DIR)/soap.o $(OBJECT_DIR)/uuid.o\
	$(OBJECT_DIR)/miniserverall.o $(OBJECT_DIR)/service_table.o \
	$(OBJECT_DIR)/uriall.o $(OBJECT_DIR)/client_table.o $(OBJECT_DIR)/utilall.o \
	$(OBJECT_DIR)/netall.o $(OBJECT_DIR)/httpall.o $(OBJECT_DIR)/urlconfigall.o \
	$(OBJECT_DIR)/gena.o \
	../../threadutil/src/obj/LinkedList.o \
	../../threadutil/src/obj/FreeList.o

all: objdir
	@set -e; for i in $(SUBDIRS); do make -C $$i ; done
	$(AR) r ../bin/libupnp.a $(OBJECTS)
	$(LD) -r -o libupnp.o $(OBJECTS)

objdir:
	@mkdir -p $(OBJECT_DIR)
	@mkdir -p ../bin

clean:
	@set -e; for i in $(SUBDIRS); do make -C $$i clean; done
	rm -rf $(OBJECT_DIR)
	rm -f ../bin/libupnp.a
	rm -f libupnp.o

.PHONY: clean all
