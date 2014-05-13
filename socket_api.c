#if defined(HPUX_HOST)
    #ifndef __INCLUDE_FROM_TIME_H
        #define __INCLUDE_FROM_TIME_H
    #endif

    #ifdef _XOPEN_SOURCE_EXTENDED
        #undef _XOPEN_SOURCE_EXTENDED
    #endif


    #ifndef _XOPEN_SOURCE_EXTENDED
        #define _XOPEN_SOURCE_EXTENDED
    #endif

    #ifdef _KERNEL_BUILD
        #undef _KERNEL_BUILD
    #endif
#endif

#if defined(OSF_HOST)
    #ifndef _XOPEN_SOURCE_EXTENDED
        #define _XOPEN_SOURCE_EXTENDED
    #endif

    #ifndef _POSIX_PII_SOCKET
        #define _POSIX_PII_SOCKET
    #endif

    #ifndef u_char
        #define u_char unsigned char
    #endif

    #ifndef u_short
        #define u_short unsigned short
    #endif

    #ifndef u_long
        #define u_long unsigned long
    #endif
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stropts.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include "socket_api.h"
#include "tools.h"

int gethostinfo(char *name, char *address)
{
    char buffer[1024];
    struct hostent *hptr;

    strcpy(name, "");
    strcpy(address, "");

    if (gethostname(buffer, sizeof(buffer)) != 0)
    {
        return -1;
    }

    strcpy(name, buffer);

    hptr = gethostbyname(name);
    if (hptr == NULL)
    {
        return -1;
    }

    inet_ntop(hptr->h_addrtype, hptr->h_addr_list[0], buffer, sizeof(buffer));

    strcpy(address, buffer);

    return 0;
}

int link_remote(const char *remote_ip, int remote_port, const char *local_ip)
{
    char buffer[1024];

    struct sockaddr_in server;
    struct sockaddr_in local_sin;
    int  count, sock_fd;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        return -1;
    }

    strcpy(buffer, local_ip);
    if (trim(buffer) != 0)
    {
        local_sin.sin_family = AF_INET;
        local_sin.sin_port = 0;
        local_sin.sin_addr.s_addr = (in_addr_t)inet_addr(buffer);
        if (bind(sock_fd, (struct sockaddr *)&local_sin, sizeof(local_sin)) != 0)
        {
            linker_cut(sock_fd, 0);
            return -1;
        }
    }

    memset((void *)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = (in_addr_t)inet_addr(remote_ip);
    server.sin_port = htons((unsigned short)remote_port);

    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        linker_cut(sock_fd, 0);
        return -1;
    }

    return sock_fd;
}

int send_data(const char *snd_buffer, int snd_size, int sock_fd)
{
    int  len, nSndBytes = 0;

    while (nSndBytes < snd_size)
    {
        len = send(sock_fd, (void *)(snd_buffer + nSndBytes), snd_size - nSndBytes, 0);

        if (len < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return -1;
        }
        nSndBytes += len;
    }

    return 0;
}

int recv_data1(char *rcv_buffer, int rcv_size, int out_time, int sock_fd)
{
#if defined(LINUX_HOST) || defined(OSF_HOST)
    fd_set ReadSet;
#else
    struct fd_set ReadSet;
#endif
    struct timeval WaitTime;
    char buffer[1024];
    int  i, nLoop, nLen, nRet, rcved_size;

    WaitTime.tv_sec = (out_time > 0) ? out_time : 5;
    WaitTime.tv_usec = 0;

    FD_ZERO(&ReadSet);
    FD_SET((unsigned int)sock_fd, &ReadSet);

    memset(rcv_buffer, 0, rcv_size);
    rcved_size = 0;
    nRet = select(sock_fd + 1, &ReadSet, NULL, NULL, &WaitTime);

    if (nRet == 0)
    {
        return rcved_size;
    }
    if (nRet < 0)
    {
        return -1;
    }
    if (FD_ISSET(sock_fd, &ReadSet))
    {
        memset(buffer, 0, sizeof(buffer));
        nLoop = rcv_size / sizeof(buffer) + 1;
        nLen = 1;

        rcved_size = recv(sock_fd, (char *)rcv_buffer, rcv_size, 0);
        if (rcved_size < 0)
        {
            rcved_size = 0;
        }
        rcv_buffer[rcved_size] = 0;
    }
    else
    {
        return 0;
    }
    printf("1\n");
    return rcved_size;
}

int recv_data2(char *rcv_buffer, int seg_size, int out_time, const char * stop_flag, int sock_fd)
{
    char tmp_buffer[1024];
    time_t long_time1, long_time2;
    int  nLen;

    if (seg_size <= 0)
    {
        seg_size = 512;
    }
    if (seg_size >= sizeof(tmp_buffer))
    {
        seg_size = sizeof(tmp_buffer) - 1;
    }

    strcpy(rcv_buffer, stop_flag);
    if (trim(rcv_buffer) == 0)
    {
        return recv_data1(rcv_buffer, seg_size, out_time, sock_fd);
    }

    strcpy(rcv_buffer, "");

    time(&long_time1);
    while (1)
    {
        nLen = recv_data1(tmp_buffer, seg_size, out_time, sock_fd);

        strcat(rcv_buffer, tmp_buffer);

        if (nLen < 0)
        {
            return strlen(rcv_buffer);
        }
        else if (nLen > 0)
        {
            time(&long_time1);
        }

        if (strstr(rcv_buffer, stop_flag) != NULL)
        {
            break;
        }
        time(&long_time2);
        if ((int)(long_time2 - long_time1) > out_time)
        {
            break;
        }
    }

    return strlen(rcv_buffer);
}

/*******************************************************************************************************
函数功能
    断开远程连接
输入参数
    int sockfd 远程连接
    int type  断开类型
输出参数
    断开成功，返回true，否则返回false
*******************************************************************************************************/
void linker_cut(int sock_fd, int type)
{
    struct linger aa ;
    struct stat file_attr;

    aa.l_onoff = 1;
    aa.l_linger = 0 ;

    if (type != 0)
    {
        setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, (char*)&aa, sizeof(aa));
        /*
        控制如何关闭一个TCP连接
        int shutdown(int sockfd, int howto)
        int howto
        =0 关闭读通道
        =1 关闭写通道
        =2 关闭读写通道
        */
        shutdown(sock_fd, 2);
    }

    close(sock_fd);
}

int get_sock_info(int sock_fd, char *ip_info, int *port_info)
{
    struct sockaddr_in svr_in;
    int  len, remote_len;

#if defined(AIX_HOST) || defined(HPUX_HOST) || defined(LINUX_HOST) || defined(OSF_HOST)
    union ip_struct
    {
        struct
        {
            u_char s_b1;
            u_char s_b2;
            u_char s_b3;
            u_char s_b4;
        } s_un_b;

        struct
        {
            u_short s_w1;
            u_short s_w2;
        } s_un_w;

        u_long s_addr;
    } ip_client;
#endif

    strcpy(ip_info, "");
    *port_info = 0;

    //取得socket的本地地址信息
    len = sizeof(svr_in);
    if (getsockname(sock_fd, (struct sockaddr *)&svr_in, (socklen_t *)&len) != 0)
    {
        return -1;
    }

#if defined(AIX_HOST) || defined(HPUX_HOST) || defined(LINUX_HOST) || defined(OSF_HOST)
    ip_client.s_addr = svr_in.sin_addr.s_addr;

    sprintf(ip_info, "%d.%d.%d.%d", ip_client.s_un_b.s_b1, ip_client.s_un_b.s_b2,
            ip_client.s_un_b.s_b3, ip_client.s_un_b.s_b4);
#endif

#if defined(SUNOS_HOST)
    sprintf(ip_info, "%d.%d.%d.%d", svr_in.sin_addr.S_un.S_un_b.s_b1, svr_in.sin_addr.S_un.S_un_b.s_b2,
            svr_in.sin_addr.S_un.S_un_b.s_b3, svr_in.sin_addr.S_un.S_un_b.s_b4);

#endif
    *port_info = svr_in.sin_port;

    return 0;

}
int get_peer_info(int sock_fd, char *ip_info, int *port_info)
{
    struct sockaddr_in remote_in;
    int  len;

#if defined(AIX_HOST) || defined(HPUX_HOST) || defined(LINUX_HOST) || defined(OSF_HOST)
    union ip_struct
    {
        struct
        {
            u_char s_b1;
            u_char s_b2;
            u_char s_b3;
            u_char s_b4;
        } s_un_b;

        struct
        {
            u_short s_w1;
            u_short s_w2;
        } s_un_w;

        u_long s_addr;
    } ip_server;
#endif

    strcpy(ip_info, "");
    *port_info = 0;

    //取得socket对应的远程地址信息
    len = sizeof(remote_in);
    if (getpeername(sock_fd, (struct sockaddr *)&remote_in, (socklen_t *)&len) != 0)
    {
        return -1;
    }
#if defined(AIX_HOST) || defined(HPUX_HOST) || defined(LINUX_HOST) || defined(OSF_HOST)
    ip_server.s_addr = remote_in.sin_addr.s_addr;
    sprintf(ip_info, "%d.%d.%d.%d", ip_server.s_un_b.s_b1, ip_server.s_un_b.s_b2,
            ip_server.s_un_b.s_b3, ip_server.s_un_b.s_b4);
#endif

#if defined(SUNOS_HOST)
    sprintf(ip_info, "%d.%d.%d.%d", remote_in.sin_addr.S_un.S_un_b.s_b1, remote_in.sin_addr.S_un.S_un_b.s_b2,
            remote_in.sin_addr.S_un.S_un_b.s_b3, remote_in.sin_addr.S_un.S_un_b.s_b4);
#endif
    *port_info = remote_in.sin_port;

    return 0;

}

int create_listen(int _port, int max_link, int non_block, int reuse_addr)
{
    struct sockaddr_in svr_in;
    int  len, sock_fd, flags;
    char buffer[1024];

    if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        return -1;
    }

    if (reuse_addr != 0)
    {
        /* 监听端口重新使用标志 0 不重用等待端口释放 1重用 不等端口释放 */
        /* 保证端口可以被重复使用 */
        flags = 1;
        len = sizeof(flags);

        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &flags, len);
    }

    svr_in.sin_family = PF_INET;
    svr_in.sin_addr.s_addr = INADDR_ANY;
    svr_in.sin_port = htons((unsigned short)_port);
    if (bind(sock_fd, (struct sockaddr *)&svr_in, sizeof(svr_in)) != 0)
    {
        linker_cut(sock_fd, 0);
        return -1;
    }

    len = sizeof(svr_in);
    if (getsockname(sock_fd,(struct sockaddr *)&svr_in, (socklen_t *)&len) != 0)
    {
        linker_cut(sock_fd, 0);
        return -1;
    }
    if (max_link <= 0)
    {
        max_link = 1;
    }

    if (listen(sock_fd, max_link) != 0)
    {
        linker_cut(sock_fd, 0);
        return -1;
    }

    if (non_block != 0)
    {
        flags = fcntl(sock_fd, F_GETFL, 0);
        fcntl(sock_fd, F_SETFL, O_NONBLOCK | flags);
    }

    return sock_fd;
}


int startup_listen(int sock_fd, int out_time)
{
    struct  timeval WaitTime;
#if defined(LINUX_HOST) || defined(OSF_HOST)
    fd_set ReadSet;
#else
    struct fd_set ReadSet;
#endif
    struct  sockaddr_in client_in;
    int  len, clnt_fd;

    WaitTime.tv_sec = (out_time > 0) ? out_time : 5;
    WaitTime.tv_usec = 0;

    FD_ZERO(&ReadSet);
    FD_SET((unsigned int)sock_fd, &ReadSet);

    if (select(sock_fd + 1, &ReadSet, NULL, NULL, &WaitTime) <= 0)
    {
        return 0;
    }

    len = sizeof(client_in);
    clnt_fd = accept(sock_fd, (struct sockaddr *)&client_in, (socklen_t *)&len);

    if (clnt_fd < 0)
    {
        return -1;
    }

    return clnt_fd;
}

