/* vi: set sw=4 ts=4: */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "md5.h"
#include "mathopd.h"
#include "imghdr.h"
#include "asyslog.h"

#define DEBUG_MSG

#ifdef DEBUG_MSG
#include <stdarg.h>
#define DEBUG_MSG_FILE		"/var/dbgmsg.txt"
static void _dbgprintf(const char * format, ...)
{
	va_list marker;
	FILE * fd;

	fd = fopen(DEBUG_MSG_FILE, "a+");
	if (fd)
	{
		va_start(marker, format);
		vfprintf(fd, format, marker);
		va_end(marker);
		fclose(fd);
	}
}
static void _dump_nbytes(const unsigned char * data, int size)
{
	int i;
	FILE * fd;

	fd = fopen(DEBUG_MSG_FILE, "a+");
	if (fd)
	{
		for (i=0; i<size; i++) fprintf(fd, "%02X ", data[i]);
		fprintf(fd, "\n");
		fclose(fd);
	}
}

#define DPRINTF(x, args...)	_dbgprintf(x, ##args)
#define DBYTES(data, size)	_dump_nbytes(data, size)
#else
#define DPRINTF(x, args...)
#define DBYTES(data, size)
#endif

extern char *g_signature;

upload upload_file={0, 0, 0, 0, 0, -1, 0};
void rlt_page(struct pool *p, char *pFName)
{
	char buf[512];
	FILE *pFile;
	size_t n;
	int r;

	if ((pFile=fopen(pFName, "r")))
	{
		while ((r=fread(buf, 1, 512, pFile)))
		{
			if (p->ceiling <= p->end) break;

			n = p->ceiling - p->end;
			if(r>n)
			{
				memcpy(p->end, buf ,n);
				p->end+=n;
				break;
			}
			memcpy(p->end, buf, r);
			p->end+=r;
		}
		fclose(pFile);
	}
}

static unsigned long _cpu_to_le(unsigned long value)
{
	static int swap = -1;
	static unsigned long patt = 0x01020304;
	static unsigned char * p = (unsigned char *)&patt;
	
	if (swap == -1)
	{
		if (p[0] == 0x04 && p[1] == 0x03 && p[2] == 0x02 && p[3] == 0x01) swap=0;
		else swap = 1;
	}
	if (swap)
	{
		return (((value & 0x000000ff) << 24) |
				((value & 0x0000ff00) << 8) |
				((value & 0x00ff0000) >> 8) |
				((value & 0xff000000) >> 24));
	}
	return value;
}

static int v2_check_image_block(const char * image, int size)
{
	int offset = sizeof(imghdr2_t);
	imgblock_t * block = NULL;
	MD5_CONTEXT ctx;
	unsigned char digest[16];

	while (offset < size)
	{
		block = (imgblock_t *)&image[offset];

		DPRINTF("Image header (0x%08x):\n", offset);
		DPRINTF("  magic  : 0x%08x\n", block->magic);
		DPRINTF("  size   : %d (0x%x)\n", block->size, block->size);
		DPRINTF("  offset : 0x%08x\n", block->offset);
		DPRINTF("  devname: \'%s\'\n", block->devname);
		DPRINTF("  digest : "); DBYTES(block->digest, 16);
		
		if (block->magic != _cpu_to_le(IMG_V2_MAGIC_NO))
		{
			DPRINTF("Wrong Magic in header !\n");
			break;
		}
		if (offset + sizeof(imgblock_t) + block->size > size)
		{
			DPRINTF("Size out of boundary !\n");
			break;
		}

		/* check MD5 digest */
		MD5Init(&ctx);
		MD5Update(&ctx, (unsigned char *)&block->offset, sizeof(block->offset));
		MD5Update(&ctx, (unsigned char *)block->devname, sizeof(block->devname));
		MD5Update(&ctx, (unsigned char *)&block[1], block->size);
		MD5Final(digest, &ctx);

		if (memcmp(digest, block->digest, 16)!=0)
		{
			DPRINTF("MD5 digest mismatch !\n");
			DPRINTF("digest caculated : "); DBYTES(digest, 16);
			DPRINTF("digest in header : "); DBYTES(block->digest, 16);
			break;
		}


		/* move to next block */
		offset += sizeof(imgblock_t);
		offset += block->size;

		DPRINTF("Advance to next block : offset=%d(0x%x), size=%d(0x%x)\n", offset, offset, size, size);
	}
	if (offset == size) return 0;


	DPRINTF("illegal block found at offset %d (0x%x)!\n", offset, offset);
	DPRINTF("  offset (%d) : \n", offset); DBYTES((unsigned char *)block, 16);

	return -1;
}

static int v2_image_check(const char * image, int size)
{
	imghdr2_t * v2hdr = (imghdr2_t *)image;
	unsigned char signature[MAX_SIGNATURE];
	int i;
	if (v2hdr && size > sizeof(imghdr2_t) && v2hdr->magic == _cpu_to_le(IMG_V2_MAGIC_NO))
	{
		/* check if the signature match */
		DPRINTF("check image signature !\n");

		memset(signature, 0, sizeof(signature));
		strncpy(signature, g_signature, sizeof(signature));

		DPRINTF("  expected signature : [%s]\n", signature);
		DPRINTF("  image signature    : [%s]\n", v2hdr->signature);

		if (strncmp(signature, v2hdr->signature, MAX_SIGNATURE)==0)
			return v2_check_image_block(image, size);
		/* check if the signature is {boardtype}_aLpHa (ex: wrgg02_aLpHa, wrgg03_aLpHa */
		for (i=0; signature[i]!='_' && i<MAX_SIGNATURE; i++);
		if (signature[i] == '_')
		{
			signature[i+1] = 'a';
			signature[i+2] = 'L';
			signature[i+3] = 'p';
			signature[i+4] = 'H';
			signature[i+5] = 'a';
			signature[i+6] = '\0';

			DPRINTF("  try this signature : [%s]\n", signature);

			if (strcmp(signature, v2hdr->signature) == 0)
				return v2_check_image_block(image, size);
		}
	}
	return -1;
}

static int v2_burn_image(const char * image, int size)
{
	int offset = sizeof(imghdr2_t);
	imgblock_t * block;
	FILE * fh;

	DPRINTF("v2_burn_image >>>>\n");
	
	while (offset < size)
	{
		block = (imgblock_t *)&image[offset];

		DPRINTF("burning image block.\n");
		DPRINTF("  size    : %d (0x%x)\n", (unsigned int)block->size, (unsigned int)block->size);
		DPRINTF("  devname : %s\n", block->devname);
		DPRINTF("  offset  : %d (0x%x)\n", (unsigned int)block->offset, (unsigned int)block->offset);

#if 1
		fh = fopen(block->devname, "w+");
		if (fh == NULL)
		{
			DPRINTF("Failed to open device %s\n", block->devname);
			return -1;
		}
		fseek(fh, block->offset, SEEK_SET);
		fwrite((const void *)&block[1], 1, block->size, fh);
		fsync(fh);
		fclose(fh);
#endif

		DPRINTF("burning done!\n");
		
		/* move to next block */
		offset += sizeof(imgblock_t);
		offset += block->size;
	}
	upload_file.flag = 0;
	
	return 0;
}

int upload_image(struct request *r, struct pool *p)
{
	DPRINTF("formUpload\n");

	/* check v2 image first. */
	if (v2_image_check(upload_file.file_ptr, upload_file.file_length)==0)
	{
		int		len=upload_file.file_length/1024;
		char	cmd[256];

		DPRINTF("V2 image checked, sending message!\n");
		DPRINTF("Prepare burning V2 image!\n");
		sprintf(cmd, "rgdb -i -s /runtime/sys/fw_size '%d'", len);
		system(cmd);

        rlt_page(p, r->c->info_fwrestart_file);
		upload_file.flag=1;
		upload_file.time=time(0);
		return 0;
	}
	rlt_page(p, r->c->error_fwup_file);

	DPRINTF("formUpload done!\n");	

	return -1;
}

int upload_config(struct request *r, struct pool *p)
{
	char *pInput=upload_file.file_ptr;
	FILE *pFile;
	int  filelen=0;
	if (!strncasecmp(pInput, g_signature, strlen(g_signature)))
	{
		filelen=strlen(g_signature)+1;
		pInput+=filelen;
		filelen=upload_file.file_length-filelen;
		if((pFile=fopen("/var/config.bin","w")))
		{
			rlt_page(p, r->c->info_cfgrestart_file);
			fwrite(pInput, 1, filelen, pFile);
			fclose(pFile);
			system("/etc/scripts/misc/profile.sh reset /var/config.bin");
			upload_file.flag=2;
			upload_file.time=time(0);
			return 0;
		}
	}
	rlt_page(p, r->c->error_cfgup_file);
	return -1;
}

void check_upgrad()
{
	if (time(0) - upload_file.time<4) return;

	system("/etc/scripts/misc/haltdemand.sh");

	switch (upload_file.flag)
	{
	case 1:	//-----Upload Image
		v2_burn_image(upload_file.file_ptr, upload_file.file_length);
		break;
	case 2:	//-----Upload Config
		system("/etc/scripts/misc/profile.sh put");
		break;
	}
	system("reboot");
}
