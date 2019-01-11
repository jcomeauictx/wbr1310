/* vi: set sw=4 ts=4: */
///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2003 Intel Corporation 
// All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 
//
// * Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
// * Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution. 
// * Neither name of Intel Corporation nor the names of its contributors 
// may be used to endorse or promote products derived from this software 
// without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include "upnp.h"
#include <stdarg.h>

#ifdef DEBUG
#include <string.h>
#endif


//File handle for the error log file
static FILE *ErrFileHnd = NULL;

//File handle for the information log file
static FILE *InfoFileHnd = NULL;

//Name of the error file
static const char *errFileName = NULL;

//Name of the info file
static const char *infoFileName = NULL;

/************************************************************************
* Function : SetLogFileNames											
*																	
* Parameters:														
*	IN const char* ErrFileName: name of the error file
*	IN const char *InfoFileName: name of the information file
*	IN int size: Size of the buffer
*	IN int starLength: This parameter provides the width of the banner
*																	
* Description:														
*	This functions takes the buffer and writes the buffer in the file as 
*	per the requested banner											
* Returns: void
***************************************************************************/
void SetLogFileNames(const char *ErrFileName, const char *InfoFileName)
{
	errFileName = ErrFileName;
	infoFileName = InfoFileName;
}

/************************************************************************
* Function : InitLog											
*																	
* Parameters:	void													
*																	
* Description:														
*	This functions initializes the log files
* Returns: int
*	-1 : If fails
*	UPNP_E_SUCCESS : if success
***************************************************************************/
int InitLog(void)
{
#if DEBUG_TARGET == 1
	{
		if (errFileName) ErrFileHnd = fopen(errFileName, "a");
		if (infoFileName) InfoFileHnd = fopen(infoFileName, "a");
	}
#endif
	return UPNP_E_SUCCESS;
}

/************************************************************************
* Function : CloseLog											
*																	
* Parameters:	void													
*																	
* Description:														
*	This functions closes the log files
* Returns: void
***************************************************************************/
void CloseLog(void)
{
	if (ErrFileHnd)
	{
		fflush(ErrFileHnd);
		fclose(ErrFileHnd);
	}
	if (InfoFileHnd)
	{
		fflush(InfoFileHnd);
		fclose(InfoFileHnd);
	}
}

#ifdef UPNP_DEBUG

void __dtrace(const char * format, ...)
{
	va_list marker;
	va_start(marker, format);
	vfprintf(stdout, format, marker);
	va_end(marker);
}

void __dassert(char * exp, char * file, int line)
{
	__dtrace("UPNP_ASSERT: file: %s, line: %d\n", file, line);
	__dtrace("\t%s\n", exp);
	abort();
}



static void UpnpDisplayBanner(FILE * fd, char **lines, int size, int starLength);
static void UpnpDisplayFileAndLine(FILE * fd, char *DbgFileName, int DbgLineNo);

void _upnp_printf(Dbg_Level DLevel, Dbg_Module Module, char *DbgFileName, int DbgLineNo, char *FmtStr, ...)
{
	va_list ArgList;

	if (DEBUG_LEVEL < DLevel) return;

	va_start(ArgList, FmtStr);

#if DEBUG_ALL == 0
	if	(1
#if DEBUG_SSDP
			&& (Module != SSDP)
#endif
#if DEBUG_SOAP
			&& (Module != SOAP)
#endif
#if DEBUG_GENA
			&& (Module != GENA)
#endif
#if DEBUG_TPOOL
			&& (Module != TPOOL)
#endif
#if DEBUG_MSERV
			&& (Module != MSERV)
#endif
#if DEBUG_DOM
			&& (Module != DOM)
#endif
#if DEBUG_HTTP
			&& (Module != HTTP)
#endif
#if DEBUG_API
			&& (Module != API)
#endif
		 ) return;
#endif

#if DEBUG_TARGET == 0
	UpnpDisplayFileAndLine(stdout, DbgFileName, DbgLineNo);
	vfprintf(stdout, FmtStr, ArgList);
	fflush(stdout);
#else
	if (DLevel == 0)
	{
		if (ErrFileHnd)
		{
			UpnpDisplayFileAndLine(ErrFileHnd, DbgFileName, DbgLineNo);
			vfprintf(ErrFileHnd, FmtStr, ArgList);
			fflush(ErrFileHnd);
		}
	}
	else
	{
		if (InfoFileHnd)
		{
			UpnpDisplayFileAndLine(InfoFileHnd, DbgFileName, DbgLineNo);
			vfprintf(InfoFileHnd, FmtStr, ArgList);
			fflush(InfoFileHnd);
		}
	}
#endif
	va_end(ArgList); 
}



/**************************************************************************
* Function : UpnpDisplayFileAndLine											
*																	
* Parameters:														
*	IN FILE *fd: File descriptor where line number and file name will be 
*			written 
*	IN char *DbgFileName: Name of the file  
*	IN int DbgLineNo : Line number of the file
*																	
* Description:
*	This function writes the file name and file number from where
*		debug statement is coming to the log file
* Returns: void
***************************************************************************/
void UpnpDisplayFileAndLine(IN FILE * fd, IN char *DbgFileName, IN int DbgLineNo)
{
	int starlength = 66;
	char *lines[2];
	char FileAndLine[500]; lines[0] = "DEBUG";

	if (DbgFileName)
	{
		sprintf(FileAndLine, "FILE: %s, LINE: %d", DbgFileName, DbgLineNo);
		lines[1] = FileAndLine;
	}

	UpnpDisplayBanner( fd, lines, 2, starlength );
	fflush( fd );
}

/************************************************************************
* Function : UpnpDisplayBanner											
*																	
* Parameters:														
*	IN FILE *fd: file descriptor where the banner will be written
*	IN char **lines: The buffer that will be written
*	IN int size: Size of the buffer
*	IN int starLength: This parameter provides the width of the banner
*																	
* Description:														
*	This functions takes the buffer and writes the buffer in the file as 
*	per the requested banner											
* Returns: void
***************************************************************************/
void UpnpDisplayBanner(IN FILE * fd, IN char **lines, IN int size, IN int starLength)
{
	char *stars = ( char * )malloc( starLength + 1 );
	char *line = NULL;
	int leftMarginLength = starLength / 2 + 1;
	int rightMarginLength = starLength / 2 + 1;
	char *leftMargin = ( char * )malloc( leftMarginLength );
	char *rightMargin = ( char * )malloc( rightMarginLength );
	int i = 0;
	int LineSize = 0;
	char *currentLine = ( char * )malloc( starLength + 1 );

	memset( stars, '*', starLength );
	stars[starLength] = 0;
	memset( leftMargin, 0, leftMarginLength );
	memset( rightMargin, 0, rightMarginLength );
	fprintf( fd, "\n%s\n", stars );
	for( i = 0; i < size; i++ )
	{
		LineSize = strlen( lines[i] );
		line = lines[i];
		while( LineSize > ( starLength - 2 ) )
		{
			memcpy( currentLine, line, ( starLength - 2 ) );
			currentLine[( starLength - 2 )] = 0;
			fprintf( fd, "*%s*\n", currentLine );
			LineSize -= ( starLength - 2 ); line += ( starLength - 2 );
		}

		if( LineSize % 2 == 0 )
		{
			leftMarginLength = rightMarginLength =
				( ( starLength - 2 ) - LineSize ) / 2;}
		else
		{
			leftMarginLength = ( ( starLength - 2 ) - LineSize ) / 2;
			rightMarginLength =
				( ( starLength - 2 ) - LineSize ) / 2 + 1;
		}

		memset( leftMargin, ' ', leftMarginLength );
		memset( rightMargin, ' ', rightMarginLength );
		leftMargin[leftMarginLength] = 0;
		rightMargin[rightMarginLength] = 0;
		fprintf( fd, "*%s%s%s*\n", leftMargin, line, rightMargin );
	}

	fprintf( fd, "%s\n\n", stars );
	free( leftMargin );
	free( rightMargin ); free( stars ); free( currentLine );
}
#endif
