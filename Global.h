#ifndef _MY_GLOBAL_H_
#define _MY_GLOBAL_H_
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <termio.h>
#include <netdb.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <locale.h>
#include <oci.h>

/*自定义头文件*/
#include "Define.h"
#include "Tools.h"



/**TuxClient.h*/
extern int ShmConfCheck();
extern int GetRunCount();
extern int GetPortShm(int port, struct SHM_CONF ** port_shm);
extern int GetInitInfo(const char * file_name);
extern void InitShm();
extern void FreeShm(int _shm_id);
extern void StopClient(const char * prog_name, int port);
extern void QueryClient(const char * prog_name);
extern void RefreshParam(const char * prog_name, int port);

/**Tools.h*/
extern int shell_command(const char *cmd, char *retbuff);
extern int ltrim(char *str);
extern int rtrim(char *str);
extern int trim(char *str);
extern void replace2(char *s_str, const char *ing_str, int start, int len);
extern void replace1(char *s_str, const char *ing_str, const char *ed_str);
extern void str_lower(char *str);
extern void str_upper(char *str);
extern void get_cur_time(const char * _format, char * time_buff);
extern void sub_str(char *s_str, char *t_str, int addr, int len, int type);
extern void split_str(char *s_str, char *t_str, const char *sp_str, int num, int type, int type1, int dect);
extern void reverse(char *str);

/*Socket.h*/
extern int gethostinfo(char *name, char *address);
extern int link_remote(const char *remote_ip, int remote_port, const char *local_ip);
extern int send_data(const char *snd_buffer, int snd_size, int sock_fd);
extern int recv_data1(char *rcv_buffer, int rcv_size, int out_time, int sock_fd);
extern int recv_data2(char *rcv_buffer, int seg_size, int out_time, const char * stop_flag, int sock_fd);
extern void linker_cut(int sock_fd, int type);
extern int get_sock_info(int sock_fd, char *ip_info, int *port_info);
extern int get_perer_info(int sock_fd, char *ip_info, int *port_info);
extern int create_listen(int _port, int max_link, int non_block, int reuse_addr);
extern int startup_listen(int sock_fd, int out_time);

/*SemTools.h*/
extern int sem_requ(int sem_key, int sem_num, int _value);
extern int sem_get(int sem_key, int sem_num);
extern int sem_free();
extern int sem_oper(int sem_no, int _values);

/*RecDeal.h*/
extern void catch_all_singal();
extern void LsnrRec(int port, int serial_no, int socket_fd);

/*OracleDB.h*/
extern int ConnectDB(const char* userName, const char* password, const char* sid);
extern int DisConnectDB();

