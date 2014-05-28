#include <stdio.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>

#include "SemTools.h"

#define LINUX_HOST

#if defined(LINUX_HOST) || defined(OSF_HOST)
    #define SEM_R      0x0100
    #define SEM_A      0x0080
#endif

typedef union _SEMA_UN
{
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
} SEMA_UN;
typedef SEMA_UN *PSEMA_UN;
typedef struct sembuf SEMA_BUF;


int gi_sem_id;

/*******************************************************************************************************
函数功能
    设置信号量集中某个信号的数值
输出参数
    int sem_id      信号量集ID
    int sem_no      信号量集中的第几个信号量
    int _values     信号量的数值
    std::string &err_info   错误信息

输出参数：
    操作成功，返回SEM_RIGHT
    失败返回SEM_ERROR
*******************************************************************************************************/
int sem_setval(int sem_no, int _values)
{
    SEMA_UN suCreate;

    suCreate.val = _values;
    if (semctl(gi_sem_id, sem_no, SETVAL, suCreate) < 0)
    {
        sprintf(gs_sem_err, "semctl SETVAL fail, error(%d):%s",
                errno,
                strerror(errno));
        return SEM_ERROR;
    }

    return SEM_RIGHT;
}

/*******************************************************************************************************
函数功能
    信号量集的创建,并设置初始数值
输出参数
    int sem_id      信号量集ID
    int sem_key     信号量集KEY
    int sem_num     信号量集中信号的个数
    int _value      信号量集中的初始化
    std::string &err_info   错误信息
输出参数：
    操作成功，返回SEM_RIGHT
    失败返回SEM_ERROR
*******************************************************************************************************/
int sem_requ(int sem_key, int sem_num, int _value)
{
    int i;

    if (sem_key <= 0)
    {
        sprintf(gs_sem_err, "semget fail, invalid sem key [%d]", sem_key);
        return SEM_ERROR;
    }
    if (sem_num <= 0)
    {
        sem_num = 1;
    }

    //gi_sem_id = semget((key_t)sem_key, sem_num, IPC_EXCL | IPC_CREAT | SEM_A | SEM_R);
    //gi_sem_id = semget((key_t)sem_key, sem_num, SEM_A | SEM_R);
    gi_sem_id = semget((key_t)sem_key, sem_num, IPC_CREAT | SEM_A | SEM_R);
    if (gi_sem_id < 0)
    {
        sprintf(gs_sem_err, "semget fail, error(%d):%s", errno, strerror(errno));
        return SEM_ERROR;
    }
    for (i = 0 ; i < sem_num ; i++)
    {
        if (sem_setval(i, _value) == SEM_ERROR)
        {
            sem_free();

            return SEM_ERROR;
        }
    }
    return SEM_RIGHT;
}

/*******************************************************************************************************
函数功能
    获取已经创建的信号量集
输出参数
    int sem_key     信号量集KEY
    int sem_num     信号量集中信号的个数
    std::string &err_info   错误信息
输出参数：
    操作成功，返回SEM_RIGHT
    失败返回SEM_ERROR
*******************************************************************************************************/
int sem_get(int sem_key, int sem_num)
{
    if (sem_key <= 0)
    {
        sprintf(gs_sem_err, "semget fail, invalid sem key [%d]", sem_key);
        return SEM_ERROR;
    }
    if (sem_num <= 0)
    {
        sem_num = 1;
    }

    //gi_sem_id = semget((key_t)sem_key, sem_num, IPC_EXCL | IPC_CREAT | SEM_A | SEM_R);
    gi_sem_id = semget((key_t)sem_key, sem_num, SEM_A | SEM_R);
    //gi_sem_id = semget((key_t)sem_key, sem_num, IPC_CREAT | SEM_A | SEM_R);
    if (gi_sem_id < 0)
    {
        sprintf(gs_sem_err, "semget fail, error(%d):%s", errno, strerror(errno));
        return SEM_ERROR;
    }
    return SEM_RIGHT;
}

/*******************************************************************************************************
函数功能
    信号量集的释放
输出参数
    int sem_id      信号量集ID
    std::string &err_info   错误信息
输出参数：
    操作成功，返回SEM_RIGHT
    失败返回SEM_ERROR
*******************************************************************************************************/
int sem_free()
{
    SEMA_UN suFree;

    if (gi_sem_id <= 0)
    {
        sprintf(gs_sem_err, "invalid sem id [%d]", gi_sem_id);
        return SEM_ERROR;
    }

    suFree.val = 0;
    if (semctl(gi_sem_id, 0, IPC_RMID, suFree ) < 0)
    {
        sprintf(gs_sem_err,"remove sem fail,error(%d):%s", errno, strerror(errno));
        return SEM_ERROR;
    }

    return SEM_RIGHT;
}

/*******************************************************************************************************
函数功能
    信号量集的使用
输出参数
    int sem_id      信号量集ID
    int sem_no      信号量集中的第几个信号量
    int _values     >0表示解锁 <0表示加锁
    std::string &err_info   错误信息

输出参数：
    操作成功，返回SEM_RIGHT
    失败返回SEM_ERROR
*******************************************************************************************************/
int sem_oper(int sem_no, int _values)
{
    SEMA_BUF sbParam;

    if (gi_sem_id <= 0)
    {
        sprintf(gs_sem_err, "invalid sem id [%d]", gi_sem_id);
        return SEM_ERROR;
    }

    if (sem_no < 0)
    {
        sem_no = 0;
    }
    sbParam.sem_num = sem_no;
    sbParam.sem_op  = _values;
    //sbParam.sem_flg = SEM_UNDO;
    sbParam.sem_flg = 0;

    if (semop(gi_sem_id, &sbParam, 1) < 0)
    {
        sprintf(gs_sem_err, "semop[sem_no=%d][value=%d] fail,error(%d):%s", sem_no, _values, errno, strerror(errno));
        //printf(gs_sem_err);
        return SEM_ERROR;
    }

    return SEM_RIGHT;
}
