#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "TuxClient.h"
#include "Tools.h"
#include "log.h"

extern char *G_shm;
extern struct INIT_CONF G_ini;

void InitShm()
{
    struct SHM_HEAD *shm_head;
    struct SHM_CONF *shm_conf;
    int i, j, count;

    shm_head = (struct SHM_HEAD *)G_shm;
    i = (int)sizeof(struct SHM_HEAD);
    shm_conf = (struct SHM_CONF *)(G_shm + i);

    shm_head->proc_num = G_ini.proc_num;
    get_cur_time("YYYY-MM-DD HH24:MI:SS", shm_head->start_time);

    j = 0;
    count = 0;
    for (i = 0; i < G_ini.proc_num; i++)
    {
        shm_conf[i].lsnr_port = G_ini.port_list[j].lsnr_port;       /*监听端口*/
        shm_conf[i].run_status = RUN_STATUS_STOPED;                 /*进程运行状态*/
        shm_conf[i].proc_id = 0;
        shm_conf[i].deal_num = 0;
        shm_conf[i].is_used = 0;
        strcpy(shm_conf[i].start_time, "");
        strcpy(shm_conf[i].cur_active, "");
        strcpy(shm_conf[i].last_time, "");

        count++;
        if ( count == G_ini.port_list[j].lsnr_num )
        {
            j++;
            count = 0;
        }
    }

    /*j == G_ini.port_num*/
}

/**
 * 从SHM中根据port确定其进程状态区的首地址，以及进程的个数
 * 输入参数: port 要查询的端口号
 * 返回port_shm
 */
int GetPortShm(int port, struct SHM_CONF **port_shm)
{
    struct SHM_HEAD *shm_head;
    struct SHM_CONF *shm_conf;

    int i, count = 0, flag = 0;

    shm_head = (struct SHM_HEAD *)G_shm;
    shm_conf = (struct SHM_CONF *)(G_shm + sizeof(struct SHM_HEAD));

    *port_shm = NULL;

    for (i = 0; i < shm_head->proc_num; i++)
    {
        if ( port != shm_conf[i].lsnr_port )
        {
            continue;
        }
        count++;
        if ( flag == 0 )
        {
            *port_shm = shm_conf + i;
            flag = 1;
        }
    }
    return count;
}

/**
 * 校验SHM中的配置是否和INI文件中的配置是否一致
 * 一致返回0 否则返回-1
 */
int ShmConfCheck()
{
    struct SHM_HEAD *shm_head;
    struct SHM_CONF *shm_conf;
    int i, count;

    shm_head = (struct SHM_HEAD *)G_shm;
    if ( shm_head->proc_num != G_ini.proc_num )
    {
        return -1;
    }

    for (i = 0; i < G_ini.port_num; i++)
    {
        count = GetPortShm(G_ini.port_list[i].lsnr_port, &shm_conf);
        if ( count != G_ini.port_list[i].lsnr_num )
        {
            return -1;
        }
    }

    return 0;
}

void FreeShm(int _shm_id)
{
    WriteLog(0, 0, OUT_SCREEN, "释放系统资源!");

    sem_free();

    if ( G_shm != (void *)-1 )
    {
        shmdt((char *)G_shm);
    }

    if ( shmctl(_shm_id, IPC_RMID, 0) == -1 )
    {
        WriteLog(0, 0, OUT_SCREEN, "shmctl error(%d):%s\n", errno, strerror(errno));
        return;
    }

    G_shm = NULL;
}

int GetRunCount()
{
    struct SHM_HEAD *shm_head;
    struct SHM_CONF *shm_conf;

    int i, count;

    shm_head = (struct SHM_HEAD *)G_shm;
    shm_conf = (struct SHM_CONF *)(G_shm + sizeof(struct SHM_HEAD));

    count = 0;
    for (i = 0; i < shm_head->proc_num; i++)
    {
        if ( shm_conf[i].proc_id == 0 )
        {
            ;
        }
        else if ( kill(shm_conf[i].proc_id, 0) != 0 )
        {
            WriteLog(0, 0, OUT_SCREEN, "port=[%6d]进程[%03d][%d]异常终止", shm_conf[i].lsnr_port, i, shm_conf[i].proc_id);
            shm_conf[i].run_status = RUN_STATUS_STOPED;
        }
        else if ( shm_conf[i].run_status == RUN_STATUS_RUNNING )
        {
            count++;
        }
    }

    return count;
}


void RefreshParam(const char *prog_name, int port)
{
    struct SHM_HEAD *shm_head;
    struct SHM_CONF *shm_conf;
    int i, count;

    shm_head = (struct SHM_HEAD *)G_shm;
    shm_conf = (struct SHM_CONF *)(G_shm + sizeof(struct SHM_HEAD));

    for (i = 0; i < shm_head->proc_num; i++)
    {
        if ( port != 0 )
        {
            if ( port != shm_conf[i].lsnr_port )
            {
                continue;
            }
        }

        if ( shm_conf[i].proc_id == 0 )
        {
            shm_conf[i].run_status = RUN_STATUS_STOPED;
        }
        else if ( kill(shm_conf[i].proc_id, 0) != 0 )
        {
            WriteLog(0, 0, OUT_SCREEN, "port=[%6d]进程[%03d][%d]异常终止", shm_conf[i].lsnr_port, i, shm_conf[i].proc_id);
            shm_conf[i].run_status = RUN_STATUS_STOPED;
        }
        else if ( shm_conf[i].run_status == RUN_STATUS_RUNNING )
        {
            WriteLog(0, 0, OUT_SCREEN, "port=[%6d]进程[%03d][%d]开始刷新参数...", shm_conf[i].lsnr_port, i, shm_conf[i].proc_id);
            shm_conf[i].run_status = RUN_STATUS_REFRESH;
            count++;
        }
    }
}

void StopClient(const char *prog_name, int port)
{
    struct SHM_HEAD *shm_head;
    struct SHM_CONF *shm_conf;

    int i, count, time_count;
    int stop_count;
    char buffer[8192], buff[32];

    shm_head = (struct SHM_HEAD *)G_shm;
    shm_conf = (struct SHM_CONF *)(G_shm + sizeof(struct SHM_HEAD));

    stop_count = 0;

    for (i = 0; i < shm_head->proc_num; i++)
    {
        if ( port != 0 )
        {
            if ( port != shm_conf[i].lsnr_port )
            {
                continue;
            }
        }

        if ( shm_conf[i].proc_id == 0 )
        {
            /*WriteLog(0, 0, OUT_SCREEN, "port=[%6d]进程[%03d][%d]异常终止", shm_conf[i].lsnr_port, i, shm_conf[i].proc_id);*/
            shm_conf[i].run_status = RUN_STATUS_STOPED;
        }
        else if ( kill(shm_conf[i].proc_id, 0) != 0 )
        {
            WriteLog(0, 0, OUT_SCREEN, "port=[%6d]进程[%03d][%d]异常终止", shm_conf[i].lsnr_port, i, shm_conf[i].proc_id);
            shm_conf[i].run_status = RUN_STATUS_STOPED;
        }
        else
        {
            WriteLog(0, 0, OUT_SCREEN, "port=[%6d]进程[%03d][%d]开始停止...", shm_conf[i].lsnr_port, i, shm_conf[i].proc_id);
            shm_conf[i].run_status = RUN_STATUS_STOPING;
            stop_count++;
        }
    }

    time_count = 0;
    count = 0;
    while (count < stop_count)
    {
        strcpy(buffer, "");

        sleep(1);
        count = 0;
        for (i = 0; i < shm_head->proc_num; i++)
        {
            if ( port != 0 )
            {
                if ( port != shm_conf[i].lsnr_port )
                {
                    continue;
                }
            }

            if ( shm_conf[i].run_status == RUN_STATUS_STOPED )
            {
                count++;
            }
            else
            {
                sprintf(buff, "[%03d]", shm_conf[i].proc_id);
                strcat(buffer, buff);
            }

        }
        if ( strlen(buffer) != 0 )
        {
            WriteLog(0, 0, OUT_SCREEN, "终止了[%03d]个进程,进程%s未停止", count, buffer);
        }
        time_count++;

        if ( time_count >= 30 )
        {
            for (i = 0; i < shm_head->proc_num; i++)
            {
                if ( port != 0 )
                {
                    if ( port != shm_conf[i].lsnr_port )
                    {
                        continue;
                    }
                }

                if ( shm_conf[i].run_status != RUN_STATUS_STOPED )
                {
                    WriteLog(0, 0, OUT_SCREEN, "port=[%6d]进程[%03d][%05d]异常，强行终止", shm_conf[i].lsnr_port, i, shm_conf[i].proc_id);
                    kill(shm_conf[i].proc_id, 9);
                    shm_conf[i].run_status = RUN_STATUS_STOPED;
                }
            }
        }
    }

    WriteLog(0, 0, OUT_SCREEN, "进程停止操作完成");
}

void QueryClient(const char *prog_name)
{
    char buffer[15];
    int  i;
    struct SHM_HEAD *shm_head;
    struct SHM_CONF *shm_conf;

    shm_head = (struct SHM_HEAD *)G_shm;
    shm_conf = (struct SHM_CONF *)(G_shm + sizeof(struct SHM_HEAD));

    WriteLog(0, 0, OUT_SCREEN, "%3s %6s %5s %7s %24s %5s %24s %5s", "SN", "port", "pid", "status", "start time", "active", "last time", "dealnum");

    for (i = 0; i < shm_head->proc_num; i++)
    {
        if ( conflict_proc(prog_name, shm_conf[i].proc_id) == 0 )
        {
            strcpy(shm_conf[i].cur_active, "EXP");
        }

        if ( shm_conf[i].run_status == RUN_STATUS_RUNNING )
        {
            strcpy(buffer, "RUNNING");
        }
        else if ( shm_conf[i].run_status == RUN_STATUS_STOPING )
        {
            strcpy(buffer, "STOPING");
        }
        else if ( shm_conf[i].run_status == RUN_STATUS_STOPED )
        {
            strcpy(buffer, "STOPED");
        }
        else if ( shm_conf[i].run_status == RUN_STATUS_REFRESH )
        {
            strcpy(buffer, "REFRESH");
        }
        else
        {
            strcpy(buffer, "UNKOWN");
        }

        WriteLog(0, 0, OUT_SCREEN, "%3d %6d %5d %7s %24s %5s %24s %5d",
                 i,
                 shm_conf[i].lsnr_port,
                 shm_conf[i].proc_id,
                 buffer,
                 shm_conf[i].start_time,
                 shm_conf[i].cur_active,
                 shm_conf[i].last_time,
                 shm_conf[i].deal_num);
    }
}

void back_ground_svr()
{
    pid_t pidSub;

    pidSub = fork();
    if ( pidSub < 0 )
    {
        fprintf(stderr, "无法将服务器转入后台!\n");
        return;
    }
    else if ( pidSub > 0 )
    {
        exit(0);
    }
    else
    {
        setsid();
        /* chdir("/"); */
        umask(0);
    }
}

/**
 *
 * @author lumw (2014/5/13)
 *
 * @param argc 程序启动时输入参数个数
 * @param argv 程序启动参数数组
 *
 * @return int
 */
int CheckParam(int argc, char *argv[])
{
    int no_found = 0;

    if ( argc < 2 )
    {
        no_found = 1;
    }
    else
    {
        if ( strcmp(argv[1], "start") == 0 )
        {
            if ( argc != 2 )
            {
                if ( argc != 3 )
                {
                    no_found = 1;
                }
            }

        }
        else if ( strcmp(argv[1], "stop") == 0 )
        {
            if ( argc != 2 )
            {
                if ( argc != 3 )
                {
                    no_found = 1;
                }
            }
        }
        else if ( strcmp(argv[1], "refresh") == 0 )
        {
            if ( argc != 2 )
            {
                if ( argc != 3 )
                {
                    no_found = 1;
                }
            }
        }
        else if ( strcmp(argv[1], "query") == 0 )
        {
            if ( argc != 2 )
            {
                no_found = 1;
            }
        }
        else
        {
            no_found = 1;
        }
    }

    if ( no_found == 1 )
    {
        WriteLog(0, 0, OUT_SCREEN, "%s terminal command help:\n[Usage]:", argv[0]);
        WriteLog(0, 0, OUT_SCREEN, "    %s start        >startup all process", argv[0]);
        WriteLog(0, 0, OUT_SCREEN, "    %s stop         >shutdown all process", argv[0]);
        WriteLog(0, 0, OUT_SCREEN, "    %s regresh      >refresh all process parameter", argv[0]);
        WriteLog(0, 0, OUT_SCREEN, "    %s start port   >startup appointed process", argv[0]);
        WriteLog(0, 0, OUT_SCREEN, "    %s stop  port   >shutdown appointed process", argv[0]);
        WriteLog(0, 0, OUT_SCREEN, "    %s regresh port >refresh appointed process parameter", argv[0]);
        WriteLog(0, 0, OUT_SCREEN, "    %s query        >query information", argv[0]);
        return -1;
    }

    return 0;
}

int conflict_proc(const char *prog_name, int pid)
{
/*  char ps[512], buf[512];*/
    int  nn;

    if ( pid == 0 )
    {
        return 0;
    }
    nn = kill(pid, 0);

    if ( nn == 0 )
    {
        return 1;
    }
/*
    sprintf(ps, "ps -ef | grep -v \\\\.sh | grep -v grep |grep -v vi | grep %d | grep -c %s", pid, prog_name);
    shell_command(ps, buf);
    if(atoi(buf) >= 1)
        return 1;
*/
    return 0;
}

int main_ini(const char *item_name, const char *item_value)
{
    if ( strcmp(item_name, "cur_version") == 0 )
    {
        strcpy(G_ini.cur_version, item_value);
    }
    else if ( strcmp(item_name, "shm_key") == 0 )
    {
        G_ini.shm_key = atoi(item_value);
    }
    else if ( strcmp(item_name, "sem_key") == 0 )
    {
        G_ini.sem_key = atoi(item_value);
    }

    return 0;
}

int tns_ini(const char *item_name, const char *item_value)
{
    int sn = G_ini.port_num - 1;

    G_ini.port_list[sn].lsnr_len = 0;

    if ( strcmp(item_name, "name") == 0 )
    {
        strcpy(G_ini.port_list[sn].name, item_value);
    }
    else if ( strcmp(item_name, "lsnr_port") == 0 )
    {
        G_ini.port_list[sn].lsnr_port = atoi(item_value);
    }
    else if ( strcmp(item_name, "lsnr_num") == 0 )
    {
        G_ini.port_list[sn].lsnr_num = atoi(item_value);
        if ( G_ini.port_list[sn].lsnr_num <= 0 )
        {
            G_ini.port_list[sn].lsnr_num = 1;
        }

        G_ini.port_list[sn].lsnr_num++;
    }
    else if ( strcmp(item_name, "lsnr_len") == 0 )
    {
        G_ini.port_list[sn].lsnr_len = atoi(item_value);
    }
    else if ( strcmp(item_name, "out_time") == 0 )
    {
        G_ini.port_list[sn].out_time = atoi(item_value);
    }
    else if ( strcmp(item_name, "is_reuse_addr") == 0 )
    {
        G_ini.port_list[sn].is_reuse_addr = atoi(item_value);
    }
    else if ( strcmp(item_name, "is_long_link") == 0 )
    {
        G_ini.port_list[sn].is_long_link = atoi(item_value);
    }
    else if ( strcmp(item_name, "is_debug") == 0 )
    {
        G_ini.port_list[sn].is_debug = atoi(item_value);
    }
    else if ( strcmp(item_name, "DBUserName") == 0 )
    {
        strcpy(G_ini.port_list[sn].DBUserName, item_value);
    }
    else if ( strcmp(item_name, "DBPassword") == 0 )
    {
        strcpy(G_ini.port_list[sn].DBPassword, item_value);
    }
    else if ( strcmp(item_name, "Sid") == 0 )
    {
        strcpy(G_ini.port_list[sn].Sid, item_value);
    }

    if ( G_ini.port_list[sn].lsnr_len == 0 )
    {
        G_ini.port_list[sn].lsnr_len = G_ini.port_list[sn].lsnr_num;
    }

    return 0;
}

/**
  函数功能:读取配置文件的信息保存到全局变量中
  参数说明:
    无
  输出说明:
    true    表示读取成功
    flase   表示读取失败 则错误信息直接打印到屏幕上
  */
int GetInitInfo(const char *file_name)
{
    char buffer[1024];
    char szBuffer[256];
    FILE *fp;
    int  i, flag = 0;

    memset((void *)&G_ini, 0, sizeof(G_ini));
    strcpy(G_ini.prog_name, file_name);

    sprintf(szBuffer, "%s.ini", file_name);

    if ( (fp = fopen(szBuffer, "r")) == NULL )
    {
        WriteLog(0, 0, OUT_SCREEN, "%s Error:(%d)%s", szBuffer, errno, strerror(errno));
        return -1;
    }

    while (!feof(fp))
    {
        memset(buffer, 0, sizeof(buffer));
        if ( fgets(buffer, sizeof(buffer), fp) == NULL )
        {
            break;
        }

        if ( trim(buffer) == 0 )
        {
            continue;
        }

        if ( buffer[0] == '#' )
        {
            continue;
        }

        if ( strcmp(buffer, "[MAIN]") == 0 )
        {
            flag = 1;
            continue;
        }
        else if ( strcmp(buffer, "[TNS]") == 0 )
        {
            flag = 2;
            G_ini.port_num++;
            continue;
        }
        else
        {
            split_str(buffer, szBuffer, "=", 1, 1, 1, 0);
        }

        if ( flag == 1 )
        {
            main_ini(szBuffer, buffer);
        }
        else if ( flag == 2 )
        {
            tns_ini(szBuffer, buffer);
        }
        else if ( flag == 3 )
        {}
    }
    fclose(fp);

    if ( G_ini.port_num <= 0 )
    {
        WriteLog(0, 0, OUT_SCREEN, "没有配置监听选项");
        return -1;
    }

    /*计算需要启动的进程的数量*/
    G_ini.proc_num = 0;
    for (i = 0; i < G_ini.port_num; i++)
    {
        G_ini.proc_num += G_ini.port_list[i].lsnr_num;
    }

    return 0;
}
