#ifndef _MY_TOOLS_H_INCLUDE_
#define _MY_TOOLS_H_INCLUDE_


int shell_command(const char *cmd, char *retbuff);
int ltrim(char *str);
int rtrim(char *str);
int trim(char *str);
void replace2(char *s_str, const char *ing_str, int start, int len);
void replace1(char *s_str, const char *ing_str, const char *ed_str);
void str_lower(char *str);
void str_upper(char *str);
void get_cur_time(const char * _format, char * time_buff);
void sub_str(char *s_str, char *t_str, int addr, int len, int type);
void split_str(char *s_str, char *t_str, const char *sp_str, int num, int type, int type1, int dect);
void reverse(char *str);
#endif
