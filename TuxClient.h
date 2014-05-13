#ifndef _TUXEDO_REALINTF_TUXCLIENT_INCLUDE_
#define _TUXEDO_REALINTF_TUXCLIENT_INCLUDE_

//#include <fml32.h>
#include "Global.h"
//#include "user_verify.h"
//#include "h2_info.h"
//#include "ctrl_data.h"

void InitShm();
void FreeShm(int _shm_id);

int ShmConfCheck();
int GetRunCount();
int GetPortShm(int port, struct SHM_CONF ** port_shm);

int GetInitInfo(const char * file_name);

void StopClient(const char * prog_name, int port);
void QueryClient(const char * prog_name);
void RefreshParam(const char * prog_name, int port);

/**
 * 调用Tuxedo校验服务，检查其合法性
 * 返回0表示校验成功　-1失败
 */
int UserVerify(int sys_id, const char *prog_name, int port, int lsnr_num);
int GetH2Info(int sys_id, struct h2_info * _h2info);
int GetCtrlCount(int sys_id);
int GetCtrlData(int sys_id, int num, struct ctrl_data *buff);

#endif
