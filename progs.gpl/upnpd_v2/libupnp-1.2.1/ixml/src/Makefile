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

TARGET_OUTPUT = libixml.a

ifeq ($(INSURE), 1)
  CC = insure
endif

OBJ_DIR = obj
CFLAGS += -Wall -O2 -pipe -I. -I./inc -I../inc
DEBUG_FLAGS = -DNDEBUG
STRIPU      = $(STRIP) $(OBJ_DIR)/$(TARGET_OUTPUT)

IXML_DIR = /home/share/ryle/$(TARGET)

OBJ = $(OBJ_DIR)/ixml.o       $(OBJ_DIR)/node.o $(OBJ_DIR)/ixmlparser.o \
      $(OBJ_DIR)/ixmlmembuf.o $(OBJ_DIR)/nodeList.o \
      $(OBJ_DIR)/element.o    $(OBJ_DIR)/attr.o $(OBJ_DIR)/document.o \
      $(OBJ_DIR)/namedNodeMap.o

VERSION=1.2.1

all: $(OBJ_DIR)/$(TARGET_OUTPUT)

$(OBJ_DIR)/%.o:	%.c
	@-mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/$(TARGET_OUTPUT): $(OBJ)
	$(AR) r  $(OBJ_DIR)/$(TARGET_OUTPUT) $(OBJ) 
	#$(STRIPU)

clean:
	@rm -rf $(OBJ_DIR)

.PHONY: clean
