#include <sys/wait.h>
#include "log.h"
#include "Global.h"
#include "RecDeal.h"
#include "SemTools.h"

extern struct INIT_CONF   G_ini;
extern char * G_trans_buffer;
extern char * G_shm;

int cur_port;
int cur_serial;
int ini_addr;
struct AUTH_IPNET auth_ipset;
struct TRSMT_CONF *G_trsmt_conf;

struct SHM_CONF * cur_shm = NULL;

/**************************************************************************************************
 *Function Name: RegProcActive
 *
 *Parameters: const char *act_name      activation name
 *            int add_flag              if add_flag is equal to 1, then deal_num + 1
 *
 *Description: record the current activation of program.
 *
 *Returns: void
 *
 **************************************************************************************************/
void RegProcActive(const char *act_name, int add_flag)
{
    char buffer[128];

    get_cur_time("YYYY-MM-DD HH24:MI:SS", buffer);
    strcpy(cur_shm[cur_serial].cur_active, act_name);
    strcpy(cur_shm[cur_serial].last_time, buffer);

    if (add_flag == 1)
    {
        cur_shm[cur_serial].deal_num++;
    }
}

/**************************************************************************************************
函数功能：
    WaitChld函数
输入参数：

输出参数：

**************************************************************************************************/
void signal_child(int signal_no)
{
    char  buffer[128];
    int   status, chld_term_sig;
    pid_t pid;


    pid = waitpid(0, &status, 0);
    if (pid < 0)
    {
        sprintf(buffer, "waitpid error (signal=%d)%d:%s", signal_no, errno, strerror(errno));
        WriteLog(cur_port, cur_serial, OUT_ULOG, buffer);
        return ;
    }

    sprintf(buffer, "进程[%d]终止,状态[%d]", (int)pid, status);
    WriteLog(cur_port, cur_serial, OUT_ULOG, buffer);

    if (WIFEXITED(status) != 0)     // 子进程正常退出
    {
        return;
    }

    chld_term_sig = WTERMSIG(status);
    switch (chld_term_sig)
    {
    case SIGILL:
        strcpy(buffer, "CoreDumped-SIGILL");
        break;
    case SIGTRAP:
        strcpy(buffer, "CoreDumped-SIGTRAP");
        break;
    case SIGABRT:
        strcpy(buffer, "CoreDumped-SIGABRT");
        break;
        //case SIGEMT:
    case SIGFPE:
        strcpy(buffer, "CoreDumped-SIGFPE");
        break;
    case SIGBUS:
        strcpy(buffer, "CoreDumped-SIGBUS");
        break;
    case SIGSEGV:
        strcpy(buffer, "CoreDumped-SIGSEGV");
        break;
        //case SIGSYS:
    case SIGXCPU:
        strcpy(buffer, "CoreDumped-SIGXCPU");
        break;
    case SIGQUIT:
        strcpy(buffer, "Killed-SIGQUIT");
        break;
    case SIGKILL:
        strcpy(buffer, "Killed-SIGKILL");
        break;
    case SIGTERM:
        strcpy(buffer, "Killed-SIGTERM");
        break;
    default:
        sprintf(buffer, "Unknown Exception SIGNAL:[%d]", chld_term_sig);
        break;
    }

    WriteLog(cur_port, cur_serial, OUT_ULOG, buffer);
}

/**************************************************************************************************
函数功能：

输入参数：

输出参数：

符号名    信号值 描述                                    是否符合POSIX
SIGHUP    1      在控制终端上检测到挂断或控制线程死亡    是
SIGINT    2      交互注意信号                            是
SIGQUIT   3      交互中止信号                            是
SIGILL    4      检测到非法硬件的指令                    是
SIGTRAP   5      从陷阱中回朔                            否
SIGABRT   6      异常终止信号                            是
SIGEMT    7      EMT 指令                                否
SIGFPE    8      不正确的算术操作信号                    是
SIGKILL   9      终止信号                                是
SIGBUS    10     总线错误                                否
SIGSEGV   11     检测到非法的内存调用                    是
SIGSYS    12     系统call的错误参数                      否
SIGPIPE   13     在无读者的管道上写                      是
SIGALRM   14     报时信号                                是
SIGTERM   15     终止信号                                是
SIGURG    16     IO信道紧急信号                          否
SIGSTOP   17     暂停信号                                是
SIGTSTP   18     交互暂停信号                            是
SIGCONT   19     如果暂停则继续                          是
SIGCHLD   20     子线程终止或暂停                        是
SIGTTIN   21     后台线程组一成员试图从控制终端上读出    是
SIGTTOU   22     后台线程组的成员试图写到控制终端上      是
SIGIO     23     允许I/O信号                             否
SIGXCPU   24     超出CPU时限                             否
SIGXFSZ   25     超出文件大小限制                        否
SIGVTALRM 26     虚时间警报器                            否
SIGPROF   27     侧面时间警报器                          否
SIGWINCH  28     窗口大小的更改                          否
SIGINFO   29     消息请求                                否
SIGUSR1   30     保留作为用户自定义的信号1               是
SIGUSR2   31     保留作为用户自定义的信号2               是
**************************************************************************************************/
void signal_ignore(int _signal_no)
{
    char buffer[256];
    char buff[512];

    switch (_signal_no)
    {
    case SIGHUP:
        sprintf(buffer, "进程[%d] 捕捉到 SIGHUP 信号", (int)getpid());
        break;
    case SIGINT:
        sprintf(buffer, "进程[%d] 捕捉到 SIGINT 信号", (int)getpid());
        break;
    case SIGQUIT:
        sprintf(buffer, "进程[%d] 捕捉到 SIGQUIT 信号", (int)getpid());
        break;
    case SIGILL:
        sprintf(buffer, "进程[%d] 捕捉到 SIGILL 信号", (int)getpid());
        break;
    case SIGTRAP:
        sprintf(buffer, "进程[%d] 捕捉到 SIGTRAP 信号", (int)getpid());
        break;
    case SIGABRT:
        sprintf(buffer, "进程[%d] 捕捉到 SIGABRT 信号", (int)getpid());
        break;
        //case SIGEMT:
        //  sprintf(buffer, "进程[%d] 捕捉到 SIGEMT 信号", (int)getpid());
        //  break;
    case SIGFPE:
        sprintf(buffer, "进程[%d] 捕捉到 SIGFPE 信号", (int)getpid());
        break;
    case SIGKILL:
        sprintf(buffer, "进程[%d] 捕捉到 SIGKILL 信号", (int)getpid());
        break;
    case SIGBUS:
        sprintf(buffer, "进程[%d] 捕捉到 SIGBUS 信号(试图访问一块无文件内容对应的内存区域)", (int)getpid());
        break;
    case SIGSEGV:
        sprintf(buffer, "进程[%d] 捕捉到 SIGSEGV 信号(试图对只读映射区域进行写操作)", (int)getpid());
        break;
    case SIGSYS:
        sprintf(buffer, "进程[%d] 捕捉到 SIGSYS 信号", (int)getpid());
        break;
    case SIGPIPE:
        sprintf(buffer, "进程[%d] 捕捉到 SIGPIPE 信号", (int)getpid());
        break;
    case SIGALRM:
        sprintf(buffer, "进程[%d] 捕捉到 SIGALRM 信号", (int)getpid());
        break;
    case SIGTERM:
        sprintf(buffer, "进程[%d] 捕捉到 SIGTERM 信号", (int)getpid());
        break;
    case SIGUSR1:
        sprintf(buffer, "进程[%d] 捕捉到 SIGUSR1 信号", (int)getpid());
        break;
    case SIGUSR2:
        sprintf(buffer, "进程[%d] 捕捉到 SIGUSR2 信号", (int)getpid());
        break;
    case SIGSTOP:
        sprintf(buffer, "进程[%d] 捕捉到 SIGSTOP 信号", (int)getpid());
        break;
    case SIGTSTP:
        sprintf(buffer, "进程[%d] 捕捉到 SIGTSTP 信号", (int)getpid());
        break;
    case SIGTTIN:
        sprintf(buffer, "进程[%d] 捕捉到 SIGTTIN 信号", (int)getpid());
        break;
    case SIGTTOU:
        sprintf(buffer, "进程[%d] 捕捉到 SIGTTOU 信号", (int)getpid());
        break;
    case SIGVTALRM:
        sprintf(buffer, "进程[%d] 捕捉到 SIGVTALRM 信号", (int)getpid());
        break;
    case SIGPROF:
        sprintf(buffer, "进程[%d] 捕捉到 SIGPROF 信号", (int)getpid());
        break;
    case SIGXCPU:
        sprintf(buffer, "进程[%d] 捕捉到 SIGXCPU 信号", (int)getpid());
        break;
    default:
        sprintf(buffer, "进程[%d] 捕捉到未知信号 %d", (int)getpid(), _signal_no);
        break;
    }

    sprintf(buff, "FILE:%s,LINE:%d", __FILE__, __LINE__);
    strcat(buffer, buff);

    WriteLog(cur_port, cur_serial, OUT_ULOG, buffer);
}

/**************************************************************************************************
函数功能：
    注册信号处理函数
输入参数：

输出参数：
    0 表示成功 -1表示失败
**************************************************************************************************/
int signal_shield(int _signal, void ( * sig_func )( int ))
{
    int nRet;
    struct sigaction sig_act;

    sigemptyset(&(sig_act.sa_mask));
    sigaddset(&(sig_act.sa_mask), _signal);

    sig_act.sa_handler = sig_func;
    sig_act.sa_flags = 0;

    nRet = sigaction(_signal, &sig_act, (struct sigaction *)0 );

    if (nRet == 0)
    {
        return 0;
    }

    return -1;
}

/**************************************************************************************************
函数功能：
    捕捉信号处理函数
输入参数：

输出参数：

**************************************************************************************************/
void catch_all_singal()
{
/*
    sigset_t sigall,sigold ;
    sigfillset( &sigall );
*/

    signal_shield(SIGHUP,    signal_ignore);
    signal_shield(SIGINT,    signal_ignore);
/*  signal_shield(SIGQUIT,   signal_ignore);
    signal_shield(SIGILL,    signal_ignore);
    signal_shield(SIGTRAP,   signal_ignore);
    signal_shield(SIGABRT,   signal_ignore);
    signal_shield(SIGEMT,    signal_ignore);
    signal_shield(SIGFPE,    signal_ignore);
    signal_shield(SIGKILL,   signal_ignore);
    signal_shield(SIGBUS,    signal_ignore);
    signal_shield(SIGSEGV,   signal_ignore);
    signal_shield(SIGSYS,    signal_ignore);
    signal_shield(SIGPIPE,   signal_ignore);
    signal_shield(SIGALRM,   signal_ignore);
    signal_shield(SIGTERM,   signal_ignore);
    signal_shield(SIGUSR1,   signal_ignore);
    signal_shield(SIGUSR2,   signal_ignore);
    signal_shield(SIGPOLL,   signal_ignore);
    signal_shield(SIGSTOP,   signal_ignore);
    signal_shield(SIGTSTP,   signal_ignore);
    signal_shield(SIGTTIN,   signal_ignore);
    signal_shield(SIGTTOU,   signal_ignore);
    signal_shield(SIGVTALRM, signal_ignore);
    signal_shield(SIGPROF,   signal_ignore);
    signal_shield(SIGXCPU,   signal_ignore);
    signal_shield(SIGPIPE,   SIG_IGN) ;
*/
    signal_shield(SIGCHLD, signal_child);
}



int RespDeal(char *buff, int len, int sub_sockfd, int is_rec)
{
    int  nlen;
    int  count = 0;

    RegProcActive("RESP", 0);

    //len = MakeRespPkg(buff, len);

    if (is_rec == 1)
        WriteLog(cur_port, cur_serial, OUT_ULOG, "反馈数据[%d][%s]", len, buff);

    while (1)
    {
        nlen = send_data(buff, len, sub_sockfd);
        if (nlen >= 0)
        {
            break;
        }
        sleep(1);
        count++;
        if (count >= 2)
        {
            break;
        }
    }

    if (nlen < 0)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "反馈数据失败,%d:%s", errno, strerror(errno));
    }

    return nlen;
}

void DealOnRec(int sub_sockfd)
{

    int  nlen, len;
    char recv_buffer[512], svc_name[512];
    int  flag;
    int  trsmt_sn = -1;
    int  trsmt_ret = -1;
    int  recflag, opercode;
    int  recv_len = 128;
    char msg_type[2 + 1];
    char msg_version[2 + 1];
    char msg_command[2 + 1];

    memset(msg_type, 0, sizeof(msg_type));
    memset(msg_version, 0, sizeof(msg_version));
    memset(msg_command, 0, sizeof(msg_command));

    RegProcActive("RECV", 1);

    long long start = getCurrmillisecond();

    nlen = recv_data1(recv_buffer, recv_len, G_ini.port_list[ini_addr].out_time, sub_sockfd);
    if (nlen <= 0)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "接收数据错误");
        return ;
    }
    WriteLog(cur_port, cur_serial, OUT_ULOG, "收到数据 长度[%d] 内容[%s]", nlen, recv_buffer);


    //调用业务处理函数，进行数据处理
    memcpy(msg_type,    recv_buffer,    2);
    memcpy(msg_version, recv_buffer+2,  2);
    memcpy(msg_command, recv_buffer+9,  2);

    WriteLog(cur_port, cur_serial, OUT_ULOG, "消息类型[%s] 消息版本号[%s] 协议命令字[%s]", msg_type, msg_version, msg_command);

    if ( strcmp(msg_command, GPS_INFO_UPLOAD_REQ) == 0)
    {

        if ( GPSInfoUpdate(recv_buffer) == ERROR ){
            WriteLog(cur_port, cur_serial, OUT_ULOG, "更新实时表出错");
        }

        if ( GPSInfoCommit(recv_buffer) == ERROR ){
            WriteLog(cur_port, cur_serial, OUT_ULOG, "写入历史表出错");
        }

    }
    else{

        WriteLog(cur_port, cur_serial, OUT_ULOG, "未识别的消息类型");
    }

    long long end = getCurrmillisecond();

    WriteLog(cur_port, cur_serial, OUT_ULOG, "数据已入库,本次处理耗时 %lld ms",  end - start);

}

void ReadAuthIP()
{
    char buffer[1024];
    char szBuffer[256];
    FILE *fp;
    int  flag = 0;
    int  is_need = 0;

    memset((void *)&auth_ipset, 0, sizeof(auth_ipset));
    auth_ipset.is_authip = 1;

    sprintf(szBuffer, "%s.ini", G_ini.prog_name);

    if ((fp = fopen(szBuffer, "r")) == NULL)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "%s Error:(%d)%s", szBuffer, errno, strerror(errno));
        return ;
    }

    while (!feof(fp))
    {
        memset(buffer, 0, sizeof(buffer));
        if (fgets(buffer, sizeof(buffer), fp) == NULL)
        {
            break;
        }

        if (trim(buffer) == 0)
        {
            continue;
        }

        if (buffer[0] == '#')
        {
            continue;
        }

        if (strcmp(buffer, "[MAIN]") == 0)
        {
            flag = 1;
            continue;
        }
        else if (strcmp(buffer, "[TNS]") == 0)
        {
            flag = 2;
            is_need = 0;
            continue;
        }
        else
        {
            split_str(buffer, szBuffer, "=", 1, 1, 1, 0);
        }


        if (flag == 2)
        {
            if (strcmp(szBuffer, "lsnr_port") == 0)
            {
                if (atoi(buffer) == G_ini.port_list[ini_addr].lsnr_port)
                {
                    is_need = 1;
                }
            }
        }

        if (flag == 2 && is_need == 1)
        {
            if (strcmp(szBuffer, "is_authip") == 0)
            {
                auth_ipset.is_authip = atoi(buffer);
            }
            else if (strcmp(szBuffer, "auth_ip") == 0)
            {
                if (auth_ipset.auth_num < MAX_AUTH_IP_NUM)
                {
                    strcpy(auth_ipset.auth_ip[auth_ipset.auth_num], buffer);
                    auth_ipset.auth_num++;
                }
            }
        }

    }
    fclose(fp);
}

/*
    0检验通过
    -1检验失败
*/
int verfiy_socket(int _sock_fd)
{
    char ip_buff[128];
    int  port_buff;
    int  i;

    get_peer_info(_sock_fd, ip_buff, &port_buff);

    if (G_ini.port_list[ini_addr].is_debug == 1)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "监听到[%d][%s:%d]的业务请求...", _sock_fd, ip_buff, port_buff);
    }

    if (auth_ipset.is_authip == 0)
    {
        return 0;
    }

    for (i = 0 ; i < auth_ipset.auth_num ; i++)
    {
        if (strcmp(auth_ipset.auth_ip[i], ip_buff) == 0)
        {
            return 0;
        }
    }

    if (G_ini.port_list[ini_addr].is_debug == 1)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "不合法的外围系统");
    }

    return -1;
}

/**
 * 判断客户端的连接数是否超过最大连接数
 * 1:没有超过 0:超过
 */
int IsPortUsed()
{
    struct SHM_HEAD * shm_head;
    struct SHM_CONF * shm_conf;
    int i, j, count;
    int id = 0;
    int nret = 1;

    if (sem_oper(id, -1) == SEM_ERROR)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "IsPortUsed:Lock,%s", gs_sem_err);
        return 1;
    }

    shm_head = (struct SHM_HEAD *)G_shm;
    i = (int)sizeof(struct SHM_HEAD);
    shm_conf = (struct SHM_CONF *)(G_shm + i);

    j = 0;
    count = 0;
    for (i = 0 ; i < shm_head->proc_num; i++)
    {
        if (shm_conf[i].lsnr_port != cur_port)
        {
            continue;
        }
        if (shm_conf[i].is_used == 0)
        {
            continue;
        }
        count++;
    }
    count++;

    if (G_ini.port_list[ini_addr].lsnr_num == count)
    {
        nret = 0;
    }
    else
    {
        cur_shm[cur_serial].is_used = 1;
    }

    if (sem_oper(id, 1) == SEM_ERROR)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "IsPortUsed:unLock,%s", gs_sem_err);
    }

    return nret;
}

/**************************************************************************************************
 *Function Name: FreeCurPortUsed
 *
 *Parameters:
 *
 *Description: modify the port status to unused in shared memroy
 *
 *Returns: void
 *
 **************************************************************************************************/
void FreeCurPortUsed()
{
    int id = 0;

    if (sem_oper(id,-1) == SEM_ERROR)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "FreeCurPortUsed:Lock,%s", gs_sem_err);
        return;
    }

    cur_shm[cur_serial].is_used = 0;

    if (sem_oper(id, 1) == SEM_ERROR)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "FreeCurPortUsed:unLock,%s", gs_sem_err);
        return;
    }
}

/**************************************************************************************************
 *Function Name: LsnrRec
 *
 *Parameters: int port          //Listen port
 *            int serial_no     //serial number of listen sub_process
 *            int socket_fd     //socket file descriptor
 *
 *Description: listen to the requests from clients.
 *
 *Returns: void
 *
 **************************************************************************************************/
void LsnrRec(int port, int serial_no, int socket_fd)
{



    char buffer[64];
    int  i, sub_sockfd;
    char DBInfo[2048];

    cur_port = port;
    cur_serial = serial_no;

    get_cur_time("YYYY-MM-DD HH24:MI:SS", buffer);

    GetPortShm(port, &cur_shm);

    cur_shm[serial_no].run_status = RUN_STATUS_RUNNING;
    cur_shm[serial_no].proc_id = (int)getpid();
    cur_shm[serial_no].deal_num = 0;
    strcpy(cur_shm[serial_no].start_time, buffer);

    RegProcActive("IDLE", 0);

    ReadAuthIP();

    if( ConnectDB(G_ini.port_list[ini_addr].DBUserName, G_ini.port_list[ini_addr].DBPassword, G_ini.port_list[ini_addr].Sid) == ERROR )
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] 数据库连接失败", (int)getpid());
    }
    WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] 数据库连接成功", (int)getpid());


    if( getDBInfo(DBInfo) == ERROR ){

        WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] 获取数据库信息失败", (int)getpid());
    }
    WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] 数据库信息\n%s", (int)getpid(), DBInfo);


    WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d],sn=[%d],socket=[%d]:开始接收客户端请求", (int)getpid(), serial_no, socket_fd);

    ini_addr = 0;
    for (i = 0 ; i < G_ini.port_num ; i++)
    {
        if (port == G_ini.port_list[i].lsnr_port)
        {
            ini_addr = i;
            break;
        }
    }

    while (cur_shm[serial_no].run_status == RUN_STATUS_RUNNING || cur_shm[serial_no].run_status == RUN_STATUS_REFRESH)
    {
        if (cur_shm[serial_no].run_status == RUN_STATUS_REFRESH)
        {
            cur_shm[serial_no].run_status = RUN_STATUS_RUNNING;
            RegProcActive("REFRESH", 0);
            ReadAuthIP();
        }

        RegProcActive("ACCEPT", 0);

        if (G_ini.port_list[ini_addr].is_debug == 1)
        {
            WriteLog(cur_port, cur_serial, OUT_ULOG, "Begin Accept [%d][%d]...", socket_fd, G_ini.port_list[ini_addr].out_time);
        }

        sub_sockfd = startup_listen(socket_fd, G_ini.port_list[ini_addr].out_time);
        if (sub_sockfd <= 0)
        {
            if (G_ini.port_list[ini_addr].is_debug == 1)
            {
                WriteLog(cur_port, cur_serial, OUT_ULOG, "Accept Error,%d:%s", errno, strerror(errno));
            }
            continue;
        }

        if (verfiy_socket(sub_sockfd) != 0)
        {
            WriteLog(cur_port, cur_serial, OUT_ULOG, "即将断开连接");
            linker_cut(sub_sockfd, 0);
            continue;
        }

        if (IsPortUsed() == 0)
        {
            WriteLog(cur_port, cur_serial, OUT_ULOG, "超过最大连接数");
            linker_cut(sub_sockfd, 0);
            continue;
        }

        while (cur_shm[serial_no].run_status == RUN_STATUS_RUNNING)
        {
            DealOnRec(sub_sockfd);
            if (G_ini.port_list[ini_addr].is_long_link == 0)
            {
                break;
            }
        }

        linker_cut(sub_sockfd, 0);

        FreeCurPortUsed();
    }

    WriteLog(cur_port, cur_serial, OUT_ULOG, "停止接收客户端请求");

    cur_shm[serial_no].run_status = RUN_STATUS_STOPED;
    cur_shm[serial_no].proc_id = 0;
    cur_shm[serial_no].deal_num = 0;
    strcpy(cur_shm[serial_no].start_time, "");
    strcpy(cur_shm[serial_no].cur_active, "");
    strcpy(cur_shm[serial_no].last_time, "");

    if( DisConnectDB() == RIGHT )
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] 断开数据库连接成功", (int)getpid());
    }
}
