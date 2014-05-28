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
    char start_time[64];        /*系统启动时间*/
};

struct SHM_CONF
{
    int  lsnr_port;             /*监听端口*/
    int  run_status;            /*进程运行状态*/
    char start_time[64];        /*进程启动时间*/
    int  proc_id;               /*进程ID*/
    char cur_active[64];        /*进程当前执行的动作*/
    char last_time[64];         /*最后一次访问时间*/
    int  deal_num;              /*处理交易数量*/
    int  is_used;
};

struct PORT_CONF
{
    char name[128];                     /**/
    int  lsnr_port;                     /*监听端口*/
    int  lsnr_num;                      /*监听数量*/
    int  lsnr_len;                      /*监听队列长度*/
    int  out_time;                      /*超时时间(秒)*/
    int  is_reuse_addr;                 /*是否重用监听端口*/
    int  is_long_link;                  /*是否是长连接*/
    int  is_debug;                      /*调试开关*/
};

struct INIT_CONF
{
    char cur_version[64];               /*当前程序版本*/
    int  shm_key;                       /*共享内存ID*/
    int  sem_key;
    char prog_name[128];

    struct PORT_CONF port_list[MAX_PORT_NUM];
    int  port_num;
    int  proc_num;
};

struct AUTH_IPNET
{
    int  is_authip;                     /*是否进行鉴权 1进行 0否，默认1*/
    char auth_ip[MAX_AUTH_IP_NUM][64];  /*鉴权IP*/
    int  auth_num;                      /*鉴权IP数量*/
};



/***********发送、接收数据结构体*****************/


/*GPS数据报文，非结构体内存对齐*/
#pragma pack(1)
struct GPS_INFO
{
    char msg_type;                   /*报文类型*/
    char msg_version;                /*报文版本号*/
    char msg_command;                /*协议命令字*/
    char gps_msg_length[4];          /*GPS数据信息总长度*/
    char device_type;                /*设备类型*/
    char device_id[33];              /*设备编号*/
    char x_coordinate[8];            /*X坐标*/
    char Y_coordinate[8];            /*Y坐标*/
    char height[8];                  /*高度*/
    char speed[8];                   /*速度*/
    char direction[8];               /*方向*/
};


#endif
