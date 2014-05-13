
#include "log.h"

#ifdef __STDC__

void WriteLog(const int port, const int sn, const int mode, const char * asFormat, ...)

#else
void WriteLog(const int port, const int sn, const int mode, const char * asFormat, va_alist)
#endif
{
    va_list  args;

    FILE *fp;
    char tmpbuf[81920];
    char timebuf[30], timebuf1[30];
    char logfilename[64];


    va_start(args, asFormat);
    vsprintf(tmpbuf, asFormat, args);
    va_end(args);

    get_cur_time("YYYYMMDD", timebuf);
    get_cur_time("HH24:MI:SS", timebuf1);

    //判断是打印到标准输出还是记入日志
    if ((mode & OUT_SCREEN) == OUT_SCREEN)
    {
        printf("%s\n", tmpbuf);
    }
    if ((mode & OUT_ULOG) == OUT_ULOG)
    {
        //sprintf(logfilename, "./log/%s_%05d_%02d.log", timebuf, port, sn);
        //modify by renqw 2007.09.11
        sprintf(logfilename, "./log/%s_%05d_%02d.log", timebuf, port, sn + 1);

        if ((fp = fopen(logfilename, "a")) == NULL)
        {
            //userlog(tmpbuf);
            //userlog("打开[%s]失败,错误原因：%d:%s\n", logfilename, errno, strerror(errno));
            return;
        }
        fprintf(fp, "%s %s\n", timebuf1, tmpbuf);
        fclose(fp);
    }
}

