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

#define TRUE                1
#define FALSE               0

#define GPS_INFO_UPLOAD_REQ     " 2"

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
    char DBUserName[128];               //数据库连接用户名
    char DBPassword[128];               //数据库连接口令
    char Sid[128];                      //数据库实例
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
    char msg_type[2 + 1];               //报文类型
    char msg_version[2 + 1];            //报文版本号
    char msg_command[2 + 1];            //协议命令字
    char gps_msg_length[5 + 1];         //GPS数据信息总长度
    char device_id[6 + 1];              //设备编号
    char warn_flag[1 + 1];              //报警标志
    char x_coordinate[11 + 1];          //X坐标
    char y_coordinate[11 + 1];          //Y坐标
    char speed[8 + 1];                  //速度
    char direction[6 + 1];              //方向
    char height[6 + 1];                 //高度
    char base_station_info1[5 + 1];     //基站信息1
    char base_station_info2[5 + 1];     //基站信息2
    char sent_msg_cnt[4 + 1];           //当日累计发包个数
};


#endif
