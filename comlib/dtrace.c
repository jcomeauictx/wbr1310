/* vi: set sw=4 ts=4: */
/*
 * dtrace.c
 *
 *	A debug / trace helper module.
 *
 * Created by david_hsieh@alphanetworks.com
 */

#ifdef DDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "dtrace.h"

/***********************************************************************/
static int ddbg_level = DBG_DEFAULT;
static FILE * out_fd = NULL;

/***********************************************************************/
void __dtrace(int level, const char * format, ...)
{
	va_list marker;
	if (ddbg_level <= level)
	{
		va_start(marker, format);
		vfprintf(out_fd ? out_fd : stdout, format, marker);
		va_end(marker);
	}
}

void __dassert(char * exp, char * file, int line)
{
	__dtrace(DBG_ALL, "DASSERT: file: %s, line: %d\n", file, line);
	__dtrace(DBG_ALL, "\t%s\n", exp);
	abort();
}

FILE * __set_output_file(const char * fname)
{
	if (out_fd) fclose(out_fd);
	out_fd = NULL;
	if (fname) out_fd = fopen(fname, "w");
	return out_fd;
}

int __set_dbg_level(int level)
{
	ddbg_level = level;
	return ddbg_level;
}

#ifdef TEST_DTRACE

int main(int argc, char * argv[])
{
	dtrace("Hello world !\n");
	dtrace("Hello: %d %s 0x%x\n", 12, "test", 12);
	return 0;
}

#endif

#endif	/* end of #ifdef DDEBUG */
