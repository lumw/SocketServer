#ifndef _LOG_H_
#define _LOG_H_


/**/
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/*
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
*/
#define MAX_BUF_SIZE 4096

#define OUT_ULOG	 0x08
#define OUT_SCREEN	 0x10

#ifndef __STDC__
#define __STDC__
#endif
	
#ifdef __STDC__

	#include <stdarg.h>
	void WriteLog(const int port, const int sn, const int mode, const char * asFormat, ...);

#else
	#include <varargs.h>
	void WriteLog(const int port, const int sn, const int mode, const char * asFormat, va_alist);
#endif


#endif
