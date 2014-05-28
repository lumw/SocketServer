#ifndef _LOG_H_INCLUDE_
#define _LOG_H_INCLUDE_


/**/
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF_SIZE 4096
#define OUT_ULOG     0x08
#define OUT_SCREEN   0x10

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
