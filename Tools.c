#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "Tools.h"


int if_program_running(const char *progName)
{
    char retBuff[256];
    char shellCommand[1024];


    sprintf(shellCommand, "ps -ef | grep -v \\\\-csh | grep -v \\\\.sh | grep -v ps | grep -v grep |grep -v vi | grep -c %s", progName);

    exec_shell_command(shellCommand, retBuff)

    if ( atoi(retBuff) >= 1)
        return TRUE;
    else
        return FALSE;

}


int exec_shell_command(const char *cmd, char *retbuff)
{
    char buf[1024];
    FILE *ptr;

    strcpy(retbuff, "");

    ptr = popen(cmd, "r");
    if ( ptr == NULL ) return -1;

    while (fgets(buf, sizeof(buf), ptr) != NULL)
    {
        /* fprintf(stderr, "[%s]\n", buf); */
        break;
    }
    pclose(ptr);

    strcpy(retbuff, buf);

    return 0;
}

int ltrim(char *str)
{
    char *ptr = " \r\n\x9";
    int j, i = 0, len;

    len = (int)strlen(str);
    if ( len == 0 ) return 0;

    while (i < len  && strchr(ptr, str[i]) != NULL)
    {
        i++;
    }

    for (j = i; j <= len; j++) str[j - i] = str[j];

    return (int)strlen(str);
}

int rtrim(char *str)
{
    char *ptr = " \r\n\x9";
    int i = (int)strlen(str) - 1;

    while (i > 0  && strchr(ptr, str[i]) != NULL)
    {
        i--;
    }

    i++;

    str[i] = 0;

    return (int)strlen(str);
}

int trim(char *str)
{
    ltrim(str);
    rtrim(str);

    return rtrim(str);
}

void replace2(char *s_str, const char *ing_str, int start, int len)
{
    int nlen;
    int i, ing_len;

    nlen = (int)strlen(s_str);
    if ( start <= 0 ) start = 0;
    if ( start >= nlen ) start = nlen;
    if ( len <= 0 ) len = 0;

    if ( start + len >= nlen ) len = nlen - start;

    ing_len = (int)strlen(ing_str);

    if ( len > ing_len )
    {
        nlen = (int)strlen(s_str + start + len);

        for (i = 0; i <= nlen; i++)
        {
            s_str[start + ing_len + i] = s_str[start + len + i];
        }
    }
    else if ( len < ing_len )
    {
        nlen = (int)strlen(s_str + start + len);
        for (i = nlen; i >= 0; i--)
        {
            s_str[start + ing_len + i] = s_str[start + len + i];
        }
    }

    memcpy(s_str + start, ing_str, ing_len);
}

void replace1(char *s_str, const char *ing_str, const char *ed_str)
{
    char buffer[4096];
    char *cur_ptr;
    int ing_len = (int)strlen(ing_str);
    int addr = 0;

    if ( strcmp(ing_str, ed_str) == 0 ) return;

    addr = (int)strlen(ed_str);
    cur_ptr = strstr(s_str, ing_str);
    while (cur_ptr != NULL)
    {
        cur_ptr[0] = 0;

        addr += (int)strlen(s_str);
        strcpy(buffer, s_str);
        strcat(buffer, ed_str);
        strcat(buffer, cur_ptr + ing_len);
        strcpy(s_str, buffer);

        cur_ptr = strstr(s_str + addr, ing_str);
    }

}
void str_upper(char *str)
{
    char *l_ptr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *s_ptr = "abcdefghijklmnopqrstuvwxyz";
    char *cur_ptr;
    unsigned int  i = 0;

    while (i < (int)strlen(str))
    {
        cur_ptr = strchr(s_ptr, str[i]);
        if ( cur_ptr != NULL )
        {
            str[i] = l_ptr[cur_ptr - s_ptr];
        }
        i++;
    }

}

void str_lower(char *str)
{
    char *l_ptr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *s_ptr = "abcdefghijklmnopqrstuvwxyz";
    char *cur_ptr;
    unsigned int  i = 0;

    while (i < (int)strlen(str))
    {
        cur_ptr = strchr(l_ptr, str[i]);
        if ( cur_ptr != NULL )
        {
            str[i] = s_ptr[cur_ptr - l_ptr];
        }
        i++;
    }
}

void get_cur_time(const char *_format, char *time_buff)
{
    time_t long_time;
    struct tm *dnew;
    char year[10], month[10], day[10], hour[10], min[10], sec[10];
    char year1[10];
    char buffer[1024];

    time(&long_time);
    dnew = localtime(&long_time);

    sprintf(year,   "%04d", dnew->tm_year + 1900);
    sprintf(year1,  "%c%c", year[2], year[3]);
    sprintf(month,  "%02d", dnew->tm_mon + 1);
    sprintf(day,    "%02d", dnew->tm_mday);
    sprintf(hour,   "%02d", dnew->tm_hour);
    sprintf(min,    "%02d", dnew->tm_min);
    sprintf(sec,    "%02d", dnew->tm_sec);

    strcpy(buffer, _format);
    str_upper(buffer);

    if ( strstr(buffer, "HH24") != NULL )
    {
        replace1(buffer, "YYYY", year);
        replace1(buffer, "YY", year1);
        replace1(buffer, "MM", month);
        replace1(buffer, "DD", day);
        replace1(buffer, "HH24", hour);
        replace1(buffer, "MI", min);
        replace1(buffer, "SS", sec);
    }
    else if ( strstr(buffer, "24HH") != NULL )
    {
        replace1(buffer, "YYYY", year);
        replace1(buffer, "YY", year1);
        replace1(buffer, "MM", month);
        replace1(buffer, "DD", day);
        replace1(buffer, "24HH", hour);
        replace1(buffer, "MI", min);
        replace1(buffer, "SS", sec);
    }
    else
    {
        replace1(buffer, "YYYY", year);
        replace1(buffer, "YY", year1);
        replace1(buffer, "MM", month);
        replace1(buffer, "DD", day);
        replace1(buffer, "HH", hour);
        replace1(buffer, "MI", min);
        replace1(buffer, "SS", sec);
    }

    strcpy(time_buff, buffer);
}

/**************************************************************************************************
 *Function Name: sub_str
 *
 *Parameters: char *s_str   source string
 *            char *t_str   target string
 *            int addr      where service code start with
 *            int len       length of service code
 *            int type      if type == 1 then earse the service code in source string
 *
 *Description: pick up service code from source to target string
 *
 *Returns: void
 *
 **************************************************************************************************/
void sub_str(char *s_str, char *t_str, int addr, int len, int type)
{
    char buffer[2048];

    int len1, i, start1;
    int nLen = (int)strlen(s_str);

    if ( addr < 0 )
    {
        start1 = 0;
    }
    else if ( addr >= nLen )
    {
        start1 = nLen;
    }
    else
    {
        start1 = addr;
    }

    if ( len == 0 )
    {
        len1 = nLen;
    }
    else if ( len < 0 )
    {
        start1 = start1 + len + 1;
        len1 = 0 - len;
    }
    else len1 = len;

    if ( start1 + len1 > nLen )
    {
        len1 = nLen - start1;
    }

    for (i = 0; i < len1; i++) t_str[i] = s_str[start1 + i];
    t_str[len1] = 0;

    if ( type == 1 )
    {
        strcpy(buffer, s_str + start1 + len1);
        s_str[start1] = 0;
        strcat(s_str, buffer);
    }
}

/*******************************************************************************************************
函数功能
    字符串截取操作
输入参数
    s_str       待操作的源字符串
    t_str       操作结果
    sp_str      分割字符串
    num         取被分割字符串分割的第num个子串 从１开始计数
    type        ＝0截取之后子串在源串中保留，=1不保留。
    type1       ＝0截取之后对应的间隔符号保留，=1不保留。
    dect        表示截取方向（0从左边截取和1从右边截取）
输出参数
    s_str 截取之后源字符串
    t_str 截取的子串
*******************************************************************************************************/
void split_str(char *s_str, char *t_str, const char *sp_str, int num, int type, int type1, int dect)
{
    char *std_ptr;
    char *cur_ptr;
    char *last_ptr;
    int  count, i;
    char tempChar;
    char buffer[1024];

    std_ptr = s_str;
    cur_ptr = std_ptr;
    last_ptr = cur_ptr;

    count = 0;

    if ( strlen(s_str) == 0 )
    {
        strcpy(t_str, "");
        return;
    }

    if ( num <= 0 ) num = 1;

    //从左边截取
    if ( dect == 0 )
    {
        while (cur_ptr != NULL && count != num)
        {
            if ( count == 0 ) last_ptr = cur_ptr;
            else last_ptr = cur_ptr + strlen(sp_str);

            cur_ptr = strstr(last_ptr, sp_str);
            count++;
        }

        if ( count != num )
        {
            strcpy(t_str, "");
            return;
        }

        /*找到符合条件的子串*/
        if ( cur_ptr != NULL )
        {
            tempChar = cur_ptr[0];
            cur_ptr[0] = 0;
            strcpy(t_str, last_ptr);
            cur_ptr[0] = tempChar;
        }
        else
        {
            strcpy(t_str, last_ptr);
        }

        if ( cur_ptr == NULL )
        {
            if ( type == 1 )
            {
                last_ptr[0] = 0;
            }
        }
        else
        {
            if ( type == 1 )
            {
                if ( type1 == 1 ) cur_ptr += strlen(sp_str);


                for (i = 0; i <= (int)strlen(cur_ptr); i++)
                {
                    last_ptr[i] = cur_ptr[i];
                }
            }
        }

        return;
    }

    strcpy(buffer, sp_str);
    //从右边截取
    reverse(s_str);
    reverse(buffer);
    split_str(s_str, t_str, buffer, num, type, type1, 0);

    reverse(s_str);
    reverse(t_str);

}

void reverse(char *str)
{
    char ch;
    int  i, len = (int)strlen(str);
    int len1;
    if ( len <= 1 ) return;

    len1 = (len + 1) / 2;
    for (i = 0; i < len1; i++)
    {
        ch = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = ch;
    }

}
