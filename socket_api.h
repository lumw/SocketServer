#ifndef _TUXEDO_REALINTF_TOOLS_INCLUDE_
#define _TUXEDO_REALINTF_TOOLS_INCLUDE_

#define LINUX_HOST 
int gethostinfo(char *name, char *address);
int link_remote(const char *remote_ip, int remote_port, const char *local_ip);
int send_data(const char *snd_buffer, int snd_size, int sock_fd);
int recv_data1(char *rcv_buffer, int rcv_size, int out_time, int sock_fd);
int recv_data2(char *rcv_buffer, int seg_size, int out_time, const char * stop_flag, int sock_fd);
void linker_cut(int sock_fd, int type);
int get_sock_info(int sock_fd, char *ip_info, int *port_info);
int get_perer_info(int sock_fd, char *ip_info, int *port_info);
int create_listen(int _port, int max_link, int non_block, int reuse_addr);
int startup_listen(int sock_fd, int out_time);
#endif
