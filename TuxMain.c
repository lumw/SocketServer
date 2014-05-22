#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>

#include "log.h"
#include "TuxClient.h"
#include "Global.h"
#include "SemTools.h"

#define CURRENT_SERVER_VERSION   "00.000.00.07.01.2000"

struct INIT_CONF   G_ini;
struct ctrl_data * G_ctrl_data;
char *             G_shm;
char *             G_trans_buffer;

void FreeResource()
{
    if (G_trans_buffer != NULL)
    {
        free((void *)G_trans_buffer);
    }

    if (G_ctrl_data != NULL)
    {
        free((void *)G_ctrl_data);
    }

    G_trans_buffer = NULL;
    G_ctrl_data = NULL;
}


int StartPort(int port, int lnr_num, int lnr_len, int is_reused)
{
    int count, i, socket_fd;
    pid_t pid;

    struct SHM_CONF * shm_ptr = NULL;

    count = GetPortShm(port, &shm_ptr);
    if (count <= 0)
    {
        WriteLog(0, 0, OUT_SCREEN, "ini�ļ�������֮���޸Ĺ�,�˿�[%d]������", port);
        return -1;
    }
    if (count != lnr_num)
    {
        WriteLog(0, 0, OUT_SCREEN, "ini�ļ�������֮���޸Ĺ�,�˿�[%d]���̸�������", port);
        return -1;
    }
    if (shm_ptr == NULL)
    {
        WriteLog(0, 0, OUT_SCREEN, "ini�ļ�������֮���޸Ĺ�,�˿�[%d]��Ӧ��SHMΪ��", port);
        return -1;
    }

    socket_fd = create_listen(port, lnr_len, 1, is_reused);
    if (socket_fd <= 0)
    {
        WriteLog(0, 0, OUT_SCREEN, "�ڶ˿�[%d]�Ͻ�������ʧ��,%d:%s", port, errno, strerror(errno));
        return -1;
    }

    for (i = 0 ; i < lnr_num ; i++)
    {
        if (shm_ptr[i].run_status != RUN_STATUS_STOPED)
        {
            if (shm_ptr[i].proc_id == 0)
            {
                shm_ptr[i].run_status = RUN_STATUS_STOPED;
                ;
            }
            else
            {
                if (kill(shm_ptr[i].proc_id, 0) != 0)
                {
                    shm_ptr[i].proc_id = 0;
                    shm_ptr[i].run_status = RUN_STATUS_STOPED;
                }
                else
                {
                    continue;
                }
            }
        }
        else
        {
            shm_ptr[i].proc_id = 0;
            shm_ptr[i].run_status = RUN_STATUS_STOPED;
        }

        pid = fork();
        if (pid < 0)
        {
            continue;
        }
        if (pid == 0)
        {
            catch_all_singal();
            LsnrRec(port, i, socket_fd);
            linker_cut(socket_fd, 0);

            FreeResource();

            exit(0);
        }
    }
    linker_cut(socket_fd, 0);

    return 0;
}

int main(int argc, char *argv[])
{
    char buffer[512], prog_name[512], sTemp[256];
    int  nret, i, share_size, count;
    int  shm_id;
    int  shm_need_init;
    int  bRet;

    /*���������������Ƿ���ȷ*/
    if (CheckParam(argc, argv) == -1)
    {
        return 0;
    }

    strcpy(buffer, argv[0]);
    split_str(buffer, prog_name, "/", 1, 1, 1, 1);

    /*�������Ƿ��Ѿ��ں�̨����*/
    char command[256];
    sprintf(command, "ps -ef | grep -v \\\\-csh | grep -v \\\\.sh | grep -v ps | grep -v grep |grep -v vi | grep -c %s", prog_name);
    shell_command(command, sTemp);
    if (strcmp(argv[1], "start") != 0)
    {
        if (atoi(sTemp) <= 1)
        {
            WriteLog(0, 0, OUT_SCREEN, "ϵͳ��δ����");
            return 0;
        }
    }

    WriteLog(0, 0, OUT_SCREEN, "��ȡ�����ļ� ... ");
    if (GetInitInfo(argv[0]) == -1)
    {
        return 0;
    }
    /*WriteLog(0, 0, OUT_SCREEN, "OK !");*/

    if (strcmp(G_ini.cur_version, CURRENT_SERVER_VERSION) != 0)
    {
        WriteLog(0, 0, OUT_SCREEN, "����汾���󣬽�ֹ����!");
        return 0;
    }

    G_shm = NULL;
    G_ctrl_data = NULL;
    G_trans_buffer = NULL;

    WriteLog(0, 0, OUT_SCREEN, "��ȡ�����ڴ� ... ");
    shm_need_init = 0;
    if (strcmp(argv[1], "start") == 0 && argc == 2)
    {
        if (atoi(sTemp) > 1)
        {
            WriteLog(0, 0, OUT_SCREEN, "ϵͳ�Ѿ�ִ����ɸ�����");
            return 0;
        }

        share_size = G_ini.proc_num * ((int)sizeof(struct SHM_CONF)) + (int)sizeof(struct SHM_HEAD);
        shm_id = shmget((key_t)G_ini.shm_key, (size_t)share_size, IPC_CREAT | IPC_EXCL | 0777);
        shm_need_init = 1;

        bRet = sem_requ(G_ini.sem_key, 1, 1);
    }
    else
    {
        shm_id = shmget((key_t)G_ini.shm_key, 0, 0777);
        bRet = sem_get(G_ini.sem_key, 1);
    }
    if (shm_id < 0)
    {
        /*EEXIST 17 File exists*/
        WriteLog(0, 0, OUT_SCREEN, " shmget error(%d):%s", errno, strerror(errno));
        FreeShm(shm_id);
        return 0;
    }
    if (bRet == SEM_ERROR)
    {
        WriteLog(0, 0, OUT_SCREEN, " semget error:%s", gs_sem_err);
        FreeShm(shm_id);
        return 0;
    }

    G_shm = (char *)shmat(shm_id, 0, 0);
    if (G_shm == (void *)-1)
    {
        WriteLog(0, 0, OUT_SCREEN, " shmat error(%d):%s", errno, strerror(errno));
        //shmctl(shm_id, IPC_RMID, 0);
        FreeShm(shm_id);
        return 0;
    }

    if (shm_need_init == 1)
    {
        /*�����ڴ���Ի�*/
        InitShm();
    }
    else
    {
        if (ShmConfCheck() == -1)
        {
            WriteLog(0, 0, OUT_SCREEN, "INI�ļ�������֮���޸Ĺ�,��ִֹ�е�ǰָ��!");
            if (shm_need_init == 1)
            {
                FreeShm(shm_id);
            }
            return 0;
        }
    }

    /*WriteLog(0, 0, OUT_SCREEN, "OK !");*/

    if (strcmp(argv[1], "stop") == 0 || strcmp(argv[1], "query") == 0 || strcmp(argv[1], "refresh") == 0)
    {
        strcpy(buffer, argv[0]);
        split_str(buffer, prog_name, "/", 1, 1, 1, 1);

        if (strcmp(argv[1], "query") == 0)
        {
            QueryClient(prog_name);
        }

        if (strcmp(argv[1], "refresh") == 0)
        {
            if (argc != 3)
            {
                RefreshParam(prog_name, 0);
            }
            else
            {
                RefreshParam(prog_name, atoi(argv[2]));
            }
        }

        if (strcmp(argv[1], "stop") == 0)
        {
            WriteLog(0, 0, OUT_SCREEN, "server stop ...");
            if (argc != 3)
            {
                StopClient(prog_name, 0);
            }
            else
            {
                StopClient(prog_name, atoi(argv[2]));
            }
        }

        if (strcmp(argv[1], "refresh") != 0)
        {
            if (GetRunCount() == 0)
            {
                /*WriteLog(0, 0, OUT_SCREEN, "�ͷ���Դ ...");*/
                FreeShm(shm_id);
            }
        }
        return 0;
    }




    /****************************************************************************************/
    count = 0;

    printf("�����˿ں� %d:", G_ini.port_num);
    for (i = 0 ; i < G_ini.port_num ; i++)
    {

        if (argc != 2)
        {
            if (atoi(argv[2]) != G_ini.port_list[i].lsnr_port)
            {
                continue;
            }
        }

        WriteLog(0, 0, OUT_SCREEN, "\n����:%s�ӿ�,port=[%d]", G_ini.port_list[i].name, G_ini.port_list[i].lsnr_port);
        nret = StartPort(G_ini.port_list[i].lsnr_port, G_ini.port_list[i].lsnr_num, G_ini.port_list[i].lsnr_len, G_ini.port_list[i].is_reuse_addr);

        FreeResource();
        if (nret < 0)
        {
            continue;
        }

        count++;
    }

    if (count == 0)
    {
        WriteLog(0, 0, OUT_SCREEN, "ϵͳ����ʧ��,�ͷ�ϵͳ��Դ!");
        FreeShm(shm_id);
    }
    else
    {
        WriteLog(0, 0, OUT_SCREEN, "ϵͳ����ָ��ִ�����!");
        sleep(2);
        if (GetRunCount() == 0)
        {
            WriteLog(0, 0, OUT_SCREEN, "��������ʧ��");
            FreeShm(shm_id);
        }
    }

    return 0;
}
