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
��������
    �����ź�������ĳ���źŵ���ֵ
�������
    int sem_id      �ź�����ID
    int sem_no      �ź������еĵڼ����ź���
    int _values     �ź�������ֵ
    std::string &err_info   ������Ϣ

���������
    �����ɹ�������SEM_RIGHT
    ʧ�ܷ���SEM_ERROR
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
��������
    �ź������Ĵ���,�����ó�ʼ��ֵ
�������
    int sem_id      �ź�����ID
    int sem_key     �ź�����KEY
    int sem_num     �ź��������źŵĸ���
    int _value      �ź������еĳ�ʼ��
    std::string &err_info   ������Ϣ
���������
    �����ɹ�������SEM_RIGHT
    ʧ�ܷ���SEM_ERROR
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
��������
    ��ȡ�Ѿ��������ź�����
�������
    int sem_key     �ź�����KEY
    int sem_num     �ź��������źŵĸ���
    std::string &err_info   ������Ϣ
���������
    �����ɹ�������SEM_RIGHT
    ʧ�ܷ���SEM_ERROR
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
��������
    �ź��������ͷ�
�������
    int sem_id      �ź�����ID
    std::string &err_info   ������Ϣ
���������
    �����ɹ�������SEM_RIGHT
    ʧ�ܷ���SEM_ERROR
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
��������
    �ź�������ʹ��
�������
    int sem_id      �ź�����ID
    int sem_no      �ź������еĵڼ����ź���
    int _values     >0��ʾ���� <0��ʾ����
    std::string &err_info   ������Ϣ

���������
    �����ɹ�������SEM_RIGHT
    ʧ�ܷ���SEM_ERROR
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
