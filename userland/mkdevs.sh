#!/bin/sh

# ram disk
mknod -m666 $1/dev/ram0 b 1 0
mknod -m666 $1/dev/ram1 b 1 1
mknod -m666 $1/dev/ram2 b 1 2
mknod -m666 $1/dev/ram3 b 1 3
mknod -m666 $1/dev/ram4 b 1 4
mknod -m666 $1/dev/ram5 b 1 5
mknod -m666 $1/dev/ram6 b 1 6

mknod -m666 $1/dev/null c 1 3

# pty (old style)
mknod -m666 $1/dev/ptyp0 c 2 0
mknod -m666 $1/dev/ptyp1 c 2 1
mknod -m666 $1/dev/ptyp2 c 2 2
mknod -m666 $1/dev/ptyp3 c 2 3
mknod -m666 $1/dev/ptyp4 c 2 4
mknod -m666 $1/dev/ptyp5 c 2 5
mknod -m666 $1/dev/ptyp6 c 2 6
mknod -m666 $1/dev/ttyp0 c 3 0
mknod -m666 $1/dev/ttyp1 c 3 1
mknod -m666 $1/dev/ttyp2 c 3 2
mknod -m666 $1/dev/ttyp3 c 3 3
mknod -m666 $1/dev/ttyp4 c 3 4
mknod -m666 $1/dev/ttyp5 c 3 5
mknod -m666 $1/dev/ttyp6 c 3 6

# UNIX 98 pty
mknod -m666 $1/dev/ptmx c 5 2
mkdir $1/dev/pts
mknod -m666 $1/dev/pts/0 c 136 0
mknod -m666 $1/dev/pts/1 c 136 1

# serial port
mknod -m666 $1/dev/ttyS0 c 4 64
mknod -m666 $1/dev/ttyS1 c 4 65
mknod -m666 $1/dev/console c 5 1

# mtd blocks
mknod -m666 $1/dev/mtd0 b 31 0
mknod -m666 $1/dev/mtd1 b 31 1
mknod -m666 $1/dev/mtd2 b 31 2
mknod -m666 $1/dev/mtd3 b 31 3
mknod -m666 $1/dev/mtd4 b 31 4
mknod -m666 $1/dev/mtd5 b 31 5
mknod -m666 $1/dev/mtd6 b 31 6

# ppp device
mknod -m666 $1/dev/ppp c 108 0

# (sentry5)
mknod -m666 $1/dev/robo c 252 0
mknod -m666 $1/dev/nvram c 253 0
