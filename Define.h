#ifndef _MY_DEFINE_H_INCLUDE_
#define _MY_DEFINE_H_INCLUDE_

#define MAX_AUTH_IP_NUM     20
#define MAX_PORT_NUM        100

#define RUN_STATUS_STOPED   0
#define RUN_STATUS_STOPING  1
#define RUN_STATUS_RUNNING  2
#define RUN_STATUS_REFRESH  3

#define RIGHT               1
#define ERROR               0

struct SHM_HEAD
{
    int  proc_num;
    char start_time[64];        /*ϵͳ����ʱ��*/
};

struct SHM_CONF
{
    int  lsnr_port;             /*�����˿�*/
    int  run_status;            /*��������״̬*/
    char start_time[64];        /*��������ʱ��*/
    int  proc_id;               /*����ID*/
    char cur_active[64];        /*���̵�ǰִ�еĶ���*/
    char last_time[64];         /*���һ�η���ʱ��*/
    int  deal_num;              /*����������*/
    int  is_used;
};

struct PORT_CONF
{
    char name[128];                     /**/
    int  lsnr_port;                     /*�����˿�*/
    int  lsnr_num;                      /*��������*/
    int  lsnr_len;                      /*�������г���*/
    int  out_time;                      /*��ʱʱ��(��)*/
    int  is_reuse_addr;                 /*�Ƿ����ü����˿�*/
    int  is_long_link;                  /*�Ƿ��ǳ�����*/
    int  is_debug;                      /*���Կ���*/
};

struct INIT_CONF
{
    char cur_version[64];               /*��ǰ����汾*/
    int  shm_key;                       /*�����ڴ�ID*/
    int  sem_key;
    char prog_name[128];

    struct PORT_CONF port_list[MAX_PORT_NUM];
    int  port_num;
    int  proc_num;
};

struct AUTH_IPNET
{
    int  is_authip;                     /*�Ƿ���м�Ȩ 1���� 0��Ĭ��1*/
    char auth_ip[MAX_AUTH_IP_NUM][64];  /*��ȨIP*/
    int  auth_num;                      /*��ȨIP����*/
};



/***********���͡��������ݽṹ��*****************/


/*GPS���ݱ��ģ��ǽṹ���ڴ����*/
#pragma pack(1)
struct GPS_INFO
{
    char msg_type;                   /*��������*/
    char msg_version;                /*���İ汾��*/
    char msg_command;                /*Э��������*/
    char gps_msg_length[4];          /*GPS������Ϣ�ܳ���*/
    char device_type;                /*�豸����*/
    char device_id[33];              /*�豸���*/
    char x_coordinate[8];            /*X����*/
    char Y_coordinate[8];            /*Y����*/
    char height[8];                  /*�߶�*/
    char speed[8];                   /*�ٶ�*/
    char direction[8];               /*����*/
};


#endif
