#ifndef _TUXCLIENT_H_INCLUDE_
#define _TUXCLIENT_H_INCLUDE_

#include "Global.h"

void InitShm();
void FreeShm(int _shm_id);

int ShmConfCheck();
int GetRunCount();
int GetPortShm(int port, struct SHM_CONF ** port_shm);

int GetInitInfo(const char * file_name);

void StopClient(const char * prog_name, int port);
void QueryClient(const char * prog_name);
void RefreshParam(const char * prog_name, int port);


#endif
