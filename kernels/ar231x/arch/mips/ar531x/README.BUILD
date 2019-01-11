       How to BUILD a linux kernel for an AR531X system

It is expected that you will build Linux on an existing Linux system, which 
has all of the standard Linux tools.

01) Obtain a MIPS BigEndian ELF gcc-compatible toolchain.  For example,
    if you're cross-compiling on a x86 Linux system, you could use:
    ftp://ftp.mips.com/pub/tools/software/sde-for-linux/sdelinux-5.01-4eb.i386.rpm

02) Obtain the latest working MIPS Linux kernel
    cvs -d :pserver:cvs@ftp.linux-mips.org:/home/cvs login    (password "cvs")
    cvs -d :pserver:cvs@ftp.linux-mips.org:/home/cvs co -r linux_2_4 linux

    Now "cd linux".  The remainder of these instructions assume
    that you are in the linux directory.

03) Place the contents of this directory at arch/mips/ar531x.

04) Use the patch command to patch generic linux files according
    to the DIFFS directory
    for i in arch/mips/ar531x/DIFFS/*.diff
    do
       patch -p1 < $i
    done
    NOTE: This version of the AR531X Linux BSP was tested with
    MIPS Linux 2.4.22 as of 11/14/03.  If you use a different
    (e.g. more recent) version of Linux source, you may need to
    resolve some minor patch and compilation issues.

05) Set up a RAMDISK image.
    See the instructions in README.RAMDISK.

06) Set up a linux configuration using ar531x/defconfig-ar531x.
    cp arch/mips/ar531x/defconfig-ar531x .config
    make oldconfig       (answer all questions that are asked)
    NOTE: For development/debug purposes, you may want to
    enable CONFIG_RUNTIME_DEBUG and CONFIG_KGDB.

07) Make dependencies.
    make dep

08) Build the linux kernel
    make

09) The linux image you just built is in vmlinux.
    See instructions in README.EXECUTE to run your vmlinux
    image on an AP531X-based board.
