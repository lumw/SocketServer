#ifndef _MY_ORACLEDB_H_
#define _MY_ORACLEDB_H_



int ConnectDB(const char* userName, const char* password, const char* sid);
int DisConnectDB();


#endif
