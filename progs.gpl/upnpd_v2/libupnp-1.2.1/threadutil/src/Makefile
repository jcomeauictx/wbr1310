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

ifeq ($(STATS),1)
DEBUG_FLAGS += -DSTATS
endif

ifeq ($(INSURE),1)
CC = insure -pthread
else
STRIPU = $(STRIP) $(TARGET_OUTPUT)
endif

DEBUG_FLAGS += -Os -DNO_DEBUG -DNDEBUG
TARGET_OUTPUT = libthreadutil.so
OBJ_DIR = obj

CFLAGS += -fPIC -Wall -D_REENTRANT -I../inc

libthreadutil.so_objects = $(OBJ_DIR)/FreeList.o $(OBJ_DIR)/LinkedList.o 


$(OBJ_DIR)/%.o : 	%.c
	@-mkdir -p $(OBJ_DIR)
	#$(CC) $(CFLAGS) -MMD $(DEBUG_FLAGS) -c $< -o $@
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

all: $(TARGET_OUTPUT)

libthreadutil.so: $(libthreadutil.so_objects)

clean:
	@rm -rf $(OBJ_DIR)

