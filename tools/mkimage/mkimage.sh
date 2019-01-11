#!/bin/sh
#-----------------------------------------------------------------------------------------------
# Jongil Park <laminaz@samsung.co.kr, laminaz.park@samsung.com>
#-----------------------------------------------------------------------------------------------
# Usage: mkimage -l image
#  mkimage -A arch -O os -T type -C comp -a addr -e ep -n name -d data_file[:data_file...] image
#-----------------------------------------------------------------------------------------------
#  -l ==> list image header information
#  -A ==> set architecture to 'arch'
#  -O ==> set operating system to 'os'
#  -T ==> set image type to 'type'
#  -C ==> set compression type 'comp'
#  -a ==> set load address to 'addr' (hex)
#  -e ==> set entry point to 'ep' (hex)
#  -n ==> set image name to 'name'
#  -d ==> use image data from 'datafile'
#-----------------------------------------------------------------------------------------------
#arch name = NULL,alpha,arm,x86,ia64,mips,mips64,ppc,s390,sh,sparc,sparc64	
#os name = NULL,openbsd,netbsd,freebsd,4_4bsd,linux,svr4,esix,solaris,irix,sco,dell,ncr,lynxos,vxworks,psos,qnx,ppcboot	
#comp name = gzip,none
#image type =standalone,kernel,	ramdisk, multi,	firmware, script

if [ $# -ne 4 ];  then
	echo "Usage: $0 load_addr(hex) entry_point(hex) in_image out_image gzip|none"
    exit 1
else
	echo "mkimage -A arm -O linux -T kernel -C gzip -a $1 -e $2 -n uClinux -d $3 $3.img"
	mkimage -A arm -O linux -T kernel -C $4 -a $1 -e $2 -n uClinux -d $3 $3.img
fi

