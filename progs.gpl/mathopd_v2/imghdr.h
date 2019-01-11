/* vi: set sw=4 ts=4: */
/* imghdr.h
 *
 *	This file defines the image header format for web upgrade.
 *	Currently the image format is used by WRGG02/WRGG03.
 *
 *	Copyright (C) 2003-2004, Alpha Networks, Inc.
 *
 *	2004/2/20 by David Hsieh
 *
 */

#ifndef _IMGHDR_HEADER_
#define _IMGHDR_HEADER_

#define MAX_SIGNATURE	32

/* Image header for WRGG02. */
typedef struct _imghdr
{
	char			signature[MAX_SIGNATURE];
	unsigned long	image_offset1;
	unsigned long	flash_offset1;
	unsigned long	size1;
	unsigned char	check1[16];
	unsigned long	image_offset2;
	unsigned long	flash_offset2;
	unsigned long	size2;
	unsigned char	check2[16];
}
imghdr;


/* Version 2 of image header. */
/*
 * Version 2 image will look like ...
 *
 * +--------------------------------------------+
 * | signature: 32 bytes                        |
 * +--------------------------------------------+
 * | image block 1 (imgblock_t)                 |
 * +--------------------------------------------+
 * | image 1                                    |
 * |                                            |
 * +--------------------------------------------+
 * | image block 2 (imgblock_t)                 |
 * +--------------------------------------------+
 * | image 2                                    |
 * |                                            |
 * +--------------------------------------------+
 */

#define IMG_MAX_DEVNAME		32
#define IMG_V2_MAGIC_NO		0x20040220	/* version 2 magic number */

typedef struct _imgblock imgblock_t;
struct _imgblock
{
	unsigned long	magic;		/* image magic number (should be IMG_V2_MAGIC_NO in little endian. */
	unsigned long	size;		/* size of the image. */
	unsigned long	offset;		/* offset from the beginning of the storage device. */
	char			devname[IMG_MAX_DEVNAME];	/* null termiated string of the storage device name. ex. "/dev/mtd6" */
	unsigned char	digest[16];	/* MD5 digest of the image */
} __attribute__ ((packed));

typedef struct _imghdr2 imghdr2_t;
struct _imghdr2
{
	char			signature[MAX_SIGNATURE];
	unsigned long	magic;	/* should be IMG_V2_MAGIC_NO in little endian. */
} __attribute__ ((packed));

#endif
