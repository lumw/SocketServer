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
�������ܣ�
    WaitChld����
���������

���������

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

    sprintf(buffer, "����[%d]��ֹ,״̬[%d]", (int)pid, status);
    WriteLog(cur_port, cur_serial, OUT_ULOG, buffer);

    if (WIFEXITED(status) != 0)     // �ӽ��������˳�
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
�������ܣ�

���������

���������

������    �ź�ֵ ����                                    �Ƿ����POSIX
SIGHUP    1      �ڿ����ն��ϼ�⵽�Ҷϻ�����߳�����    ��
SIGINT    2      ����ע���ź�                            ��
SIGQUIT   3      ������ֹ�ź�                            ��
SIGILL    4      ��⵽�Ƿ�Ӳ����ָ��                    ��
SIGTRAP   5      �������л�˷                            ��
SIGABRT   6      �쳣��ֹ�ź�                            ��
SIGEMT    7      EMT ָ��                                ��
SIGFPE    8      ����ȷ�����������ź�                    ��
SIGKILL   9      ��ֹ�ź�                                ��
SIGBUS    10     ���ߴ���                                ��
SIGSEGV   11     ��⵽�Ƿ����ڴ����                    ��
SIGSYS    12     ϵͳcall�Ĵ������                      ��
SIGPIPE   13     ���޶��ߵĹܵ���д                      ��
SIGALRM   14     ��ʱ�ź�                                ��
SIGTERM   15     ��ֹ�ź�                                ��
SIGURG    16     IO�ŵ������ź�                          ��
SIGSTOP   17     ��ͣ�ź�                                ��
SIGTSTP   18     ������ͣ�ź�                            ��
SIGCONT   19     �����ͣ�����                          ��
SIGCHLD   20     ���߳���ֹ����ͣ                        ��
SIGTTIN   21     ��̨�߳���һ��Ա��ͼ�ӿ����ն��϶���    ��
SIGTTOU   22     ��̨�߳���ĳ�Ա��ͼд�������ն���      ��
SIGIO     23     ����I/O�ź�                             ��
SIGXCPU   24     ����CPUʱ��                             ��
SIGXFSZ   25     �����ļ���С����                        ��
SIGVTALRM 26     ��ʱ�侯����                            ��
SIGPROF   27     ����ʱ�侯����                          ��
SIGWINCH  28     ���ڴ�С�ĸ���                          ��
SIGINFO   29     ��Ϣ����                                ��
SIGUSR1   30     ������Ϊ�û��Զ�����ź�1               ��
SIGUSR2   31     ������Ϊ�û��Զ�����ź�2               ��
**************************************************************************************************/
void signal_ignore(int _signal_no)
{
    char buffer[256];
    char buff[512];

    switch (_signal_no)
    {
    case SIGHUP:
        sprintf(buffer, "����[%d] ��׽�� SIGHUP �ź�", (int)getpid());
        break;
    case SIGINT:
        sprintf(buffer, "����[%d] ��׽�� SIGINT �ź�", (int)getpid());
        break;
    case SIGQUIT:
        sprintf(buffer, "����[%d] ��׽�� SIGQUIT �ź�", (int)getpid());
        break;
    case SIGILL:
        sprintf(buffer, "����[%d] ��׽�� SIGILL �ź�", (int)getpid());
        break;
    case SIGTRAP:
        sprintf(buffer, "����[%d] ��׽�� SIGTRAP �ź�", (int)getpid());
        break;
    case SIGABRT:
        sprintf(buffer, "����[%d] ��׽�� SIGABRT �ź�", (int)getpid());
        break;
        //case SIGEMT:
        //  sprintf(buffer, "����[%d] ��׽�� SIGEMT �ź�", (int)getpid());
        //  break;
    case SIGFPE:
        sprintf(buffer, "����[%d] ��׽�� SIGFPE �ź�", (int)getpid());
        break;
    case SIGKILL:
        sprintf(buffer, "����[%d] ��׽�� SIGKILL �ź�", (int)getpid());
        break;
    case SIGBUS:
        sprintf(buffer, "����[%d] ��׽�� SIGBUS �ź�(��ͼ����һ�����ļ����ݶ�Ӧ���ڴ�����)", (int)getpid());
        break;
    case SIGSEGV:
        sprintf(buffer, "����[%d] ��׽�� SIGSEGV �ź�(��ͼ��ֻ��ӳ���������д����)", (int)getpid());
        break;
    case SIGSYS:
        sprintf(buffer, "����[%d] ��׽�� SIGSYS �ź�", (int)getpid());
        break;
    case SIGPIPE:
        sprintf(buffer, "����[%d] ��׽�� SIGPIPE �ź�", (int)getpid());
        break;
    case SIGALRM:
        sprintf(buffer, "����[%d] ��׽�� SIGALRM �ź�", (int)getpid());
        break;
    case SIGTERM:
        sprintf(buffer, "����[%d] ��׽�� SIGTERM �ź�", (int)getpid());
        break;
    case SIGUSR1:
        sprintf(buffer, "����[%d] ��׽�� SIGUSR1 �ź�", (int)getpid());
        break;
    case SIGUSR2:
        sprintf(buffer, "����[%d] ��׽�� SIGUSR2 �ź�", (int)getpid());
        break;
    case SIGSTOP:
        sprintf(buffer, "����[%d] ��׽�� SIGSTOP �ź�", (int)getpid());
        break;
    case SIGTSTP:
        sprintf(buffer, "����[%d] ��׽�� SIGTSTP �ź�", (int)getpid());
        break;
    case SIGTTIN:
        sprintf(buffer, "����[%d] ��׽�� SIGTTIN �ź�", (int)getpid());
        break;
    case SIGTTOU:
        sprintf(buffer, "����[%d] ��׽�� SIGTTOU �ź�", (int)getpid());
        break;
    case SIGVTALRM:
        sprintf(buffer, "����[%d] ��׽�� SIGVTALRM �ź�", (int)getpid());
        break;
    case SIGPROF:
        sprintf(buffer, "����[%d] ��׽�� SIGPROF �ź�", (int)getpid());
        break;
    case SIGXCPU:
        sprintf(buffer, "����[%d] ��׽�� SIGXCPU �ź�", (int)getpid());
        break;
    default:
        sprintf(buffer, "����[%d] ��׽��δ֪�ź� %d", (int)getpid(), _signal_no);
        break;
    }

    sprintf(buff, "FILE:%s,LINE:%d", __FILE__, __LINE__);
    strcat(buffer, buff);

    WriteLog(cur_port, cur_serial, OUT_ULOG, buffer);
}

/**************************************************************************************************
�������ܣ�
    ע���źŴ�����
���������

���������
    0 ��ʾ�ɹ� -1��ʾʧ��
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
�������ܣ�
    ��׽�źŴ�����
���������

���������

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
        WriteLog(cur_port, cur_serial, OUT_ULOG, "��������[%d][%s]", len, buff);

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
        WriteLog(cur_port, cur_serial, OUT_ULOG, "��������ʧ��,%d:%s", errno, strerror(errno));
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
        WriteLog(cur_port, cur_serial, OUT_ULOG, "�������ݴ���");
        return ;
    }
    WriteLog(cur_port, cur_serial, OUT_ULOG, "�յ����� ����[%d] ����[%s]", nlen, recv_buffer);


    //����ҵ���������������ݴ���
    memcpy(msg_type,    recv_buffer,    2);
    memcpy(msg_version, recv_buffer+2,  2);
    memcpy(msg_command, recv_buffer+9,  2);

    WriteLog(cur_port, cur_serial, OUT_ULOG, "��Ϣ����[%s] ��Ϣ�汾��[%s] Э��������[%s]", msg_type, msg_version, msg_command);

    if ( strcmp(msg_command, GPS_INFO_UPLOAD_REQ) == 0)
    {

        if ( GPSInfoUpdate(recv_buffer) == ERROR ){
            WriteLog(cur_port, cur_serial, OUT_ULOG, "����ʵʱ�����");
        }

        if ( GPSInfoCommit(recv_buffer) == ERROR ){
            WriteLog(cur_port, cur_serial, OUT_ULOG, "д����ʷ�����");
        }

    }
    else{

        WriteLog(cur_port, cur_serial, OUT_ULOG, "δʶ�����Ϣ����");
    }

    long long end = getCurrmillisecond();

    WriteLog(cur_port, cur_serial, OUT_ULOG, "���������,���δ����ʱ %lld ms",  end - start);

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
    0����ͨ��
    -1����ʧ��
*/
int verfiy_socket(int _sock_fd)
{
    char ip_buff[128];
    int  port_buff;
    int  i;

    get_peer_info(_sock_fd, ip_buff, &port_buff);

    if (G_ini.port_list[ini_addr].is_debug == 1)
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "������[%d][%s:%d]��ҵ������...", _sock_fd, ip_buff, port_buff);
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
        WriteLog(cur_port, cur_serial, OUT_ULOG, "���Ϸ�����Χϵͳ");
    }

    return -1;
}

/**
 * �жϿͻ��˵��������Ƿ񳬹����������
 * 1:û�г��� 0:����
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
        WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] ���ݿ�����ʧ��", (int)getpid());
    }
    WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] ���ݿ����ӳɹ�", (int)getpid());


    if( getDBInfo(DBInfo) == ERROR ){

        WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] ��ȡ���ݿ���Ϣʧ��", (int)getpid());
    }
    WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] ���ݿ���Ϣ\n%s", (int)getpid(), DBInfo);


    WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d],sn=[%d],socket=[%d]:��ʼ���տͻ�������", (int)getpid(), serial_no, socket_fd);

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
            WriteLog(cur_port, cur_serial, OUT_ULOG, "�����Ͽ�����");
            linker_cut(sub_sockfd, 0);
            continue;
        }

        if (IsPortUsed() == 0)
        {
            WriteLog(cur_port, cur_serial, OUT_ULOG, "�������������");
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

    WriteLog(cur_port, cur_serial, OUT_ULOG, "ֹͣ���տͻ�������");

    cur_shm[serial_no].run_status = RUN_STATUS_STOPED;
    cur_shm[serial_no].proc_id = 0;
    cur_shm[serial_no].deal_num = 0;
    strcpy(cur_shm[serial_no].start_time, "");
    strcpy(cur_shm[serial_no].cur_active, "");
    strcpy(cur_shm[serial_no].last_time, "");

    if( DisConnectDB() == RIGHT )
    {
        WriteLog(cur_port, cur_serial, OUT_ULOG, "pid=[%d] �Ͽ����ݿ����ӳɹ�", (int)getpid());
    }
}
